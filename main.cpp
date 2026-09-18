#include "app.h"
#include <limits.h>
#include <unistd.h>

std::string get_icon_path() {
    char executable_path[PATH_MAX];
    ssize_t length = readlink("/proc/self/exe", executable_path, sizeof(executable_path) - 1);
    if (length > 0) {
        executable_path[length] = '\0';
        std::string path(executable_path);
        size_t separator = path.find_last_of('/');
        if (separator != std::string::npos) {
            return path.substr(0, separator + 1) + "app-icon.svg";
        }
    }
    return "app-icon.svg";
}

int main(int argc, char *argv[]) {
    curl_global_init(CURL_GLOBAL_ALL);
    gtk_init(&argc, &argv);
    load_settings();

    // 1. Configure the core structural popup layout
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "AI-Chat-Local");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 550);
    GError *icon_error = NULL;
    std::string icon_path = get_icon_path();
    gtk_window_set_icon_from_file(GTK_WINDOW(window), icon_path.c_str(), &icon_error);
    if (icon_error) {
        g_printerr("Could not load app icon: %s\n", icon_error->message);
        g_error_free(icon_error);
    }

    window_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(window_provider, "window { background-color: #010736; }", -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(window_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 2. Setup structural grid box
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);
    box_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(box_provider, "#box { background-color: #010736; }", -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(box_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_widget_set_name(box, "box");

    model_status_label = gtk_label_new("");
    update_status_label();
    gtk_box_pack_start(GTK_BOX(box), model_status_label, FALSE, FALSE, 0);

    // 3. Create the model selector panel at the top
    GtkWidget *model_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *model_label = gtk_label_new("Model:");
    model_combo = gtk_combo_box_text_new();
    gtk_box_pack_start(GTK_BOX(model_box), model_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(model_box), model_combo, TRUE, TRUE, 0);
    g_signal_connect(model_combo, "changed", G_CALLBACK(on_model_changed), NULL);

    GtkWidget *service_label = gtk_label_new("Ollama");
    gtk_box_pack_start(GTK_BOX(model_box), service_label, FALSE, FALSE, 0);
    service_switch = gtk_switch_new();
    gtk_box_pack_start(GTK_BOX(model_box), service_switch, FALSE, FALSE, 0);
    g_signal_connect(service_switch, "state-set", G_CALLBACK(on_toggle_ollama_service), NULL);

    GtkWidget *settings_button = gtk_button_new_with_label("Settings");
    gtk_box_pack_end(GTK_BOX(model_box), settings_button, FALSE, FALSE, 0);
    g_signal_connect(settings_button, "clicked", G_CALLBACK(on_settings_clicked), window);
    gtk_box_pack_start(GTK_BOX(box), model_box, FALSE, FALSE, 0);

    // 4. Create the text panel layout below the controls
    GtkWidget *chat_overlay = gtk_overlay_new();
    gtk_box_pack_start(GTK_BOX(box), chat_overlay, TRUE, TRUE, 0);
    chat_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(chat_overlay), chat_scrolled_window);

    chat_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(chat_provider, "scrolledwindow, scrolledwindow viewport, scrolledwindow viewport textview, textview.view { background-color: #0D1C42; color: #FFFFFF; }", -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(chat_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    chat_buffer_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(chat_buffer_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(chat_buffer_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(chat_buffer_view), GTK_WRAP_WORD);
    chat_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_buffer_view));
    gtk_container_add(GTK_CONTAINER(chat_scrolled_window), chat_buffer_view);
    chat_viewport = gtk_bin_get_child(GTK_BIN(chat_scrolled_window));
    chat_widget_provider = gtk_css_provider_new();
    gtk_style_context_add_provider(gtk_widget_get_style_context(chat_scrolled_window), GTK_STYLE_PROVIDER(chat_widget_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(gtk_widget_get_style_context(chat_viewport), GTK_STYLE_PROVIDER(chat_widget_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(gtk_widget_get_style_context(chat_buffer_view), GTK_STYLE_PROVIDER(chat_widget_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    apply_background_colors();

    thinking_indicator = gtk_label_new("");
    gtk_widget_set_halign(thinking_indicator, GTK_ALIGN_START);
    gtk_widget_set_valign(thinking_indicator, GTK_ALIGN_END);
    gtk_widget_set_margin_top(thinking_indicator, 4);
    gtk_widget_set_margin_bottom(thinking_indicator, 8);
    gtk_widget_set_margin_start(thinking_indicator, 8);
    gtk_widget_set_no_show_all(thinking_indicator, TRUE);
    gtk_widget_hide(thinking_indicator);
    gtk_overlay_add_overlay(GTK_OVERLAY(chat_overlay), thinking_indicator);

    refresh_service_switch();

    // 5. Create the native user text submission panel
    entry_input = gtk_entry_new();
    input_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(input_provider, "entry { background-color: #22396F; color: #FFFFFF; }", -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(input_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_input), "Type your message...");
    gtk_box_pack_end(GTK_BOX(box), entry_input, FALSE, FALSE, 0);
    g_signal_connect(entry_input, "activate", G_CALLBACK(on_send_message), NULL);
    apply_background_colors();

    if (is_ollama_running()) {
        append_system_message("Native C++ application loaded. Ollama is running.", "success");
    } else {
        append_system_message("Ollama is not running. Please switch it on in the chat window.", "error");
    }

    gtk_widget_show_all(window);
    gtk_widget_grab_focus(entry_input);
    gtk_main();

    curl_global_cleanup();
    return 0;
}

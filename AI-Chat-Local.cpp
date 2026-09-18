#include <gtk/gtk.h>
#include <curl/curl.h>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#include "app_state.h"

std::string get_settings_path() {
    return std::string(g_get_user_config_dir()) + "/ai-chat-local/settings.ini";
}

void load_settings() {
    GKeyFile *key_file = g_key_file_new();
    GError *error = NULL;
    std::string path = get_settings_path();

    if (g_key_file_load_from_file(key_file, path.c_str(), G_KEY_FILE_NONE, &error)) {
        gchar *ollama_endpoint = g_key_file_get_string(key_file, "Ollama", "endpoint", NULL);
        gchar *window_color = g_key_file_get_string(key_file, "Colors", "window", NULL);
        gchar *chat_color = g_key_file_get_string(key_file, "Colors", "chat", NULL);
        gchar *input_background_color = g_key_file_get_string(key_file, "Colors", "input_background", NULL);
        gchar *input_text = g_key_file_get_string(key_file, "Colors", "input_text", NULL);
        gchar *user_sender = g_key_file_get_string(key_file, "MessageColors", "user_sender", NULL);
        gchar *user_message = g_key_file_get_string(key_file, "MessageColors", "user_message", NULL);
        gchar *ai_sender = g_key_file_get_string(key_file, "MessageColors", "ai_sender", NULL);
        gchar *ai_message = g_key_file_get_string(key_file, "MessageColors", "ai_message", NULL);
        gchar *system_sender = g_key_file_get_string(key_file, "MessageColors", "system_sender", NULL);
        gchar *system_info = g_key_file_get_string(key_file, "MessageColors", "system_info", NULL);
        gchar *system_warning = g_key_file_get_string(key_file, "MessageColors", "system_warning", NULL);
        gchar *system_error = g_key_file_get_string(key_file, "MessageColors", "system_error", NULL);
        gchar *system_success = g_key_file_get_string(key_file, "MessageColors", "system_success", NULL);
        if (ollama_endpoint && *ollama_endpoint) {
            OLLAMA_ENDPOINT = ollama_endpoint;
        }
        g_free(ollama_endpoint);
        if (window_color) {
            gdk_rgba_parse(&window_background, window_color);
            g_free(window_color);
        }
        if (chat_color) {
            gdk_rgba_parse(&chat_background, chat_color);
            g_free(chat_color);
        }
        if (input_background_color) {
            gdk_rgba_parse(&input_background, input_background_color);
            g_free(input_background_color);
        }
        if (input_text) {
            gdk_rgba_parse(&input_text_color, input_text);
            g_free(input_text);
        }
        if (user_sender) { gdk_rgba_parse(&user_sender_color, user_sender); g_free(user_sender); }
        if (user_message) { gdk_rgba_parse(&user_message_color, user_message); g_free(user_message); }
        if (ai_sender) { gdk_rgba_parse(&ai_sender_color, ai_sender); g_free(ai_sender); }
        if (ai_message) { gdk_rgba_parse(&ai_message_color, ai_message); g_free(ai_message); }
        if (system_sender) { gdk_rgba_parse(&system_sender_color, system_sender); g_free(system_sender); }
        if (system_info) { gdk_rgba_parse(&system_info_color, system_info); g_free(system_info); }
        if (system_warning) { gdk_rgba_parse(&system_warning_color, system_warning); g_free(system_warning); }
        if (system_error) { gdk_rgba_parse(&system_error_color, system_error); g_free(system_error); }
        if (system_success) { gdk_rgba_parse(&system_success_color, system_success); g_free(system_success); }
    }

    if (error) {
        g_error_free(error);
    }
    g_key_file_free(key_file);
}

void save_settings() {
    GKeyFile *key_file = g_key_file_new();
    g_key_file_set_string(key_file, "Ollama", "endpoint", OLLAMA_ENDPOINT.c_str());
    gchar *window_color = gdk_rgba_to_string(&window_background);
    gchar *chat_color = gdk_rgba_to_string(&chat_background);
    gchar *input_background_color = gdk_rgba_to_string(&input_background);
    gchar *input_text = gdk_rgba_to_string(&input_text_color);
    gchar *user_sender = gdk_rgba_to_string(&user_sender_color);
    gchar *user_message = gdk_rgba_to_string(&user_message_color);
    gchar *ai_sender = gdk_rgba_to_string(&ai_sender_color);
    gchar *ai_message = gdk_rgba_to_string(&ai_message_color);
    gchar *system_sender = gdk_rgba_to_string(&system_sender_color);
    gchar *system_info = gdk_rgba_to_string(&system_info_color);
    gchar *system_warning = gdk_rgba_to_string(&system_warning_color);
    gchar *system_error = gdk_rgba_to_string(&system_error_color);
    gchar *system_success = gdk_rgba_to_string(&system_success_color);
    std::string path = get_settings_path();
    std::string directory = path.substr(0, path.find_last_of('/'));

    g_mkdir_with_parents(directory.c_str(), 0700);
    g_key_file_set_string(key_file, "Colors", "window", window_color);
    g_key_file_set_string(key_file, "Colors", "chat", chat_color);
    g_key_file_set_string(key_file, "Colors", "input_background", input_background_color);
    g_key_file_set_string(key_file, "Colors", "input_text", input_text);
    g_key_file_set_string(key_file, "MessageColors", "user_sender", user_sender);
    g_key_file_set_string(key_file, "MessageColors", "user_message", user_message);
    g_key_file_set_string(key_file, "MessageColors", "ai_sender", ai_sender);
    g_key_file_set_string(key_file, "MessageColors", "ai_message", ai_message);
    g_key_file_set_string(key_file, "MessageColors", "system_sender", system_sender);
    g_key_file_set_string(key_file, "MessageColors", "system_info", system_info);
    g_key_file_set_string(key_file, "MessageColors", "system_warning", system_warning);
    g_key_file_set_string(key_file, "MessageColors", "system_error", system_error);
    g_key_file_set_string(key_file, "MessageColors", "system_success", system_success);

    gsize length = 0;
    gchar *data = g_key_file_to_data(key_file, &length, NULL);
    g_file_set_contents(path.c_str(), data, static_cast<gssize>(length), NULL);

    g_free(data);
    g_free(window_color);
    g_free(chat_color);
    g_free(input_background_color);
    g_free(input_text);
    g_free(user_sender);
    g_free(user_message);
    g_free(ai_sender);
    g_free(ai_message);
    g_free(system_sender);
    g_free(system_info);
    g_free(system_warning);
    g_free(system_error);
    g_free(system_success);
    g_key_file_free(key_file);
}

void apply_background_colors() {
    gchar *window_color = gdk_rgba_to_string(&window_background);
    gchar *chat_color = gdk_rgba_to_string(&chat_background);

    if (window_provider) {
        std::string css = "window { background-color: " + std::string(window_color) + "; }";
        gtk_css_provider_load_from_data(window_provider, css.c_str(), -1, NULL);
    }
    if (box_provider) {
        std::string css = "#box { background-color: " + std::string(window_color) + "; }";
        gtk_css_provider_load_from_data(box_provider, css.c_str(), -1, NULL);
    }
    if (chat_provider) {
        std::string css = "scrolledwindow, scrolledwindow viewport, scrolledwindow viewport textview, textview.view { background-color: " + std::string(chat_color) + "; color: #FFFFFF; }";
        gtk_css_provider_load_from_data(chat_provider, css.c_str(), -1, NULL);
    }
    if (chat_widget_provider) {
        std::string css = "* { background-color: " + std::string(chat_color) + "; }";
        gtk_css_provider_load_from_data(chat_widget_provider, css.c_str(), -1, NULL);
    }

    gchar *input_background_color = gdk_rgba_to_string(&input_background);
    gchar *input_text = gdk_rgba_to_string(&input_text_color);
    if (input_provider) {
        std::string css = "entry { background-color: " + std::string(input_background_color) + "; color: " + std::string(input_text) + "; caret-color: " + std::string(input_text) + "; }";
        gtk_css_provider_load_from_data(input_provider, css.c_str(), -1, NULL);
    }

    g_free(window_color);
    g_free(chat_color);
    g_free(input_background_color);
    g_free(input_text);
}

std::string ollama_url(const std::string &path) {
    std::string endpoint = OLLAMA_ENDPOINT;
    while (!endpoint.empty() && endpoint.back() == '/') {
        endpoint.pop_back();
    }
    if (endpoint.empty()) {
        endpoint = "http://localhost:11434";
    }
    return endpoint + path;
}

void on_ollama_endpoint_changed(GtkEntry *entry, gpointer user_data) {
    (void)user_data;
    const gchar *value = gtk_entry_get_text(entry);
    if (value && *value) {
        OLLAMA_ENDPOINT = value;
        while (!OLLAMA_ENDPOINT.empty() && OLLAMA_ENDPOINT.back() == '/') {
            OLLAMA_ENDPOINT.pop_back();
        }
        save_settings();
    }
}

void on_window_color_set(GtkColorButton *button, gpointer user_data) {
    (void)user_data;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(button), &window_background);
    save_settings();
    apply_background_colors();
}

void on_chat_color_set(GtkColorButton *button, gpointer user_data) {
    (void)user_data;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(button), &chat_background);
    save_settings();
    apply_background_colors();
}

void on_input_background_color_set(GtkColorButton *button, gpointer user_data) {
    (void)user_data;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(button), &input_background);
    save_settings();
    apply_background_colors();
}

void on_input_text_color_set(GtkColorButton *button, gpointer user_data) {
    (void)user_data;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(button), &input_text_color);
    save_settings();
    apply_background_colors();
}

void on_message_color_set(GtkColorButton *button, gpointer user_data) {
    auto *color = static_cast<GdkRGBA*>(user_data);
    if (color) {
        gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(button), color);
        save_settings();
    }
}

void add_color_setting(GtkWidget *settings_box, const char *label_text, GdkRGBA *color) {
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *label = gtk_label_new(label_text);
    GtkWidget *button = gtk_color_button_new_with_rgba(color);
    gtk_box_pack_start(GTK_BOX(row), label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(row), button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(settings_box), row, FALSE, FALSE, 0);
    g_signal_connect(button, "color-set", G_CALLBACK(on_message_color_set), color);
}

void show_settings_dialog(GtkWidget *parent) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Settings",
        GTK_WINDOW(parent),
        GTK_DIALOG_MODAL,
        "Close", GTK_RESPONSE_CLOSE,
        NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *settings_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(settings_box), 12);
    gtk_box_pack_start(GTK_BOX(content), settings_box, TRUE, TRUE, 0);

    GtkWidget *window_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *window_label = gtk_label_new("Window color");
    GtkWidget *window_button = gtk_color_button_new_with_rgba(&window_background);
    gtk_box_pack_start(GTK_BOX(window_row), window_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(window_row), window_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(settings_box), window_row, FALSE, FALSE, 0);
    g_signal_connect(window_button, "color-set", G_CALLBACK(on_window_color_set), NULL);

    GtkWidget *chat_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *chat_label = gtk_label_new("Chat background color");
    GtkWidget *chat_button = gtk_color_button_new_with_rgba(&chat_background);
    gtk_box_pack_start(GTK_BOX(chat_row), chat_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(chat_row), chat_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(settings_box), chat_row, FALSE, FALSE, 0);
    g_signal_connect(chat_button, "color-set", G_CALLBACK(on_chat_color_set), NULL);

    GtkWidget *input_background_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *input_background_label = gtk_label_new("Chat box color");
    GtkWidget *input_background_button = gtk_color_button_new_with_rgba(&input_background);
    gtk_box_pack_start(GTK_BOX(input_background_row), input_background_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(input_background_row), input_background_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(settings_box), input_background_row, FALSE, FALSE, 0);
    g_signal_connect(input_background_button, "color-set", G_CALLBACK(on_input_background_color_set), NULL);

    GtkWidget *input_text_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *input_text_label = gtk_label_new("Chat box text color");
    GtkWidget *input_text_button = gtk_color_button_new_with_rgba(&input_text_color);
    gtk_box_pack_start(GTK_BOX(input_text_row), input_text_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(input_text_row), input_text_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(settings_box), input_text_row, FALSE, FALSE, 0);
    g_signal_connect(input_text_button, "color-set", G_CALLBACK(on_input_text_color_set), NULL);

    add_color_setting(settings_box, "You sender color", &user_sender_color);
    add_color_setting(settings_box, "You message color", &user_message_color);
    add_color_setting(settings_box, "AI sender color", &ai_sender_color);
    add_color_setting(settings_box, "AI response color", &ai_message_color);
    add_color_setting(settings_box, "System sender color", &system_sender_color);
    add_color_setting(settings_box, "System info color", &system_info_color);
    add_color_setting(settings_box, "System warning color", &system_warning_color);
    add_color_setting(settings_box, "System error color", &system_error_color);
    add_color_setting(settings_box, "System success color", &system_success_color);

    GtkWidget *endpoint_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *endpoint_label = gtk_label_new("Ollama endpoint");
    GtkWidget *endpoint_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(endpoint_entry), OLLAMA_ENDPOINT.c_str());
    gtk_entry_set_placeholder_text(GTK_ENTRY(endpoint_entry), "http://localhost:11434");
    gtk_box_pack_start(GTK_BOX(endpoint_row), endpoint_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(endpoint_row), endpoint_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(settings_box), endpoint_row, FALSE, FALSE, 0);
    g_signal_connect(endpoint_entry, "changed", G_CALLBACK(on_ollama_endpoint_changed), NULL);

    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_settings_clicked(GtkButton *button, gpointer user_data) {
    (void)button;
    show_settings_dialog(GTK_WIDGET(user_data));
}

void stop_thinking_animation() {
    if (!thinking_active) {
        return;
    }

    thinking_active = false;
    if (thinking_timer_id != 0) {
        g_source_remove(thinking_timer_id);
        thinking_timer_id = 0;
    }

    if (thinking_indicator) {
        gtk_widget_hide(thinking_indicator);
        gtk_label_set_text(GTK_LABEL(thinking_indicator), "");
    }
}

gboolean update_thinking_animation(gpointer user_data) {
    (void)user_data;
    if (!thinking_active || !thinking_indicator) {
        return FALSE;
    }

    static const char *states[] = {"Thinking", "Thinking.", "Thinking..", "Thinking..."};
    gtk_label_set_text(GTK_LABEL(thinking_indicator), states[thinking_frame % 4]);
    thinking_frame++;
    return TRUE;
}

void start_thinking_animation() {
    if (thinking_active || !thinking_indicator) {
        return;
    }

    thinking_active = true;
    thinking_frame = 0;
    gtk_label_set_text(GTK_LABEL(thinking_indicator), "Thinking");
    gtk_widget_show(thinking_indicator);
    gtk_widget_set_visible(thinking_indicator, TRUE);
    thinking_timer_id = g_timeout_add(400, update_thinking_animation, NULL);
}

// Helper to write text into the GTK window UI safely
void scroll_chat_to_bottom() {
    if (!chat_buffer || !chat_buffer_view) {
        return;
    }

    GtkTextMark *mark = gtk_text_buffer_get_insert(chat_buffer);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(chat_buffer_view), mark, 0.0, FALSE, 0, 1.0);
}

void append_to_chat(const std::string &text) {
    if (!chat_buffer) {
        return;
    }

    GtkTextIter end;
    gtk_text_buffer_get_end_iter(chat_buffer, &end);
    gtk_text_buffer_insert(chat_buffer, &end, text.c_str(), -1);
    
    // Automatically scroll window to the absolute bottom
    scroll_chat_to_bottom();
}

void insert_markdown_span(const std::string &text, GtkTextTag *tag) {
    if (text.empty()) {
        return;
    }

    GtkTextIter start;
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(chat_buffer, &start);
    gtk_text_buffer_insert(chat_buffer, &start, text.c_str(), -1);
    gtk_text_buffer_get_end_iter(chat_buffer, &end);
    gint end_offset = gtk_text_iter_get_offset(&end);
    gint start_offset = end_offset - static_cast<gint>(g_utf8_strlen(text.c_str(), -1));
    gtk_text_buffer_get_iter_at_offset(chat_buffer, &start, start_offset);
    gtk_text_buffer_apply_tag(chat_buffer, tag, &start, &end);
}

void insert_inline_markdown(const std::string &text, double heading_scale = 1.0) {
    size_t position = 0;
    GtkTextTag *heading_tag = nullptr;
    if (heading_scale > 1.0) {
        heading_tag = gtk_text_buffer_create_tag(chat_buffer, NULL,
            "weight", PANGO_WEIGHT_BOLD,
            "scale", heading_scale,
            NULL);
    }

    while (position < text.size()) {
        size_t marker_start = std::string::npos;
        size_t marker_length = 0;
        const std::string markers[] = {"**", "__", "`", "*", "_"};
        for (const std::string &marker : markers) {
            size_t candidate = text.find(marker, position);
            if (candidate != std::string::npos && (marker_start == std::string::npos || candidate < marker_start)) {
                marker_start = candidate;
                marker_length = marker.size();
            }
        }

        if (marker_start == std::string::npos) {
            GtkTextIter end;
            gtk_text_buffer_get_end_iter(chat_buffer, &end);
            gtk_text_buffer_insert(chat_buffer, &end, text.substr(position).c_str(), -1);
            break;
        }

        if (marker_start > position) {
            GtkTextIter end;
            gtk_text_buffer_get_end_iter(chat_buffer, &end);
            gtk_text_buffer_insert(chat_buffer, &end, text.substr(position, marker_start - position).c_str(), -1);
        }

        size_t marker_end = text.find(text.substr(marker_start, marker_length), marker_start + marker_length);
        if (marker_end == std::string::npos || marker_end == marker_start + marker_length) {
            GtkTextIter end;
            gtk_text_buffer_get_end_iter(chat_buffer, &end);
            gtk_text_buffer_insert(chat_buffer, &end, text.substr(marker_start).c_str(), -1);
            break;
        }

        std::string formatted_text = text.substr(marker_start + marker_length, marker_end - marker_start - marker_length);
        GtkTextTag *tag = heading_tag;
        if (!tag) {
            if (marker_length == 1 && (text[marker_start] == '`')) {
                tag = gtk_text_buffer_create_tag(chat_buffer, NULL,
                    "family", "monospace",
                    "background", "#334155",
                    NULL);
            } else if (marker_length == 1) {
                tag = gtk_text_buffer_create_tag(chat_buffer, NULL, "style", PANGO_STYLE_ITALIC, NULL);
            } else {
                tag = gtk_text_buffer_create_tag(chat_buffer, NULL, "weight", PANGO_WEIGHT_BOLD, NULL);
            }
        }
        insert_markdown_span(formatted_text, tag);
        position = marker_end + marker_length;
    }
}

void insert_markdown_message(const std::string &message) {
    size_t line_start = 0;
    while (line_start <= message.size()) {
        size_t line_end = message.find('\n', line_start);
        if (line_end == std::string::npos) {
            line_end = message.size();
        }

        std::string line = message.substr(line_start, line_end - line_start);
        size_t content_start = line.find_first_not_of(" \t");
        if (content_start == std::string::npos) {
            content_start = line.size();
        }
        std::string content = line.substr(content_start);
        double heading_scale = 1.0;
        size_t heading_markers = 0;
        while (heading_markers < content.size() && content[heading_markers] == '#') {
            heading_markers++;
        }
        if (heading_markers > 0 && heading_markers <= 6 && heading_markers < content.size() && content[heading_markers] == ' ') {
            content = content.substr(heading_markers + 1);
            heading_scale = heading_markers == 1 ? 1.6 : heading_markers == 2 ? 1.35 : 1.15;
            GtkTextIter heading_start;
            gtk_text_buffer_get_end_iter(chat_buffer, &heading_start);
            gint heading_start_offset = gtk_text_iter_get_offset(&heading_start);
            insert_inline_markdown(content, heading_scale);
            GtkTextIter heading_end;
            gtk_text_buffer_get_end_iter(chat_buffer, &heading_end);
            GtkTextIter heading_start_iter;
            gtk_text_buffer_get_iter_at_offset(chat_buffer, &heading_start_iter, heading_start_offset);
            GtkTextTag *heading_tag = gtk_text_buffer_create_tag(chat_buffer, NULL,
                "weight", PANGO_WEIGHT_BOLD,
                "scale", heading_scale,
                NULL);
            gtk_text_buffer_apply_tag(chat_buffer, heading_tag, &heading_start_iter, &heading_end);
        } else {
            bool unordered = content.size() >= 2 && (content[0] == '*' || content[0] == '-' || content[0] == '+') && content[1] == ' ';
            if (unordered) {
                GtkTextIter end;
                gtk_text_buffer_get_end_iter(chat_buffer, &end);
                gtk_text_buffer_insert(chat_buffer, &end, "• ", -1);
                insert_inline_markdown(content.substr(2));
            } else {
                size_t number_end = 0;
                while (number_end < content.size() && content[number_end] >= '0' && content[number_end] <= '9') {
                    number_end++;
                }
                bool ordered = number_end > 0 && number_end + 1 < content.size() && content[number_end] == '.' && content[number_end + 1] == ' ';
                if (ordered) {
                    insert_inline_markdown(content.substr(0, number_end + 2));
                    insert_inline_markdown(content.substr(number_end + 2));
                } else {
                    insert_inline_markdown(line);
                }
            }
        }

        if (line_end < message.size()) {
            GtkTextIter end;
            gtk_text_buffer_get_end_iter(chat_buffer, &end);
            gtk_text_buffer_insert(chat_buffer, &end, "\n", 1);
        }
        if (line_end == message.size()) {
            break;
        }
        line_start = line_end + 1;
    }
}

void append_colored_message(const std::string &prefix, const std::string &message, const std::string &prefix_color, const std::string &message_color, bool add_blank_line = true, bool add_trailing_newline = true, const std::string &message_background_color = "", bool render_markdown = false) {
    if (!chat_buffer) {
        return;
    }

    GtkTextIter end;
    gtk_text_buffer_get_end_iter(chat_buffer, &end);

    if (add_blank_line) {
        gtk_text_buffer_insert(chat_buffer, &end, "\n", 1);
        gtk_text_buffer_get_end_iter(chat_buffer, &end);
    }

    gint prefix_start_offset = gtk_text_iter_get_offset(&end);
    gtk_text_buffer_insert(chat_buffer, &end, prefix.c_str(), -1);
    GtkTextIter prefix_end;
    gtk_text_buffer_get_end_iter(chat_buffer, &prefix_end);
    GtkTextIter prefix_start;
    gtk_text_buffer_get_iter_at_offset(chat_buffer, &prefix_start, prefix_start_offset);

    GtkTextTag *prefix_tag = gtk_text_buffer_create_tag(chat_buffer, NULL, "foreground", prefix_color.c_str(), NULL);
    gtk_text_buffer_apply_tag(chat_buffer, prefix_tag, &prefix_start, &prefix_end);

    gtk_text_buffer_get_end_iter(chat_buffer, &end);
    gtk_text_buffer_insert(chat_buffer, &end, ": ", 2);

    GtkTextIter msg_start;
    gtk_text_buffer_get_end_iter(chat_buffer, &msg_start);
    gint message_start_offset = gtk_text_iter_get_offset(&msg_start);
    gtk_text_buffer_get_end_iter(chat_buffer, &end);

    if (render_markdown) {
        insert_markdown_message(message);
    } else {
        gtk_text_buffer_insert(chat_buffer, &end, message.c_str(), -1);
    }

    GtkTextIter msg_end;
    gtk_text_buffer_get_end_iter(chat_buffer, &msg_end);
    gtk_text_buffer_get_iter_at_offset(chat_buffer, &msg_start, message_start_offset);
    GtkTextTag *message_tag;
    if (message_background_color.empty()) {
        message_tag = gtk_text_buffer_create_tag(chat_buffer, NULL, "foreground", message_color.c_str(), NULL);
    } else {
        message_tag = gtk_text_buffer_create_tag(chat_buffer, NULL,
            "foreground", message_color.c_str(),
            "background", message_background_color.c_str(),
            NULL);
    }
    gtk_text_buffer_apply_tag(chat_buffer, message_tag, &msg_start, &msg_end);

    if (add_trailing_newline) {
        gtk_text_buffer_get_end_iter(chat_buffer, &end);
        gtk_text_buffer_insert(chat_buffer, &end, "\n", 1);
    }

    scroll_chat_to_bottom();
    while (gtk_events_pending()) gtk_main_iteration();
}

void append_system_message(const std::string &message, const std::string &tone);
void append_user_message(const std::string &message);
void append_ai_message(const std::string &response);

std::string rgba_to_string(const GdkRGBA &color) {
    gchar *value = gdk_rgba_to_string(&color);
    std::string result = value ? value : "#FFFFFF";
    g_free(value);
    return result;
}

void append_ai_response_chunk(const std::string &response) {
    if (!chat_buffer) {
        return;
    }

    GtkTextIter end;
    gtk_text_buffer_get_end_iter(chat_buffer, &end);
    gtk_text_buffer_insert(chat_buffer, &end, response.c_str(), -1);

    GtkTextIter start;
    gtk_text_buffer_get_end_iter(chat_buffer, &start);
    gint response_start_offset = gtk_text_iter_get_offset(&start) - static_cast<gint>(g_utf8_strlen(response.c_str(), -1));
    gtk_text_buffer_get_iter_at_offset(chat_buffer, &start, response_start_offset);
    gtk_text_buffer_get_end_iter(chat_buffer, &end);

    GtkTextTag *message_tag = gtk_text_buffer_create_tag(chat_buffer, NULL, "foreground", "#22c55e", NULL);
    gtk_text_buffer_apply_tag(chat_buffer, message_tag, &start, &end);
    scroll_chat_to_bottom();
}

struct CurlStringBuffer {
    std::string data;
};

static size_t curl_write_string(void *contents, size_t size, size_t nmemb, void *userp);

struct AIRequestData {
    std::string model;
    std::string prompt;
};

gboolean handle_ai_message_ready(gpointer user_data) {
    auto *message = static_cast<std::string*>(user_data);
    if (!message) {
        return G_SOURCE_REMOVE;
    }

    if (!ai_response_started) {
        append_ai_message(*message);
        ai_response_started = true;
    } else {
        append_ai_response_chunk(*message);
    }

    delete message;
    return G_SOURCE_REMOVE;
}

gboolean handle_ai_response_finished(gpointer user_data) {
    (void)user_data;
    stop_thinking_animation();
    append_to_chat("\n");
    return G_SOURCE_REMOVE;
}

static void queue_ollama_response_chunks(const std::string &raw) {
    std::string assembled;
    size_t search_pos = 0;
    while ((search_pos = raw.find("\"response\":\"", search_pos)) != std::string::npos) {
        size_t value_start = search_pos + 12;
        std::string token;
        bool escape_next = false;

        for (size_t i = value_start; i < raw.size(); ++i) {
            char ch = raw[i];
            if (escape_next) {
                switch (ch) {
                    case 'n': token += '\n'; break;
                    case 'r': token += '\r'; break;
                    case 't': token += '\t'; break;
                    case '\\': token += '\\'; break;
                    case '"': token += '"'; break;
                    case '/': token += '/'; break;
                    default: token += ch; break;
                }
                escape_next = false;
                continue;
            }

            if (ch == '\\') {
                escape_next = true;
                continue;
            }

            if (ch == '"') {
                break;
            }

            token += ch;
        }

        if (!token.empty()) {
            assembled += token;
        }

        search_pos = value_start + 1;
    }

    if (!assembled.empty()) {
        g_idle_add(handle_ai_message_ready, new std::string(assembled));
    }
}

static gpointer ai_request_worker(gpointer user_data) {
    auto *request = static_cast<AIRequestData*>(user_data);
    if (!request) {
        return nullptr;
    }

    CURL *curl = curl_easy_init();
    if (curl) {
        std::string url = ollama_url("/api/generate");
        std::string json_payload = "{\"model\":\"" + request->model + "\",\"prompt\":\"" + request->prompt + "\"}";

        CurlStringBuffer buffer;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_string);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            queue_ollama_response_chunks(buffer.data);
        }
    }

    g_idle_add(handle_ai_response_finished, nullptr);
    delete request;
    return nullptr;
}

// Callback execution function for handling incoming live curl streaming chunks
size_t curl_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    (void)userp;
    size_t total_size = size * nmemb;
    std::string data((char*)contents, total_size);

    size_t search_pos = 0;
    while ((search_pos = data.find("\"response\":\"", search_pos)) != std::string::npos) {
        size_t value_start = search_pos + 12;
        std::string token;
        bool escape_next = false;

        for (size_t i = value_start; i < data.size(); ++i) {
            char ch = data[i];

            if (escape_next) {
                switch (ch) {
                    case 'n': token += '\n'; break;
                    case 'r': token += '\r'; break;
                    case 't': token += '\t'; break;
                    case '\\': token += '\\'; break;
                    case '"': token += '"'; break;
                    case '/': token += '/'; break;
                    default: token += ch; break;
                }
                escape_next = false;
                continue;
            }

            if (ch == '\\') {
                escape_next = true;
                continue;
            }

            if (ch == '"') {
                break;
            }

            token += ch;
        }

        if (!token.empty()) {
            if (!ai_response_started) {
                stop_thinking_animation();
                append_ai_message(token);
                ai_response_started = true;
            } else {
                append_ai_response_chunk(token);
            }
        }

        search_pos = value_start + 1;
    }

    // Keep processing GUI cycles during intense system math to prevent window freezing
    while (gtk_events_pending()) gtk_main_iteration();
    return total_size;
}

static size_t curl_write_string(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    auto *buffer = static_cast<CurlStringBuffer*>(userp);
    buffer->data.append(static_cast<char*>(contents), total_size);
    return total_size;
}

static size_t curl_discard_output(void *contents, size_t size, size_t nmemb, void *userp) {
    (void)contents;
    (void)userp;
    return size * nmemb;
}

void update_status_label() {
    if (model_status_label) {
        std::string label = "Current model: " + MODEL_NAME;
        gtk_label_set_text(GTK_LABEL(model_status_label), label.c_str());
    }
}

void append_system_message(const std::string &message, const std::string &tone = "info") {
    std::string prefix = "System";
    std::string prefix_color = rgba_to_string(system_sender_color);
    std::string message_color = rgba_to_string(system_info_color);

    if (tone == "error") {
        message_color = rgba_to_string(system_error_color);
    } else if (tone == "warning") {
        message_color = rgba_to_string(system_warning_color);
    } else if (tone == "success") {
        message_color = rgba_to_string(system_success_color);
    }

    append_colored_message(prefix, message, prefix_color, message_color, true, true, "#ffffff");
}

void append_user_message(const std::string &message) {
    append_colored_message("You", message, rgba_to_string(user_sender_color), rgba_to_string(user_message_color));
}

void append_ai_message(const std::string &response) {
    append_colored_message(MODEL_NAME, response, rgba_to_string(ai_sender_color), rgba_to_string(ai_message_color), true, false, "", true);
}

bool is_ollama_running() {
    CURL *curl = curl_easy_init();
    if (!curl) {
        return false;
    }

    std::string url = ollama_url("/api/tags");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_discard_output);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, nullptr);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res == CURLE_OK;
}

gboolean model_ready_callback(gpointer user_data) {
    (void)user_data;
    if (model_combo) {
        gtk_widget_set_sensitive(model_combo, TRUE);
    }
    if (entry_input) {
        gtk_widget_set_sensitive(entry_input, TRUE);
    }
    append_system_message("Model ready: " + MODEL_NAME, "success");
    return FALSE;
}

void unload_current_model() {
    if (MODEL_NAME.empty() || MODEL_NAME == "None") {
        return;
    }

    if (!is_ollama_running()) {
        MODEL_NAME = "None";
        update_status_label();
        return;
    }

    CURL *curl = curl_easy_init();
    if (!curl) {
        MODEL_NAME = "None";
        update_status_label();
        return;
    }

    std::string url = ollama_url("/api/stop");
    std::string json_payload = "{\"name\":\"" + MODEL_NAME + "\"}";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    MODEL_NAME = "None";
    update_status_label();
    append_system_message("Model unloaded.", "warning");
}

void on_set_model() {
    if (!model_combo || !entry_input) {
        return;
    }

    const gchar *selected = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(model_combo));
    if (!selected || std::string(selected).empty()) {
        return;
    }

    std::string selected_model = selected;
    if (selected_model == "None") {
        unload_current_model();
        if (entry_input) {
            gtk_widget_set_sensitive(entry_input, FALSE);
        }
        return;
    }

    MODEL_NAME = selected_model;
    update_status_label();
    append_system_message("Switching model to " + MODEL_NAME + "...", "info");
    gtk_widget_set_sensitive(entry_input, FALSE);
    gtk_widget_set_sensitive(model_combo, FALSE);
    g_timeout_add(1200, model_ready_callback, nullptr);
}

std::vector<std::string> get_installed_models() {
    std::vector<std::string> models;
    if (!is_ollama_running()) {
        return models;
    }

    CURL *curl = curl_easy_init();
    if (!curl) {
        return models;
    }

    CurlStringBuffer buffer;
    std::string url = ollama_url("/api/tags");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_string);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        return models;
    }

    std::string data = buffer.data;
    size_t pos = 0;
    while ((pos = data.find("\"name\":\"", pos)) != std::string::npos) {
        pos += 8;
        size_t end = data.find("\"", pos);
        if (end == std::string::npos) break;

        std::string model = data.substr(pos, end - pos);
        if (!model.empty() && std::find(models.begin(), models.end(), model) == models.end()) {
            models.push_back(model);
        }
        pos = end + 1;
    }

    return models;
}

void populate_model_combo() {
    if (!model_combo) {
        return;
    }

    model_combo_initializing = true;

    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(model_combo));
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(model_combo), "None");

    std::vector<std::string> models = get_installed_models();
    if (models.empty()) {
        gtk_widget_set_sensitive(model_combo, TRUE);
        if (MODEL_NAME.empty() || MODEL_NAME == "None") {
            gtk_combo_box_set_active(GTK_COMBO_BOX(model_combo), 0);
        }
        update_status_label();
        return;
    }

    gtk_widget_set_sensitive(model_combo, TRUE);
    for (const auto &model : models) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(model_combo), model.c_str());
    }

    if (MODEL_NAME == "None") {
        gtk_combo_box_set_active(GTK_COMBO_BOX(model_combo), 0);
    } else {
        auto it = std::find(models.begin(), models.end(), MODEL_NAME);
        if (it == models.end()) {
            MODEL_NAME = "None";
        }
    }

    for (int i = 0; i < gtk_tree_model_iter_n_children(gtk_combo_box_get_model(GTK_COMBO_BOX(model_combo)), NULL); ++i) {
        GtkTreeIter iter;
        gtk_tree_model_iter_nth_child(gtk_combo_box_get_model(GTK_COMBO_BOX(model_combo)), &iter, NULL, i);
        gchar *text = NULL;
        gtk_tree_model_get(gtk_combo_box_get_model(GTK_COMBO_BOX(model_combo)), &iter, 0, &text, -1);
        if (text && MODEL_NAME == text) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(model_combo), i);
            g_free(text);
            break;
        }
        g_free(text);
    }

    model_combo_initializing = false;
    update_status_label();
}

void refresh_service_switch() {
    if (!service_switch || !model_combo) {
        return;
    }

    bool running = is_ollama_running();
    gtk_switch_set_active(GTK_SWITCH(service_switch), running);
    gtk_widget_set_sensitive(service_switch, TRUE);

    if (running) {
        gtk_widget_set_sensitive(model_combo, TRUE);
        populate_model_combo();
        append_system_message("Ollama is running.", "success");
    } else {
        gtk_widget_set_sensitive(model_combo, FALSE);
        gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(model_combo));
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(model_combo), "Start Ollama to load models");
        append_system_message("Ollama is stopped.", "warning");
    }
}

gboolean refresh_service_switch_delayed(gpointer data) {
    bool *target_running = static_cast<bool*>(data);
    bool running = is_ollama_running();

    if (running == *target_running) {
        refresh_service_switch();
        return FALSE;
    }

    g_timeout_add(1000, refresh_service_switch_delayed, data);
    return FALSE;
}

void on_toggle_ollama_service(GtkSwitch *toggle_switch, gboolean state, gpointer user_data) {
    (void)user_data;
    GError *error = NULL;
    bool running = is_ollama_running();
    bool desired_running = state;

    if (service_switch) {
        gtk_widget_set_sensitive(service_switch, FALSE);
    }

    if (running == desired_running) {
        if (service_switch) {
            gtk_widget_set_sensitive(service_switch, TRUE);
        }
        return;
    }

    if (running) {
        if (!g_spawn_command_line_async("systemctl stop ollama", &error)) {
            append_system_message("Failed to stop Ollama: " + std::string(error->message), "error");
            g_error_free(error);
            if (service_switch) {
                gtk_widget_set_sensitive(service_switch, TRUE);
            }
            gtk_switch_set_active(toggle_switch, TRUE);
            return;
        }

        append_system_message("Stopping Ollama service in the background...", "warning");
        bool *stop_target = new bool(false);
        g_timeout_add(1000, refresh_service_switch_delayed, stop_target);
        return;
    }

    if (!g_spawn_command_line_async("systemctl start ollama", &error)) {
        append_system_message("Failed to start Ollama: " + std::string(error->message), "error");
        g_error_free(error);
        gtk_widget_set_sensitive(service_switch, TRUE);
        gtk_switch_set_active(toggle_switch, FALSE);
        return;
    }

    append_system_message("Starting Ollama service in the background...", "info");
    bool *start_target = new bool(true);
    g_timeout_add(1000, refresh_service_switch_delayed, start_target);
}

void on_model_changed(GtkComboBox *combo, gpointer user_data) {
    (void)user_data;
    if (model_combo_initializing) {
        return;
    }

    const gchar *selected = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
    if (!selected || std::string(selected).empty()) {
        return;
    }

    std::string selected_model = selected;
    if (selected_model == "None") {
        MODEL_NAME = "None";
        update_status_label();
        on_set_model();
        return;
    }

    MODEL_NAME = selected_model;
    update_status_label();
    on_set_model();
}

// Event triggered immediately when user types their query and hits Enter
void on_send_message(GtkEntry *entry, gpointer user_data) {
    (void)user_data;
    std::string text = gtk_entry_get_text(entry);
    if (text.empty()) return;

    if (!is_ollama_running()) {
        append_system_message("Ollama is not running. Please switch it on in the chat window and try again.", "error");
        return;
    }

    if (MODEL_NAME.empty() || MODEL_NAME == "None") {
        append_system_message("No model loaded. Please select a model from the dropdown.", "warning");
        return;
    }

    // Clear user typing box and update history panel immediately
    gtk_entry_set_text(entry, "");
    append_user_message(text);
    ai_response_started = false;
    start_thinking_animation();

    auto *request = new AIRequestData{MODEL_NAME, text};
    g_thread_new("ai-request", ai_request_worker, request);
}

#if 0
int main(int argc, char *argv[]) {
    curl_global_init(CURL_GLOBAL_ALL);
    gtk_init(&argc, &argv);
    load_settings();

    // 1. Configure the core structural popup layout
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "AI-Chat-Local");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 550);

    window_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(window_provider, "window { background-color: #010736; }", -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(window_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Set absolute window priority to float above all standard desktop environments
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
    gtk_style_context_add_provider(gtk_widget_get_style_context(chat_scrolled_window),
        GTK_STYLE_PROVIDER(chat_widget_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(gtk_widget_get_style_context(chat_viewport),
        GTK_STYLE_PROVIDER(chat_widget_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_provider(gtk_widget_get_style_context(chat_buffer_view),
        GTK_STYLE_PROVIDER(chat_widget_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
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
    gtk_css_provider_load_from_data(input_provider, "entry { background-color: #22396F; color: #FFFFFF; caret-color: #FFFFFF; }", -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(input_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_entry_set_cursor_visible(GTK_ENTRY(entry_input), TRUE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_input), "Type your message...");
    gtk_box_pack_end(GTK_BOX(box), entry_input, FALSE, FALSE, 0);
    g_signal_connect(entry_input, "activate", G_CALLBACK(on_send_message), NULL);
    apply_background_colors();

    if (is_ollama_running()) {
        append_system_message("Native C++ application loaded. Ollama is running.", "success");
    } else {
        append_system_message("Ollama is not running. Please switch it on in the chat window.", "error");
    }

    // Display components and hand control off to native UI thread loops
    gtk_widget_show_all(window);
    gtk_widget_grab_focus(entry_input);
    gtk_main();

    curl_global_cleanup();
    return 0;
}
#endif

#pragma once

#include <gtk/gtk.h>
#include <string>

extern std::string MODEL_NAME;
extern std::string OLLAMA_ENDPOINT;

extern GtkWidget *chat_buffer_view;
extern GtkTextBuffer *chat_buffer;
extern GtkWidget *entry_input;
extern GtkWidget *model_combo;
extern GtkWidget *service_switch;
extern GtkWidget *model_status_label;
extern GtkWidget *thinking_indicator;
extern GtkWidget *chat_scrolled_window;
extern GtkWidget *chat_viewport;
extern GtkCssProvider *window_provider;
extern GtkCssProvider *box_provider;
extern GtkCssProvider *chat_provider;
extern GtkCssProvider *chat_widget_provider;
extern GtkCssProvider *input_provider;

extern bool ai_response_started;
extern bool thinking_active;
extern bool model_combo_initializing;
extern guint thinking_timer_id;
extern int thinking_frame;

extern GdkRGBA window_background;
extern GdkRGBA chat_background;
extern GdkRGBA input_background;
extern GdkRGBA input_text_color;
extern GdkRGBA user_sender_color;
extern GdkRGBA user_message_color;
extern GdkRGBA ai_sender_color;
extern GdkRGBA ai_message_color;
extern GdkRGBA system_sender_color;
extern GdkRGBA system_info_color;
extern GdkRGBA system_warning_color;
extern GdkRGBA system_error_color;
extern GdkRGBA system_success_color;

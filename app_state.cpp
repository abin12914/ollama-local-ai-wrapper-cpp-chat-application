#include "app_state.h"

std::string MODEL_NAME = "llama3.2:1b";
std::string OLLAMA_ENDPOINT = "http://localhost:11434";
std::vector<ConversationMessage> conversation_history;
std::string current_ai_response;
int conversation_context_limit = 20;

GtkWidget *chat_buffer_view = nullptr;
GtkTextBuffer *chat_buffer = nullptr;
GtkWidget *entry_input = nullptr;
GtkWidget *model_combo = nullptr;
GtkWidget *service_switch = nullptr;
GtkWidget *model_status_label = nullptr;
GtkWidget *thinking_indicator = nullptr;
GtkWidget *chat_scrolled_window = nullptr;
GtkWidget *chat_viewport = nullptr;
GtkCssProvider *window_provider = nullptr;
GtkCssProvider *box_provider = nullptr;
GtkCssProvider *chat_provider = nullptr;
GtkCssProvider *chat_widget_provider = nullptr;
GtkCssProvider *input_provider = nullptr;

bool ai_response_started = false;
bool thinking_active = false;
bool model_combo_initializing = false;
guint thinking_timer_id = 0;
int thinking_frame = 0;

GdkRGBA window_background = { 0.0039215686, 0.0274509804, 0.2117647059, 1.0 };
GdkRGBA chat_background = { 0.0509803922, 0.1098039216, 0.2588235294, 1.0 };
GdkRGBA input_background = { 0.1333333333, 0.2235294118, 0.4352941176, 1.0 };
GdkRGBA input_text_color = { 1.0, 1.0, 1.0, 1.0 };
GdkRGBA user_sender_color = { 1.0, 1.0, 1.0, 1.0 };
GdkRGBA user_message_color = { 0.3764705882, 0.6470588235, 0.9803921569, 1.0 };
GdkRGBA ai_sender_color = { 1.0, 1.0, 1.0, 1.0 };
GdkRGBA ai_message_color = { 0.1333333333, 0.7725490196, 0.3647058824, 1.0 };
GdkRGBA system_sender_color = { 1.0, 1.0, 1.0, 1.0 };
GdkRGBA system_info_color = { 0.231372549, 0.5098039216, 0.9647058824, 1.0 };
GdkRGBA system_warning_color = { 0.9607843137, 0.6196078431, 0.0431372549, 1.0 };
GdkRGBA system_error_color = { 0.937254902, 0.2666666667, 0.2666666667, 1.0 };
GdkRGBA system_success_color = { 0.1333333333, 0.7725490196, 0.3647058824, 1.0 };

#pragma once

#include "app_state.h"
#include <curl/curl.h>

void load_settings();
std::string ollama_url(const std::string &path);
void apply_background_colors();
void update_status_label();
void refresh_service_switch();
bool is_ollama_running();
void append_system_message(const std::string &message, const std::string &tone = "info");
void on_model_changed(GtkComboBox *combo, gpointer user_data);
void on_toggle_ollama_service(GtkSwitch *toggle_switch, gboolean state, gpointer user_data);
void on_settings_clicked(GtkButton *button, gpointer user_data);
void on_send_message(GtkEntry *entry, gpointer user_data);

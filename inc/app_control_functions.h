/*
Copyright (C) 2017  Henrik Ullman
License: GPL Version 3
*/
#ifndef __APP_CONTROL_FUNCTIONS_H__
#define __APP_CONTROL_FUNCTIONS_H__
#ifdef TIZEN

void win_delete_request_cb (void *data, Evas_Object * obj, void *event_info);

void layout_back_cb (void *data, Evas_Object * obj, void *event_info);

void
app_get_resource (const char *edj_file_in, char *edj_path_out,
		  int edj_path_max);

bool app_create (void *data);

void app_control (app_control_h app_control, void *data);

void app_pause (void *data);

void app_resume (void *data);

void app_terminate (void *data);

void ui_app_lang_changed (app_event_info_h event_info, void *user_data);

void ui_app_orient_changed (app_event_info_h event_info, void *user_data);

void ui_app_region_changed (app_event_info_h event_info, void *user_data);

void ui_app_low_battery (app_event_info_h event_info, void *user_data);

void ui_app_low_memory (app_event_info_h event_info, void *user_data);

#endif


#endif /* __APP_CONTROL_FUNCTIONS_H__ */

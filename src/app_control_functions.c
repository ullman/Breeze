/*
Copyright (C) 2017  Henrik Ullman
License: GPL Version 3
*/
#include "breeze.h"
#include "app_control_functions.h"


void
win_delete_request_cb (void *data, Evas_Object * obj, void *event_info)
{
  ui_app_exit ();
}

void
layout_back_cb (void *data, Evas_Object * obj, void *event_info)
{
  appdata_s *ad = data;
  /* Let window go to hide state. */
  elm_win_lower (ad->win);
}

void
app_get_resource (const char *edj_file_in, char *edj_path_out,
		  int edj_path_max)
{
  char *res_path = app_get_resource_path ();
  if (res_path)
    {
      snprintf (edj_path_out, edj_path_max, "%s%s", res_path, edj_file_in);
      free (res_path);
    }
}

bool
app_create (void *data)
{
  appdata_s *ad = data;

  create_base_gui (ad);

  return true;
}

void
app_control (app_control_h app_control, void *data)
{
  /* Handle the launch request. */
}

void
app_pause (void *data)
{
  /* Take necessary actions when application becomes invisible. */
}

void
app_resume (void *data)
{
  /* Take necessary actions when application becomes visible. */
}

void
app_terminate (void *data)
{
  /* Release all resources. */
}

void
ui_app_lang_changed (app_event_info_h event_info, void *user_data)
{
  /*APP_EVENT_LANGUAGE_CHANGED */

  int ret;
  char *language;

  ret = app_event_get_language (event_info, &language);
  if (ret != APP_ERROR_NONE)
    {
      dlog_print (DLOG_ERROR, LOG_TAG,
		  "app_event_get_language() failed. Err = %d.", ret);
      return;
    }

  if (language != NULL)
    {
      elm_language_set (language);
      free (language);
    }
}

void
ui_app_orient_changed (app_event_info_h event_info, void *user_data)
{
  /*APP_EVENT_DEVICE_ORIENTATION_CHANGED */
  return;
}

void
ui_app_region_changed (app_event_info_h event_info, void *user_data)
{
  /*APP_EVENT_REGION_FORMAT_CHANGED */
}

void
ui_app_low_battery (app_event_info_h event_info, void *user_data)
{
  /*APP_EVENT_LOW_BATTERY */
}

void
ui_app_low_memory (app_event_info_h event_info, void *user_data)
{
  /*APP_EVENT_LOW_MEMORY */
}

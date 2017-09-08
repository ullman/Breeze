/*
Copyright (C) 2017  Henrik Ullman
License: GPL Version 3
*/
#ifdef TIZEN
#include <efl_extension.h>
#include <app_common.h>
#endif

#ifndef TIZEN
#include <EWebKit2.h>
#endif

#include <glib.h>

#include "breeze.h"
#include "app_control_functions.h"
#include "rss_functions.h"
#include "cb_functions.h"
#include "mrss.h"
#include "database.h"


int
exists (const char *fname)
{
  FILE *file;
  if ((file = fopen (fname, "r")))
    {
      fclose (file);
      return 1;
    }
  return 0;
}


char *
item_label_get (void *data, Evas_Object * obj, const char *part)
{

  mrss_item_t *this_item;
  this_item = data;

  if (!strcmp (part, "elm.text"))
    {
      if (this_item->title)
	      {

          return strdup (elm_entry_utf8_to_markup (this_item->title));//TODO free this somewhere
        }
      else
      return strdup ("error: no title");
    }
  else if (!strcmp (part, "elm.text.sub"))
    {
      if (this_item->pubDate)
	{
	  return strdup (this_item->pubDate);
	}
      else
	return strdup ("No date");

    }
  else if (!strcmp (part, "elm.text.end"))
    {
      if (this_item->source)
	{
	  return strdup (this_item->source);
	}
      else
	return strdup ("No rss name");
    }
  else
    return NULL;
}


char *
feed_label_get (void *data, Evas_Object * obj, const char *part)
{
  feed_s *this_feed;
  this_feed = data;
  if (!strcmp (part, "elm.text"))
    {
      return strdup (this_feed->name);
    }
  else
    return NULL;
}

void
create_base_gui (appdata_s * ad)
{

  ad->rss_items = NULL;
  ad->feeds = NULL;
  Evas_Object *bg;
  Elm_Object_Item *nf_it;
  char *data_folder;



  /* Window */
  ad->win = elm_win_util_standard_add (PACKAGE, PACKAGE);
  elm_win_conformant_set (ad->win, EINA_TRUE);
  elm_win_autodel_set (ad->win, EINA_TRUE);
  evas_object_resize (ad->win, 300, 400);

  if (elm_win_wm_rotation_supported_get (ad->win))
    {
      int rots[4] = { 0, 90, 180, 270 };
      elm_win_wm_rotation_available_rotations_set (ad->win,
						   (const int *) (&rots), 4);
    }

#ifdef TIZEN
  evas_object_smart_callback_add (ad->win, "delete,request",
				  win_delete_request_cb, NULL);
#endif

  /* Conformant */
  ad->conform = elm_conformant_add (ad->win);
  elm_win_indicator_mode_set (ad->win, ELM_WIN_INDICATOR_SHOW);
  elm_win_indicator_opacity_set (ad->win, ELM_WIN_INDICATOR_OPAQUE);
  evas_object_size_hint_weight_set (ad->conform, EVAS_HINT_EXPAND,
				    EVAS_HINT_EXPAND);
  elm_win_resize_object_add (ad->win, ad->conform);


  /* Base Layout */

  /*create naviframe */
  ad->nf = elm_naviframe_add (ad->conform);
  elm_naviframe_content_preserve_on_pop_set (ad->nf, EINA_TRUE);	// crash if not set due to ewebkit memory being freed
#ifdef TIZEN
  eext_object_event_callback_add (ad->nf, EEXT_CALLBACK_BACK,
				  cb_button_back_clicked, ad);
//                                        eext_naviframe_back_cb, ad);
#endif
  evas_object_size_hint_weight_set (ad->nf, EVAS_HINT_EXPAND,
				    EVAS_HINT_EXPAND);
  elm_object_content_set (ad->conform, ad->nf);

  /*add item style */
  ad->itc = elm_genlist_item_class_new ();
#ifdef TIZEN
  ad->itc->item_style = "type1";
#else
  ad->itc->item_style = "double_label";
#endif
    ad->itc->func.text_get = item_label_get;
  ad->itc->func.content_get = NULL;
  ad->itc->func.state_get = NULL;
  ad->itc->func.del = NULL;

  /*add item style for feeds */
  ad->itc_feeds = elm_genlist_item_class_new ();
  ad->itc_feeds->item_style = "default";
  ad->itc_feeds->func.text_get = feed_label_get;
  ad->itc_feeds->func.content_get = NULL;
  ad->itc_feeds->func.state_get = NULL;
  ad->itc_feeds->func.del = NULL;

  /*background */
  bg = elm_bg_add (ad->nf);
  elm_bg_color_set (bg, 230, 230, 230);

  /*create genlist */
  ad->item_list = elm_genlist_add (bg);
  elm_object_style_set (ad->item_list, "solid/default");
  elm_genlist_mode_set (ad->item_list, ELM_LIST_COMPRESS);
  elm_genlist_highlight_mode_set (ad->item_list, EINA_TRUE);
  elm_object_part_content_set (bg, "overlay", ad->item_list);



  /*update button */
  ad->button_update = elm_button_add (ad->nf);
  elm_object_style_set (ad->button_update, "naviframe/title_left");
  elm_object_text_set (ad->button_update, "Update");

  /*options button */
  ad->button_options = elm_button_add (ad->nf);
  elm_object_style_set (ad->button_options, "naviframe/title_left");
  elm_object_text_set (ad->button_options, "Feeds");

  evas_object_smart_callback_add (ad->button_update, "clicked",
				  cb_button_update_clicked, ad);
  evas_object_smart_callback_add (ad->button_options, "clicked",
				  cb_button_options_clicked, ad);


  /* create rss list page in naviframe */
  nf_it =
    elm_naviframe_item_push (ad->nf, "Feed Items", ad->button_options,
			     ad->button_update, bg, NULL);

#ifdef TIZEN
  elm_object_part_content_unset (ad->nf, "title_right_btn");
  elm_object_part_content_unset (ad->nf, "title_left_btn");
  elm_object_part_content_set (ad->nf, "title_right_btn", ad->button_update);
  elm_object_part_content_set (ad->nf, "title_left_btn", ad->button_options);
#endif
  /* create database if not exists */

#ifdef TIZEN
  data_folder = app_get_data_path ();
#else
  data_folder = "./";
#endif
  strcpy (ad->database, data_folder);
  strcat (ad->database, "breeze.db");
  //dlog_print (DLOG_DEBUG, LOG_TAG, ad->database);
  if (!exists (ad->database))
    {
      //dlog_print (DLOG_DEBUG, LOG_TAG, "database does not exist");
      printf ("database does not exist\n");
      database_create (ad->database);
    }
  else
    {
      //dlog_print (DLOG_DEBUG, LOG_TAG, "database exists");
      /*load feeds from database */
      printf ("database does exist\n");
      database_load_feeds (ad->database, ad);
    }

  elm_naviframe_item_pop_cb_set (nf_it, naviframe_pop_cb, ad);
  /*popup menu */
#ifdef TIZEN
  ecore_event_handler_add (ECORE_EVENT_KEY_DOWN, cb_hardware_key, ad);
  eext_win_keygrab_set (ad->win, "XF86Menu");
#endif

  /*initiallize webview */
#ifndef TIZEN			//TODO try to run on tizen
  ewk_init ();
#endif


  /* Show window after base gui is set up */
  evas_object_show (ad->button_update);
  evas_object_show (ad->item_list);
  evas_object_show (ad->button_options);
  evas_object_show (ad->conform);
  evas_object_show (bg);
  evas_object_show (ad->nf);
  evas_object_show (ad->win);


  //dlog_print (DLOG_DEBUG, LOG_TAG, "ut is setup");

}



#ifdef TIZEN
int
main (int argc, char *argv[])
{
  appdata_s ad = { 0, };
  int ret = 0;

  ui_app_lifecycle_callback_s event_callback = { 0, };
  app_event_handler_h handlers[5] = { NULL, };

  event_callback.create = app_create;
  event_callback.terminate = app_terminate;
  event_callback.pause = app_pause;
  event_callback.resume = app_resume;
  event_callback.app_control = app_control;

  ui_app_add_event_handler (&handlers[APP_EVENT_LOW_BATTERY],
			    APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
  ui_app_add_event_handler (&handlers[APP_EVENT_LOW_MEMORY],
			    APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
  ui_app_add_event_handler (&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			    APP_EVENT_DEVICE_ORIENTATION_CHANGED,
			    ui_app_orient_changed, &ad);
  ui_app_add_event_handler (&handlers[APP_EVENT_LANGUAGE_CHANGED],
			    APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed,
			    &ad);
  ui_app_add_event_handler (&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			    APP_EVENT_REGION_FORMAT_CHANGED,
			    ui_app_region_changed, &ad);

  ret = ui_app_main (argc, argv, &event_callback, &ad);
  if (ret != APP_ERROR_NONE)
    {
      dlog_print (DLOG_ERROR, LOG_TAG, "ui_app_main() is failed. err = %d",
		  ret);
    }

  return ret;
}
#else

  //TODO write new main function here


EAPI_MAIN int
elm_main (int argc, char **argv)
{
  //Evas_Object *win, *btn;

  appdata_s ad = { 0, };
  appdata_s *ad_real;
  ad_real = &ad;
  elm_policy_set (ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

  //win = elm_win_util_standard_add("Main", "Hello, World!");
  //elm_win_autodel_set(win, EINA_TRUE);
  //
  //btn = elm_button_add(win);
  //elm_object_text_set(btn, "Goodbye Cruel World");
  //elm_win_resize_object_add(win, btn);
  //evas_object_smart_callback_add(btn, "clicked", on_click, win);
  //evas_object_show(btn);
  //evas_object_show(win);

  create_base_gui (ad_real);


  elm_run ();

  return 0;
}
ELM_MAIN ()

#endif

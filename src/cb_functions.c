/*
Copyright (C) 2017  Henrik Ullman
License: GPL Version 3
*/
#define _GNU_SOURCE
#include <EWebKit.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <glib.h>

#include "breeze.h"
#include "app_control_functions.h"
#include "rss_functions.h"
#include "cb_functions.h"
#include "database.h"
#include "mrss.h"





Eina_Bool
naviframe_pop_cb (void *data, Elm_Object_Item * it)
{
  appdata_s *ad = data;
  /*pop naviframe if not on main menu */

  /* Let window go to hide state if on main */
  elm_win_lower (ad->win);

  return EINA_FALSE;
}

static gint
sort_rss_items (gconstpointer a, gconstpointer b)
{
  mrss_item_t *a_item;
  mrss_item_t *b_item;
  struct tm rss_tm_a;
  struct tm rss_tm_b;
  a_item = (mrss_item_t *) a;
  b_item = (mrss_item_t *) b;
  time_t time_a;
  time_t time_b;
  strptime (a_item->pubDate, "%a, %d %b %Y %H:%M:%S %z", &rss_tm_a);
  strptime (b_item->pubDate, "%a, %d %b %Y %H:%M:%S %z", &rss_tm_b);
  time_a = mktime (&rss_tm_a);
  time_b = mktime (&rss_tm_b);

  //dlog_print (DLOG_DEBUG, LOG_TAG, a_item->pubDate);
  //dlog_print (DLOG_DEBUG, LOG_TAG, b_item->pubDate);
  if (time_a > time_b)
    {
      //  dlog_print (DLOG_DEBUG, LOG_TAG, "a larger");
      return -1;
    }
  else if (time_b > time_a)
    {
      // dlog_print (DLOG_DEBUG, LOG_TAG, "b larger");
      return 1;
    }
  else
    {
      // dlog_print (DLOG_DEBUG, LOG_TAG, "a = b");
      return 0;
    }
  //TODO the underlying order of the entries are not changed
}

static void
editfield_focused_cb (void *data, Evas_Object * obj, void *event_info)
{
  Evas_Object *editfield = (Evas_Object *) data;
  elm_object_signal_emit (editfield, "elm,state,focused", "");

  if (!elm_entry_is_empty (obj))
    elm_object_signal_emit (editfield, "elm,action,show,button", "");
}

static void
editfield_unfocused_cb (void *data, Evas_Object * obj, void *event_info)
{
  Evas_Object *editfield = (Evas_Object *) data;
  elm_object_signal_emit (editfield, "elm,state,unfocused", "");
  elm_object_signal_emit (editfield, "elm,action,hide,button", "");
}

static void
editfield_changed_cb (void *data, Evas_Object * obj, void *event_info)
{
  Evas_Object *editfield = (Evas_Object *) data;

  if (!elm_entry_is_empty (obj) && elm_object_focus_get (obj))
    elm_object_signal_emit (editfield, "elm,action,show,button", "");
  else
    elm_object_signal_emit (editfield, "elm,action,hide,button", "");
}

static void
editfield_clear_button_clicked_cb (void *data, Evas_Object * obj,
				   void *event_info)
{
  Evas_Object *entry = (Evas_Object *) data;

  elm_entry_entry_set (entry, "");
}

static Evas_Object *
create_singleline_editfield_layout (Evas_Object * parent, char *title)
{
  Evas_Object *editfield, *entry, *button;

  editfield = elm_layout_add (parent);
  elm_layout_theme_set (editfield, "layout", "editfield", "singleline");
  evas_object_size_hint_align_set (editfield, EVAS_HINT_FILL, 0.0);
  evas_object_size_hint_weight_set (editfield, EVAS_HINT_EXPAND, 0.0);

  entry = elm_entry_add (editfield);
  elm_entry_single_line_set (entry, EINA_TRUE);
  elm_entry_scrollable_set (entry, EINA_TRUE);
  elm_object_part_text_set (entry, "elm.guide", title);
  evas_object_smart_callback_add (entry, "focused", editfield_focused_cb,
				  editfield);
  evas_object_smart_callback_add (entry, "unfocused", editfield_unfocused_cb,
				  editfield);
  evas_object_smart_callback_add (entry, "changed", editfield_changed_cb,
				  editfield);
  evas_object_smart_callback_add (entry, "preedit,changed",
				  editfield_changed_cb, editfield);
  elm_object_part_content_set (editfield, "elm.swallow.content", entry);

  button = elm_button_add (editfield);
  elm_object_style_set (button, "editfield_clear");
  evas_object_smart_callback_add (button, "clicked",
				  editfield_clear_button_clicked_cb, entry);
  elm_object_part_content_set (editfield, "elm.swallow.button", button);

  return editfield;
}

static void
_display_rss_items (gpointer feed_list, gpointer input_ad)
{
  mrss_item_t *feed_item;
  feed_item = feed_list;
  appdata_s *ad = input_ad;


  (void) elm_genlist_item_append (ad->item_list, ad->itc,
				  feed_item, NULL,
				  ELM_GENLIST_ITEM_NONE,
				  cb_rss_item_clicked, input_ad);
}

static void
feed_list_update (gpointer feed_item, gpointer input_ad)
{
  appdata_s *ad;
  Evas_Object *entry_handle;
  feed_s *feed_to_add;
  feed_to_add = feed_item;

  ad = input_ad;
  dlog_print (DLOG_DEBUG, LOG_TAG, "feed_list_update");
  dlog_print (DLOG_DEBUG, LOG_TAG, feed_to_add->name);
  //TODO combine this with crss function
  entry_handle = elm_genlist_item_append (ad->genlist_feeds, ad->itc_feeds,
					  feed_to_add, NULL,
					  ELM_GENLIST_ITEM_NONE,
					  cb_press_feed, ad);

  elm_genlist_item_show (entry_handle, ELM_GENLIST_ITEM_SCROLLTO_IN);
  dlog_print (DLOG_DEBUG, LOG_TAG, "feed_list_update done");

}

static void
feed_list_parse (gpointer feed_item, gpointer input_ad)
{
  appdata_s *ad = input_ad;
  feed_s *feed_to_parse;
  GSList *rss_items;
  int err;
  char error_str[100];

  rss_items = ad->rss_items;
  feed_to_parse = feed_item;
  dlog_print (DLOG_DEBUG, LOG_TAG, "feed_list_parse");
  dlog_print (DLOG_DEBUG, LOG_TAG, feed_to_parse->url);
  err = crss_parse_feed (feed_to_parse, &rss_items);
  if (err == 0)
    {
      ad->rss_items = rss_items;
    }
  else
    {
      if (err == 9)
	{
	  sprintf (error_str, "Incompatible encoding in feed: %s \n",
		   feed_to_parse->name);
	  strcat (ad->error_text, error_str);

	}
      else if (err == MRSS_ERR_DOWNLOAD)
	{
	  sprintf (error_str, "Download timeout for feed: %s \n",
		   feed_to_parse->name);
	  strcat (ad->error_text, error_str);
	}
      else
	{
	  sprintf (error_str, "Error retrieving feed: %s \n",
		   feed_to_parse->name);
	  strcat (ad->error_text, error_str);
	}
    }
}

static Evas_Object *
button_back_create (appdata_s * ad)
{
  Evas_Object *button_back;

  button_back = elm_button_add (ad->nf);
  evas_object_smart_callback_add (button_back, "clicked",
				  cb_button_back_clicked, ad);
  elm_object_style_set (button_back, "naviframe/title_left");
  elm_object_text_set (button_back, "Back");

  return button_back;
}

static void
cb_button_add_entry_clicked (void *input_ad, Evas_Object * obj,
			     void *event_info)
{

  appdata_s *ad;
  Evas_Object *entry_handle;
  feed_s *feed_item;
  GSList *feed_list;
  GSList *last_in_list;
  Evas_Object *entry_name;
  Evas_Object *entry_rss;

  const char *input_name;
  const char *input_rss;


  dlog_print (DLOG_DEBUG, LOG_TAG, "cb_button_add_entry_clicked");
  ad = input_ad;
  feed_list = ad->feeds;



  entry_name =
    elm_object_part_content_get (ad->entry_name, "elm.swallow.content");
  entry_rss =
    elm_object_part_content_get (ad->entry_rss, "elm.swallow.content");
  input_name = elm_entry_entry_get (entry_name);
  input_rss = elm_entry_entry_get (entry_rss);


  dlog_print (DLOG_DEBUG, LOG_TAG, input_name);
  dlog_print (DLOG_DEBUG, LOG_TAG, input_rss);
  crss_add_feed (input_rss, input_name, &feed_list);	//TODO: this should pickup the proper values
  dlog_print (DLOG_DEBUG, LOG_TAG, "crss_add_feed finished");

  last_in_list = g_slist_last (feed_list);
  feed_item = last_in_list->data;
  dlog_print (DLOG_DEBUG, LOG_TAG, feed_item->url);
  database_add_feed (ad->database, feed_item);

  /*add item to genlist *///TODO: move this to crss_add_feed function
  entry_handle = elm_genlist_item_append (ad->genlist_feeds, ad->itc_feeds,
					  feed_item, NULL,
					  ELM_GENLIST_ITEM_NONE,
					  cb_press_feed, ad);
  elm_genlist_item_show (entry_handle, ELM_GENLIST_ITEM_SCROLLTO_IN);
  ad->feeds = feed_list;
  elm_naviframe_item_pop (ad->nf);

}

void
cb_rss_item_clicked (void *input_ad, Evas_Object * obj, void *event_info)
{
  appdata_s *ad = input_ad;
  Evas_Object *button_back;
  Evas_Object *label_item_content;
  GSList *feed_item;
  mrss_item_t *item_content;
  Evas_Object *bg;
  Evas_Object *scroller;
  Evas *evas_canvas;
  int clicked_index;

  char *html_string;
  size_t len_description, len_content, len_pubdate, len_title, len_total;

  elm_genlist_item_selected_set (event_info, EINA_FALSE);

  feed_item = ad->rss_items;






  clicked_index = elm_genlist_item_index_get (event_info);
  item_content = g_slist_nth_data (feed_item, clicked_index - 1);

  button_back = button_back_create (ad);

  /*calc length of body and allocate memory */

  len_title = strlen (item_content->title);
  len_pubdate = strlen (item_content->pubDate);


  if (item_content->description)
    {
      len_description = strlen (item_content->description);
      if (item_content->content)
	{
	  len_content = strlen (item_content->content);
	  len_total =
	    len_description + len_content + len_title + len_pubdate + 26;
	}
      else
	{
	  len_total = len_description + len_title + len_pubdate + 25;
	}
    }
  else if (item_content->content)
    {
      len_content = strlen (item_content->content);
      len_total = len_content + len_title + len_pubdate + 25;

    }
  else
    {
      len_total = len_title + len_pubdate + 24;
    }

  html_string = malloc (sizeof (char) * len_total);


  /*web view */

  evas_canvas = evas_object_evas_get (ad->nf);
  label_item_content = ewk_view_add (evas_canvas);
  ewk_settings_default_font_size_set (ewk_view_settings_get
				      (label_item_content), 45);


  //TODO: add display of description

  evas_object_size_hint_align_set (label_item_content, EVAS_HINT_FILL, 0);
  evas_object_size_hint_weight_set (label_item_content, EVAS_HINT_FILL,
				    EVAS_HINT_EXPAND);
  elm_label_line_wrap_set (label_item_content, ELM_WRAP_MIXED);
  dlog_print (DLOG_DEBUG, LOG_TAG, "html_string lentgh:");
  char lenn[20];
  snprintf (lenn, 20, "%d", len_total);
  dlog_print (DLOG_DEBUG, LOG_TAG, lenn);
  strcpy (html_string, "<h3>");	//TODO replace strcpy and cat with snprintf
  strcat (html_string, item_content->title);
  strcat (html_string, "</h3>");
  strcat (html_string, "<h4>");
  strcat (html_string, item_content->pubDate);
  strcat (html_string, "</h4>");
  strcat (html_string, "<hr>");
  if (item_content->description)
    {
      strcat (html_string, item_content->description);
    }

  if (item_content->content)
    {
      strcat (html_string, item_content->content);
    }

  ewk_view_html_string_load (label_item_content, html_string, NULL, NULL);

  /*scroller */

  scroller = elm_scroller_add (ad->nf);
  elm_object_content_set (scroller, label_item_content);

  /*background */
  bg = elm_bg_add (ad->nf);
  elm_bg_color_set (bg, 230, 230, 230);


  elm_object_part_content_set (bg, "overlay", scroller);
  evas_object_show (bg);
  evas_object_show (scroller);
  evas_object_show (label_item_content);


  (void) elm_naviframe_item_push (ad->nf, "RSS Article", button_back, NULL,
				  bg, NULL);
  elm_object_part_content_set (ad->nf, "title_left_btn", button_back);
  dlog_print (DLOG_DEBUG, LOG_TAG, "rss item callback click");


}

void
cb_button_back_clicked (void *input_ad, Evas_Object * obj, void *event_info)
{
  appdata_s *ad = input_ad;
  dlog_print (DLOG_DEBUG, LOG_TAG, "popping naviframe");
  elm_naviframe_item_pop (ad->nf);
}

static void
thread_update_run (void *input_ad, Ecore_Thread * thread)
{
  appdata_s *ad = input_ad;
  GSList *rss_items;
  GSList *feed_list;

  feed_list = ad->feeds;

  ad->rss_items = NULL;
  rss_items = ad->rss_items;

  if (feed_list != NULL)
    {
      g_slist_foreach (feed_list, feed_list_parse, ad);
      dlog_print (DLOG_DEBUG, LOG_TAG, "feed parsed");


      rss_items = ad->rss_items;

      rss_items = g_slist_sort (rss_items, sort_rss_items);	//crash is here when encoding is not utf8
      ad->rss_items = rss_items;

    }
  else
    {
      dlog_print (DLOG_DEBUG, LOG_TAG, "nothing to update");
    }
}

static void
thread_update_end (void *input_ad, Ecore_Thread * thread)
{
  appdata_s *ad = input_ad;
  Evas_Object *error_popup;

  /*all UI operations should be here as GUI is not thread safe */

  elm_genlist_clear (ad->item_list);
  if (ad->feeds != NULL)
    {
      g_slist_foreach (ad->rss_items, _display_rss_items, ad);

      elm_genlist_item_show (elm_genlist_first_item_get (ad->item_list),
			     ELM_GENLIST_ITEM_SCROLLTO_IN);
      if (ad->error_text[0] != '\0')
	{
	  error_popup = elm_popup_add (ad->nf);
	  elm_object_style_set (error_popup, "toast");
	  elm_object_text_set (error_popup, ad->error_text);
	  elm_popup_timeout_set (error_popup, 3.0);
	  eext_object_event_callback_add (error_popup, EEXT_CALLBACK_BACK,
					  eext_popup_back_cb, NULL);
	  evas_object_smart_callback_add (error_popup, "timeout",
					  eext_popup_back_cb, NULL);
	  evas_object_show (error_popup);
	}
    }
  dlog_print (DLOG_DEBUG, LOG_TAG, "thread has ended");
}

static void
thread_update_cancel (void *input_ad, Ecore_Thread * thread)
{
  dlog_print (DLOG_DEBUG, LOG_TAG, "thread has been cancelled");
}

void
cb_button_update_clicked (void *input_ad, Evas_Object * obj, void *event_info)
{
  Ecore_Thread *update_thread;
  appdata_s *ad = input_ad;

  ad->error_text[0] = '\0';

  update_thread =
    ecore_thread_run (thread_update_run, thread_update_end,
		      thread_update_cancel, input_ad);
  dlog_print (DLOG_DEBUG, LOG_TAG, "update thread was sent off");
}

void
cb_button_options_clicked (void *input_ad, Evas_Object * obj,
			   void *event_info)
{
  appdata_s *ad = input_ad;
  Elm_Object_Item *nf_options;
  Evas_Object *button_back;
  Evas_Object *bg;


  ad->button_add_feed = elm_button_add (ad->nf);
  evas_object_smart_callback_add (ad->button_add_feed, "clicked",
				  cb_button_add_feed_clicked, ad);
  elm_object_style_set (ad->button_add_feed, "naviframe/title_right");
  elm_object_text_set (ad->button_add_feed, "Add");

  button_back = button_back_create (ad);

  /*background */
  bg = elm_bg_add (ad->nf);
  elm_bg_color_set (bg, 230, 230, 230);


  /* genlist feeds */
  ad->genlist_feeds = elm_genlist_add (ad->nf);

  elm_object_style_set (ad->genlist_feeds, "solid/default");
  elm_genlist_highlight_mode_set (ad->genlist_feeds, EINA_TRUE);
  elm_genlist_mode_set (ad->genlist_feeds, ELM_LIST_COMPRESS);

  elm_object_part_content_set (bg, "overlay", ad->genlist_feeds);



  nf_options =
    elm_naviframe_item_push (ad->nf, "RSS Feeds", button_back,
			     ad->button_add_feed, bg, NULL);
  elm_object_part_content_set (ad->nf, "title_right_btn", ad->button_add_feed);	//TODO: temp
  elm_object_part_content_set (ad->nf, "title_left_btn", button_back);

  //TODO: add already updated feeds
  if (ad->feeds == NULL)
    {
      dlog_print (DLOG_DEBUG, LOG_TAG, "feed list is NULL");
    }
  g_slist_foreach (ad->feeds, feed_list_update, ad);
}


void
cb_button_add_feed_clicked (void *input_ad, Evas_Object * obj,
			    void *event_info)
{
  appdata_s *ad;
  GSList *feed_list;
  Elm_Object_Item *nf_options;
  dlog_print (DLOG_DEBUG, LOG_TAG, "adding feed");
  ad = input_ad;
  feed_list = ad->feeds;

  Evas_Object *bg;
  Evas_Object *button_back;
  Evas_Object *add_box;
  Evas_Object *add_button;


  /*add feed naviframe window */

  /*background */
  bg = elm_bg_add (ad->nf);
  elm_bg_color_set (bg, 230, 230, 230);

  /*box */
  add_box = elm_box_add (bg);
  evas_object_size_hint_align_set (add_box, EVAS_HINT_FILL, 0.0);
  evas_object_size_hint_weight_set (add_box, EVAS_HINT_EXPAND, 0.0);
  elm_object_part_content_set (bg, "overlay", add_box);
  evas_object_show (add_box);

  /*name entry */

  ad->entry_name = create_singleline_editfield_layout (add_box, "Feed name");
  elm_box_pack_end (add_box, ad->entry_name);
  evas_object_show (ad->entry_name);

  /*rss entry */
  ad->entry_rss = create_singleline_editfield_layout (add_box, "Feed URL");
  elm_box_pack_end (add_box, ad->entry_rss);
  evas_object_show (ad->entry_rss);


  /*add button */
  add_button = elm_button_add (add_box);
  elm_box_pack_end (add_box, add_button);
  elm_object_part_text_set (add_button, "default", "Add");
  evas_object_smart_callback_add (add_button, "clicked",
				  cb_button_add_entry_clicked, ad);

  evas_object_size_hint_align_set (add_button, 0.1, 0.0);
  evas_object_size_hint_weight_set (add_button, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_min_set (add_button, 300, 100);
  evas_object_show (add_button);

  /*back button */
  button_back = button_back_create (ad);

  /*add to naviframe */

  nf_options =
    elm_naviframe_item_push (ad->nf, "Add feed", button_back, NULL, bg, NULL);
  elm_object_part_content_set (ad->nf, "title_left_btn", button_back);


}

void
cb_press_feed (void *input_ad, Evas_Object * obj, void *event_info)
{
  appdata_s *ad;
  ad = input_ad;
  char popup_text[100];

  sprintf (popup_text, "Do you want to delete %s?",
	   elm_object_item_part_text_get (event_info, "elm.text"));
  dlog_print (DLOG_DEBUG, LOG_TAG, "press_feed running");
  ad->delete_popup = elm_popup_add (ad->nf);
  elm_object_part_text_set (ad->delete_popup, "title,text",
			    "Confirm deletion");
  elm_object_text_set (ad->delete_popup, popup_text);

  ad->clicked_index = elm_genlist_item_index_get (event_info);

  Evas_Object *button_go;
  Evas_Object *button_cancel;

  button_go = elm_button_add (ad->delete_popup);
  elm_object_text_set (button_go, "OK");

  button_cancel = elm_button_add (ad->delete_popup);
  elm_object_text_set (button_cancel, "Cancel");

  elm_object_part_content_set (ad->delete_popup, "button1", button_go);
  elm_object_part_content_set (ad->delete_popup, "button2", button_cancel);
  evas_object_smart_callback_add (button_go, "clicked",
				  cb_button_delete_entry_clicked, ad);
  evas_object_smart_callback_add (button_cancel, "clicked",
				  cb_popup_dismissed, ad);
  evas_object_show (ad->delete_popup);
  elm_genlist_item_selected_set (event_info, EINA_FALSE);
}



void
cb_button_delete_entry_clicked (void *input_ad, Evas_Object * obj,
				void *event_info)
{
  dlog_print (DLOG_DEBUG, LOG_TAG, "delete callback clicked");
  appdata_s *ad;
  ad = input_ad;
  GSList *feed_list;
  feed_list = ad->feeds;
  crss_delete_feed (ad, ad->clicked_index, ad->database, &feed_list);


  elm_popup_dismiss (ad->delete_popup);
  evas_object_del (ad->delete_popup);

}

void
cb_button_entry_released (void *input_ad, Evas_Object * obj, void *event_info)
{
  Eina_Bool activated = elm_genlist_item_selected_get (event_info);
  dlog_print (DLOG_DEBUG, LOG_TAG, "released callback clicked");

  if (activated == EINA_FALSE)
    {
      dlog_print (DLOG_DEBUG, LOG_TAG, "activated is true");
      elm_genlist_item_selected_set (event_info, EINA_TRUE);
    }
}

void
create_popup_menu (appdata_s * ad)
{

  if (ad->popup_menu != NULL)
    {
      elm_ctxpopup_dismiss (ad->popup_menu);
    }
  else
    {


      ad->popup_menu = elm_ctxpopup_add (ad->win);

      Evas_Coord w, h;
      elm_win_screen_size_get (ad->win, NULL, NULL, &w, &h);

      elm_ctxpopup_item_append (ad->popup_menu, "About", NULL, cb_popup1, ad);
      elm_object_style_set (ad->popup_menu, "more/default");

      elm_ctxpopup_direction_priority_set (ad->popup_menu,
					   ELM_CTXPOPUP_DIRECTION_UP,
					   ELM_CTXPOPUP_DIRECTION_UNKNOWN,
					   ELM_CTXPOPUP_DIRECTION_UNKNOWN,
					   ELM_CTXPOPUP_DIRECTION_UNKNOWN);
      evas_object_smart_callback_add (ad->popup_menu, "dismissed",
				      cb_popup_menu_dismissed, ad);

      evas_object_move (ad->popup_menu, (w / 2), h);

      evas_object_show (ad->popup_menu);
    }

}

void
cb_popup1 (void *input_ad, Evas_Object * obj, void *event_info)
{
  appdata_s *ad = input_ad;
  Evas_Object *button_back;
  Evas_Object *bg;
  Evas_Object *box;
  Evas_Object *about_label;
  Elm_Object_Item *nf_about;
  char about_string[350];

  elm_ctxpopup_dismiss (obj);
  /*create about view */

  bg = elm_bg_add (ad->nf);
  elm_bg_color_set (bg, 230, 230, 230);

  button_back = button_back_create (ad);
  box = elm_box_add (bg);
  about_label = elm_label_add (box);
  sprintf (about_string, "<font_size=50><color=#000000>Breeze %s</font_size>"
	   "<font_size=30><br>Copyright © 2017 Henrik Ullman"
	   "<br>Licence:GNU General Public Licence v3"
	   "<br>Libraries:"
	   "<br>libnxml © 2008  Andrea Marchesini - LGPL 2.0 "
	   "<br>libmrss © 2008  Andrea Marchesini - LGPL 2.0 </color></font_size>",
	   VERSION);
  elm_object_text_set (about_label, about_string);
  evas_object_show (about_label);
  evas_object_show (box);
  elm_object_part_content_set (bg, "overlay", box);
  elm_box_pack_end (box, about_label);


  nf_about =
    elm_naviframe_item_push (ad->nf, "About", button_back, NULL, bg, NULL);
  elm_object_part_content_set (ad->nf, "title_left_btn", button_back);

}

void
cb_popup_menu_dismissed (void *input_ad, Evas_Object * obj, void *event_info)
{
  appdata_s *ad = input_ad;
  evas_object_del (ad->popup_menu);
  ad->popup_menu = NULL;
}

void
cb_popup_dismissed (void *input_ad, Evas_Object * obj, void *event_info)
{
  appdata_s *ad = input_ad;
  evas_object_del (event_info);
  elm_popup_dismiss (ad->delete_popup);
  evas_object_del (ad->delete_popup);
}

Eina_Bool
cb_hardware_key (void *input_ad, int type, void *ev)
{
  appdata_s *ad = input_ad;
  Ecore_Event_Key *event = ev;
  if (!strcmp ("XF86Menu", event->key))
    {
      create_popup_menu (ad);

    }
  return ECORE_CALLBACK_PASS_ON;
}

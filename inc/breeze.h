/*
Copyright (C) 2017  Henrik Ullman
License: GPL Version 3
*/
#ifndef __BREEZE_H__
#define __BREEZE_H__

#ifdef TIZEN

#include <app.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "breeze"




#endif

#if !defined(PACKAGE)
#define PACKAGE "kr.ullman.breeze"
#endif

#if !defined(VERSION)
#define VERSION "0.1.0"
#endif

#include <Elementary.h>
#include <glib.h>




typedef struct
{
  Evas_Object *win;
  Evas_Object *nf;
  Evas_Object *layout;
  Evas_Object *conform;
  Evas_Object *title;
  Evas_Object *item_list;
  Evas_Object *button_options;
  Evas_Object *button_update;
  Evas_Object *tabbar;
  Evas_Object *bg;
  Evas_Object *name_input;
  Evas_Object *url_input;
  Evas_Object *genlist_feeds;
  Evas_Object *button_add_feed;
  Evas_Object *entry_name;
  Evas_Object *entry_rss;
  Evas_Object *popup_menu;
  Evas_Object *delete_popup;
  Evas_Object *ewebkit_view;
  GSList *rss_items;
  GSList *rss_current;
  GSList *feeds;
  Elm_Genlist_Item_Class *itc;
  Elm_Genlist_Item_Class *itc_feeds;
  char database[100];
  char error_text[500];
  unsigned int clicked_index;
} appdata_s;

typedef struct
{
  char *url;
  char *name;
  char *icon;
} feed_s;

void
create_base_gui (appdata_s * ad);

#endif /* __BREEZE_H__ */

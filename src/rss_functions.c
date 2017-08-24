/*
Copyright (C) 2017  Henrik Ullman
License: GPL Version 3
*/
#include "breeze.h"
#include <glib.h>
#include <string.h>
#include "mrss.h"
#include "database.h"

/*
 * This file is to include rss parsing and management functions 
 */



char *
rep_str (char *target, size_t target_len, const char *sub, const char *repl)
{
  static char buffer[1000];	// remove static with malloc
  char *l;

  strcpy (buffer, target);

  if (!(l = strstr (target, sub)))
    {

      return buffer;
    }
  strncpy (buffer, target, l - target);
  buffer[l - target] = '\0';
  sprintf (buffer + (l - target), "%s%s", repl, l + strlen (sub));

  return rep_str (buffer, strlen (buffer), sub, repl);

}


int
crss_parse_feed (feed_s * rss_feed, GSList ** item_llist)
{
  mrss_error_t err;
  mrss_t *feed_data;
  mrss_item_t *feed_item;
  mrss_item_t *feed_item2;
  char *feed_url;
  CURLcode code;
  GSList *item_llist2;
  mrss_options_t *feed_options;


  feed_url = rss_feed->url;


  feed_options = mrss_options_new (10, NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL);	//10 sec timeout


  feed_url = rep_str (feed_url, strlen (feed_url), "&amp;", "&");	//is replaced in database
  //TODO add error if network is not setup
  err = mrss_parse_url_with_options_and_error (feed_url,
					       &feed_data, feed_options,
					       &code);

  dlog_print (DLOG_DEBUG, LOG_TAG, "exited mrss_parse_url");

  if (err == MRSS_ERR_DOWNLOAD)
    {
      dlog_print (DLOG_DEBUG, LOG_TAG, "MRSS_ERR_DOWNLOAD");
      return MRSS_ERR_DOWNLOAD;
    }
  if (feed_data->encoding == NULL)
    {
      dlog_print (DLOG_DEBUG, LOG_TAG, "encoding is NULL, assuming utf8");
      feed_data->encoding = "utf-8";
    }
  else if (!strcmp (feed_data->encoding, "UTF-8"))
    {
      feed_data->encoding = "utf-8";
    }
  //TODO get icon from feed
  //rss_feed->icon = malloc(sizeof(char) *1000);
  //rss_feed->icon = feed_data->image_url;
  //dlog_print(DLOG_DEBUG, LOG_TAG, "image url:");
  //dlog_print(DLOG_DEBUG, LOG_TAG, rss_feed->icon);
  dlog_print (DLOG_DEBUG, LOG_TAG, "encoding is");
  dlog_print (DLOG_DEBUG, LOG_TAG, feed_data->encoding);
  if (err == MRSS_OK && !strcmp (feed_data->encoding, "utf-8"))
    {
      for (feed_item = feed_data->item; feed_item;
	   feed_item = feed_item->next)
	{

	  feed_item->source = rss_feed->name;	//TODO remove this workaround
	  feed_item2 = feed_item;
	  item_llist2 = g_slist_append (*item_llist, feed_item2);
	  *item_llist = item_llist2;

	}
      return 0;
    }
  else if (strcmp (feed_data->encoding, "utf-8"))
    {
      return 9;
    }
  else
    {
      dlog_print (DLOG_DEBUG, LOG_TAG, "feed parse error");
      return 1;
    }

}

void
crss_add_feed (const char *feed_url, const char *feed_name,
	       GSList ** feed_list)
{
  /*add feed to linked_list */
  feed_s *this_feed;
  GSList *feed_list2;
  char *add_url;
  char *add_name;
  size_t url_len, name_len;

  url_len = strlen (feed_url);
  name_len = strlen (feed_name);
  this_feed = malloc (sizeof (feed_s));	//TODO free when removing feed
  this_feed->url = malloc (sizeof (char) * (unsigned int) url_len + 1);
  this_feed->name = malloc (sizeof (char) * (unsigned int) name_len + 1);

  add_url = strdup (feed_url);
  add_name = strdup (feed_name);
  strcpy (this_feed->url, feed_url);
  strcpy (this_feed->name, feed_name);
  feed_list2 = g_slist_append (*feed_list, this_feed);	//TODO: trouble with appending to ad->feeds 
  *feed_list = feed_list2;

}

void
crss_delete_feed (void *input_ad, unsigned int rss_feed, char *database_name,
		  GSList ** feed_list)
{
  appdata_s *ad = input_ad;
  feed_s *this_feed;
  GSList *feed_list2;

  char rss_temp[10];
  sprintf (rss_temp, "%d", rss_feed - 1);
  dlog_print (DLOG_DEBUG, LOG_TAG, rss_temp);
  Evas_Object *delete_feed;
  delete_feed = elm_genlist_nth_item_get (ad->genlist_feeds, rss_feed - 1);
  elm_object_item_del (delete_feed);


  this_feed = g_slist_nth_data (*feed_list, rss_feed - 1);
  feed_list2 = g_slist_remove (*feed_list, this_feed);

  if (feed_list2 == NULL)
    {
      ad->feeds = NULL;
    }
  else
    {
      ad->feeds = feed_list2;
    }

  //delete from database
  database_delete_feed (ad->database, this_feed->name);

}

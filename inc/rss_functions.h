/*
Copyright (C) 2017  Henrik Ullman
License: GPL Version 3
*/
#ifndef __RSS_FUNCTIONS_H__
#define __RSS_FUNCTIONS_H__



int 
crss_parse_feed (feed_s * rss_feed, GSList ** item_llist);


void
crss_add_feed (const char *feed_url, const char *feed_name,
	       GSList ** feed_list);

void
crss_delete_feed (void *input_ad, unsigned int rss_feed,
		       char *database_name, GSList ** feed_list);


#endif /* __RSS_FUNCTIONS_H__ */

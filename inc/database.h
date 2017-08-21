/*
Copyright (C) 2017  Henrik Ullman
License: GPL Version 3
*/
#define SQL_SIZE 500

void 
database_create (char *database_name);

void 
database_add_feed (char *database_name, feed_s * feed);

void 
database_load_feeds (char *database_name, appdata_s * ad);

void 
database_delete_feed (char *database_name, char *feed_name);

/*
Copyright (C) 2017  Henrik Ullman
License: GPL Version 3
*/
#include <sqlite3.h>
#include <stdio.h>
#include "breeze.h"
#include "database.h"
#include "rss_functions.h"

void
database_create (char *database_name)
{
  char sql[SQL_SIZE];
  int db_err;
  sqlite3 *db_handle;
  char *err_msg;

  err_msg = 0;
  db_err = sqlite3_open (database_name, &db_handle);

  snprintf (sql, SQL_SIZE, "CREATE TABLE "
	    "main.ex(ID INTEGER PRIMARY KEY, "
	    "NAME TEXT NOT NULL, " "URL TEXT NOT NULL);");
  db_err = sqlite3_exec (db_handle, sql, 0, 0, &err_msg);

  if (db_err != SQLITE_OK)
    {
      //dlog_print (DLOG_DEBUG, LOG_TAG, "database create error");
    }

}

void
database_add_feed (char *database_name, feed_s * feed)
{
  sqlite3 *db_handle;
  int db_err;
  char *err_msg;
  char sql[SQL_SIZE];



  err_msg = 0;
  db_err = sqlite3_open (database_name, &db_handle);
  if (db_err != SQLITE_OK)
    {
      //dlog_print (DLOG_DEBUG, LOG_TAG, "database opening error");
    }
  snprintf (sql, SQL_SIZE, "INSERT INTO ex(NAME,URL) " "VALUES('%s','%s');",
	    feed->name, feed->url);
  //dlog_print (DLOG_DEBUG, LOG_TAG, sql);
  db_err = sqlite3_exec (db_handle, sql, 0, 0, &err_msg);
  if (db_err != SQLITE_OK)
    {
      //dlog_print (DLOG_DEBUG, LOG_TAG, "database write error");
      //dlog_print (DLOG_DEBUG, LOG_TAG, err_msg);
    }
  sqlite3_close (db_handle);

}

static int
callback (void *count, int argc, char **argv, char **azColName)
{
  int *c = count;
  *c = atoi (argv[0]);
  return 0;
}

void
database_load_feeds (char *database_name, appdata_s * ad)
{
  /*load all feeds from database to gslist */
  sqlite3 *db_handle;
  int db_err;
  char *err_msg;

  char *sql;
  sqlite3_stmt *res;
  int step;
  const unsigned char *rss_name;
  const unsigned char *rss_url;
  GSList *feed_list;
  int count = 0;
  int i;

  feed_list = ad->feeds;

  err_msg = 0;
  db_err = sqlite3_open (database_name, &db_handle);
  db_err =
    sqlite3_exec (db_handle, "SELECT count(*) FROM ex", callback, &count,
		  &err_msg);

  sql = "SELECT ID, NAME, URL FROM ex";
  db_err = sqlite3_prepare_v2 (db_handle, sql, -1, &res, 0);

  for (i = 1; i <= count; i++)
    {


      step = sqlite3_step (res);

      if (step == SQLITE_ROW)
	{

	  rss_name = sqlite3_column_text (res, 1);
	  rss_url = sqlite3_column_text (res, 2);
	  crss_add_feed ((const char *) rss_url, (const char *) rss_name,
			 &feed_list);
	}

    }
  sqlite3_finalize (res);
  ad->feeds = feed_list;
  sqlite3_close (db_handle);


}

void
database_delete_feed (char *database_name, char *feed_name)
{
  sqlite3 *db_handle;
  int db_err;
  char *sql;
  int bind_length;
  sqlite3_stmt *res;

  db_err = sqlite3_open (database_name, &db_handle);
  sql = "DELETE FROM ex WHERE NAME = ?";
  db_err = sqlite3_prepare_v2 (db_handle, sql, -1, &res, 0);
  bind_length = strlen (feed_name);

  sqlite3_bind_text (res, 1, feed_name, bind_length, (void *) -1);
  db_err = sqlite3_step (res);
  sqlite3_finalize (res);

  sqlite3_close (db_handle);
}

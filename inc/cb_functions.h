/*
Copyright (C) 2017  Henrik Ullman
License: GPL Version 3
*/
Eina_Bool 
naviframe_pop_cb (void *data, Elm_Object_Item * it);

void
cb_rss_item_clicked (void *input_ad, Evas_Object * obj, void *event_info);

void
cb_button_back_clicked (void *input_ad, Evas_Object * obj, void *event_info);

void 
cb_button_update_clicked (void *ad, Evas_Object * obj, void *event_info);

void
cb_button_options_clicked (void *input_ad, Evas_Object * obj,
			   void *event_info);

void
cb_button_add_feed_clicked (void *input_ad, Evas_Object * obj,
			    void *event_info);

void
cb_button_delete_entry_clicked (void *input_ad, Evas_Object * obj,
				void *event_info);

void
cb_button_entry_released (void *input_ad, Evas_Object * obj,
			  void *event_info);

void 
cb_press_feed (void *input_ad, Evas_Object * obj, void *event_info);

#ifdef TIZEN
void 
create_popup_menu (appdata_s * ad);
#endif

void 
cb_popup1 (void *input_ad, Evas_Object * obj, void *event_info);

void 
cb_popup_menu_dismissed (void *input_ad, Evas_Object * obj,
			      void *event_info);

void 
cb_popup_dismissed (void *input_ad, Evas_Object * obj, void *event_info);

#ifdef TIZEN
Eina_Bool 
cb_hardware_key (void *input_ad, int type, void *event_info);
#endif

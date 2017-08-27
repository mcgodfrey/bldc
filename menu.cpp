#include "menu.h"

/*************
 * MenuItem
 *************/
MenuItem::MenuItem(){
  key='\0';
  text="";
};

MenuItem::MenuItem(char key_in, String text_in, void (*callback_in)()){
  key = key_in;
  text = text_in;
  callback = callback_in;
};




/*************
 * MenuSystem
 *************/
MenuSystem::MenuSystem(){
  num_items = 0;
};

void MenuSystem::add_item(MenuItem new_item){
  items[num_items++] = new_item;
};

void MenuSystem::print_menu(){
  Serial.println("Menu System:");
  for(byte i=0; i < num_items; i++){
    Serial.print(items[i].key);
    Serial.print(") ");
    Serial.println(items[i].text);
  }
  Serial.print("> ");
};

void MenuSystem::parse_menu(char c){
  byte valid_item = 0;
  for(byte i=0; i<num_items; i++){
    if (c == items[i].key) {
      Serial.println("");
      items[i].callback();
      Serial.println("");
      valid_item = 1;
      break;
    }
  }
}


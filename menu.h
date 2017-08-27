/*
 * Serial Menu interface.
 * 
 * Menu is composed of an array of menu items 
 *  (max 9, hard coded with a MAX_MENU_ITEMS. You can increase it if you like
 *   but it is pre-allocated so it will take up space)
 * 
 * Each MenuItem is a class with a:
 *  - command key (single character)
 *  - text string which is displayed in the menu
 *  - callback function when it is selected (no args)
 *  
 *  Usage:
 *   - Create a MenuSystem object in your main code 
 *     -  MenuSystem m;
 *   - Add your menuItems to it
 *     -  m.add_item(MenuItem('u', "Speed up", *speedUp));
 *   - display the menu with:
 *     - m.print_menu();
 *   - when you have some user input, call parse_menu
 *     - m.parse_menu(char);
 *     
 * It makes sense to put this in your main loop and and call parse_menu whenever
 * there is any data on the serial line.\
 * 
 * Full example
 * 

void speedUp(){
  Serial.println("speeding up");
}

void setup() {
  Serial.begin(9600);
  m.add_item(MenuItem('u', "Speed up", *speedUp));
  m.prin_menu();
}

void loop() {
  if(Serial.available()){
    m.parse_menu(Serial.read()); //process it with callbacks
    m.print_menu();              //re-print the menu
  }
}

 * 
 */
 
#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <HardwareSerial.h>

#define MAX_MENU_ITEMS 9

/*
 * A menu item is composed of a shortcut key, some menu text and a callback function when it is selected
 */
class MenuItem{
  public:
    MenuItem();
    MenuItem(char key_in, String text_in, void (*callback_in)());
    char key;
    String text;
    void (*callback)();
  private:
};


/*
 * A MenuSystem is an array of MenuItems which can be printed and parsed.
 */
class MenuSystem{
  public:
    MenuSystem();
    MenuItem items[MAX_MENU_ITEMS];
    byte num_items;
    void add_item(MenuItem new_item);
    void print_menu();
    void parse_menu(char c);
};

#endif

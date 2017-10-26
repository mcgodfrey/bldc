#include "configuration.h"
#include "EEPROM.h"


/*Load config from eeprom*/
void config_load(config_t *c){
  EEPROM.get(0, *c);
}

/*Save config to eeprom*/
void config_save(config_t *c){
  EEPROM.put(0, *c);
}

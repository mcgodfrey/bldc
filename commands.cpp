#include "commands.h"

static rx_cmd_t *rx_commands;
static byte num_rx_commands;
static config_t *config;

void rx_commands_init(rx_cmd_t *commands, byte num_commands, config_t *conf){
  rx_commands = commands;
  num_rx_commands = num_commands;
  config = conf;
}

void rx_process_command(const char *rx_buffer){
  #ifdef DEBUG
  Serial.println("");
  Serial.println("");
  Serial.println(F("Received command:"));
  Serial.println(rx_buffer);
  #endif
  char new_command[COMMAND_LEN];
  byte i;
  for(i=0; i<COMMAND_LEN; i++){
    if(rx_buffer[i] == '='){
      new_command[i] = 0;
      break;
    } else {
      new_command[i] = rx_buffer[i];
    }
  }
  const char *args = rx_buffer+i+1;

  //Loop through looking for a match with one of the commands
  for(byte i=0; i<num_rx_commands; i++){
    if(strcmp(new_command, rx_commands[i].cmd) == 0){
      rx_commands[i].f(config, args);
      break;
    }
  }
}


void rx_process_char(char c){
  static char rx_buffer[RX_BUFFER_SIZE];
  static byte rx_buf_n = 0;

  switch(c){
  case '\r':
    // ignore. wait for \n
    break;
  case '\n':
    rx_buffer[rx_buf_n] = 0; //Null terminator
    rx_process_command(rx_buffer);
    // reset buffer
    rx_buf_n = 0;
    break;
  default:
    if(rx_buf_n < RX_BUFFER_SIZE - 1){ //allow room for null terminator
      rx_buffer[rx_buf_n++] = c;
    }
  }
}



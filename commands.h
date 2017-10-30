#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "Arduino.h"
#include "constants.h"
#include "configuration.h"

typedef void (*rx_fun)(config_t *, const char *);

typedef struct{
  char cmd[COMMAND_LEN];
  rx_fun f;
} rx_cmd_t;

void rx_commands_init(rx_cmd_t *commands, byte num_commands, config_t *conf);
void rx_process_command(const char *rx_buffer);
void rx_process_char(char c);

#endif //COMMANDS_H_

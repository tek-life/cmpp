#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "cmpp.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
  	printf("Usage: ismg [addr] [port]\n");
  	return 0;
  }

  int err;
  cmpp_ismg_t cmpp;

  char *addr = argv[1];
  char *port = argv[2];
  err = cmpp_init_ismg(&cmpp, addr, atoi(port));
  if (err) {
    printf("ERROR: %s\n", cmpp_get_error(err));
    return 0;
  }
  printf("cmpp server bind %s port %s successfull\n", addr, port);
  cmpp_event_loop(&cmpp);
  cmpp_close(&cmpp);
  return 0;
}

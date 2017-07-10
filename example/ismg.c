#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "cmpp.h"

int main(int argc, argv char *argv[]) {
  if (argc != 3) {
  	printf("Usage: ismg [addr] [port]\n");
  	return 0;
  }

  int err;
  CMPP_T cmpp;

  err = cmpp_init(&cmpp, argv[1], atoi(argv[2]), CMPP_ISMG);
  if (err) {
    printf("ERROR: cmpp init error\n");
    return 0;
  }
  printf("cmpp server bind %s port %s successfull\n", argv[1], argv[2]);
  cmpp_event_loop(&cmpp);
  cmpp_close(&cmpp);
  return 0;
}

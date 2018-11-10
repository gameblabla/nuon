#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <unistd.h>

#include "bless_defines.h"

/*
  paddriver.c

  Given the signed blessings section for a driver, pad to correct length
*/

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
  char buf[BUF_SIZE], *newName;
  size_t bytesWritten, bytesRead;
  int result = 0, i;
  FILE *out, *coff;
  struct stat fstat;

  if (argc != 2) {
    printf("Usage: %s driver-blessings\n", argv[0]);
  }
  else {
    newName = (char *)malloc(strlen(argv[1]) + 8); /* append ".padded\0" */
    if (newName) {
      sprintf(newName, "%s.padded", argv[1]);
      out = fopen(newName,"wb");
      if (out) {
        coff = fopen(argv[1], "rb");
        if (coff) {
          result = stat(argv[1], &fstat);
          if (!result) {
            /* load signed blessings section */
            bytesRead = fread(buf, 1, fstat.st_size, coff);
            if (bytesRead == fstat.st_size) {
              for (i = fstat.st_size; i < DRIVER_BLESS_SECTION_LENGTH; i++) {
                buf[i] = 0;
              }
              bytesWritten = fwrite(buf, DRIVER_BLESS_SECTION_LENGTH, 1, out);
            }
          }
          else {
            printf("Cannot stat file!\n");
            result = -4;
          }
          fclose(coff);
        }
        else {
          result = -3;
        }
        fclose(out);
      }
      else {
        result = -2;
      }
      free(newName);
    }
    else {
      result = -1;
    }
  }
  return result;
}

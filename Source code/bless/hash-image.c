#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <unistd.h>

/* RSAREF headers */
#include "global.h"
#include "md5.h"

#include "bless_defines.h"

/*
  hash-image.c

  The 'hash-image' utility creates a list of MD5 hashes of a UDF image file.
*/

#define UDF_HEADER_SIZE 0
#define UDF_SECTOR_SIZE (DVD_SECTOR_SIZE + UDF_HEADER_SIZE)
#define SECTOR_HEADER_SIZE 16

int main(int argc, char *argv[])
{
  char *buf, *newName, hash[16];
  unsigned long symbolTableOffset;
  size_t bytesWritten, bytesRead, count = 0, read;
  int result = 0, i, totalBytes;
  FILE *out, *udf;
  MD5_CTX ctx;
  struct stat fstat;

  if (argc != 2) {
    printf("Usage: %s UDF-image\n", argv[0]);
  }
  else {
    newName = (char *)malloc(strlen(argv[1]) + 6); /* append ".hash\0" */
    if (newName) {
      sprintf(newName, "%s.hash", argv[1]);
      out = fopen(newName,"wb");
      if (out) {
        udf = fopen(argv[1], "rb");
        if (udf) {
          buf = (char *)malloc(UDF_SECTOR_SIZE);
          if (buf) {
            totalBytes = 0;
            result = stat(argv[1], &fstat);
            if (!result) { /* stat successful */
              totalBytes = fstat.st_size;
              do {
                MD5Init(&ctx);
                bytesRead = 0;
                read = 0;
                do {
                  read = fread(buf, 1, UDF_SECTOR_SIZE, udf);
                  bytesRead += read;
                  totalBytes -= read;
                  if (read > UDF_HEADER_SIZE) {
                    MD5Update(&ctx, (buf + UDF_HEADER_SIZE), read);
                  }
                  else {
                    break;
                  }
                } while (bytesRead < HASH_BLOCK_SIZE);
                MD5Final(hash, &ctx);
                count = fwrite((void *)hash, 1, 16, out);
              } while(totalBytes > 0);
            }
          }
          else {
            result = -4;
          }
          fclose(udf);
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

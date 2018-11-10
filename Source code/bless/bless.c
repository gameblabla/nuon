#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>

/* RSAREF headers */
#include "global.h"
#include "md5.h"

#include "bless_defines.h"

/*
  bless.c

  The 'bless' utility creates a blessings section containing the correct MD5
  hash of a coff file.
*/

#define BUF_SIZE 32768

char bless_template[] = {
  /* blessings section - 72 bytes */
  /* 'Bles' magic number - 4 bytes */
  0x42, 0x6C, 0x65, 0x73,
  /* Blessings file format - 2 bytes */
  0x00, 0x01,
  /* Application type - 1 byte */
  0x00, 
  /* NUON Scrambling flag - 1 byte */
  0x00,
  /* NUON Scrambling key - 8 bytes*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* BCA code - 8 bytes */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* Perform Sector Header Pattern Test flag - 2 bytes */
  0x00, 0x00,
  /* Sector Header Pattern - 8 bytes*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* Perform 16-byte Signature Test flag - 2 bytes */
  0x00, 0x00,
  /* Signature Test Sector - 4 bytes */
  0x00, 0x00, 0x00, 0x00,
  /* 16-byte signature - 16 bytes */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* Game Rating - 2 bytes */
  0x00, 0x00,
  /* Speed/Compatibility setting - 2 bytes */
  0x00, 0x00,
  /* NUON region code - 4 bytes */
  0x00, 0x00, 0x00, 0x00,
  /* Executable MD5 hash - 16 bytes */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /* Offset to start of coff - 4 bytes */
  0x00, 0x00, 0x00, 0x00,
  /* Offset to strings list - 4 bytes */
  0x00, 0x00, 0x00, 0x00,
  /* Length of string list section - 4 bytes */
  0x00, 0x00, 0x00, 0x00,
  /* Offset to MD5 hash list - 4 bytes */
  0x00, 0x00, 0x00, 0x00,
  /* Length of MD5 hash list - 4 bytes */
  0x00, 0x00, 0x00, 0x00,
  /* Length, in sectors, of game.dat file - 4 bytes */
  0x00, 0x00, 0x00, 0x00,
  /* Start sector of disk check ignore region - 4 bytes */
  0x00, 0x00, 0x00, 0x00,
  /* Length in sectors of disk check ignore region - 4 bytes */
  0x00, 0x00, 0x00, 0x00
};
char sig_template[] = {
  /* signature section - 1024 bytes */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define COFF_HDR_SIZE 20
#define COFF_SECT_SIZE 44      /* size of each section header */
#define COFF_OPTHDR   16       /* ofs in hdr to get size of optional header */
#define SECT_HDR_FLAGS 40      /* ofs to section header flags */
#define SECT_HDR_SCNPTR 20     /* ofs to file pointer to section data */
#define SECT_HDR_SIZE 16     /* ofs to size of section in bytes */

#define FLAG_NOLOAD 0x02
#define FLAG_BSS 0x0080
#define FLAG_INFO 0x200

int main(int argc, char *argv[])
{
  char buf[BUF_SIZE], hdr[COFF_HDR_SIZE], sectHdr[BUF_SIZE], *newName, *sect, *tmp;
  unsigned long symbolTableOffset, numSections, offset;
  size_t bytesWritten, bytesRead, count = 0, sectHdrSize, sectSize, optHdrSize;
  int result = 0, datSectors = 0, i;
  FILE *out, *coff;
  MD5_CTX ctx;
  struct stat datStat;
  long flags;

  if (argc < 2) {
    printf("Usage: %s coff-file [app-type]\n", argv[0]);
  }
  else {
    newName = (char *)malloc(strlen(argv[1]) + 7); /* append ".bless\0" */
    if (newName) {
      sprintf(newName, "%s.bless", argv[1]);
      out = fopen(newName,"wb");
      if (out) {
        result = stat(DATA_FILE_NAME, &datStat);
        if (!result) {
          datSectors = (datStat.st_size + (DVD_SECTOR_SIZE - 1)) / DVD_SECTOR_SIZE;
        }
        result = 0;
        coff = fopen(argv[1], "rb");
        if (coff) {
          MD5Init(&ctx);
          /* now, load coff file and run md5 over it up until symbol table */
          bytesRead = fread(hdr, 1, COFF_HDR_SIZE, coff);
          symbolTableOffset = *((unsigned long *)(hdr + 8));
          symbolTableOffset = (symbolTableOffset & 0x000000FF) << 24 |
                              (symbolTableOffset & 0x0000FF00) << 8 |
                              (symbolTableOffset & 0x00FF0000) >> 8 |
                              (symbolTableOffset & 0xFF000000) >> 24;
          numSections = *((unsigned char *)hdr + 2) << 8 | *((unsigned char *)hdr + 3);
          optHdrSize = *((unsigned char *)hdr + COFF_OPTHDR) << 8 | *((unsigned char *)hdr + COFF_OPTHDR + 1);
          sectHdrSize = numSections * COFF_SECT_SIZE + COFF_HDR_SIZE + optHdrSize;
          fseek(coff, 0, SEEK_SET);
          /* load section headers */
          bytesRead = fread(sectHdr, 1, sectHdrSize, coff);
          /* hash section headers */
          MD5Update(&ctx, sectHdr, bytesRead);
          sect = sectHdr + COFF_HDR_SIZE + optHdrSize;
          /* loop for each section */
          for (i = 0; i < numSections; i++) {
            flags = *((long *)(sect + SECT_HDR_FLAGS));
            flags = htonl(flags);
            if (!(FLAG_NOLOAD & flags) && !(FLAG_INFO & flags) && !(FLAG_BSS & flags)) {
              sectSize = *((size_t *)(sect + SECT_HDR_SIZE));
              sectSize = htonl(sectSize);
              tmp = (char *)malloc(sectSize);
              if (tmp) {
                offset = *((long *)(sect + SECT_HDR_SCNPTR));
                offset = htonl(offset);
                fseek(coff, offset, SEEK_SET);
                bytesRead = fread(tmp, 1, sectSize, coff);
                if (bytesRead == sectSize) {
                  MD5Update(&ctx, tmp, sectSize);
                  free(tmp);
                  tmp = NULL;
                }
                else {
                  printf("Couldn't read section from file!\n");
                  fclose(coff);
                  fclose(out);
                  exit(-1);
                }
              }
              else {
                printf("Couldn't allocate memory for section!\n");
                fclose(coff);
                fclose(out);
                exit(-1);
              }
            }
            sect += COFF_SECT_SIZE;
          }
          /* now symbolTableOffset - count bytes left over in buf */
          MD5Final((unsigned char *)(bless_template + MD5_HASH_OFFSET), &ctx);
          /* fill in fields of blessings section */
          *((unsigned long *)(bless_template + COFF_START_OFFSET)) =
            htonl(APP_BLESS_TEMPLATE_LENGTH + APP_SIG_TEMPLATE_LENGTH);
          *((unsigned long *)(bless_template + HASH_LIST_OFFSET)) =
            htonl(APP_BLESS_TEMPLATE_LENGTH);
          if (datSectors > 0) {
            *((unsigned long *)(bless_template + DATA_FILE_LENGTH_OFFSET)) = htonl(datSectors);
          }
          /* set application type if passed in */
          if (argc >= 3) {
            /* reuse 'i' */
            i = atoi(argv[2]);
            *(bless_template + NUON_APP_TYPE) = i;
          }
          /* write blessings template */
          bytesWritten = fwrite(bless_template, APP_BLESS_TEMPLATE_LENGTH, 1, out);
          /* now fill in blank MD5 hashes */
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

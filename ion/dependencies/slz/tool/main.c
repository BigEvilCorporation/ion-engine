//***************************************************************************
// "main.c"
// Program entry point, parses command line and runs stuff as required
//***************************************************************************
// Slz compression tool
// Copyright 2011 Javier Degirolmo
//
// This file is part of the slz tool.
//
// The slz tool is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// The slz tool is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the slz tool.  If not, see <http://www.gnu.org/licenses/>.
//***************************************************************************

// Required headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "compress.h"
#include "decompress.h"

// Possible actions
enum {
   ACTION_DEFAULT,         // No action specified
   ACTION_COMPRESS,        // Compress
   ACTION_DECOMPRESS,      // Decompress
   ACTION_TOOMANY          // Too many actions specified
};

//***************************************************************************
// Program entry point
//***************************************************************************

int main(int argc, char **argv) {
   // To know if there was an error or not
   int errcode = 0;

   // Scan all arguments
   int show_help = 0;
   int show_ver = 0;
   int action = ACTION_DEFAULT;
   int format = FORMAT_DEFAULT;
   const char *infilename = NULL;
   const char *outfilename = NULL;

   int scan_ok = 1;
   int err_manyfiles = 0;

   int curr_arg;
   for (curr_arg = 1; curr_arg < argc; curr_arg++) {
      // Get pointer to argument, to make our lives easier
      const char *arg = argv[curr_arg];

      // If it's an option, parse it
      if (scan_ok && arg[0] == '-') {
         // Stop parsing options?
         if (!strcmp(arg, "--"))
            scan_ok = 0;

         // Show help or version?
         else if (!strcmp(arg, "-h") || !strcmp(arg, "--help"))
            show_help = 1;
         else if (!strcmp(arg, "-v") || !strcmp(arg, "--version"))
            show_ver = 1;

         // Compress?
         else if (!strcmp(arg, "-c") || !strcmp(arg, "--compress"))
            action = action == ACTION_DEFAULT ?
                     ACTION_COMPRESS : ACTION_TOOMANY;

         // Decompress?
         else if (!strcmp(arg, "-d") || !strcmp(arg, "--decompress"))
            action = action == ACTION_DEFAULT ?
                     ACTION_DECOMPRESS : ACTION_TOOMANY;

         // Specify format?
         else if (!strcmp(arg, "-16") || !strcmp(arg, "--slz16"))
            format = format == FORMAT_DEFAULT ?
                     FORMAT_SLZ16 : FORMAT_TOOMANY;
         else if (!strcmp(arg, "-24") || !strcmp(arg, "--slz24"))
            format = format == FORMAT_DEFAULT ?
                     FORMAT_SLZ24 : FORMAT_TOOMANY;

         // Unknown argument
         else {
            fprintf(stderr, "Error: unknown option \"%s\"\n", arg);
            errcode = 1;
         }
      }

      // Input filename?
      else if (infilename == NULL)
         infilename = arg;

      // Output filename?
      else if (outfilename == NULL)
         outfilename = arg;

      // Too many files specified?
      else
         err_manyfiles = 1;
   }

   // Look for error conditions
   if (action == ACTION_TOOMANY) {
      errcode = 1;
      fprintf(stderr, "Error: can't specify more than one action\n");
   } else if (!show_help && !show_ver) {
      if (infilename == NULL) {
         errcode = 1;
         fprintf(stderr, "Error: input filename missing\n");
      } else if (outfilename == NULL) {
         errcode = 1;
         fprintf(stderr, "Error: output filename missing\n");
      } else if (err_manyfiles) {
         errcode = 1;
         fprintf(stderr, "Error: too many filenames specified\n");
      }
   }
   if (format == FORMAT_TOOMANY) {
      errcode = 1;
      fprintf(stderr, "Error: too many formats specified\n");
   }

   // If there was an error then quit
   if (errcode)
      return EXIT_FAILURE;

   // No action specified?
   if (action == ACTION_DEFAULT)
      action = ACTION_COMPRESS;

   // No format specified?
   if (format == FORMAT_DEFAULT)
      format = FORMAT_SLZ16;

   // Show tool version?
   if (show_ver) {
      puts("1.2");
      return EXIT_SUCCESS;
   }

   // Show tool usage?
   if (show_help) {
      printf("Usage:\n"
             "  %s -c <infile> <outfile>\n"
             "  %s -d <infile> <outfile>\n"
             "\n"
             "Options:\n"
             "  -c or --compress ..... Compress a blob into SLZ\n"
             "  -d or --decompress ... Decompress SLZ into a blob\n"
             "  -16 or --slz16 ....... Use SLZ16 format (64KB limit)\n"
             "  -24 or --slz24 ....... Use SLZ24 format (16MB limit)\n"
             "  -h or --help ......... Show this help\n"
             "  -v or --version ...... Show tool version\n"
             "\n"
             "If no option is specified, compression is done by default.\n"
             "If no format is specified, SLZ16 is used by default.\n",
             argv[0], argv[0]);
      return EXIT_SUCCESS;
   }

   // Open input file
   FILE *infile = fopen(infilename, "rb");
   if (infile == NULL) {
      fprintf(stderr, "Error: can't open input file \"%s\"\n", infilename);
      return EXIT_FAILURE;
   }

   // Open output file
   FILE *outfile = fopen(outfilename, "wb");
   if (outfile == NULL) {
      fprintf(stderr, "Error: can't open output file \"%s\"\n", outfilename);
      fclose(infile);
      return EXIT_FAILURE;
   }

   // Perform the requested action
   switch (action) {
      // Compress file
      case ACTION_COMPRESS:
         errcode = compress(infile, outfile, format);
         break;

      // Decompress file
      case ACTION_DECOMPRESS:
         errcode = decompress(infile, outfile, format);
         break;

      // Oops!
      default:
         errcode = ERR_UNKNOWN;
         break;
   }

   // If there was an error, show a message
   if (errcode) {
      // Determine message to show
      const char *msg;
      switch(errcode) {
         case ERR_CANTREAD: msg = "can't read from input file"; break;
         case ERR_CANTWRITE: msg = "can't write to output file"; break;
         case ERR_TOOLARGE16: msg = "input file size can't be over "
            "65,535 bytes"; break;
         case ERR_TOOLARGE24: msg = "input file size can't be over "
            "16,777,215 bytes"; break;
         case ERR_CORRUPT: msg = "input file isn't valid SLZ"; break;
         case ERR_NOMEMORY: msg = "ran out of memory"; break;
         default: msg = "unknown error"; break;
      }

      // Show message on screen
      fprintf(stderr, "Error: %s\n", msg);
   }

   // Quit program
   fclose(outfile);
   fclose(infile);
   if (errcode) remove(outfilename);
   return errcode ? EXIT_FAILURE : EXIT_SUCCESS;
}

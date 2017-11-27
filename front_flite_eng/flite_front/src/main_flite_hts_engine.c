/* ----------------------------------------------------------------- */
/*           The English TTS System "Flite+hts_engine"               */
/*           developed by HTS Working Group                          */
/*           http://hts-engine.sourceforge.net/                      */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2005-2016  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                                                                   */
/*                2005-2008  Tokyo Institute of Technology           */
/*                           Interdisciplinary Graduate School of    */
/*                           Science and Engineering                 */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the HTS working group nor the names of its  */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission.   */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "flite_hts_engine.h"

#define INPUT_BUFF_SIZE 1024

/* usage: output usage */
static void usage(void)
{
   fprintf(stderr, "The English TTS System \"Flite+hts_engine\"\n");
   fprintf(stderr, "Version 1.07 (http://hts-engine.sourceforge.net/)\n");
   fprintf(stderr, "Copyright (C) 2005-2016  Nagoya Institute of Technology\n");
   fprintf(stderr, "              2005-2008  Tokyo Institute of Technology\n");
   fprintf(stderr, "All rights reserved.\n");

   fprintf(stderr, "CMU Flite\n");
   fprintf(stderr, "Version 2.0.0 (http://www.festvox.org/flite/)\n");
   fprintf(stderr, "Copyright (C) 1999-2014  Carnegie Mellon University\n");
   fprintf(stderr, "All rights reserved.\n");
   fprintf(stderr, "\n");
   fprintf(stderr, "flite_hts_engine - The English TTS system \"Flite+hts_engine\"\n");
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       flite_hts_engine [ options ] [ infile ] \n");
   fprintf(stderr, "  options:                                                           [  def][ min-- max]\n");
   fprintf(stderr, "    -m  htsvoice   : HTS voice                                       [  N/A]\n");
   fprintf(stderr, "    -o  wav        : filename of output wav audio (generated speech) [  N/A]\n");
   fprintf(stderr, "    -s  i          : sampling frequency                              [ auto][   1--    ]\n");
   fprintf(stderr, "    -p  i          : frame period (point)                            [ auto][   1--    ]\n");
   fprintf(stderr, "    -a  f          : all-pass constant                               [ auto][ 0.0-- 1.0]\n");
   fprintf(stderr, "    -b  f          : postfiltering coefficient                       [  0.0][ 0.0-- 1.0]\n");
   fprintf(stderr, "    -r  f          : speech speed rate                               [  1.0][ 0.0--    ]\n");
   fprintf(stderr, "    -fm            : additional half-tone                            [  0.0][    --    ]\n");
   fprintf(stderr, "    -u  f          : voiced/unvoiced threshold                       [  0.5][ 0.0-- 1.0]\n");
   fprintf(stderr, "    -jm f          : weight of GV for spectrum                       [  1.0][ 0.0--    ]\n");
   fprintf(stderr, "    -jf f          : weight of GV for Log F0                         [  1.0][ 0.0--    ]\n");
   fprintf(stderr, "    -g  f          : volume (dB)                                     [  0.0][    --    ]\n");
   fprintf(stderr, "    -z  i          : audio buffer size (if i==0, turn off)           [    0][   0--    ]\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "    text file                                                                [stdin]\n");
   fprintf(stderr, "\n");

   exit(0);
}

int main(int argc, char **argv)
{
   size_t i;
   char buff[INPUT_BUFF_SIZE];
   FILE *fp_txt = stdin;
   char *fn_txt = NULL;
   char *fn_label = NULL;
   char *fn_voice = NULL;


   /* parse command line */
   if (argc == 1)
      usage();


   /* get HTS voice file name */
   for (i = 0; i < argc; i++) {
      if (argv[i][0] == '-' && argv[i][1] == 'm')
         fn_voice = argv[++i];
      if (argv[i][0] == '-' && argv[i][1] == 'h')
         usage();
   }


   /* read command */
   while (--argc) {
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'm':             /* HTS voice was already loaded */
            ++argv;
            --argc;
            break;
         case 'o':
            fn_label = *++argv;
            --argc;
            break;
         case 'h':
            usage();
            break;
         default:
            fprintf(stderr, "flite_hts_engine: Invalid option '-%c'.\n", *(*argv + 1));
            exit(1);
         }
      } else {
         fn_txt = *argv;
      }
   }

   /* synthesis */
   if (fn_txt != NULL)
      fp_txt = fopen(fn_txt, "r");

   if (fgets(buff, INPUT_BUFF_SIZE, fp_txt) != NULL && strlen(buff) > 0)
   {
	   Flite_HTS_Engine_synthesize(buff, fn_label);
   }
      
   
   if (fn_txt != NULL)
      fclose(fp_txt);


   return 0;
}

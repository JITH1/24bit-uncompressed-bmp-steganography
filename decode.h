#ifndef DEDODE_H
#define DECODE_H

#include "steggo.h"

typedef struct
{
   /* Input bmp file */
   FILE *input_bmp_ptr ;
   char *input_bmp_fname;

   /* Output Secret File*/
   FILE *Output_secret_fptr ;
   char *output_secret_fname ;
   char output_secret_extn[5] ;
   long output_secret_size ;

   uint decoded_bytes ;

}Decode_Info ;

Status validate_decode_arguments(char *argv[]);
Status validate_decode_files(Decode_Info *decoInfo ,char *argv[]);

#endif 
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
   long output_file_ext_size ;

   uint decoded_bytes ;

}Decode_Info ;

Status validate_decode_arguments(char *argv[]);
Status validate_decode_files(Decode_Info *decoInfo ,char *argv[]);
Status do_decoding(Decode_Info *decoInfo); 
Status decode_magic_string(Decode_Info *decoInfo ,char *magic_string);
Status decode_extention_size(Decode_Info  *decoInfo);
Status decode_extention(Decode_Info *decoInfo);
Status decode_secret_file_size(Decode_Info *decoInfo);
Status decode_secret_data(Decode_Info *decoInfo);

void decode_bits(char *bits,char buffer[]);
void decode_size(uint *size ,char buffer[]);

#endif 
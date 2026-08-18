#ifndef ENCODE_H
#define ENCODE_H

#include "steggo.h"

typedef struct 
{
   /* Source File Elements */
   FILE *source_file_ptr ;
   char *source_file_name ;
   uint source_bmp_capacity  ;
   uint source_pixel_offset ;

   /* Secret File Elements */
   FILE *Secret_file_ptr ;
   char *Secret_file_name ;
   char secret_file_extn[5] ;
   char secret_file_data[200];
   long secret_file_size ;

   /* Output Bmp File Elements */
   FILE *Output_file_ptr ;
   char *Output_file_name ;

   uint bytes_encoded ;

}Encode_info;

Status Validate_encode_arguments(char *argv[]);
Status Validate_encode_files(Encode_info *encoInfo,char *argv[]);
Status extract_secret_file_extn_data(Encode_info *encoInfo);
Status Start_encoding(Encode_info *encoInfo);
Status Open_all_files(Encode_info *encoInfo);
Status add_bmp_header(Encode_info *encoInfo);
Status Encode_Magic_String(Encode_info *encoInfo,char *magic_str);
Status Encode_secret_file_extn_size(Encode_info *encoInfo);
Status Encode_secret_file_extn(Encode_info *encoInfo);
Status Encode_secret_file_size(Encode_info *encoInfo);
Status Encode_secret_file_data(Encode_info *encoInfo);
Status Encode_remaining(Encode_info *encoInfo);


Status Encode_bits(char bits,char buffer[]);
Status Encode_size(uint size,char buffer[]);

#endif
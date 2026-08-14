#ifndef ENCODE_H
#define ENCODE_H

#include "steggo.h"

typedef struct 
{
   /* Source File Elements */
   FILE *source_file_ptr ;
   char *source_file_name ;
   uint source_bmp_capacity  ;

   /* Secret File Elements */
   FILE *Secret_file_ptr ;
   char *Secret_file_name ;
   char secret_file_extn[5] ;
   char secret_file_data[200];
   long secret_file_size ;

   /* Output Bmp File Elements */
   FILE *Output_file_ptr ;
   char *Output_file_name ;

}Encode_info;

Status Validate_encode_arguments(Encode_info *encoInfo,char *argv[]);
Status Validate_encode_files(Encode_info *encoInfo,char *argv[]);

#endif
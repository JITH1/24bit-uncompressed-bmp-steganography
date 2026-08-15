#include "decode.h"
#include "steggo.h"

Status validate_decode_arguments(char *argv[])
{
    if(*argv[2] == '.')
    {
        printf(RED"\nInvalid .bmp file format (Input File)...!\n"RESET);
        return FAILURE ;
    }
    else if(*argv[3] == '.')
    {
        printf(RED"\nInvalid .txt file format (Output File)...!\n"RESET);
        return FAILURE ;
    }

    char *ptr1 = strstr(argv[2],".bmp");
    char *ptr2 = strstr(argv[3],".txt");

    if(ptr1 != NULL)
    {
       if(ptr1[4]!= '\0')
       {
          printf(RED"\nInvalid Input File Format expected .bmp ...!\n"RESET);
          return FAILURE ;
       }

    }    
    else 
    {
        printf(RED"\nInvalid Input File Format expected .bmp ...!\n"RESET);
        return FAILURE ;
    }

    if(ptr2 != NULL)
    {
       if(ptr2[4]!= '\0')
       {
          printf(RED"\nInvalid Input File Format expected .bmp ...!\n"RESET);
          return FAILURE ;
       }

    }    
    else 
    {
        printf(RESET"\nInvalid Output File Format Expected .txt ...!\n"RESET);
        return FAILURE ;
    }

    return SUCCESS ;
}

Status validate_decode_files(Decode_Info *decoInfo ,char *argv[])
{
    char buffer[5];
    uint read ;
    uint bit_type ;
 
    decoInfo->input_bmp_ptr = fopen(argv[2],"r");

    if(decoInfo->input_bmp_ptr == NULL)
    {
        printf(RED"\nCan't Open Input .bmp File...!\n"RESET);
        return FAILURE ;
    }

    read = fread(buffer,1,2,decoInfo->input_bmp_ptr);

    buffer[read] = '\0';

    if(!(strcmp(buffer,"BM") == 0))
    {
        printf(RED"\nThe input file is not .bmp type file...!\n"RESET);
        return FAILURE ;
    }

    fseek(decoInfo->input_bmp_ptr,26,SEEK_CUR);

    read = fread(&bit_type,1,2,decoInfo->input_bmp_ptr);
    
    if(!((short)bit_type == 24))
    {
        printf(RED"\nThe given Input bmp file is not 24 bit bmp..Unsupported Input for my project...!\n"RESET);
        return FAILURE ;
    }

    fread(&bit_type,4,1,decoInfo->input_bmp_ptr);

    if(!(bit_type == 0))
    {
        printf(RED"\nThe given Input bmp file is not Uncompressed...Unsupported Input for my project...!\n"RESET);
        return FAILURE ;
    }

    decoInfo->input_bmp_fname = argv[2] ;
    decoInfo->output_secret_fname = argv[3] ;

    fclose(decoInfo->input_bmp_ptr);
    
    return SUCCESS ;
    
}
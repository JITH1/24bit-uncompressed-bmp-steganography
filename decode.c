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

Status do_decoding(Decode_Info *decoInfo)
{
    decoInfo->decoded_bytes = 0;

    if(!decode_magic_string(decoInfo,MAGIC))
    {
        printf(RED"\n# Magic string decoding Failed...!\n"RESET);
        return FAILURE ;
    }

    if(!(decode_extention_size(decoInfo)))
    {
        printf(RED"\n# Decode extention size failed...!"RESET);
        return FAILURE ;
    }

    if(!(decode_extention(decoInfo)))
    {
        printf(RED"\n# Decode extention failed...!"RESET);
        return FAILURE ;
    }

    decode_secret_file_size(decoInfo);

    if(!decode_secret_data(decoInfo))
    {
        printf(RED"\nEncoded Secret data failed...!\n"RESET);
        return FAILURE ;
    }
    
    return SUCCESS ;
}

Status decode_magic_string(Decode_Info *decoInfo ,char *magic_string)
{
    decoInfo->input_bmp_ptr = fopen(decoInfo->input_bmp_fname,"r");
    
    char buffer[8];
    char magic[3] = {0};

    uint n = strlen(magic_string);

    int i ;

    fseek(decoInfo->input_bmp_ptr,54,SEEK_SET);

    for(i = 0 ; i<n ; i++)
    {
        fread(buffer,1,8,decoInfo->input_bmp_ptr);
        decode_bits(&magic[i],buffer);
    }

    magic[i] = '\0';

    if(!(strcmp(magic,magic_string) == 0))
    {
        printf(RED"\nRequired magic string is not found in %s message is not encoded in this file...!\n"RESET,decoInfo->input_bmp_fname);
        return FAILURE ;
    }

    printf(GREEN"\n# Magic string decoded successfully %s authentification successfull...!\n"RESET,decoInfo->input_bmp_fname);

    return SUCCESS ;
}

Status decode_extention_size(Decode_Info  *decoInfo)
{
    char buffer[32];
    uint size = 0;

    fread(buffer,1,32,decoInfo->input_bmp_ptr);
    decode_size(&size,buffer);
    decoInfo->output_file_ext_size = size ;
    
    if(decoInfo->output_file_ext_size != (long) 4 )
    {
        return FAILURE ;
    }

    printf(GREEN"\n# decoded extention size : %ld bytes\n"RESET,decoInfo->output_file_ext_size);

    return SUCCESS ;
}

Status decode_extention(Decode_Info *decoInfo)
{
    char buffer[8];
    int n = (int)decoInfo->output_file_ext_size , i;
    char extn[5] = {0};
      
    for(i = 0 ; i<n ; i++)
    {
        fread(buffer,1,8,decoInfo->input_bmp_ptr);
        decode_bits(&extn[i],buffer) ;
    } 
    
    extn[i] = '\0';

    if(!(strcmp(extn,".txt") == 0))
    {
        printf(RED"\nExpected extention .txt\n"RESET);
        return FAILURE;
    }

    strcpy(decoInfo->output_secret_extn,extn);

    printf(GREEN"\n# Decoded extention : %s \n"RESET,decoInfo->output_secret_extn);

    return SUCCESS ;
    
}

Status decode_secret_file_size(Decode_Info *decoInfo)
{
    uint n = 0 ;
    char buffer[32];

    fread(buffer,1,32,decoInfo->input_bmp_ptr);
    decode_size(&n,buffer);
    decoInfo->output_secret_size = n ;

    printf(GREEN"\n# Encoded secret data size Decoded : %ld bytes\n"RESET,decoInfo->output_secret_size);

    return SUCCESS ;
}

Status decode_secret_data(Decode_Info *decoInfo)
{
    long n = decoInfo->output_secret_size ;
    char buffer[8] ;

    decoInfo->Output_secret_fptr = fopen(decoInfo->output_secret_fname,"w");

    if(!decoInfo->Output_secret_fptr)
    {
        printf(RED"\nCant open file %s\n"RESET,decoInfo->output_secret_fname);
        return FAILURE ;
    }

    for(int i = 0 ; i<n ; i++)
    {
        char ch = 0;
        fread(buffer,1,8,decoInfo->input_bmp_ptr);
        decode_bits(&ch,buffer);
        fputc(ch,decoInfo->Output_secret_fptr);
    }

    if((ftell(decoInfo->Output_secret_fptr) == decoInfo->output_secret_size))
    {
        printf(GREEN"\n# Encoded Secret Data Successfully Decoded...!\n"RESET);
        return SUCCESS ;
    }
    else
    {
        return FAILURE ;
    }
    
}

void decode_size(uint *size ,char buffer[])
{
     for(int i = 0 ; i<32 ; i++)
     {
         uint n = buffer[i] & (1U) ;
         n = n<<(31-i);
         *size = *size | n ; 
     }
}

void decode_bits(char *bits,char buffer[])
{
    for(int i = 0 ; i<8 ; i++)
    {
        uint n = (buffer[i] & 1U) ;
        n = n<<(7-i);
        *bits = *bits | n ; 
    } 
}


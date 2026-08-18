#include "steggo.h"
#include "encode.h"

Status Validate_encode_arguments(char *argv[])
{
    if(*argv[2] == '.')
    {
        printf(RED"\nInvalid .bmp file format (Source File)...!\n"RESET);
        return FAILURE ;
    }
    else if(*argv[3] == '.')
    {
        printf(RED"\nInvalid .txt file format (Secret File)...!\n"RESET);
        return FAILURE ;
    }

    char *ptr1 = strstr(argv[2],".bmp");
    char *ptr2 = strstr(argv[3],".txt");

    if(ptr1 != NULL)
    {
       if(ptr1[4]!= '\0')
       {
          printf(RED"\nInvalid Source File Format expected .bmp ...!\n"RESET);
          return FAILURE ;
       }

    }    
    else 
    {
        printf(RED"\nInvalid Source File Format expected .bmp ...!\n"RESET);
        return FAILURE ;
    }

    if(ptr2 != NULL)
    {
       if(ptr2[4]!= '\0')
       {
          printf(RED"\nInvalid Source File Format expected .bmp ...!\n"RESET);
          return FAILURE ;
       }

    }    
    else 
    {
        printf(RESET"\nInvalid Secret File Format Expected .txt ...!\n"RESET);
        return FAILURE ;
    }

    return SUCCESS ;
   
}

Status Validate_encode_files(Encode_info *encoInfo,char *argv[])
{
    char buffer[5];
    uint read ;
    uint8_t arr[4];
    
    uint width , hight ;

    FILE *fsource = fopen(argv[2],"r");

    if(fsource == NULL)
    {
        printf("\nCan't open file...!\n");
        return FAILURE ;
    }

    read = fread(buffer,1,2,fsource);
    buffer[read] = '\0';

    if(!(strcmp(buffer,"BM") == 0))
    {
        printf(RED"\nThe Given File Is not .bmp File...!"RESET);
        return FAILURE ;
    }
    
    fseek(fsource,26,SEEK_CUR);

    read = fread(arr,1,2,fsource);
    
    if(!(*(short *)arr == 24))
    {
        printf(RED"\nThe given Source bmp file is not 24 bit bmp..Unsupported Input for my project...!\n"RESET);
        return FAILURE ;
    }

    fread(arr,4,1,fsource);

    if(!(*(int *)arr == 0))
    {
        printf(RED"\nThe given Source bmp file is not Uncompressed...Unsupported Input for my project...!\n"RESET);
        return FAILURE ;
    }

    encoInfo->source_file_name = argv[2];
    
    encoInfo->Secret_file_ptr = fopen(argv[3],"r");

    if(encoInfo->Secret_file_ptr == NULL)
    {
        printf(RED"\nCan't Open Messege File...!\n"RESET);
        return FAILURE;
    }

    encoInfo->Secret_file_name = argv[3];

    fseek(encoInfo->Secret_file_ptr,0,SEEK_END);

    read = ftell(encoInfo->Secret_file_ptr);

    encoInfo->secret_file_size  = read ;

    if(encoInfo->secret_file_size > (long) sizeof(encoInfo->secret_file_data))
    {
        printf(RED"\nPlease Increase Buffer size...!\n"RESET);
        return FAILURE ;
    }

    printf(GREEN"\n# The size of secret file %s is : %lu\n"RESET,encoInfo->Secret_file_name,encoInfo->secret_file_size);

    fseek(fsource,18,SEEK_SET);

    fread(&width,sizeof(width),1,fsource);
    fread(&hight,sizeof(hight),1,fsource);

    printf(GREEN"# The Width of %s is : %u\n",encoInfo->source_file_name,width);
    printf("# The hight of %s is %u\n"RESET,encoInfo->source_file_name,hight);

    encoInfo->source_bmp_capacity = width*hight*3 ;
    
    printf(GREEN"# The %s capacity is : %u\n"RESET,encoInfo->source_file_name,encoInfo->source_bmp_capacity);
    
    uint total_encoding_size = (uint)16 + (uint)32 + (uint)32 + (uint)32 + encoInfo->secret_file_size * 8 ;

    printf(GREEN"\nTotal Size Required For Encoding : %u\n",total_encoding_size);

    if(total_encoding_size <= encoInfo->source_bmp_capacity)
    {
        printf(GREEN"\n-> bmp file have Sufficient Size For Encoding Secret Data...!\n"RESET);
    }
    else
    {
        printf(RED"\nInsufficient Space For Encoding Secret Data...!\n"RESET);
        return FAILURE ;
    }

    fseek(fsource,10,SEEK_SET);

    fread(&encoInfo->source_pixel_offset,4,1,fsource);
    
    fclose(encoInfo->Secret_file_ptr);
    fclose(fsource);

    if(!extract_secret_file_extn_data(encoInfo))
    {
        return FAILURE ;
    }
    
    return SUCCESS ;

}

Status extract_secret_file_extn_data(Encode_info *encoInfo)
{
    char *ptr = strrchr(encoInfo->Secret_file_name,'.');

    if(!ptr)
    {
        printf(RED"\nError Can't Find Extention...!\n"RESET);
        return FAILURE ; 
    }

    strcpy(encoInfo->secret_file_extn,ptr);
    
    printf(GREEN"\n# Secret File Extn %s extracted \n"RESET,encoInfo->secret_file_extn);
    
    encoInfo->Secret_file_ptr = fopen(encoInfo->Secret_file_name,"r");

    int i = 0 ;

    char ch ;

    while((ch = fgetc(encoInfo->Secret_file_ptr))!= EOF)
    {
        encoInfo->secret_file_data[i] = ch ;
        i++;
    }
    
    encoInfo->secret_file_data[i] = '\0' ;

    printf(GREEN"# Secret File Data Extracted...!\n"RESET);

    fclose(encoInfo->Secret_file_ptr);
    
    return SUCCESS ;

}

Status Start_encoding(Encode_info *encoInfo)
{
    encoInfo->bytes_encoded = 0;

    if(!Open_all_files(encoInfo))
    {
        printf(GREEN"\nCan't Open Files...!\n"RESET);
        return FAILURE ;
    }

    if(!add_bmp_header(encoInfo))
    {
        printf(GREEN"\nHeader Encoding Failed...!\n"RESET);
        return FAILURE ;
    }
    
    if(!Encode_Magic_String(encoInfo,MAGIC))
    {
        printf(RED"\nMagic String Encoding Failed...!\n"RESET);
        return FAILURE ;
    }
    
    if(!Encode_secret_file_extn_size(encoInfo))
    {
        printf(RED"\nSecret File Extn Encoding Failed...!\n"RESET);
        return FAILURE ;
    }
    
    if(!Encode_secret_file_extn(encoInfo))
    {
        printf(RED"\n# Secret File Extn Failed...!\n"RESET);
        return FAILURE ;
    }
   
    if(!Encode_secret_file_size(encoInfo))
    {
        printf(RED"\n# Secret File Size Encoding Failed...!\n"RESET);
        return FAILURE ;
    }
    
    if(!Encode_secret_file_data(encoInfo))
    {
        printf(RED"\n# Secret File Data Encoding Failed...!\n"RESET);
        return FAILURE ;
    }
    
    if(!Encode_remaining(encoInfo))
    {
        printf(RED"\n# Pixel Encoding Failed...!\n"RESET);
        return FAILURE ;
    }

    return SUCCESS ; 

}

Status Open_all_files(Encode_info *encoInfo)
{ 
     encoInfo->source_file_ptr = fopen(encoInfo->source_file_name,"r");

     if(!encoInfo->source_file_ptr)
     {
        printf(RED"\nCan't Open Source File..!\n"RESET);
        return FAILURE;
     }

     encoInfo->Secret_file_ptr = fopen(encoInfo->Secret_file_name,"r");

     if(!encoInfo->Secret_file_ptr)
     {
        printf(RED"\nCan't Open Secret File..!\n"RESET);
        return FAILURE;
     }

     encoInfo->Output_file_name = "Steggo_out.bmp";

     encoInfo->Output_file_ptr = fopen(encoInfo->Output_file_name,"w");

     if(!encoInfo->Output_file_ptr)
     {
        printf(RED"\nCan't Create/open Output File...!\n"RESET);
        return FAILURE ;
     }

     printf(GREEN"\n# File's Opened Successfully...!\n"RESET);

     return SUCCESS ;

} 

Status add_bmp_header(Encode_info *encoInfo)
{

     if(encoInfo->source_pixel_offset != 54)
     {
        printf("Unsupported BMP header layout\n");
        return FAILURE;
     }

     uint8_t buffer[54];
     
     fread(buffer,1,encoInfo->source_pixel_offset,encoInfo->source_file_ptr);
     
     uint write = fwrite(buffer,1,54,encoInfo->Output_file_ptr);

     printf(GREEN"\n# bmp Header %u bytes Copied to output file...!\n"RESET,write);
     
     encoInfo->bytes_encoded = encoInfo->bytes_encoded + write ;

     printf(GREEN"\n>> Bytes encoded : %u bytes\n",encoInfo->bytes_encoded);

     return SUCCESS ;
}

Status Encode_Magic_String(Encode_info *encoInfo,char *magic_str)
{
     uint n = strlen(magic_str);
     uint write = 0 ;

     char buffer[8];

     for(unsigned int i = 0 ; i<n ; i++)
     {
        fread(buffer,1,8,encoInfo->source_file_ptr);
        Encode_bits(magic_str[i],buffer);
        write = write + fwrite(buffer,1,8,encoInfo->Output_file_ptr); 
     }
     
     encoInfo->bytes_encoded = encoInfo->bytes_encoded + write ;
     
     printf(GREEN"\n>> Bytes encoded : %u bytes\n",encoInfo->bytes_encoded);

     printf("\n# Magic String Successfully Encoded\n"RESET);

     return SUCCESS ;
}

Status Encode_secret_file_extn_size(Encode_info *encoInfo)
{
    char buffer[32];
    uint size = strlen(encoInfo->secret_file_extn);

    fread(buffer,1,32,encoInfo->source_file_ptr);

    Encode_size(size,buffer);

    encoInfo->bytes_encoded = encoInfo->bytes_encoded + fwrite(buffer,1,32,encoInfo->Output_file_ptr);
    
    printf(GREEN"\n>> Bytes encoded : %u bytes\n",encoInfo->bytes_encoded);

    printf("\n# Secret File Extn Size Encoded...!\n"RESET);
    
    return SUCCESS ;

}

Status Encode_secret_file_extn(Encode_info *encoInfo)
{
    uint size = strlen(encoInfo->secret_file_extn);

    char buffer[8] ;

    uint i ;
    
    for( i = 0 ; i<size ;  i++)
    {
        fread(buffer,1,8,encoInfo->source_file_ptr);

        Encode_bits(encoInfo->secret_file_extn[i],buffer);

        encoInfo->bytes_encoded = encoInfo->bytes_encoded + fwrite(buffer,1,8,encoInfo->Output_file_ptr); 
    }

    printf(GREEN"\n>> Bytes encoded : %u bytes\n",encoInfo->bytes_encoded);

    printf("\n# Secret File Extn Encoded...!\n"RESET);

    return SUCCESS ;

}

Status Encode_secret_file_size(Encode_info *encoInfo)
{
    char buffer[32];
    
    fread(buffer,1,32,encoInfo->source_file_ptr);

    Encode_size(encoInfo->secret_file_size,buffer);

    encoInfo->bytes_encoded = encoInfo->bytes_encoded + fwrite(buffer,1,32,encoInfo->Output_file_ptr);

    printf(GREEN"\n>> Bytes encoded : %u bytes\n",encoInfo->bytes_encoded);

    printf("\n# Secret File Size Encoded...!\n"RESET);

    return SUCCESS ;
}

Status Encode_secret_file_data(Encode_info *encoInfo)
{
    char buffer[8];

    uint size = encoInfo->secret_file_size ;
    
    uint i;
    
    for( i = 0 ; i<size ; i++)
    {
        fread(buffer,1,8,encoInfo->source_file_ptr);
        Encode_bits(encoInfo->secret_file_data[i],buffer);
        encoInfo->bytes_encoded = encoInfo->bytes_encoded + fwrite(buffer,1,8,encoInfo->Output_file_ptr); 
    }
    
    printf(GREEN"\n>> Bytes encoded : %u bytes\n",encoInfo->bytes_encoded);

    printf("\n# Secret File Data Encoded...!\n"RESET);

    return SUCCESS ;
    
}

Status Encode_remaining(Encode_info *encoInfo)
{
    char buffer[32];

    size_t read ;

    while((read = fread(buffer,1,32,encoInfo->source_file_ptr)) > 0)
    {
        encoInfo->bytes_encoded = encoInfo->bytes_encoded + fwrite(buffer,1,read,encoInfo->Output_file_ptr);
    }

    return SUCCESS ;
}

Status Encode_bits(char bits,char buffer[])
{
    for(int i = 0; i<8 ; i++)
    {
        uint n = (bits >> (7-i)) & 1 ;
        buffer[i] = (buffer[i] & 0xFE ) | n ;
    }

    return SUCCESS;
}

Status Encode_size(uint size,char buffer[])
{
    for(int i = 0 ; i<32 ; i++)
    {
        unsigned int n = (size >> (31 - i)) & 1 ;
        buffer[i] = (buffer[i] & 0xFE) | n ;
    }

    return SUCCESS ;
}


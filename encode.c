#include "steggo.h"
#include "encode.h"

Status Validate_encode_arguments(Encode_info *encoInfo,char *argv[])
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
        printf(GREEN"\nSufficient Size For Encoding Secret Data...!\n"RESET);
    }
    else
    {
        printf(RED"\nInsufficient Space For Encoding Secret Data...!\n"RESET);
        return FAILURE ;
    }

    fclose(encoInfo->Secret_file_ptr);
    fclose(fsource);

    return SUCCESS ;

}
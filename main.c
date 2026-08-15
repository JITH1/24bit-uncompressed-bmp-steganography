#include "steggo.h"
#include "encode.h"
#include "decode.h"

Operation_type validate_operation(char *argv[])
{
    if(strcmp(argv[1],"-e") == 0)
    {
        return type_encode;
    }
    else if(strcmp(argv[1],"-d") == 0)
    {
        return type_decode ;
    }
    else 
    {
        return type_unsupport ;
    }

}

int main(int argc ,char *argv[])
{
    int n ;

    Encode_info encoInfo ;
    Decode_Info decoInfo ; 

    if(argc == 4)
    {
        if((n = validate_operation(argv)) == type_unsupport)
        {
            printf(RED"\nUnsuppoerted Operation Flag...Expected Flag (-e/-d)..!\n"RESET);
            return FAILURE ;
        }
    }
    else if(argc < 4)
    {
        printf(RED"\nInsufficient Number Of Arguments...!\n"RESET);
        return FAILURE ;
    }
    else if(argc>4)
    {
        printf(RED"\nToo Many Arguments...!\n"RESET);
        return FAILURE ;
    }

    if(n == type_encode)
    {
        if(Validate_encode_arguments(&encoInfo,argv))
        {
            printf(GREEN"\nEncode Argument Validation Successfull...!\n"RESET);

            if(Validate_encode_files(&encoInfo,argv))
            {
                printf(GREEN"\n-> Encode File Validation Successfull...!\n"RESET);

                printf(GREEN"\n-> Proceeding to Encoding .... ! \n"RESET);

                if(Start_encoding(&encoInfo))
                {
                    printf(GREEN"\nEncoding Successfull...!\n\n"RESET);
                }
                else
                {
                    printf(RED"\nEncoding Failed...!\n"RESET);
                    return FAILURE ;
                }
                
            }
            else
            {
                printf(RED"\nEncode File Validation Successfull...!\n"RESET);
                return FAILURE ;
            }
        } 
        else
        {
            printf(RED"\nEncode Argument Validation Failed...!\n"RESET);
        }
    }
    else if(n == type_decode)
    {
        if(validate_decode_arguments(argv))
        {
            printf(GREEN"\nDecode Argument Validation Successfull...!\n"RESET);
            
            if(validate_decode_files(&decoInfo,argv))
            {
                printf(GREEN"\n-> Decode File Validation Successfull...!\n"RESET);
                printf(GREEN"\n-> Proceeding to Decoding .... ! \n"RESET);
                 
                if(do_decoding(&decoInfo))
                {
                    printf(GREEN"\nDecoding Successfull...!\n"RESET);
                    return SUCCESS ;
                }
                else
                {
                    printf(GREEN"\nDecoding Failed...!\n\n"RESET);
                    return FAILURE ;
                }
                
            }
            else
            {
                printf(RED"\nDecode File Validation Failed...!\n"RESET);
                return FAILURE ;
            }
        }
        else
        {
            printf(RED"\nDecode Argument Validation Failed...!\n"RESET);
            return FAILURE ;
        }
    }

    return 0 ;

}
#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    //call OperationType check_operation_type(argv[1][1]==e_encode)
    char opt=argv[1][1];
    if(check_operation_type(opt)==e_encode)
    {
    if(read_and_validate_encode_args(argv,&encInfo)==e_failure)
    {
        printf("Argument validation failed\n");
        return 1;
    }
    printf("Encoding started\n");
    if(do_encoding(argv,&encInfo)!=e_success)
    {
        printf("encoding failed\n");
        return 1;
    }
    printf("encoding complited successfully\n");
    return 0;
    }
    else if(check_operation_type(opt)==e_decode)
    {
        if(read_and_validate_dec_args(argv,&decInfo)==e_failure)
    {
        printf("Argument validation failed\n");
        return 1;
    }
    printf("decoding started\n");
    if(do_decoding(&decInfo)!=e_success)
    {
        printf("decoding failed\n");
        return 1;
    }
    printf("decoding complited successfully\n");
    return 0;

    }
    else
    {
        printf("Unsupported");
    }
}
OperationType check_operation_type(char opt)
{
    /* if opt is 'e'
    return e_encode
    if opt is'd'
    return e_decode
    else
    return e_unsupported
    */
    if(opt=='e')
    {
        return e_encode;
    }
    else if(opt=='d')
    {
        return e_decode;
    }
    else 
    {
        return e_unsupported;
    }
}

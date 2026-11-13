#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"



Status read_and_validate_dec_args(char *argv[], DecodeInfo *decInfo)
{
  decInfo->fptr_src_image=fopen(argv[2],"rb");
  if(decInfo->fptr_src_image==NULL)
  {
    return e_failure;
  }
  else
  {
    char *bmp=strstr(argv[2],".bmp");
    if(bmp==NULL)
    {
       
        return e_failure;
    }
    else
    {
        if(*(bmp+4)!='\0')
        {
           
            return e_failure;
        }
        else
        {
           
            decInfo->src_image_fname=argv[2];
        }
    }
  }
  
  decInfo->fptr_secret=fopen(argv[3],"r");
  if(decInfo->fptr_secret==NULL)
  {
    
    strcpy(decInfo->secret_fname,"decode");
    return e_success;
  }
  else
  {
    char *tok=strtok(argv[3],".");
    strcpy(decInfo->secret_fname,tok);
    
    return e_success;
  }
}


Status do_decoding(DecodeInfo *decInfo)
{
 if(skip_bmp_header(decInfo->fptr_src_image)==e_failure)
 {
  return e_failure;
 }

 if(dec_magic_string(MAGIC_STRING, decInfo) == e_failure)
 {
  return e_failure;
 }

if(dec_secret_file_extn_size(decInfo) == e_failure)
{
    return e_failure;
}

if(dec_secret_file_extn(decInfo) == e_failure)
{
    return e_failure;
}

if(dec_secret_file_size(decInfo) == e_failure)
{
    return e_failure;
}

if(dec_secret_file_data(decInfo) == e_failure)
{
    return e_failure;
}

fclose(decInfo->fptr_secret);
fclose(decInfo->fptr_src_image);
return e_success;
}

Status skip_bmp_header(FILE *fptr_src_image)
{
  if(fseek(fptr_src_image,54,SEEK_SET)!=0)
  {

   
    return e_failure;

  }
  else
  {

    
    return e_success;

  }
}



Status dec_magic_string(char *magic_string, DecodeInfo *decInfo)
{
  char buff[8];
  char decode;

  for(int i=0;i<strlen(magic_string);i++)
  {
    if(fread(buff,8,1,decInfo->fptr_src_image)!=1)
    {

      
      return e_failure;

    }
    else
    {
        dec_byte_from_lsb(&decode,buff);
        if(decode !=magic_string[i])
        {

            
            return e_failure;

        }
    }
  }
  return e_success;
}




Status dec_byte_from_lsb(char *data,char *buff)
{
    *data = 0;

    for(int k=0;k<8;k++)
    {
        if(buff[k]&1)
        {
          *data=*data|(1<<(k));
        }
    }

    return e_success;
}




Status dec_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buff[32];
    int size;

    if(fread(buff,32,1,decInfo->fptr_src_image)!=1)
    {

        
        return e_failure;

    }
    dec_size_from_lsb(&size,buff);
    decInfo->secret_file_extn_size=size;

    
    return e_success;
}




Status dec_size_from_lsb(int *size,char *buff)
{
    *size=0;
    for(int i=0;i<32;i++)
    {
        if(buff[i]&1)
        {
          *size=*size|(1<<(i));
        }
    }

    return e_success;
}





Status dec_secret_file_extn(DecodeInfo *decInfo)
{
    
    char extension[decInfo->secret_file_extn_size+1];
    char buff[8];

    for(int i=0;i<decInfo->secret_file_extn_size;i++)
    {
        if(fread(buff,8,1,decInfo->fptr_src_image)!=1)
        {

            
            return e_failure;

        }

        dec_byte_from_lsb(&extension[i],buff);
    }
    extension[decInfo->secret_file_extn_size]='\0';
    strcat(decInfo->secret_fname,extension);

    decInfo->fptr_secret=fopen(decInfo->secret_fname,"w");
    if(decInfo->fptr_secret==NULL)
    {

        
        return e_failure;

    }
    
    return e_success;
}





Status dec_secret_file_size(DecodeInfo *decInfo)
{
    int size;
    char buff[32];

    if(fread(buff,32,1,decInfo->fptr_src_image)!=1)
    {

      
      return e_failure;

    }
    dec_size_from_lsb(&size,buff);
    decInfo->size_secret_file=size;
    
    

    
    return e_success;
}






Status dec_secret_file_data(DecodeInfo *decInfo)
{
    char buff[8];
    char data;

    for(int i=0;i<decInfo->size_secret_file;i++)
    {
        if(fread(buff,8,1,decInfo->fptr_src_image)!=1)
        {

         
         return e_failure;

        }

        dec_byte_from_lsb(&data,buff);

        if(fwrite(&data,1,1,decInfo->fptr_secret)!=1)
        {

          
          return e_failure;

        }
        
    }

    
    return e_success;
}


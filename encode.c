#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    /*
       ->check the argv[2] must be having ".bmp" extern at last 4 char
       if yes,encInfo->src_image_fname=argv[2]
        open the file in read mode arrgv[2]
       encInfo->fptr_src_image=fopen()
       if the file not present return e_failure
       if not,return e_failure
       encInfo->fptr_src_image=fopen("argv[2]","r");
    */  
   encInfo->fptr_src_image=fopen(argv[2],"rb");
    if(encInfo->fptr_src_image==NULL)
    {
        return e_failure;
    }
    char *bmp=strstr(argv[2],".bmp");
    if(bmp==NULL)
    {
        return e_failure;
    }
    if(*(bmp+4)!='\0')
    {
        return e_failure;
    }
    encInfo->src_image_fname=argv[2];

    /*
       -> check the argv[3] is a valid file
       encInfo->secret_fname=argv[3]
       is the file is present or not 
       if present,encInfo->fptr_secret=fopen()
       if not,return e_failure
    */
     encInfo->fptr_secret=fopen(argv[3],"r");
     if(encInfo->fptr_secret==NULL)
     {
        return e_failure;
     }
     encInfo->secret_fname=argv[3];
    
    /*  

       ->check the argv[4] id null
       if not null,encInfo->stego_image_fname=argv[4]
       if null,encInfo->stego_image_fname="default.bmp"
       check the file extn is only of ".bmp"
       if not, return e_failure
       open the file in write mode and store in file pointer


    */
   if(argv[4]==NULL)
   {
    encInfo->stego_image_fname="default.bmp";
    encInfo->fptr_stego_image=fopen(encInfo->stego_image_fname,"wb");

   }
   else
   {

    encInfo->stego_image_fname=argv[4];
    encInfo->fptr_stego_image=fopen(encInfo->stego_image_fname,"wb");
    return e_success;
   }
    char *bmp2=strstr(encInfo->stego_image_fname,".bmp");
    if(bmp2==NULL)
    {
        return e_failure;
    }
    if(*(bmp2+4)!='\0')
    {
        return e_failure;
    }
    return e_success;
}    

Status do_encoding(char *argv[],EncodeInfo *encInfo)
{
       /*
          ->check_capacity(encInfo)==e_failure
          print error msg,return e_failure
          ->copy_bmp_header(encInfo ->fptr_src_image,encInfo->fptr_stego_image)==e_failure
          print error msg ,return e_failure 
          ->encode_magic_string(MAGIC_STRING,encInfo)==e_failure
          print error msg ,return e_failure 

          ->char *dot=strchr(arg[3],'.')
          ->strcpy(extn_secret_file,dot)
          ->encode_secret_file_extn_size(strlen(extn_secret_file),encInfo)==e_failure
           print error msg ,return e_failure 
          ->encode_secret_file_extn(extn_secret_file,encInfo)==e_failure
          print error msg ,return e_failure 
          ->encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_failure
          print error msg ,return e_failure
          ->encode_secret_file_data(encInfo)==e_failure
          print error msg ,return e_failure
          ->copy_remaining_img_data(fptr_src_image,fptr_stego_image)==e_failure
          print error msg ,return e_failure

        */
       printf("checking capacity\n");
       if(check_capacity(encInfo)==e_failure)
       {
        printf("Error:Insuficient capacity in the destination image\n");
        return e_failure;
       }
       printf("capacity check passed\n");
       if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
       {
        printf("Error:Failed to copy BMP header\n");
        return e_failure;
       }
       printf("bmp header copied\n");
       if(encode_magic_string(MAGIC_STRING,encInfo)==e_failure)
       {
        printf("Error:Failed to encode magic string\n");
        return e_failure;
       }
       printf("magic string copied\n");
       char *dot=strchr(argv[3],'.');
       strcpy(encInfo->extn_secret_file,dot);
       if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_failure)
       {
        printf("Error:Failed to encode secret file extension size\n");
        return e_failure;
       }
       if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_failure)
       {
        printf("Error:Failed to encode secret file extension\n");
        return e_failure;
       }
       if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_failure)
       {
         printf("Error:Failed to encode secret file size\n");
         return e_failure;
       }
       if(encode_secret_file_data(encInfo)==e_failure)
       {
         printf("Error:Failed to encode secret file data\n");
         return e_failure;
       }
       if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
       {
          printf("Error:Failed to copy remaining image data\n");
          return e_failure;
       }
       printf("All encoding steps completed\n");
       return e_success;
       
}
Status check_capacity(EncodeInfo *encInfo)
{
        /*
           ->encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
           ->get_image_size_for_bmp(encInfo->fptr_src_image) >
           ((get_file_size(encInfo->fptr_secret)+14)*8)
           return e_success
           else
           return e_failure
        */
      encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
      if(get_image_size_for_bmp(encInfo->fptr_src_image)>(encInfo->size_secret_file+14)*8)
      {
        rewind(encInfo->fptr_secret);
        return e_success;
       
      }
      else
      {
        return e_failure;
      }
}
uint get_file_size(FILE *fptr)
{
        /*
           ->fseeek()to SEEK_END
            return ftell()        
        */
       fseek(fptr,0,SEEK_END);
       return ftell(fptr);
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
        /* 
        
          *rewind(),src and secret files
          *declare a buff of 54 bytes,char buff[54]
          *read 54 bytes from src image to buff
          *write 54 bytes from buff to dest image
         */

         char buff[54];
         rewind(fptr_src_image);
         if(fread(buff,1,54,fptr_src_image)!=54)
         {
          printf("failed to read bmp header\n");
            return e_failure;
         }
         if(fwrite(buff,1,54,fptr_dest_image)!=54)
         {
          printf("failed to write bmp header\n");
            return e_failure;
         }
         return e_success;
         
         

}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
        /*
           ->declare a buff of 8 byte
           ==>run aloop for length of magic string
           ->read 8 byte from src_image into buff
           ->encode_byte_to_lsb(magic_string[],buff);
           ->write 8 bytes of encoded data from buffer to our dest_image

           return e_success
        
        */
       char buff[8];
       for(int i=0;i<strlen(magic_string);i++)
       {
         if(fread(buff,1,8,encInfo->fptr_src_image)!=8)
         {
            return e_failure;
         }
         encode_byte_to_lsb(magic_string[i],buff);

         if(fwrite(buff,1,8,encInfo->fptr_stego_image)!=8)
         {
           return e_failure;
         }
       }
       return e_success;

}
Status encode_byte_to_lsb(char data, char *image_buffer)

{
        /*

         ==loop for 8 times
         check the nth bit is set or not
         ->if set ,set the LSB of image_buffer[]
         ->if clear ,clear the LSB of image_buffer[]
         return e_success
        
        */
       for(int i=7;i>=0;i--)
       {
        if(data&(1<<i))
        {
            image_buffer[i]=image_buffer[i]|1;
        }
        else
        {
          image_buffer[i]=image_buffer[i]&(~1);
        }

       }
       return e_success;
}
Status encode_secret_file_extn_size(uint size,EncodeInfo *encInfo)
{
        /*
         ->declare a buff of 32 bytes
         ->read 32 bytes of buff from src_image
         ->encode_size_to_lsb(size,buff)
         ->write buff to dest_image
         ->return e_success
        */
       char buff[32];
       if(fread(buff,32,1,encInfo->fptr_src_image)!=1)
       {
        return e_failure;
       }
       encode_size_to_lsb(size,buff);
       if(fwrite(buff,32,1,encInfo->fptr_stego_image)!=1)
       {
        return e_failure;
       }
       return e_success;
       

}

Status encode_size_to_lsb(uint size,char *image_buffer)
{
        /*
        ->loop for 32 times
        *check the nth bit is set or not
        ->if set,set the lsb of image_buffer[]
        ->if clear,clear the lsb of image_buffer[]
        return e_success
        
        
        */
       for(int i=31;i>=0;i--)
       {
        if(size&(1u<<i))
        {
            image_buffer[i]=image_buffer[i]|1;
        }
        else
        {
          image_buffer[i]=image_buffer[i]&(~1);
        }

       }
       return e_success;
}
Status encode_secret_file_extn(const char *file_extn,EncodeInfo *encInfo)
{
        /*
        ->declare a buff of 8 byes
        ->run a loop for length of file_extn
        ->read 8 bytes from src_image into buff
        ->encode_bytes_to_lsb(file_extn[],buff);
        ->write 8 bytes of encoded data from buff to dest_image
        ->return e_success
        */
     char buff[8];
     for(int i=0;i<strlen(file_extn);i++)
     {
       if(fread(buff,8,1,encInfo->fptr_src_image)!=1)
       {
        return e_failure;
       }
       encode_byte_to_lsb(file_extn[i],buff);
       if(fwrite(buff,8,1,encInfo->fptr_stego_image)!=1)
       {
        return e_failure;
       }
      
     }
     return e_success;


}
Status encode_secret_file_size(long file_size,EncodeInfo *encInfo)
{
        /*
         ->declare a buff of 32
         ->read 32 bytes of buff from src_image
         ->encode_size_to_lsb(size,buff)
         ->write buff to dest_image
         ->return e_success
        
        */
       char buff[32];
       if(fread(buff,32,1,encInfo->fptr_src_image)!=1)
       {
        return e_failure;
       }
       encode_size_to_lsb(file_size,buff);
       if(fwrite(buff,32,1,encInfo->fptr_stego_image)!=1)
       {
        return e_failure;
       }
       return e_success;
     

}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
        /*
          ->declare a buff of 8 bytes
          ->char buff[8],data
          ->run a loop for secret_file_size times
          ->read 8 bytes from src_image into buff
          ->read a char from secret_file to data
          ->encode_bytes_to_lsb(data,buff);
          ->write 8 bytes of encoded data from buff to dest_image
          ->return e_success
        */
        char buff[8];
        char data;
        for(int i=0;i<encInfo->size_secret_file;i++)
        {
          if(fread(buff,8,1,encInfo->fptr_src_image)!=1)
          {
            return e_failure;
          }
          data=fgetc(encInfo->fptr_secret);
          encode_byte_to_lsb(data,buff);
          if(fwrite(buff,8,1,encInfo->fptr_stego_image)!=1)
          {
            return e_failure;
          }
        }
        return e_success;

}
Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)
{
        /*
          ->run a loop till EOF
          ->read a char from fptr_src
          ->write the char to fptr_dest
        
        */
            int ch;
            while((ch=fgetc(fptr_src))!=EOF)
            {
                if(fputc(ch,fptr_dest)==EOF)
                {
                    return e_failure;
                }
            }
          return e_success;
        
}

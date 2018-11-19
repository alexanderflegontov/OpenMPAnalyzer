#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

int main(int argc, char* argv[])
{
    const int PASSWARD_LENGTH = 256;
    char string[PASSWARD_LENGTH] = "\0";

    if(argc > 1)
    {
       int len = strlen(argv[1]);
       if((0 < len) && (len < PASSWARD_LENGTH))
       {
         memcpy(string, argv[1], len);
       }
       else
       {
          printf("give me password! \n");
          return 0;
       }
    }
    else
    {
	printf("give me password! \n");
  	return 0;
    }

    unsigned char digest[SHA256_DIGEST_LENGTH];   
    SHA256((unsigned char*)string, strlen(string), (unsigned char*)&digest);    
 
    char mdString[SHA256_DIGEST_LENGTH*2+1];
 
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
         sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
 
    //printf("SHA256 digest(%s): %s\n", string, mdString);
    printf("%s\n", mdString);
 
    return 0;
}

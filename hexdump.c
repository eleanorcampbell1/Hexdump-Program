#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "hexdump.h"

/**
 * Generate the hexdump of infile, and write result to outfile
 * 1. return 1 if success, and 0 otherwise
 * 2. outfile should not be created if infile does not exist
 */
int hexdump(const char * infile, const char * outfile) {

    int fpin = open(infile, O_RDONLY);      //open infile
    
    if(fpin == -1){                         //check for valid infile
        printf("Invalid file\n");
        perror("Failed: ");
        return 0;
    }
    
    FILE * fpout = fopen(outfile, "w+");    //open or create outfile
    if(fpout == NULL){                      //outfile open failed
        printf("exit at fpout\n");
        perror("Failed");
        return 0;
    }
    int offset = 0;
    int byteswritten = 0;
    int bytechunkwritten = 0;
    char* singlespace = " ";
    char* doublespace = "  ";
    fprintf(fpout, "%08x  ", offset);       // write first offset
    unsigned char buf[2];
    char hex[5];
    char hex2[5];

    struct stat st;                         //get size of infile
    stat(infile, &st);
    int size = st.st_size;
    
    for(int i = 0; i < (size/2); i++){
        if(bytechunkwritten == 2){
            fprintf(fpout, "\n%08x  ", offset);
            bytechunkwritten = 0;
        }
        int bytesread = read(fpin, buf, 2);

        if(bytesread == 2){                     //write a pair of bytes
            sprintf(&hex[0], "%02x", buf[1]);
            sprintf(&hex[2], "%02x", buf[0]);

            fwrite(&hex, 2, 2, fpout);
            byteswritten = byteswritten + 2;
            offset = offset + 2;
        }else if(bytesread == 1){               //write 1 byte
            sprintf(&hex[0], "%04x", buf[0]);
            fwrite(&hex, 1, 1, fpout);
            byteswritten++;
            offset++;
        }else{
            return 0;
        }
        if(byteswritten == 16){
            bytechunkwritten++;
            fprintf(fpout, "%s", doublespace);
            byteswritten = 0;                   //reset byteswritten to 0
            
        }else{
            fprintf(fpout, "%s", singlespace);
        }
        
    }
    if((size%2 != 0)){
        read(fpin, buf, 1);
        sprintf(hex2, "%04x", buf[0]);
        fwrite(&hex2, 2, 2, fpout);
        offset++;
    }
    fprintf(fpout, "\n%08x\n", offset);

    

    fclose(fpout);
    close(fpin);
}
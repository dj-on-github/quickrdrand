#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "rdrand_stdint.h"

#define DEBUG 0
#define BUFFERSZ 128
#define MAX_MEGABYTES 10000

void fixsmallbuff(unsigned long long int *buff)
{
    unsigned char *cbuff;
    int i;
    cbuff = (unsigned char *)buff;

    for (i=0;i<(BUFFERSZ*8);i++)
    {
        cbuff[i]=cbuff[i*2];
    }
}

void fixbuff(unsigned long long int *buff)
{
    unsigned char *cbuff;
    int i;
    cbuff = (unsigned char *)buff;

    for (i=0;i<(512*1024);i++)
    {
        cbuff[i]=cbuff[i*2];
    }
}

int main(int argc, char** argv, char** environ)
{
int n;
int ni[5];
unsigned short int ji[5];
unsigned int ki[5];
unsigned long long int li[5];

unsigned int i;
unsigned int j;
unsigned long long int foo;

unsigned char seed[16];
uint64_t data[2*BUFFERSZ];
uint64_t megabuff[65536];
uint64_t megabuff2[65536];
unsigned char *bytedata;
unsigned char *ptr;

int abort = 0;
//struct arg_lit *a_binary = arg_lit0("b", "binary"," Output in binary");
//struct arg_lit *a_bugfix = arg_lit0("f", "fix"," Delete every other byte");
//struct arg_lit *a_html = arg_lit0("w", "html"," Output with HTML headers");
//struct arg_int *a_kilobytes = arg_int0("k", "kilobytes", "<number of KibiBytes to output>"," Number of KibiBytes to output");
//struct arg_int *a_delay = arg_int0("d", "delay", "<ms>"," Number of millisecond delay between each output line");
//struct arg_lit *a_continuous = arg_lit0("c", "continuous", " Output random data continuously");
//struct arg_lit *a_help = arg_lit0("h", "help"," Print this help and exit");
//struct arg_end *a_end = arg_end(20);
//void *argtable[] = {a_binary, a_kilobytes, a_continuous, a_delay, a_html, a_bugfix, a_help,a_end};
const char progname[12] = "webrandmeg";
int exitcode = 0;
int nerrors = 0;

int html = 0;
int binary = 0;
int kilobytes = 1;
int continuous = 0;
int delay = 0;
int bugfix = 0;
unsigned char *cgifile;
unsigned char *cgimegabytes;
int megabytes;
int f;
unsigned char filename[255];
unsigned char shortfilename[255];
int content_length;
unsigned char *content_length_ptr;
unsigned char post_buffer[10000];
int docgi;
mode_t oldmask;

    megabytes = 0;

int bflag = 0;
char *kvalue = NULL;
int c;
int index;
FILE *devrandom;

while ((c = getopt (argc, argv, "bk:")) != -1)
    switch (c)
    {
        case 'b':
            bflag = 1;
            break;
        case 'k':
            kvalue = optarg;
            kilobytes = atoi(kvalue);
            break;
        default:
	    exit(1);
    }
    
//printf ("bflag = %d, kvalue = %s, kilobytes=%d\n",bflag, kvalue, kilobytes);

devrandom = fopen("/dev/random","r");
fread(megabuff2,8,65536,devrandom);
fclose(devrandom);
             
for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);

    printf("Content-type: application/octet-stream\n\n");
    fflush(stdout);

    if (rdrand_check_support()==1) {
        n = rdrand_get_n_uint64_retry(65536,30,megabuff);
        fwrite(megabuff,8,65536,stdout);
        fwrite(megabuff2,8,65536,stdout);
        fflush(stdout);
    }
    else {
        printf("No RDRAND support\n");
    }

    exit:
    //arg_freetable(argtable, sizeof(argtable)/sizeof(argtable[0]));
    return exitcode;
}


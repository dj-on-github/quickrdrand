#include "stdafx.h"
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>

#include <intrin.h>
#include <io.h>
//#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "rdrand_stdint.h"
#include "ya_getopt.h"

#include <Windows.h>


#define DEBUG 0
#define BUFFERSZ 128
#define MAX_MEGABYTES 10000

void fixsmallbuff(uint64_t *buff)
{
	unsigned char *cbuff;
	int i;
	cbuff = (unsigned char *)buff;

	for (i = 0;i<(BUFFERSZ * 8);i++)
	{
		cbuff[i] = cbuff[i * 2];
	}
}

void fixbuff(uint64_t *buff)
{
	unsigned char *cbuff;
	int i;
	cbuff = (unsigned char *)buff;

	for (i = 0;i<(512 * 1024);i++)
	{
		cbuff[i] = cbuff[i * 2];
	}
}

int pull64_rdrand(int thirtytwobit, uint32_t amount, uint32_t retries, uint64_t *megabuff) {

	int n;
	if (thirtytwobit == 0) {
		n = rdrand_get_n_uint64_retry(amount, retries, megabuff);
		return n;
	}
	else {
		n = rdrand_get_n_uint32_retry(amount * 2, retries, (uint32_t *)megabuff);
		return n;
	}
};

int pull64_rdseed(int thirtytwobit, uint32_t amount, uint32_t retries, uint64_t *megabuff) {

	int n;
	if (thirtytwobit == 0) {
		n = rdseed_get_n_uint64_retry(amount, retries, megabuff);
		return n;
	}
	else {
		n = rdseed_get_n_uint32_retry(amount * 2, retries, (uint32_t *)megabuff);
		return n;
	}
};

void usage() {
	printf("Usage : quickrdand_windows[-b][-s][-S][-c][-h][-t][-m][-g <8 | 16 | 32 | 63>][-k n]\n");
	printf("    Version : w2 : No NL insertion Bug\n");
	printf("\n");
	printf("    Output random numbers using the RdRand or RdSeed instructions\n");
	printf("    Author : David Johnston, dj@deadhat.com\n");
	printf("\n");
	printf("    -b   : Dump out in binary(default is hex)\n");
	printf("    -s : Use RdSeed instead of RdRand\n");
	printf("    -k n : Dump KibiBytes of data\n");
	printf("    -c : Dump out continuously.\n");
	printf("    -m : Memory Resident.Malloc space to hold all the data before\n");
	printf("    collecting.Ensures no disc access interupts the flow of\n");
	printf("    data during collection.Limited by the amount of memory you\n");
	printf("    can allocate.\n");
	printf("    -t : Use 32 bit reads instead of 64.\n");
	printf("    -S : Stutter Mode\n");}
//int main(int argc, char** argv, char** environ)
int main(int argc, char** argv)
{
	uint32_t n;
	//int ni[5];
	//unsigned short int ji[5];
	//unsigned int ki[5];
	//uint64_t li[5];

	int i;
	int j;
	//uint64_t foo;

	unsigned char *bigbuff;
	uint64_t      *bigbuff64;
	//unsigned char seed[16];
	uint64_t data[2 * BUFFERSZ];
	uint64_t megabuff[131072];
	//unsigned char *bytedata;
	//unsigned char *ptr;

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
	const char progname[12] = "quickrdrand";
	int exitcode = 0;
	int nerrors = 0;

	int html = 0;
	int binary = 0;
	int kilobytes = 1;
	int continuous = 0;
	int delay = 0;
	int bugfix = 0;
	//unsigned char *cgifile;
	//unsigned char *cgimegabytes;
	int megabytes;
	//int f;
	char filename[255];
	//unsigned char shortfilename[255];
	//int content_length;
	//unsigned char *content_length_ptr;
	//unsigned char post_buffer[10000];
	//int docgi;
	//mode_t oldmask;

	char *ovalue;

	megabytes = 0;

	int bflag = 0;
	char *kvalue = NULL;
	int c;
	int index;
	int rdseed = 0;
	int thirtytwobit = 0;
	int memory_resident = 0;
	int stutter = 0;
	int sleep_useconds = 100;
	int output_to_file = 0;
	FILE *fileobject;

	while ((c = getopt(argc, argv, "bsShctmo:k:")) != -1)
		switch (c)
		{
		case 'c':
			continuous = 1;
			break;
		case 'b':
			bflag = 1;
			break;
		case 's':
			rdseed = 1;
			break;
		case 'o':
			ovalue = optarg;
			strcpy(filename,ovalue);
			output_to_file = 1;
			break;
		case 'k':
			kvalue = optarg;
			kilobytes = atoi(kvalue);
			break;
		case 't':
			thirtytwobit = 1;
			break;
		case 'm':
			memory_resident = 1;
			break;
		case 'S':
			stutter = 1;
			break;
		case 'h':
			usage();
			exit(EXIT_SUCCESS);
			break;
		default:
			usage();
			exit(EXIT_FAILURE);
		}

	//printf ("bflag = %d, kvalue = %s, kilobytes=%d\n",bflag, kvalue, kilobytes);

	for (index = optind; index < argc; index++)
		printf("Non-option argument %s\n", argv[index]);

	
	if (bflag == 1) {
		binary = 1;
		if (output_to_file == 1) fileobject = fopen(filename, "wb");
		else {
			setmode(fileno(stdout), O_BINARY);
			fileobject = stdout;
		}
	}
	else {
		if (output_to_file == 1) fileobject = fopen(filename, "w");
		else fileobject = stdout;
	}

	//docgi = 0;
	//if (strstr(argv[0], "quickrdrand.cgi") != NULL) docgi = 1;
	//
	//if (docgi == 1)
	//{
	//	printf("Content-type: text/html\n\n");
	//	printf("<html><body><pre>\n");
	//	if (DEBUG > 1) printf("argv[0]:%s\n", argv[0]);
	//
	//	i = 0;
	//	ptr = (unsigned char *)environ[i];
	//	while (ptr != NULL)
	//	{
	//		if (DEBUG > 1) printf("%s\n", environ[i]);
	//		if (strstr(environ[i], "CONTENT_LENGTH") != NULL)
	//		{
	//			if (DEBUG > 1) printf("FOUND CONTENT_LENGTH\n");
	//			sscanf(environ[i], "CONTENT_LENGTH=%d", &content_length);
	//			if (DEBUG > 1) printf("FOUND CONTENT_LENGTH = %d\n", content_length);
	//		}
	//		i++;
	//		ptr = (unsigned char *)environ[i];
	//	}
	//
	//	binary = 1;
	//	fgets((char *)post_buffer, content_length + 1, stdin);
	//	sscanf((char *)post_buffer, "DRNG_MEGABYTES=%d", &megabytes);
	//	if (DEBUG > 1) printf("MEGABYTES=%d\n", megabytes);
	//	if (DEBUG > 1) printf("POST DATA:%s:\n", post_buffer);
	//}

	//if (html == 1) {
	//	printf("Content-type: text/html\n\n");
	//	printf("<html><body><pre>\n");
	//	fflush(stdout);
	//}

	if (((rdrand_check_support() == 1) && (rdseed == 0)) || ((rdseed_check_support() == 1) && (rdseed == 1))) {
		i = 0;
		j = 0;
		//if ((docgi == 1))
		//{
		//	if (megabytes == 0) megabytes = 1;
		//
		//	if (DEBUG > 1) printf("<p>megabytes %d</p>", megabytes);
		//	if (megabytes > MAX_MEGABYTES) megabytes = MAX_MEGABYTES;
		//	rdrand32_step(&n);
		//	sprintf((char *)filename, "/var/www/html/webrandfiles/rand_%d.bin", (unsigned int)abs((int)n));
		//	//sprintf(shortfilename,"http://134.134.159.83/randfiles/rand_%d.bin",(unsigned int)abs(n));
		//	sprintf((char *)shortfilename, "http://davidsdesktop.com/webrandfiles/rand_%d.bin", (unsigned int)abs((int)n));
		//	//f = open(filename, (O_WRONLY | O_CREAT | S_IROTH | S_IWOTH | S_IRUSR | S_IWUSR ));
		//	f = open((char *)filename, (O_WRONLY | O_CREAT));
		//	if (DEBUG > 1) printf("<p>open %s for writing = %d</p>\n", filename, f);
		//	if (f != -1)
		//	{
		//		for (i = 0;i<megabytes;i++)
		//		{
		//			if (DEBUG > 1) printf("<p>Getting megabyte #%d</p>\n", i);
		//			if (bugfix == 1)
		//			{
		//				//n = rdrand_get_n_uint64_retry(131072,30,megabuff);
		//				n = pull64_rdrand(thirtytwobit, 131072, 30, megabuff);
		//				fixbuff(megabuff);
		//				n = write(f, megabuff, (512 * 1024));
		//				//n = rdrand_get_n_uint64_retry(131072,30,megabuff);
		//				n = pull64_rdrand(thirtytwobit, 131072, 30, megabuff);
		//				fixbuff(megabuff);
		//				n = write(f, megabuff, (512 * 1024));
		//			}
		//			else
		//			{
		//				//n = rdrand_get_n_uint64_retry(131072,30,megabuff);
		//				n = pull64_rdrand(thirtytwobit, 131072, 30, megabuff);
		//				n = write(f, megabuff, (1024 * 1024));
		//			}
		//			if (DEBUG > 1) printf("<p>write returned #%d</p>\n", n);
		//		}
		//		close(f);
		//		//chmod(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		//		printf("<form method=POST action=\"quickrdrand.cgi\">\n");
		//		printf("How Many megabytes of random data would you like to download (max 16)?\n");
		//		printf("<input type=\"text\" value=\"1\" maxlen=\"3\" name=\"DRNG_MEGABYTES\" >\n");
		//		printf("<input type=\"SUBMIT\">\n");
		//
		//		if ((megabytes < 1) || (megabytes > 16)) {
		//			megabytes = 1;
		//		}
		//
		//		if (megabytes == 1) printf("<p><a href=\"%s\">1 Megabyte of random data. Download Me.</a></p>\n", shortfilename);
		//		else printf("<p><a href=\"%s\">%d Megabytes of random data. Download Me.</a></p>\n", shortfilename, megabytes);
		//		printf("<p> This file will be deleted after 5 minutes.</p>\n");
		//	}
		//	else
		//	{
		//		printf("<p>File Error. Sorry</p>\n");
		//	}
		//}
		//else if (continuous == 1)
		if (continuous == 1)
		{
			while (1)
			{
				/*printf("RDRAND rdrand_get_n_uints_retry(1024, 10 data) test\n");*/
				if (bugfix == 1)
				{
					if (rdseed == 1)
						n = pull64_rdseed(thirtytwobit, 2 * BUFFERSZ, 10000, megabuff);
					else
						n = pull64_rdrand(thirtytwobit, 2 * BUFFERSZ, 10, data);
					fixsmallbuff(data);

					if (binary == 1)
					{
						fwrite(data, 1, 1024, fileobject);
					}
					else
						for (i = 0;i<(BUFFERSZ);)
						{
							printf("%016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n", data[i], data[i+1], data[i+2], data[i+3]);
							i += 4;
							//usleep(delay * 1000);
						}
				}
				else
				{
					if (rdseed == 1)
						n = pull64_rdseed(thirtytwobit, 2 * BUFFERSZ, 10000, data);
					else
						n = pull64_rdrand(thirtytwobit, 2 * BUFFERSZ, 10, data);
					//n = rdrand_get_n_qints_retry(BUFFERSZ, 10, data);
					if (binary == 1)
					{
						fwrite(data, 1, 1024, fileobject);
					}
					else
						for (i = 0;i<(BUFFERSZ);)
						{
							printf("%016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n", data[i], data[i+1], data[i+2], data[i+3]);
							i += 4;
							//usleep(delay * 1000);
						}
				}
			}
		}
		else
		{
			for (j = 0; j<kilobytes; j++)
			{
				if (bugfix == 1)
				{
					if (rdseed == 1)
						//n = rdseed_get_n_uint64_retry(2*BUFFERSZ, 1000, data);
						n = pull64_rdseed(thirtytwobit, 2 * BUFFERSZ, 10000, data);
					else
						//n = rdrand_get_n_uint64_retry(2*BUFFERSZ, 1000, data);
						n = pull64_rdrand(thirtytwobit, 2 * BUFFERSZ, 10, data);
					//n = rdrand_get_n_qints_retry(2*BUFFERSZ, 1000, data);
					fixsmallbuff(data);
					if (binary == 1)
					{
						fwrite(data, 1, 1024, fileobject);
					}
					else
					{
						/* printf("Collected %d qints\n",2*BUFFERSZ); */
						for (i = 0;i<(BUFFERSZ);)
						{
							/* printf("i=%d\n",i);*/
							printf("%016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n", data[i], data[i+1], data[i+2], data[i+3]);
							i += 4;
							/* usleep(delay*1000); */
						}
					}
				}
				else if ((memory_resident == 0) || (stutter==1))
				{
					if (rdseed == 1)
						//n = rdseed_get_n_uint64_retry(2*BUFFERSZ, 1000, data);
						n = pull64_rdseed(thirtytwobit, 2 * BUFFERSZ, 10000, data);
					else
						//n = rdrand_get_n_uint64_retry(2*BUFFERSZ, 1000, data);
						n = pull64_rdrand(thirtytwobit, 2 * BUFFERSZ, 10, data);
					//n = rdrand_get_n_qints_retry(BUFFERSZ, 100000, data);
					if (binary == 1)
					{
						fwrite(data, 1, 1024, fileobject);
					}
					else
						for (i = 0;i<(BUFFERSZ);)
						{
							printf("%016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n", data[i], data[i+1], data[i+2], data[i+3]);
							i += 4;
							//usleep(delay * 1000);
						}
					if (stutter == 1) Sleep(10);
				}
				else // memory resident version
				{
					//printf("A\n"); fflush(stdout);

					bigbuff = (unsigned char *)malloc((kilobytes * 1024) + 1);
					if (bigbuff == NULL) {
						printf("Could not allocate %d bytes in memory\n", (kilobytes * 1024));
						exit(EXIT_FAILURE);
					}
					bigbuff64 = (uint64_t *)bigbuff;

					//printf("B\n"); fflush(stdout);
					if (rdseed == 1)
						n = pull64_rdseed(thirtytwobit, (kilobytes * 128), 10000, bigbuff64);
					else
						n = pull64_rdrand(thirtytwobit, (kilobytes * 128), 10, bigbuff64);
					//n = rdrand_get_n_qints_retry(BUFFERSZ, 100000, data);
					j = j + kilobytes;
					//printf("C\n"); fflush(stdout);
					if (binary == 1)
					{
						fwrite(bigbuff, 1, (kilobytes * 1024), fileobject);
					}
					else {
						for (i = 0;i<((kilobytes * 1024) / 8);)
						{
							printf("%016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n", bigbuff64[i], bigbuff64[i + 1], bigbuff64[i + 2], bigbuff64[i + 3]);
							i += 4;
							//usleep(delay * 1000);
						}
					}
					//printf("D\n"); fflush(stdout);

					free(bigbuff);

				}
			}
		}
	}
	else {
		if ((rdseed == 0) && (rdrand_check_support() == 0)) printf("No RDRAND support\n");
		else if ((rdseed == 1) && (rdseed_check_support() == 0)) printf("No RDSEED support\n");
		else printf("Unknown Error\n");
	}

	//if (((binary == 0) && (html == 1)) || (docgi == 1)) {
	//	printf("</pre></body></html>\n");
	//}
//exit:
	//arg_freetable(argtable, sizeof(argtable)/sizeof(argtable[0]));
	return exitcode;
}

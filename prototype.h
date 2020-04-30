#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "qrcodegen.h"
#include <locale.h>
#include <wchar.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <gtk/gtk.h>



typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}
pixel_t;

typedef struct
{
    pixel_t *pixels;
    size_t width;
    size_t height;
}
bitmap_t;


typedef struct 
{
	const gchar * firstname;
	const gchar * lastnameTemp;
	const gchar * mail;
	const gchar * phone;
	const gchar * address;
	const gchar * city;
	const gchar * postalCode;
	const gchar * licenseID;
	const gchar * account;
}entries_t;


static int save_png_to_file (bitmap_t *bitmap, const char *path);
bool printQRCode(const char *fileName, const char *text);
int qrGenerate(FILE * file, const char* qrcodePath); 
int upload(const char * pathQR);
int checkUser(char * mail);
int main();
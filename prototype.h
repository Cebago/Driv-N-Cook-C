#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
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

typedef struct {
	char * ip_ftp;
	char * port_ftp;
	char * user_ftp;
	char * public_keyfile;
	char * private_keyfile;
	char * dest_path;
	char * output_name;
	char * upload_protocol;
	char * ip_sql;
	unsigned int port_sql;
	char * user_sql;
	char * pwd_sql;
	char * table_name;
}conf_file_t;


typedef struct confread_file {
	char *name;
	struct confread_section *sections;
}confread_file;

// The struct for each config section
typedef struct confread_section {
	char *name;
	struct confread_pair *pairs;
	struct confread_section *next;
}confread_section;

// The key-value pairs
typedef struct confread_pair {
	char *key;
	char *value;
	struct confread_pair *next;
}confread_pair;


// functions
struct confread_file *confread_open(char *path);
struct confread_section *confread_find_section(struct confread_file *confFile, char *name);
struct confread_pair *confread_find_pair(struct confread_section *confSec, char *key);
char *confread_find_value(struct confread_section *confSec, char *key);
int confread_check_pair(struct confread_section *section, char *key, char *value);
void confread_close(struct confread_file **confFile);
void * cleanConfFile(conf_file_t * globalConfRead);


static int save_png_to_file (bitmap_t *bitmap, const char *path);
bool printQRCode(const char *fileName, const char *text);
int qrGenerate(FILE * file, const char* qrcodePath); 
int upload(const char * pathQR);
int checkUser(char * mail);
conf_file_t * getConf();
int main();
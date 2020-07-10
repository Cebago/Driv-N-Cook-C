#include "prototype.h"

extern conf_file_t * globalConfRead; 

int upload(const char * pathQR){
  CURL *curl;
  CURLcode res;
  struct stat file_info;
  curl_off_t speed_upload, total_time;
  FILE *fd;
 
  fd = fopen(pathQR, "rb"); /* open file to upload */ 
  if(!fd)
    return 1; /* can't continue */ 
 
  /* to get the file size */ 
  if(fstat(fileno(fd), &file_info) != 0)
    return 1; /* can't continue */ 
 
  curl = curl_easy_init();
  if(curl) {
    
    curl_easy_setopt(curl, CURLOPT_SSH_PUBLIC_KEYFILE, globalConfRead->public_keyfile);
    curl_easy_setopt(curl, CURLOPT_SSH_PRIVATE_KEYFILE, globalConfRead->private_keyfile);
    curl_easy_setopt(curl, CURLOPT_USERNAME, globalConfRead->user_ftp);

    char * url = malloc(sizeof(char)*200);
    strcpy(url, globalConfRead->upload_protocol);
    strcat(url, "://");
    strcat(url, globalConfRead->ip_ftp);
    strcat(url, ":");
    strcat(url, globalConfRead->port_ftp);
    strcat(url, globalConfRead->dest_path);
    strcat(url, globalConfRead->output_name);
    printf("Connecting to: %s\n", url);
    /* upload to this place */ 
    curl_easy_setopt(curl, CURLOPT_URL, url);

 
    /* tell it to "upload" to the URL */ 
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
 
    /* set where to read from (on Windows you need to use READFUNCTION too) */ 
    curl_easy_setopt(curl, CURLOPT_READDATA, fd);
 
    /* and give the size of the upload (optional) */ 
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                     (curl_off_t)file_info.st_size);
 
    /* enable verbose for easier tracing -> print infos*/ 
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
        curl_easy_strerror(res));
 
    }
    else {
      /* now extract transfer info */ 
      curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD, &speed_upload);
      curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
 
      fprintf(stderr, "Speed: %" CURL_FORMAT_CURL_OFF_T " bytes/sec during %"
              CURL_FORMAT_CURL_OFF_T ".%06ld seconds\n",
              speed_upload,
              (total_time / 1000000), (long)(total_time % 1000000));
 
    }
    /* always cleanup */ 
    curl_easy_cleanup(curl);
    free(url);
  }
  fclose(fd);
  return 0;
}

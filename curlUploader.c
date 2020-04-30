#include "prototype.h"

 
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

    curl_easy_setopt(curl, CURLOPT_SSH_PUBLIC_KEYFILE, "keys/id_rsa.pub");
    curl_easy_setopt(curl, CURLOPT_SSH_PRIVATE_KEYFILE, "keys/id_rsa");
    curl_easy_setopt(curl, CURLOPT_USERNAME, "bdemarche");


    /* upload to this place */ 
    curl_easy_setopt(curl, CURLOPT_URL, "sftp://51.178.31.223:13796/home/bdemarche/qrcodeReader/qrcode.png");
 
    /* tell it to "upload" to the URL */ 
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
 
    /* set where to read from (on Windows you need to use READFUNCTION too) */ 
    curl_easy_setopt(curl, CURLOPT_READDATA, fd);
 
    /* and give the size of the upload (optional) */ 
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                     (curl_off_t)file_info.st_size);
 
    /* enable verbose for easier tracing */ 
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
  }
  fclose(fd);
  return 0;
}

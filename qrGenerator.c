#include "prototype.h"

// The main application program.
int qrGenerate(FILE * file, const char* qrcodePath) {
     // path where file will be saved

    fseek(file, 0, SEEK_END);
	int size = ftell(file);
	char * fileContent = malloc(sizeof(char)*size);
	fseek(file, 0, SEEK_SET);
	fread(fileContent, 1, size, file);
	printf("\nFichier : \n%s\n\n",fileContent);
	fclose(file);


    printQRCode(qrcodePath, fileContent);
    printf("QRcode ecrit\n");
	return EXIT_SUCCESS;
}



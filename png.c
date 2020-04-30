#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "prototype.h"
#include "qrcodegen.h"


#define CORRECTION_VALUE 32 // correction value to generate each bitmap value
#define RGB_MAX 255
#define RGB_MIN 0
#define PIXEL_PER_QUADRANT 10
#define PADDING 4


static pixel_t * pixel_at (bitmap_t * bitmap, int x, int y)
{
    return bitmap->pixels + bitmap->width * y + x;
}
    

bool printQRCode(const char *fileName, const char *text) {

// Set error correction level
	enum qrcodegen_Ecc errorCorrecrionLevel = qrcodegen_Ecc_LOW;

	// Make and print the QR Code symbol
	uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
	uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];

	// Convert text to QR code
	// Result is retrieved as a matrix
	bool isEncoded = qrcodegen_encodeText(text, tempBuffer, qrcode, errorCorrecrionLevel,
		qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
	if (isEncoded) {

        bitmap_t img;
        int size = qrcodegen_getSize(qrcode);
        int widthCorrected = PIXEL_PER_QUADRANT * (size + PADDING);  
        img.width = widthCorrected;
        img.height = widthCorrected;
    	img.pixels = calloc((img.width * PIXEL_PER_QUADRANT) * (img.height * PIXEL_PER_QUADRANT), sizeof(pixel_t));


        for (size_t y = 0; y <= (PADDING / 2) * PIXEL_PER_QUADRANT; y += 1) {
            for (size_t x = 0; x < img.width; x += 1) {
                for (size_t xi = 0; xi < PIXEL_PER_QUADRANT; xi += 1) {
                    pixel_t* pixel = pixel_at(&img, x * PIXEL_PER_QUADRANT + xi, y);
                    pixel->red = 250;
                    pixel->green = 250;
                    pixel->blue = 250;
                }
            }
        }
    	/**
    	* Main loop write qrcode 
    	*/
    	for (size_t y = 0; y < img.height; y += 1) {
    		for (size_t yi = 0; yi < PIXEL_PER_QUADRANT; yi += 1) {
    			for (size_t x = 0; x < img.width; x += 1) {
    				for (size_t xi = 0; xi < PIXEL_PER_QUADRANT; xi += 1) {
    					if (qrcodegen_getModule(qrcode, x, y)) {
    						pixel_t* pixel = pixel_at(&img, (x + 2) * PIXEL_PER_QUADRANT + xi, (y + 2) * PIXEL_PER_QUADRANT + yi);
    						pixel->red = 0;
    						pixel->green = 0;
    						pixel->blue = 0;
    					} else {
    						pixel_t* pixel = pixel_at(&img, (x + 2) * PIXEL_PER_QUADRANT + xi, (y + 2) * PIXEL_PER_QUADRANT + yi);
    						pixel->red = 250;
    						pixel->green = 250;
    						pixel->blue = 250;
    					}
    				}
    			}
    		}
    	}

    	if (save_png_to_file(&img, "./qrcode.png")) {
    		fprintf(stderr, "Error writing file.\n");
    		exit(EXIT_FAILURE);
    	}

    	    free (img.pixels);

    }
}


static int save_png_to_file (bitmap_t *bitmap, const char *path){
    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;
    /* "status" contains the return value of this function. At first
       it is set to a value which means 'failure'. When the routine
       has finished its work, it is set to a value which means
       'success'. */
    int status = -1;
    /* The following number is set by trial and error only. I cannot
       see where it it is documented in the libpng manual.
    */
    int pixel_size = 3;
    int depth = 8;
    
    fp = fopen (path, "w+b");
    if (! fp) {
        goto fopen_failed;
    }

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        goto png_create_write_struct_failed;
    }
    
    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        goto png_create_info_struct_failed;
    }
    
    /* Set up error handling. */

    if (setjmp (png_jmpbuf (png_ptr))) {
        goto png_failure;
    }
    
    /* Set image attributes. */

    png_set_IHDR (png_ptr,
                  info_ptr,
                  bitmap->width,
                  bitmap->height,
                  depth,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);
    
    /* Initialize rows of PNG. */

    row_pointers = png_malloc (png_ptr, bitmap->height * sizeof (png_byte *));
    for (y = 0; y < bitmap->height; y++) {
        png_byte *row = 
            png_malloc (png_ptr, sizeof (uint8_t) * bitmap->width * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < bitmap->width; x++) {
            pixel_t * pixel = pixel_at (bitmap, x, y);
            *row++ = pixel->red;
            *row++ = pixel->green;
            *row++ = pixel->blue;
        }
    }
    
    /* Write the image data to "fp". */

    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* The routine has successfully written the file, so we set
       "status" to a value which indicates success. */

    status = 0;
    
    for (y = 0; y < bitmap->height; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);
    
 png_failure:
 png_create_info_struct_failed:
    png_destroy_write_struct (&png_ptr, &info_ptr);
 png_create_write_struct_failed:
    fclose (fp);
 fopen_failed:
    return status;
}



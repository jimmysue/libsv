#include "libsv/sv_codec.h"

#include <stdio.h>
#include <vector>
#include <memory>

#if defined(__cplusplus)
extern "C" {
#endif
#include <jpeglib.h>
#include <jconfig.h>

#if defined(__cplusplus)
}
#endif

#include "sv_log.h"


sv_image_t * sv_imread_jpeg(const char* path)
{
    FILE *file = fopen( path, "rb" );
    if ( file == NULL )
    {
        LOGE("can't open image file: %s", path);
        return NULL;
    }

    struct jpeg_decompress_struct info; //for our jpeg info
    struct jpeg_error_mgr err; //the error handler
    JSAMPROW row_pointer[1];

    info.err = jpeg_std_error( &err );     
    jpeg_create_decompress( &info ); //fills info structure

    jpeg_stdio_src( &info, file );    
    jpeg_read_header( &info, TRUE );

    jpeg_start_decompress( &info );

    int w = info.output_width;
    int h = info.output_height;
    int numChannels = info.num_components; // 3 = RGB, 4 = RGBA
    unsigned long dataSize = w * h * numChannels;

    sv_pix_fmt_t format = numChannels == 3 ? SV_PIX_FMT_RGB : SV_PIX_FMT_RGBA;

    sv_image_t* result = sv_image_create(w, h, format, 1);

    if ( !result ) return result;

    unsigned char *data = result->data;
    unsigned char* rowptr = nullptr;
    while ( info.output_scanline < h )
    {
        rowptr = data + info.output_scanline * w * numChannels;
        jpeg_read_scanlines( &info, &rowptr, 1 );
    }

    jpeg_finish_decompress( &info );    

    fclose( file );

    return result;
}

sv_image_t * sv_imread(const char* filename)
{
    return sv_imread_jpeg(filename);
}

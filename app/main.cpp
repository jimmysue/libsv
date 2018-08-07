#include "libsv/sv_image.h"

int main(int argc, char** argv)
{
    sv_image_t *image = sv_image_create(100, 100, SV_PIX_FMT_BGR888, 1);

    sv_image_str(image);

    sv_image_destroy(image);

    sv_image_t *image2 = sv_image_create();

    sv_image_str(image2);

    sv_image_swap(image2, image);
    

    return 0;
}
#include "libsv/sv_image.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <libyuv.h>
#include <libyuv/convert.h>

#ifdef __cplusplus
}  // extern "C"
#endif


static const int  g_orient_shifter[8][8] = {

};

sv_image_t * sv_image_rotate(const sv_image_t* image, int dst_orient)
{
    return nullptr;
}


static sv_image_t * sv_image_to_GRAY8(const sv_image_t* image)
{
    // TODO
    return nullptr;
}

static sv_image_t * sv_image_to_BGR888(const sv_image_t* image)
{
    // TODO
    return nullptr;
}

sv_image_t * sv_image_convert(const sv_image_t* image, int fmt)
{
    return nullptr;
}
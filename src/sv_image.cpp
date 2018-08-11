#include "libsv/sv_image.h"


#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <memory>


#ifdef __cplusplus
extern "C" {
#endif

#include <libyuv.h>

#ifdef __cplusplus
}  // extern "C"
#endif

#include "sv_log.h"

#define ASSIGN_INFO(dst, src)   \
    dst->width = src->width;    \
    dst->height = src->height;  \
    dst->format = src->format;  \
    dst->orient = src->orient



sv_image_t * sv_image_create(int width, int height, sv_pix_fmt_t fmt, int orient)
{
    sv_image_t * image = new (std::nothrow) sv_image_t;

    if(!image){
        LOGE("failed allocing image struct");
        return nullptr;
    }

    int ybpp = SV_GET_FMT_YBPP(fmt);
    int ubpp = SV_GET_FMT_UBPP(fmt);
    int vbpp = SV_GET_FMT_VBPP(fmt);

    int num_pixel = width * height;

    int ysize = num_pixel * ybpp / 8;
    int usize = num_pixel * ubpp / 8;
    int vsize = num_pixel * vbpp / 8;

    unsigned char* data = new (std::nothrow) unsigned char [ysize + usize + vsize + sizeof(int)];

    if(!data){
        LOGE("failed alloc pixel data: width=%d, height=%d, format=%s", width, height, sv_image_fmt_str(fmt));
        delete image;
        return nullptr;
    }

    image->width = width;
    image->height = height;
    image->format = fmt;
    image->orient = orient;
    image->refcount = (int*) (data + ysize + usize + vsize);
    image->data = data;
    image->ystride = ybpp * width / 8;
    image->ustride = ubpp * width / 4;
    image->vstride = vbpp * width / 4;
    
    (*image->refcount) = 1;

    if(usize > 0){
        image->u = data + ysize;
    }else{
        image->u = nullptr;
    }

    if(ysize > 0){
        image->v = data + ysize + usize;
    }else{
        image->v = nullptr;
    }

    return image;
}


void sv_image_destroy(sv_image_t* image)
{
    if(image){
        delete image;
    }
}


sv_image_t * sv_image_clone(const sv_image_t* src)
{
    assert(src);
    sv_image_t * image = sv_image_create(src->width, src->height, src->format, src->orient);

    if (!image) {
        return nullptr;
    }
    const int fmt = image->format;
    const int width = image->width, height = image->height;
    int ybpp = SV_GET_FMT_YBPP(fmt);
    int ubpp = SV_GET_FMT_UBPP(fmt);
    int vbpp = SV_GET_FMT_VBPP(fmt);

    int num_pixel = width * height;

    int ysize = num_pixel * ybpp / 8;
    int usize = num_pixel * ubpp / 8;
    int vsize = num_pixel * vbpp / 8;

    memcpy(image->y, src->y, ysize);
    memcpy(image->u, src->u, usize);
    memcpy(image->v, src->v, vsize);

    return image;
}


void sv_image_copyTo(const sv_image_t* src, sv_image_t* dst)
{
    assert(src);
    assert(dst);
    const int size_src = sv_image_size(src);

    if (sv_image_capacity(dst) >= sv_image_size(src)){
        
        int num_pixel = src->width * src->height;
        int ysize = SV_GET_FMT_YBPP(src->format) * num_pixel / 8;
        int usize = SV_GET_FMT_UBPP(src->format) * num_pixel / 8;
        int vsize = SV_GET_FMT_VBPP(src->format) * num_pixel / 8;

        memcpy(dst->y, src->y, ysize);
        dst->u = dst->y + ysize;
        memcpy(dst->u, src->u, usize);
        dst->v = dst->u + usize;
        memcpy(dst->v, src->v, vsize);

        ASSIGN_INFO(dst, src);
    }else{
        sv_image_t *tmp = sv_image_clone(src);
        sv_image_swap(tmp, dst);
        sv_image_destroy(tmp);
    }
}

void sv_image_swap(sv_image_t* lhs, sv_image_t* rhs)
{
    std::swap(lhs->y, rhs->y);
    std::swap(lhs->u, rhs->u);
    std::swap(lhs->v, rhs->v);
    std::swap(lhs->ystride, rhs->stride);
    std::swap(lhs->ustride, rhs->ustride);
    std::swap(lhs->vstride, rhs->vstride);
    std::swap(lhs->width, rhs->width);
    std::swap(lhs->height, rhs->height);
    std::swap(lhs->orient, rhs->orient);
    std::swap(lhs->format, rhs->format);
    std::swap(lhs->refcount, rhs->refcount);
}


int sv_image_capacity(const sv_image_t* image)
{
    assert(image);
    if(image->refcount){
        const unsigned char* p = (const unsigned char*)image->refcount;
        return p - image->data;
    }
    return 0;
}

int sv_image_size(const sv_image_t* image)
{
    const int fmt = image->format;
    const int width = image->width;
    const int height = image->height;
    const int ybpp = SV_GET_FMT_YBPP(fmt);
    const int ubpp = SV_GET_FMT_UBPP(fmt);
    const int vbpp = SV_GET_FMT_VBPP(fmt);

    const int num_pixel = width * height;

    const int ysize = num_pixel * ybpp / 8;
    const int usize = num_pixel * ubpp / 8;
    const int vsize = num_pixel * vbpp / 8;
    return ysize + usize + vsize;
}

sv_image_t * sv_image_roi(const sv_image_t* image, int x, int y, int width, int height)
{
    assert(x >= 0);
    assert(y >= 0);
    assert(width >0);
    assert(height > 0);
    assert(x + width <= image->width);
    assert(y + height <= image->height);

    sv_image_t * result = new (std::nothrow) sv_image_t;

    if ( !result ) return result;

    *result = *image;

    if ( image->y ){
        result->y = image->y + y * image->ystride + x * image->ystride / image->width;
    }
    
    if ( image->u ){
        result->u = image->u + y * image->ustride + x * image->ustride / image->width;
    }
    
    if ( image->v ){
        result->v = image->v + y * image->vstride + x * image->vstride / image->width;
    }

    result->refcount = nullptr;

    return result;
}

sv_image_t * sv_image_from_plane_1(int width, int height, sv_pix_fmt_t fmt, uint8_t* data, int stride, int orient)
{
    auto* res = new (std::nothrow) sv_image_t;
    res->width = width;
    res->height = height;
    res->format = fmt;
    res->data = data;
    res->stride = stride;
    res->orient = orient;
    return res;
}

/// debug utilities

int  sv_image_isEqual(const sv_image_t* lhs, const sv_image_t* rhs)
{
    int equal = 1;
    equal = equal && (lhs->width == rhs->width);
    equal = equal && lhs->height == rhs->height;
    equal = equal && lhs->format == rhs->format;
    equal = equal && lhs->orient == rhs->orient;

    if ( !equal ){
        return equal;
    }
    int ysize = SV_GET_FMT_YBPP(lhs->format) * lhs->width / 8;
    int usize = SV_GET_FMT_UBPP(lhs->format) * lhs->width / 8;
    int vsize = SV_GET_FMT_VBPP(lhs->format) * lhs->width / 8;

    for(int y = 0; y < lhs->height; ++y){
        const uint8_t * py = lhs->y;
        const uint8_t * pu = lhs->u;
        const uint8_t * pv = lhs->v;
        const uint8_t * qy = rhs->y;
        const uint8_t * qu = rhs->u;
        const uint8_t * qv = rhs->v;
        if( memcmp(py, qy, ysize) != 0 ) return 0;
        if( memcmp(pu, qu, usize) != 0 ) return 0;
        if( memcmp(pv, qv, vsize) != 0 ) return 0;
    }
    return 1;
}

const char*  sv_image_fmt_str(int fmt)
{
    static const char * fmt_names[] = {
        "GRAY",
        "BGR",
        "BGRA",
        "RGB",
        "RGBA",
        "J420",
        "NV12",
        "NV21"
    };

    int id = SV_GET_FMT_ID(fmt);
    constexpr int num = sizeof(fmt_names) / sizeof(fmt_names[0]);
    if (id >=0 && id < num){
        return fmt_names[id];
    }
    return "Unknown Pixel Format";
}


void sv_image_str(const sv_image_t* image)
{
    const char* fmt_str = NULL;
    switch(image->format){
        case SV_PIX_FMT_BGR: fmt_str = "BGR"; break;
        case SV_PIX_FMT_BGRA: fmt_str = "BGRA"; break;
        case SV_PIX_FMT_GRAY8: fmt_str = "GRAY8"; break;
        case SV_PIX_FMT_RGB: fmt_str = "RGB"; break;
        case SV_PIX_FMT_RGBA: fmt_str = "RGBA"; break;
        case SV_PIX_FMT_J420: fmt_str = "J420"; break;
        case SV_PIX_FMT_NV12: fmt_str = "NV12"; break;
        case SV_PIX_FMT_NV21: fmt_str = "NV21"; break;
        default: fmt_str = "Unknown";
    }
    printf(
        "sv_image_t: {\n"
        "   width: %d\n"
        "   height: %d\n"
        "   format: %s\n"
        "   orient: %d\n"
        "}\n",
        image->width, image->height, fmt_str, image->orient
        );
}
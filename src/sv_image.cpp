#include "libsv/sv_image.h"

#include <memory>


#ifdef __cplusplus
extern "C" {
#endif

#include <libyuv.h>

#ifdef __cplusplus
}  // extern "C"
#endif

#define ASSIGN_INFO(dst, src)   \
    dst->width = src->width;    \
    dst->height = src->height;  \
    dst->format = src->format;  \
    dst->orient = src->orient



sv_image_t * sv_image_create(int width, int height, int fmt, int orient)
{
    sv_image_t * image = new (std::nothrow) sv_image_t;

    if(!image){
        return nullptr;
    }

    int ybpp = SV_PIX_FMT_YBPP(fmt);
    int ubpp = SV_PIX_FMT_UBPP(fmt);
    int vbpp = SV_PIX_FMT_VBPP(fmt);

    int num_pixel = width * height;

    int ysize = num_pixel * ybpp / 8;
    int usize = num_pixel * ubpp / 8;
    int vsize = num_pixel * vbpp / 8;

    unsigned char* data = new (std::nothrow) unsigned char [ysize + usize + vsize + sizeof(int)];

    if(!data){
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
    image->ustride = ubpp * width / 8;
    image->vstride = vbpp * width / 8;
    
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
    int ybpp = SV_PIX_FMT_YBPP(fmt);
    int ubpp = SV_PIX_FMT_UBPP(fmt);
    int vbpp = SV_PIX_FMT_VBPP(fmt);

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
        int ysize = SV_PIX_FMT_YBPP(src->format) * num_pixel / 8;
        int usize = SV_PIX_FMT_UBPP(src->format) * num_pixel / 8;
        int vsize = SV_PIX_FMT_VBPP(src->format) * num_pixel / 8;

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
    const int ybpp = SV_PIX_FMT_YBPP(fmt);
    const int ubpp = SV_PIX_FMT_UBPP(fmt);
    const int vbpp = SV_PIX_FMT_VBPP(fmt);

    const int num_pixel = width * height;

    const int ysize = num_pixel * ybpp / 8;
    const int usize = num_pixel * ubpp / 8;
    const int vsize = num_pixel * vbpp / 8;
    return ysize + usize + vsize;
}

/// debug utilities

void sv_image_str(const sv_image_t* image)
{
    const char* fmt_str = NULL;
    switch(image->format){
        case SV_PIX_FMT_BGR888: fmt_str = "BGR888"; break;
        case SV_PIX_FMT_BGRA8888: fmt_str = "BGRA8888"; break;
        case SV_PIX_FMT_GRAY8: fmt_str = "GRAY8"; break;
        case SV_PIX_FMT_RGB888: fmt_str = "RGB888"; break;
        case SV_PIX_FMT_RGBA8888: fmt_str = "RGBA8888"; break;
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
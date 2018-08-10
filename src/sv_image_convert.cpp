#include "libsv/sv_image.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <libyuv.h>
#include <libyuv/convert.h>
#include <libyuv/convert_argb.h>

#ifdef __cplusplus
}  // extern "C"
#endif

#include <assert.h>

#include "sv_log.h"


#define RETURN_ON_STATUS(error, res) \
    if(error){                       \
        LOGE("error: %d", error);    \
        sv_image_destroy(res);       \
        return nullptr;              \
    }                                \
    return res;                      \

static sv_image_t * dummy_convert(const sv_image_t* src, sv_pix_fmt_t fmt)
{
    LOGE("unsupport convert: %s => %s", sv_image_fmt_str(src->format), sv_image_fmt_str(fmt));
    return nullptr;
}

static sv_image_t * Clone(const sv_image_t* image, sv_pix_fmt_t fmt)
{
    return sv_image_clone(image);
}

static sv_image_t * Y2BGR(const sv_image_t* image, sv_pix_fmt_t fmt)
{
    sv_image_t * result = sv_image_create(image->width, image->height, SV_PIX_FMT_BGR888, image->format);
    assert(result);
    for(int r = 0; r < image->height; ++r){
        const uint8_t* src = image->data + r * image->stride;
        uint8_t *dst = result->data + r * result->stride;
        for(int c = 0; c < image->width; ++c){
            *dst++ = *src;
            *dst++ = *src;
            *dst++ = *src++;
        }
    }
    return result;
}

static sv_image_t * Y2BGRA(const sv_image_t* image, sv_pix_fmt_t fmt)
{
    auto * result = sv_image_create(image->width, image->height, SV_PIX_FMT_BGRA8888, image->orient);
    assert(result);
    int error = libyuv::J400ToARGB(
        image->data,
        image->stride,
        result->data,
        result->stride,
        result->width,
        result->height
    );
    RETURN_ON_STATUS(error, result);
}

#define Y2RGB Y2BGR
#define Y2RGBA Y2BGRA

static  sv_image_t * Y2J420(const sv_image_t* src, sv_pix_fmt_t fmt)
{
    auto* dst = sv_image_create(src->width, src->height, SV_PIX_FMT_J420, src->orient);
    assert(dst);

    int error = libyuv::J400ToJ420(
        src->data, src->stride,
        dst->y, dst->ystride,
        dst->u, dst->ustride,
        dst->v, dst->vstride,
        dst->width,
        dst->height);

    RETURN_ON_STATUS(error, dst);
}

#define  _              dummy_convert
#define  Y2NV12         _
#define  Y2NV21         _

static sv_image_t* BGR2Y(const sv_image_t* src, sv_pix_fmt_t fmt)
{
    auto* dst = sv_image_create(src->width, src->height, SV_PIX_FMT_GRAY8, src->orient);
    int error = libyuv::RGB24ToI400(
        src->data, src->stride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(error, dst);
}

static sv_image_t* BGRA2Y(const sv_image_t* src, sv_pix_fmt_t fmt)
{
    auto* dst = sv_image_create(src->width, src->height, SV_PIX_FMT_GRAY8, src->orient);
    int error = libyuv::ARGBToI400(
        src->data, src->stride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(error, dst);
}

static sv_image_t* RGB2Y(const sv_image_t* src, sv_pix_fmt_t fmt)
{
    auto* dst = sv_image_create(src->width, src->height, SV_PIX_FMT_GRAY8, src->orient);
    int error = libyuv::BGR24ToI400(
        src->data, src->stride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(error, dst);
}

static sv_image_t* RGBA2Y(const sv_image_t* src, sv_pix_fmt_t fmt)
{
    auto* dst = sv_image_create(src->width, src->height, SV_PIX_FMT_GRAY8, src->orient);
    int error = libyuv::ABGRToI400(
        src->data, src->stride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(error, dst);
}


sv_image_t * sv_image_convert(const sv_image_t * src, sv_pix_fmt_t fmt)
{
    typedef sv_image_t * (*cvt_func_t)(const sv_image_t*, sv_pix_fmt_t fmt);

    static cvt_func_t converts[][8] = {
            //  Y       BGR    BGRA    RGB     RGBA     J420,   NV12,  NV21
    /*Y    */{  Clone,  Y2BGR, Y2BGRA, Y2RGB,  Y2RGBA,  Y2J420,      _,      _},
    /*BGR  */{  BGR2Y,  Clone,      _,     _,       _,       _,      _,      _},
    /*BGRA */{ BGRA2Y,      _,  Clone,     _,       _,       _,      _,      _},
    /*RGB  */{  RGB2Y,      _,      _, Clone,       _,       _,      _,      _},
    /*RGBA */{ RGBA2Y,      _,      _,     _,   Clone,       _,      _,      _},
    /*I420 */{      _,      _,      _,     _,       _,   Clone,      _,      _},
    /*NV12 */{      _,      _,      _,     _,       _,       _,  Clone,      _},
    /*NV21 */{      _,      _,      _,     _,       _,       _,      _,  Clone}
    };

    auto src_id = SV_GET_FMT_ID(src->format);
    auto dst_id = SV_GET_FMT_ID(fmt);

    constexpr int num = sizeof(converts)/sizeof(converts[0]);

    if((unsigned)src_id < num && (unsigned)dst_id < num){
        return (*converts[src_id][dst_id])(src, fmt);
    }
    return  _(src, fmt);
}

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

#define CVT_FUNC(name) static sv_image_t* name(const sv_image_t * src, sv_pix_fmt_t fmt)
#define RETURN_ON_STATUS(error, res) \
    if(error){                       \
        LOGE("error: %d", error);    \
        sv_image_destroy(res);       \
        return nullptr;              \
    }                                \
    return res;                      \

static void SwapRBInplace(sv_image_t* src, int step)
{
    // TODO: FixMe
    // codes below assume src is continuos
    uint8_t* r = src->data;
    uint8_t* b = r + 2;
    for(int i = 0; i < src->width * src->height; ++i){
        uint8_t t = *r;
        *r = *b;
        *b = t;

        r += step;
        b += step;
    }
}

CVT_FUNC(unsupport_convert)
{
    LOGE("unsupport convert: %s => %s", sv_image_fmt_str(src->format), sv_image_fmt_str(fmt));
    return nullptr;
}

CVT_FUNC(Clone)
{
    return sv_image_clone(src);
}

CVT_FUNC(Y_BGR)
{
    sv_image_t * result = sv_image_create(src->width, src->height, fmt, src->format);
    assert(result);
    for(int r = 0; r < src->height; ++r){
        const uint8_t* s = src->data + r * src->stride;
        uint8_t *dst = result->data + r * result->stride;
        for(int c = 0; c < src->width; ++c){
            *dst++ = *s;
            *dst++ = *s;
            *dst++ = *s++;
        }
    }
    return result;
}

CVT_FUNC(Y_BGRA)
{
    auto * result = sv_image_create(src->width, src->height, fmt, src->orient);
    assert(result);
    int error = libyuv::J400ToARGB(
        src->data,
        src->stride,
        result->data,
        result->stride,
        result->width,
        result->height
    );
    RETURN_ON_STATUS(error, result);
}

CVT_FUNC(Y_J420)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
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


CVT_FUNC(BGR_Y)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int error = libyuv::RGB24ToI400(
        src->data, src->stride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(error, dst);
}

CVT_FUNC(BGRA_Y)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int error = libyuv::ARGBToI400(
        src->data, src->stride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(error, dst);
}

CVT_FUNC(RGB_Y)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int error = libyuv::BGR24ToI400(
        src->data, src->stride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(error, dst);
}

CVT_FUNC(RGBA_Y)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int error = libyuv::ABGRToI400(
        src->data, src->stride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(error, dst);
}

CVT_FUNC(I420_Y)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int error = libyuv::J420ToJ400(
        src->y, src->ystride,
        src->u, src->ustride,
        src->v, src->vstride,
        dst->y, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(error, dst);
}

CVT_FUNC(NV12_Y)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    for(int y = 0; y < dst->height; ++y){
        memcpy(dst->data + y * dst->stride, src->data + y * src->stride, dst->width);
    }
    return dst;
}

CVT_FUNC(BGRA_BGR)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int err = libyuv::ARGBToRGB24(
        src->data, src->stride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(err, dst);
}

CVT_FUNC(SwapRB)
{
    LOGI("there is an efficiency issue this call");
    // TODO: to optimize
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    
    // copy & swap
    for(int y = 0; y < dst->stride; ++y){
        uint8_t* r = dst->data + y * dst->stride;
        uint8_t* g = r + 1;
        uint8_t* b = g + 1;

        uint8_t* sr = src->data + y * src->stride;
        uint8_t* sg = sr + 1;
        uint8_t* sb = sg + 1;

        for(int x = 0; x < dst->width; ++x){
            *r = *sb;
            *g = *sg;
            *b = *sr;

            r += 3;
            g += 3;
            b += 3;

            sb += 3;
            sg += 3;
            sr += 3;
        }
    }
    return dst;
}

CVT_FUNC(I420_BGR)
{
    assert(fmt == SV_PIX_FMT_BGR);
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int err = libyuv::I420ToRGB24(
        src->y, src->ystride,
        src->u, src->ustride,
        src->v, src->vstride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(err, dst);
}

CVT_FUNC(I420_RGB)
{
    auto* dst = I420_BGR(src, fmt);
    SwapRBInplace(dst, 3);
    return dst;
}

CVT_FUNC(NV12_BGR)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int err = libyuv::NV12ToRGB24(
        src->y, src->ystride,
        src->u, src->ustride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(err, dst);
}

CVT_FUNC(NV12_RGB)
{
    auto* dst = NV12_BGR(src, fmt);
    SwapRBInplace(dst, 3);
    return dst;
}

CVT_FUNC(NV21_BGR)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int err = libyuv::NV21ToRGB24(
        src->y, src->ystride,
        src->u, src->ustride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(err, dst);    
}

CVT_FUNC(NV21_RGB)
{
    auto * dst = NV21_BGR(src, fmt);
    SwapRBInplace(dst, 3);
    return dst;    
}

CVT_FUNC(BGR_BGRA)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int err = libyuv::RGB24ToARGB(
        src->data, src->stride,
        dst->data, src->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(err, dst);
}



CVT_FUNC(RGB_BGRA)
{
    auto* dst = BGR_BGRA(src, fmt);
    SwapRBInplace(dst, 4);
    return dst;
}


CVT_FUNC(BGRA_RGB)
{
    auto *dst = BGRA_BGR(src, fmt);
    SwapRBInplace(dst, 3);
    return dst;
}

CVT_FUNC(BGRA_RGBA)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int err = libyuv::ARGBToABGR(
        src->data, src->stride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(err, dst);
}

CVT_FUNC(I420_BGRA)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int err = libyuv::J420ToARGB(
        src->y, src->ystride,
        src->u, src->ustride,
        src->v, src->vstride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(err, dst);
}

CVT_FUNC(I420_RGBA)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int err = libyuv::J420ToABGR(
        src->y, src->ystride,
        src->u, src->ustride,
        src->v, src->vstride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(err, dst);    
}

CVT_FUNC(NV12_BGRA)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int err = libyuv::NV12ToARGB(
        src->y, src->ystride,
        src->u, src->ustride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(err, dst);    
}

CVT_FUNC(NV12_RGBA)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int err = libyuv::NV12ToABGR(
        src->y, src->ystride,
        src->u, src->ustride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(err, dst);    
}

CVT_FUNC(NV21_RGBA)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int err = libyuv::NV21ToABGR(
        src->y, src->ystride,
        src->u, src->ustride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(err, dst);    
}

CVT_FUNC(NV21_BGRA)
{
    auto* dst = sv_image_create(src->width, src->height, fmt, src->orient);
    int err = libyuv::NV21ToARGB(
        src->y, src->ystride,
        src->u, src->ustride,
        dst->data, dst->stride,
        dst->width, dst->height
    );
    RETURN_ON_STATUS(err, dst);    
}

// equivalences
#define Y_RGB       Y_BGR
#define Y_RGBA      Y_BGRA
#define NV21_Y      NV12_Y
#define RGB_BGR     SwapRB
#define BGR_RGB     SwapRB
#define RGBA_RGB    BGRA_BGR
#define RGB_RGBA    BGR_BGRA
#define BGR_RGBA    RGB_BGRA
#define RGBA_BGRA   BGRA_RGBA
#define BGRA_BGR    RGBA_RGB
#define RGBA_BGR    BGRA_RGB

sv_image_t * sv_image_convert(const sv_image_t * src, sv_pix_fmt_t fmt)
{
    typedef sv_image_t * (*cvt_func_t)(const sv_image_t*, sv_pix_fmt_t fmt);

    #define _ unsupport_convert

    static cvt_func_t converts[][8] = {
            //  Y            BGR        BGRA    RGB          RGBA     J420,     NV12,    NV21
    /*Y    */{  Clone,     Y_BGR,    Y_BGRA,     Y_RGB,     Y_RGBA,   Y_J420,       _,       _},
    /*BGR  */{  BGR_Y,     Clone,  BGR_BGRA,   BGR_RGB,   BGR_RGBA,        _,       _,       _},
    /*BGRA */{ BGRA_Y,  BGRA_BGR,     Clone,  BGRA_RGB,  BGRA_RGBA,        _,       _,       _},
    /*RGB  */{  RGB_Y,   RGB_BGR,  RGB_BGRA,     Clone,   RGB_RGBA,        _,       _,       _},
    /*RGBA */{ RGBA_Y,  RGBA_BGR, BGRA_RGBA,  BGRA_BGR,       Clone,        _,       _,       _},
    /*I420 */{ I420_Y,  I420_BGR, I420_BGRA,  I420_RGB,   I420_RGBA,    Clone,       _,       _},
    /*NV12 */{ NV12_Y,  NV12_BGR, NV12_BGRA,  NV12_RGB,   NV12_RGBA,        _,   Clone,       _},
    /*NV21 */{ NV21_Y,  NV21_BGR, NV21_BGRA,  NV21_RGB,   NV21_RGBA,        _,       _,   Clone}
    };

    auto src_id = SV_GET_FMT_ID(src->format);
    auto dst_id = SV_GET_FMT_ID(fmt);

    constexpr int num = sizeof(converts)/sizeof(converts[0]);

    if((unsigned)src_id < num && (unsigned)dst_id < num){
        return (*converts[src_id][dst_id])(src, fmt);
    }
    return  _(src, fmt);
}

#ifndef __libsv_sv_image_h__
#define __libsv_sv_image_h__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SV_IMAGE_MAX_CHANNEL    3

/// format bit layout
/// 0      8      16     24     32
/// |  id  | Ybpp | Ubpp | Vbpp | 

#define SV_MAKE_PIX_FMT(id, Ybpp, Ubpp, Vbpp) (Ybpp | (Ubpp << 8) | (Vbpp << 16) | (id << 24))
#define SV_GET_FMT_ID(fmt)   ((fmt & 0xff000000) >> 24)
#define SV_GET_FMT_YBPP(fmt) (fmt & 0x000000ff)
#define SV_GET_FMT_UBPP(fmt) ((fmt & 0x0000ff00) >> 8)
#define SV_GET_FMT_VBPP(fmt) ((fmt & 0x00ff0000) >> 16)


enum{
    SV_PIX_FMT_GRAY8    = SV_MAKE_PIX_FMT(0, 8, 0, 0),
    SV_PIX_FMT_BGR888   = SV_MAKE_PIX_FMT(1, 24, 0, 0),
    SV_PIX_FMT_BGRA8888 = SV_MAKE_PIX_FMT(2, 32, 0, 0),        
    SV_PIX_FMT_RGB888   = SV_MAKE_PIX_FMT(3, 24, 0, 0),        
    SV_PIX_FMT_RGBA8888 = SV_MAKE_PIX_FMT(4, 32, 0, 0),        
    SV_PIX_FMT_J420     = SV_MAKE_PIX_FMT(5, 8, 2, 2), 
    SV_PIX_FMT_NV12     = SV_MAKE_PIX_FMT(6, 8, 4, 0),        
    SV_PIX_FMT_NV21     = SV_MAKE_PIX_FMT(7, 8, 4, 0),        // plane: 1; bbp: 32
};

enum{
    SV_ORIENT_TL = 1, ///< Horizontal (normal)
    SV_ORIENT_TR = 2, ///< Mirrored horizontal
    SV_ORIENT_BR = 3, ///< Rotate 180
    SV_ORIENT_BL = 4, ///< Mirrored vertical
    SV_ORIENT_LT = 5, ///< Mirrored horizontal & rotate 270 CW
    SV_ORIENT_RT = 6, ///< Rotate 90 CW
    SV_ORIENT_RB = 7, ///< Mirrored horizontal & rotate 90 CW
    SV_ORIENT_LB = 8  ///< Rotate 270 CW
};


typedef struct {
    union {
        uint8_t *data;
        uint8_t *y;
    };
    uint8_t *u;
    uint8_t *v;

    union {
        int stride;
        int ystride;
    };

    int ustride;
    int vstride;

    int             width   = 0;
    int             height  = 0;
    int            format   = SV_PIX_FMT_GRAY8;                          
    int            orient   = 1;    
    int           *refcount = NULL;    ///< NULL for external data, 
                                       ///< NonNULL for internal data                  
}sv_image_t;


sv_image_t * sv_image_create(int width=0, int height=0, int fmt=SV_PIX_FMT_GRAY8, int orient=1);
void         sv_image_destroy(sv_image_t* image);
int          sv_image_size(const sv_image_t* image);
int          sv_image_capacity(const sv_image_t * image);

sv_image_t * sv_image_clone(const sv_image_t* src);
void         sv_image_copyTo(const sv_image_t* src, sv_image_t* dst);
void         sv_image_swap(sv_image_t* lhs, sv_image_t* rhs);

sv_image_t * sv_image_rotate(const sv_image_t* image, int dst_orient);
sv_image_t * sv_image_convert(const sv_image_t* image, int dst_fmt);
sv_image_t * sv_image_resize(const sv_image_t* image, int dst_width, int dst_height);
sv_image_t * sv_image_roi(const sv_image_t* image, int x, int y, int width, int height);
sv_image_t * sv_image_crop(const sv_image_t* image, int x, int y, int width, int height);
sv_image_t * sv_image_warp(const sv_image_t* image, const float M[6], int width, int height);


void sv_image_str(const sv_image_t* image);

#ifdef __cplusplus
}
#endif


#endif  // __libsv_sv_image_h__

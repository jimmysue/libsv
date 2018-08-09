#ifndef __libsv_sv_codec_h_
#define __libsv_sv_codec_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "sv_image.h"

/// @brief read image
/// @param [in] filename filename
/// @return sv_image_t*
sv_image_t * sv_imread(const char* filename);

/// @brief save image
/// @param [in] filename save full path
/// @param [in] image the image to be saved
void sv_imwrite(const char* filename, const sv_image_t* image);


#ifdef __cplusplus
}
#endif

#endif // __libsv_sv_codec_h_
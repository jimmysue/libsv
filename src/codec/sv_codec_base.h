#ifndef __libsv_src_codec_sv_codec_base_h__
#define __libsv_src_codec_sv_codec_base_h__

#include <string>
#include <vector>
#include <memory>

#include "libsv/sv_image.h"

namespace libsv{


    struct ImageDecoder{
        virtual ~ImageDecoder() = default;
        virtual bool checkSignature(const std::string& signature) = 0;
        virtual int  signatureLength() = 0;
        virtual sv_image_t * read(const char* filename) = 0;
    };


    struct ImageEncoder{
        virtual ~ImageEncoder() = default;
        virtual void write(const char*filename, const sv_image_t* image) = 0;
        virtual  const std::string& ext() = 0;
    };


    typedef std::shared_ptr<ImageDecoder> (*DecoderCreator)(void);
    typedef std::shared_ptr<ImageEncoder> (*EncoderCreator)(void);

    struct CodecRegistry{
        CodecRegistry(EncoderCreator, DecoderCreator);
        
        
        static sv_image_t* read(const char* path);
        static void write(const char* path, const sv_image_t* image);

        static std::vector<std::shared_ptr<ImageDecoder>> & decoders();
        static std::vector<std::shared_ptr<ImageEncoder>> & encoders();
    };
    
};

#define REGISTER_CODEC(EncoderType, DecoderType)                                   \
std::shared_ptr<ImageEncoder> EncoderType ## Creator() {                           \
                                         return std::make_shared<EncoderType>(); } \
std::shared_ptr<ImageDecoder> DecoderType ## Creator() {                           \
                                         return std::make_shared<DecoderType>(); } \
static CodecRegistry __g_ ## EncoderType ## DecoderType (EncoderType ## Creator,   \
                                                         DecoderType ## Creator)



#endif //__libsv_src_codec_sv_codec_base_h__
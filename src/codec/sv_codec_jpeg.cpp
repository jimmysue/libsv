#include "sv_codec_base.h"


namespace libsv{

struct JpegEncoder : public ImageEncoder{

    virtual void write(const char* path, const sv_image_t * image)
    {
        // TODO
    }

    virtual  const std::string& ext()
    {
        return std::string("");
    }

};

struct JpegDecoder : public ImageDecoder{

    virtual bool checkSignature(const std::string& signature)
    {
        return false;
    }

    virtual int signatureLength()
    {
        return -1;
    }

    virtual sv_image_t* read(const char* path)
    {
        return nullptr;
    }
};

REGISTER_CODEC(JpegEncoder, JpegDecoder);

} // namespace libsv


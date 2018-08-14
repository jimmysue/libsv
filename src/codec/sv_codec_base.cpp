#include "sv_codec_base.h"

namespace libsv{

CodecRegistry::CodecRegistry(EncoderCreator encoder_creator, DecoderCreator decoder_creator)
{
    encoders().emplace_back( encoder_creator() );
    decoders().emplace_back( decoder_creator() );
}

std::vector<std::shared_ptr<ImageDecoder>> & CodecRegistry::decoders()
{
    static std::vector<std::shared_ptr<ImageDecoder>> _encoders;
    return _encoders;
}

std::vector<std::shared_ptr<ImageEncoder>> & CodecRegistry::encoders()
{
    static  std::vector<std::shared_ptr<ImageEncoder>> _decoders;
    return _decoders;
}

} // namespace libsv
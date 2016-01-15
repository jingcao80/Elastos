#include "elastos/droid/media/CMediaCodecInfoCodecProfileLevel.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaCodecInfoCodecProfileLevel, Object, IMediaCodecInfoCodecProfileLevel)

CAR_OBJECT_IMPL(CMediaCodecInfoCodecProfileLevel)

CMediaCodecInfoCodecProfileLevel::CMediaCodecInfoCodecProfileLevel()
    : mProfile(0)
    , mLevel(0)
{
}

CMediaCodecInfoCodecProfileLevel::~CMediaCodecInfoCodecProfileLevel()
{
}

ECode CMediaCodecInfoCodecProfileLevel::constructor()
{
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos

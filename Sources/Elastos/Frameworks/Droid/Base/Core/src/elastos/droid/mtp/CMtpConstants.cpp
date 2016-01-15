#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/mtp/CMtpConstants.h"

namespace Elastos {
namespace Droid {
namespace Mtp {

CAR_INTERFACE_IMPL(CMtpConstants, Singleton, IMtpConstants)

CAR_SINGLETON_IMPL(CMtpConstants)

ECode CMtpConstants::IsAbstractObject(
        /* [in] */ Int32 format,
        /* [out] */ Boolean* result)
{
    switch (format) {
        case IMtpConstants::FORMAT_ABSTRACT_MULTIMEDIA_ALBUM:
        case IMtpConstants::FORMAT_ABSTRACT_IMAGE_ALBUM:
        case IMtpConstants::FORMAT_ABSTRACT_AUDIO_ALBUM:
        case IMtpConstants::FORMAT_ABSTRACT_VIDEO_ALBUM:
        case IMtpConstants::FORMAT_ABSTRACT_AV_PLAYLIST:
        case IMtpConstants::FORMAT_ABSTRACT_AUDIO_PLAYLIST:
        case IMtpConstants::FORMAT_ABSTRACT_VIDEO_PLAYLIST:
        case IMtpConstants::FORMAT_ABSTRACT_MEDIACAST:
        case IMtpConstants::FORMAT_ABSTRACT_DOCUMENT:
            *result = TRUE;
        default:
            *result = FALSE;
    }
    return NOERROR;
}

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos

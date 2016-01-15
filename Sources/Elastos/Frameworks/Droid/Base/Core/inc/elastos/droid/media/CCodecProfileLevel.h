
#ifndef __ELASTOS_DROID_MEDIA_CCODECPROFILELEVEL_H__
#define __ELASTOS_DROID_MEDIA_CCODECPROFILELEVEL_H__

#include "_Elastos_Droid_Media_CCodecProfileLevel.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CCodecProfileLevel)
{
public:
    CCodecProfileLevel();

    CARAPI GetPrfile(
        /* [out] */ Int32* value);

    CARAPI SetProfile(
        /* [in] */ Int32 value);

    CARAPI GetLevel(
        /* [out] */ Int32* value);

    CARAPI SetLevel(
       /* [in] */ Int32 value);

private:
    Int32 mProfile;
    Int32 mLevel;
};


} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CCODECPROFILELEVEL_H__

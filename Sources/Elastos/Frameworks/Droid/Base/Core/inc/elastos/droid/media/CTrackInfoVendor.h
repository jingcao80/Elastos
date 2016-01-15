#ifndef __ELASTOS_DROID_MEDIA_CTRACKINFOVENDOR_H__
#define __ELASTOS_DROID_MEDIA_CTRACKINFOVENDOR_H__

#include "_Elastos_Droid_Media_CTrackInfoVendor.h"

namespace Elastos {
namespace Droid {
namespace Media {


CarClass(CTrackInfoVendor)
{
public:
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* name,
        /* [in] */ const String& charset);

    CARAPI SetName(
        /* [in] */ ArrayOf<Byte>* name);

    CARAPI GetName(
        /* [out, callee] */ ArrayOf<Byte>** name);

    CARAPI SetCharset(
        /* [in] */ const String& charset);

    CARAPI GetCharset(
        /* [out] */ String* charset);

public:
    AutoPtr<ArrayOf<Byte> > mName;
    String mCharset;
};


} // namespace media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CTRACKINFOVENDOR_H__

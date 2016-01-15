#ifndef __ELASTOS_DROID_MEDIA_TV_CTVINPUTHARDWAREINFO_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVINPUTHARDWAREINFO_H__

#include "_Elastos_Droid_Media_Tv_CTvInputHardwareInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * Simple container for information about TV input hardware.
 * Not for third-party developers.
 *
 * @hide
 */
// @SystemApi
CarClass(CTvInputHardwareInfo)
    , public Object
    , public ITvInputHardwareInfo
    , public IParcelable
{
public:
    friend class CTvInputHardwareInfoBuilder;

    CTvInputHardwareInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CTvInputHardwareInfo();

    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetDeviceId(
        /* [out] */ Int32* result);

    CARAPI GetType(
        /* [out] */ Int32* result);

    CARAPI GetAudioType(
        /* [out] */ Int32* result);

    CARAPI GetAudioAddress(
        /* [out] */ String* result);

    CARAPI GetHdmiPortId(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    static String TAG;

    Int32 mDeviceId;
    Int32 mType;
    Int32 mAudioType;
    Int32 mHdmiPortId;
    String mAudioAddress;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVINPUTHARDWAREINFO_H__

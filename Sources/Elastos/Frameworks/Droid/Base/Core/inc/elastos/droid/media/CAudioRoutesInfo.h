
#ifndef __ELASTOS_DROID_MEDIA_CAUDIOROUTESINFO_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOROUTESINFO_H__

#include "_Elastos_Droid_Media_CAudioRoutesInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Information available from AudioService about the current routes.
 * @hide
 */
CarClass(CAudioRoutesInfo)
    , public Object
    , public IAudioRoutesInfo
    , public IParcelable
{
public:
    CAudioRoutesInfo();

    virtual ~CAudioRoutesInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IAudioRoutesInfo* o);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* dest);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetBluetoothName(
        /* [out] */ ICharSequence** result);

    CARAPI SetBluetoothName(
        /* [in] */ ICharSequence* bluetoothName);

    CARAPI GetMainType(
        /* [out] */ Int32* result);

    CARAPI SetMainType(
        /* [in] */ Int32 mainType);

public:
    static const Int32 MAIN_SPEAKER;
    static const Int32 MAIN_HEADSET;
    static const Int32 MAIN_HEADPHONES;
    static const Int32 MAIN_DOCK_SPEAKERS;
    static const Int32 MAIN_HDMI;

private:
    AutoPtr<ICharSequence> mBluetoothName;
    Int32 mMainType;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIOROUTESINFO_H__

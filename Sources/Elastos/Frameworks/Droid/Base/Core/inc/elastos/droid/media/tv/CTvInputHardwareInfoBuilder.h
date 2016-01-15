#ifndef __ELASTOS_DROID_MEDIA_TV_CTVINPUTHARDWAREINFOBUILDER_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVINPUTHARDWAREINFOBUILDER_H__

#include "_Elastos_Droid_Media_Tv_CTvInputHardwareInfoBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CarClass(CTvInputHardwareInfoBuilder)
    , public Object
    , public ITvInputHardwareInfoBuilder
{
public:
    CTvInputHardwareInfoBuilder();

    virtual ~CTvInputHardwareInfoBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI DeviceId(
        /* [in] */ Int32 deviceId);

    CARAPI Type(
        /* [in] */ Int32 type);

    CARAPI AudioType(
        /* [in] */ Int32 audioType);

    CARAPI AudioAddress(
        /* [in] */ const String& audioAddress);

    CARAPI HdmiPortId(
        /* [in] */ Int32 hdmiPortId);

    CARAPI Build(
        /* [out] */ ITvInputHardwareInfo** result);

private:
    String mAudioAddress;
    Int32 mAudioType;
    AutoPtr<IInteger32> mDeviceId;
    AutoPtr<IInteger32> mType;
    AutoPtr<IInteger32> mHdmiPortId;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVINPUTHARDWAREINFOBUILDER_H__

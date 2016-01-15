#ifndef __ELASTOS_DROID_MEDIA_TV_CTVINPUTMANAGERHARDWARE_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVINPUTMANAGERHARDWARE_H__

#include "_Elastos_Droid_Media_Tv_CTvInputManagerHardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ISurface;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * The Hardware provides the per-hardware functionality of TV hardware.
 *
 * TV hardware is physical hardware attached to the Android device; for example, HDMI ports,
 * Component/Composite ports, etc. Specifically, logical devices such as HDMI CEC logical
 * devices don't fall into this category.
 *
 * @hide
 */
// @SystemApi
CarClass(CTvInputManagerHardware)
    , public Object
    , public ITvInputManagerHardware
{
public:
    friend class CTvInputManager;

    CTvInputManagerHardware();

    virtual ~CTvInputManagerHardware();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IITvInputHardware* hardwareInterface);

    CARAPI SetSurface(
        /* [in] */ ISurface* surface,
        /* [in] */ ITvStreamConfig* config,
        /* [out] */ Boolean* result);

    CARAPI SetStreamVolume(
        /* [in] */ Float volume);

    CARAPI DispatchKeyEventToHdmi(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OverrideAudioSink(
        /* [in] */ Int32 audioType,
        /* [in] */ const String& audioAddress,
        /* [in] */ Int32 samplingRate,
        /* [in] */ Int32 channelMask,
        /* [in] */ Int32 format);

private:
    CARAPI_(AutoPtr<IITvInputHardware>) GetInterface();

private:
    AutoPtr<IITvInputHardware> mInterface;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVINPUTMANAGERHARDWARE_H__

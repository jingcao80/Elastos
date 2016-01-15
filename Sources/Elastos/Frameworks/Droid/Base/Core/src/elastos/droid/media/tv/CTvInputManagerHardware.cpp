#include "elastos/droid/media/tv/CTvInputManagerHardware.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_INTERFACE_IMPL(CTvInputManagerHardware, Object, ITvInputManagerHardware)

CAR_OBJECT_IMPL(CTvInputManagerHardware)

CTvInputManagerHardware::CTvInputManagerHardware()
{
}

CTvInputManagerHardware::~CTvInputManagerHardware()
{
}

ECode CTvInputManagerHardware::constructor(
    /* [in] */ IITvInputHardware* hardwareInterface)
{
    mInterface = hardwareInterface;
    return NOERROR;
}

ECode CTvInputManagerHardware::SetSurface(
    /* [in] */ ISurface* surface,
    /* [in] */ ITvStreamConfig* config,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return mInterface->SetSurface(surface, config, result);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManagerHardware::SetStreamVolume(
    /* [in] */ Float volume)
{
    // try {
    return mInterface->SetStreamVolume(volume);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManagerHardware::DispatchKeyEventToHdmi(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return mInterface->DispatchKeyEventToHdmi(event, result);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManagerHardware::OverrideAudioSink(
    /* [in] */ Int32 audioType,
    /* [in] */ const String& audioAddress,
    /* [in] */ Int32 samplingRate,
    /* [in] */ Int32 channelMask,
    /* [in] */ Int32 format)
{
    // try {
    return mInterface->OverrideAudioSink(audioType,
        audioAddress, samplingRate, channelMask, format);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

AutoPtr<IITvInputHardware> CTvInputManagerHardware::GetInterface()
{
    return mInterface;
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

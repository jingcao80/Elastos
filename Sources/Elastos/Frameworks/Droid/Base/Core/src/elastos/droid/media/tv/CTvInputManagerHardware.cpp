//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

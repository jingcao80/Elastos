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

#ifndef __ELASTOS_DROID_INCALLUI_AUDIOMODEPROVIDER_H__
#define __ELASTOS_DROID_INCALLUI_AUDIOMODEPROVIDER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos.Droid.Dialer.h"
#include <elastos/droid/telecom/Phone.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Telecom::IAudioState;
using Elastos::Droid::Telecom::IPhone;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * Proxy class for getting and setting the audio mode.
 */
class AudioModeProvider
    : public Object
    , public IInCallPhoneListener
{
private:
    class PhoneListener
        : public Elastos::Droid::Telecom::Phone::Listener
    {
    public:
        PhoneListener(
            /* [in] */ AudioModeProvider* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnAudioStateChanged(
            /* [in] */ IPhone* phone,
            /* [in] */ IAudioState* audioState);

    private:
        AudioModeProvider* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    AudioModeProvider();

    static CARAPI_(AutoPtr<AudioModeProvider>) GetInstance();

    // @Override
    CARAPI SetPhone(
        /* [in] */ IPhone* phone);

    // @Override
    CARAPI ClearPhone();

    CARAPI_(void) OnAudioModeChange(
        /* [in] */ Int32 newMode,
        /* [in] */ Boolean muted);

    CARAPI_(void) OnSupportedAudioModeChange(
        /* [in] */ Int32 newModeMask);

    CARAPI_(void) AddListener(
        /* [in] */ IAudioModeListener* listener);

    CARAPI_(void) RemoveListener(
        /* [in] */ IAudioModeListener* listener);

    CARAPI_(Int32) GetSupportedModes();

    CARAPI_(Int32) GetAudioMode();

    CARAPI_(Boolean) GetMute();

public:
    static const Int32 AUDIO_MODE_INVALID;

private:
    static AutoPtr<AudioModeProvider> sAudioModeProvider;
    Int32 mAudioMode;
    Boolean mMuted;
    Int32 mSupportedModes;
    AutoPtr<IList> mListeners;
    AutoPtr<IPhone> mPhone;

    AutoPtr<PhoneListener> mPhoneListener;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_AUDIOMODEPROVIDER_H__

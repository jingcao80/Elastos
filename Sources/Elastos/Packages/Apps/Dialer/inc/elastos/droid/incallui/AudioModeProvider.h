
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

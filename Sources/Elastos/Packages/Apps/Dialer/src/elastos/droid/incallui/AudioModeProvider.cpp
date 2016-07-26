
#include "elastos/droid/incallui/AudioModeProvider.h"

using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//==========================================================
// AudioModeProvider::PhoneListener
//==========================================================
ECode AudioModeProvider::PhoneListener::OnAudioStateChanged(
    /* [in] */ IPhone* phone,
    /* [in] */ IAudioState* audioState)
{
    Boolean isMuted;
    audioState->GetIsMuted(&isMuted);
    Int32 route, supportedRouteMask;
    audioState->GetRoute(&route);
    audioState->GetSupportedRouteMask(&supportedRouteMask);
    mHost->OnAudioModeChange(route, isMuted);
    mHost->OnSupportedAudioModeChange(supportedRouteMask);
    return NOERROR;
}


//==========================================================
// AudioModeProvider
//==========================================================
const Int32 AudioModeProvider::AUDIO_MODE_INVALID = 0;
AutoPtr<AudioModeProvider> AudioModeProvider::sAudioModeProvider = new AudioModeProvider();

CAR_INTERFACE_IMPL(AudioModeProvider, Object, IInCallPhoneListener)

AudioModeProvider::AudioModeProvider()
    : mAudioMode(IAudioState::ROUTE_EARPIECE)
    , mMuted(FALSE)
    , mSupportedModes(IAudioState::ROUTE_ALL)
{
    CArrayList::New((IList**)&mListeners);
    mPhoneListener = new PhoneListener(this);
}

AutoPtr<AudioModeProvider> AudioModeProvider::GetInstance()
{
    return sAudioModeProvider;
}

ECode AudioModeProvider::SetPhone(
    /* [in] */ IPhone* phone)
{
    mPhone = phone;
    mPhone->AddListener(mPhoneListener);
    return NOERROR;
}

ECode AudioModeProvider::ClearPhone()
{
    mPhone->RemoveListener(mPhoneListener);
    mPhone = NULL;
    return NOERROR;
}

void AudioModeProvider::OnAudioModeChange(
    /* [in] */ Int32 newMode,
    /* [in] */ Boolean muted)
{
    if (mAudioMode != newMode) {
        mAudioMode = newMode;
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            IAudioModeListener* l = IAudioModeListener::Probe(obj);
            l->OnAudioMode(mAudioMode);
        }
    }

    if (mMuted != muted) {
        mMuted = muted;
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            IAudioModeListener* l = IAudioModeListener::Probe(obj);
            l->OnMute(mMuted);
        }
    }
}

void AudioModeProvider::OnSupportedAudioModeChange(
    /* [in] */ Int32 newModeMask)
{
    mSupportedModes = newModeMask;

    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IAudioModeListener* l = IAudioModeListener::Probe(obj);
        l->OnSupportedAudioMode(mSupportedModes);
    }
}

void AudioModeProvider::AddListener(
    /* [in] */ IAudioModeListener* listener)
{
    Boolean contains;
    if (mListeners->Contains(listener, &contains), !contains) {
        mListeners->Add(listener);
        listener->OnSupportedAudioMode(mSupportedModes);
        listener->OnAudioMode(mAudioMode);
        listener->OnMute(mMuted);
    }
}

void AudioModeProvider::RemoveListener(
    /* [in] */ IAudioModeListener* listener)
{
    Boolean contains;
    if (mListeners->Contains(listener, &contains), contains) {
        mListeners->Remove(listener);
    }
}

Int32 AudioModeProvider::GetSupportedModes()
{
    return mSupportedModes;
}

Int32 AudioModeProvider::GetAudioMode()
{
    return mAudioMode;
}

Boolean AudioModeProvider::GetMute()
{
    return mMuted;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

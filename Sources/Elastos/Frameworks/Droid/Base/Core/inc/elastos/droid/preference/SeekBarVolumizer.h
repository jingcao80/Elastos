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

#ifndef __ELASTOS_DROID_PREFERENCE_SEEKBARVOLUMIZER_H__
#define __ELASTOS_DROID_PREFERENCE_SEEKBARVOLUMIZER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Preference.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IRingtone;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::ISeekBar;

namespace Elastos {
namespace Droid {
namespace Preference {

class ECO_PUBLIC SeekBarVolumizer
    : public Object
    , public ISeekBarVolumizer
    , public IHandlerCallback
{
private:
    class InnerListener
        : public Object
        , public ISeekBarOnSeekBarChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("SeekBarVolumizer::InnerListener")

        InnerListener(
            /* [in] */ SeekBarVolumizer* host);

        CARAPI OnProgressChanged(
            /* [in] */ ISeekBar* seekBar,
            /* [in] */ Int32 progress,
            /* [in] */ Boolean fromTouch);

        CARAPI OnStartTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

        CARAPI OnStopTrackingTouch(
            /* [in] */ ISeekBar* seekBar);
    private:
        SeekBarVolumizer* mHost;
    };

    class ECO_LOCAL SeekBarVolumizerH
        : public Handler
    {
        friend class SeekBarVolumizer;
    public:
        TO_STRING_IMPL("SeekBarVolumizer::SeekBarVolumizerH")

        SeekBarVolumizerH(
            /* [in] */ SeekBarVolumizer* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI PostUpdateSlider(
            /* [in] */ Int32 volume);

    private:
        const static Int32 UPDATE_SLIDER = 1;
        SeekBarVolumizer* mHost;
    };

    class ECO_LOCAL SeekBarVolumizerObserver
        : public ContentObserver
    {
        friend class SeekBarVolumizer;
    public:
        TO_STRING_IMPL("SeekBarVolumizer::SeekBarVolumizerObserver")

        CARAPI constructor(
            /* [in] */ IHandler* handler,
            /* [in] */ SeekBarVolumizer* host);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);
    private:
        SeekBarVolumizer* mHost;
    };

    class ECO_LOCAL SeekBarVolumizerReceiver
        : public BroadcastReceiver
    {
        friend class SeekBarVolumizer;
    public:
        TO_STRING_IMPL("SeekBarVolumizer::SeekBarVolumizerReceiver")

        SeekBarVolumizerReceiver(
            /* [in] */ SeekBarVolumizer* host);

        CARAPI_(void) SetListening(
            /* [in] */ Boolean listening);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    private:
        Boolean mListening;
        SeekBarVolumizer* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SeekBarVolumizer();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 streamType,
        /* [in] */ IUri* defaultUri,
        /* [in] */ ISeekBarVolumizerCallback* callback);

    CARAPI SetSeekBar(
        /* [in] */ ISeekBar* seekBar);

    CARAPI HandleMessage(
        /* [in] */ IMessage* msg,
        /* [out] */ Boolean* result);

    CARAPI PostStopSample();

    CARAPI Stop();

    CARAPI RevertVolume();

    CARAPI OnProgressChanged(
        /* [in] */ ISeekBar* seekBar,
        /* [in] */ Int32 progress,
        /* [in] */ Boolean fromTouch);

    CARAPI PostSetVolume(
        /* [in] */ Int32 progress);

    CARAPI OnStartTrackingTouch(
        /* [in] */ ISeekBar* seekBar);

    CARAPI OnStopTrackingTouch(
        /* [in] */ ISeekBar* seekBar);

    CARAPI IsSamplePlaying(
        /* [out] */ Boolean* result);

    CARAPI StartSample();

    CARAPI StopSample();

    CARAPI GetSeekBar(
        /* [out] */ ISeekBar** seekBar);

    CARAPI ChangeVolumeBy(
        /* [in] */ Int32 amount);

    CARAPI MuteVolume();

    CARAPI OnSaveInstanceState(
        /* [in] */ IVolumePreferenceVolumeStore* volumeStore);

    CARAPI OnRestoreInstanceState(
        /* [in] */ IVolumePreferenceVolumeStore* volumeStore);

private:
    ECO_LOCAL CARAPI_(void) OnInitSample();
    ECO_LOCAL CARAPI_(void) PostStartSample();
    ECO_LOCAL CARAPI_(void) OnStartSample();
    ECO_LOCAL CARAPI_(void) OnStopSample();

private:
    ECO_LOCAL const static String TAG;
    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mHandler;
    AutoPtr<SeekBarVolumizerH> mUiHandler;
    AutoPtr<ISeekBarVolumizerCallback> mCallback;
    AutoPtr<IUri> mDefaultUri;
    AutoPtr<IAudioManager> mAudioManager;
    Int32 mStreamType;
    Int32 mMaxStreamVolume;
    AutoPtr<SeekBarVolumizerReceiver> mReceiver;
    AutoPtr<SeekBarVolumizerObserver> mVolumeObserver;

    Int32 mOriginalStreamVolume;
    AutoPtr<IRingtone> mRingtone;
    Int32 mLastProgress;
    AutoPtr<ISeekBar> mSeekBar;
    Int32 mVolumeBeforeMute;

    ECO_LOCAL const static Int32 MSG_SET_STREAM_VOLUME = 0;
    ECO_LOCAL const static Int32 MSG_START_SAMPLE = 1;
    ECO_LOCAL const static Int32 MSG_STOP_SAMPLE = 2;
    ECO_LOCAL const static Int32 MSG_INIT_SAMPLE = 3;
    ECO_LOCAL const static Int32 CHECK_RINGTONE_PLAYBACK_DELAY_MS = 1000;
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_SEEKBARVOLUMIZER_H__
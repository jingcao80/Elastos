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

#ifndef __ELASTOS_DROID_DIALER_SETTINGS_GENERALSETTINGSFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_SETTINGS_GENERALSETTINGSFRAGMENT_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/preference/PreferenceFragment.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Preference.h"
#include "Elastos.CoreLibrary.Core.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Preference::PreferenceFragment;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Settings {

class GeneralSettingsFragment
    : public PreferenceFragment
    , public IGeneralSettingsFragment
    , public IPreferenceOnPreferenceChangeListener
{
private:
    class RingtoneLookupCompleteHandler
        : public Handler
    {
    public:
        RingtoneLookupCompleteHandler(
            /* [in] */ GeneralSettingsFragment* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        GeneralSettingsFragment* mHost;
    };

    class RingtoneLookupRunnable
        : public Runnable
    {
    public:
        RingtoneLookupRunnable(
            /* [in] */ GeneralSettingsFragment* host);

        // @Override
        CARAPI Run();

    private:
        GeneralSettingsFragment* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    GeneralSettingsFragment();

    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    /**
     * Supports onPreferenceChangeListener to look for preference changes.
     *
     * @param preference The preference to be changed
     * @param objValue The value of the selection, NOT its localized display value.
     */
    // @Override
    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* objValue,
        /* [out] */ Boolean* result);

    /**
     * Click listener for toggle events.
     */
    // @Override
    CARAPI OnPreferenceTreeClick(
        /*[in]*/ IPreferenceScreen* preferenceScreen,
        /*[in]*/ IPreference* preference,
        /*[out]*/ Boolean* result);

    // @Override
    CARAPI OnResume();

private:
    static const String CATEGORY_SOUNDS_KEY;
    static const String BUTTON_RINGTONE_KEY;
    static const String BUTTON_VIBRATE_ON_RING;
    static const String BUTTON_PLAY_DTMF_TONE;
    static const String BUTTON_RESPOND_VIA_SMS_KEY;

    static const Int32 MSG_UPDATE_RINGTONE_SUMMARY;

    AutoPtr<IContext> mContext;

    AutoPtr<IPreference> mRingtonePreference;
    AutoPtr<ICheckBoxPreference> mVibrateWhenRinging;
    AutoPtr<ICheckBoxPreference> mPlayDtmfTone;
    AutoPtr<IPreference> mRespondViaSms;

    AutoPtr<IRunnable> mRingtoneLookupRunnable;
    AutoPtr<IHandler> mRingtoneLookupComplete;
};

} // Settings
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_SETTINGS_GENERALSETTINGSFRAGMENT_H__

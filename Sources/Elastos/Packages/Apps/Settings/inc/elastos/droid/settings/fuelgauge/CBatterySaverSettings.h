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

#ifndef __ELASTOS_DROID_SETTINGS_FUELGAUGE_CBATTERYSAVERSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_FUELGAUGE_CBATTERYSAVERSETTINGS_H__

#include "_Elastos_Droid_Settings_Fuelgauge_CBatterySaverSettings.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/settings/notification/SettingPref.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::Settings::Notification::SettingPref;
using Elastos::Droid::Settings::Widget::ISwitchBar;
using Elastos::Droid::Settings::Widget::ISwitchBarOnSwitchChangeListener;
using Elastos::Droid::Settings::SettingsPreferenceFragment;

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Widget::ISwitch;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

CarClass(CBatterySaverSettings)
    , public SettingsPreferenceFragment
{
public:
    class InnerListener
        : public Object
        , public ISwitchBarOnSwitchChangeListener
    {
    public:
        TO_STRING_IMPL("CBatterySaverSettings::InnerListener")

        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CBatterySaverSettings* host);

        //@Override
        CARAPI OnSwitchChanged(
            /* [in] */ ISwitch* switchView,
            /* [in] */ Boolean isChecked);

    private:
        CBatterySaverSettings* mHost;
    };

private:
    class OnActivityCreatedSettingPref
        : public SettingPref
    {
    public:
        TO_STRING_IMPL("CBatterySaverSettings::OnActivityCreatedSettingPref")

        OnActivityCreatedSettingPref(
            /* [in] */ Int32 type,
            /* [in] */ const String& key,
            /* [in] */ const String& setting,
            /* [in] */ Int32 def,
            /* [in] */ ArrayOf<Int32>* values);

        //@Override
        virtual CARAPI GetCaption(
            /* [in] */ IResources* res,
            /* [in] */ Int32 value,
            /* [out] */ String* result);
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CBatterySaverSettings::MyRunnable")

        MyRunnable(
            /* [in] */ CBatterySaverSettings* host,
            /* [in] */ Int32 id);

        //@Override
        CARAPI Run();

    private:
        CBatterySaverSettings* mHost;
        Int32 mId;
    };

    class Runnable2
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CBatterySaverSettings::Runnable2")

        Runnable2(
            /* [in] */ CBatterySaverSettings* host);

        //@Override
        CARAPI Run();

    private:
        CBatterySaverSettings* mHost;
    };

    class Receiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CBatterySaverSettings::Receiver")

        Receiver(
            /* [in] */ CBatterySaverSettings* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI SetListening(
            /* [in] */ Boolean listening);

    private:
        Boolean mRegistered;
        CBatterySaverSettings* mHost;
    };

    class SettingsObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CBatterySaverSettings::SettingsObserver")

        SettingsObserver(
            /* [in] */ CBatterySaverSettings* host);

        CARAPI constructor(
            /* [in] */ IHandler* handler);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

        CARAPI SetListening(
            /* [in] */ Boolean listening);

    private:
        AutoPtr<IUri> LOW_POWER_MODE_TRIGGER_LEVEL_URI;
        CBatterySaverSettings* mHost;
    };

public:
    TO_STRING_IMPL("CBatterySaverSettings")

    CAR_OBJECT_DECL()

    CBatterySaverSettings();

    ~CBatterySaverSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnDestroyView();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnSwitchChanged(
        /* [in] */ ISwitch* switchView,
        /* [in] */ Boolean isChecked);

private:
    CARAPI_(void) TrySetPowerSaveMode(
        /* [in] */ Boolean mode);

    CARAPI_(void) UpdateSwitch();

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const String KEY_TURN_ON_AUTOMATICALLY;
    static const Int64 WAIT_FOR_SWITCH_ANIM;

    AutoPtr<IHandler> mHandler;
    AutoPtr<SettingsObserver> mSettingsObserver;
    AutoPtr<Receiver> mReceiver;

    AutoPtr<IContext> mContext;
    Boolean mCreated;
    AutoPtr<SettingPref> mTriggerPref;
    AutoPtr<ISwitchBar> mSwitchBar;
    AutoPtr<ISwitch> mSwitch;
    Boolean mValidListener;
    AutoPtr<IPowerManager> mPowerManager;

    AutoPtr<MyRunnable> mUpdateSwitch;

    AutoPtr<MyRunnable> mStartMode;
    AutoPtr<InnerListener> mListener;
};

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_FUELGAUGE_CBATTERYSAVERSETTINGS_H__
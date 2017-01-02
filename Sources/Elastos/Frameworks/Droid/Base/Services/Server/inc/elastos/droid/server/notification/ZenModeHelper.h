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

#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_ZENMODEHELPER_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_ZENMODEHELPER_H__

#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Service.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Service::Notification::IZenModeConfig;
using Elastos::Core::ICharSequence;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

/**
 * NotificationManagerService helper for functionality related to zen mode.
 */
class NotificationRecord;
class ValidateNotificationPeople;

class ZenModeHelper
    : public Object
{
public:
    class Callback
        : public Object
    {
    public:
        Callback();

        ~Callback();

        CARAPI_(void) OnConfigChanged();

        CARAPI_(void) OnZenModeChanged();
    };

private:
    class SettingsObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("ZenModeHelper::SettingsObserver")

        CARAPI constructor(
            /* [in] */ IHandler* handle,
            /* [in] */ ZenModeHelper* host);

        ~SettingsObserver();

        CARAPI_(void) Observe();

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

        CARAPI_(void) Update(
            /* [in] */ IUri* uri);

    private:
        ZenModeHelper* mHost;
        AutoPtr<IUri> ZEN_MODE;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ ZenModeHelper* host);

        ~MyRunnable();

        // @Override
        CARAPI Run();

    private:
        ZenModeHelper* mHost;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ ZenModeHelper* host);

        ~MyBroadcastReceiver();

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        // @Override
        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = "ZenModeHelper::MyBroadcastReceiver: ";
            info->AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        ZenModeHelper* mHost;
    };

public:
    ZenModeHelper(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler);

    ~ZenModeHelper();

    static CARAPI ReadDefaultConfig(
        /* [in] */ IResources* resources,
        /* [out] */ IZenModeConfig** _config);

    CARAPI_(void) AddCallback(
        /* [in] */ Callback* callback);

    CARAPI_(void) SetAudioManager(
        /* [in] */ IAudioManager* audioManager);

    CARAPI_(Int32) GetZenModeListenerInterruptionFilter();

    CARAPI_(void) RequestFromListener(
        /* [in] */ Int32 interruptionFilter);

    CARAPI_(Boolean) ShouldIntercept(
        /* [in] */ NotificationRecord* record);

    CARAPI_(Int32) GetZenMode();

    CARAPI_(void) SetZenMode(
        /* [in] */ Int32 zenModeValue,
        /* [in] */ const String& reason);

    CARAPI_(void) UpdateZenMode();

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI ReadXml(
        /* [in] */ IXmlPullParser* parser);

    CARAPI WriteXml(
        /* [in] */ IXmlSerializer* out);

    CARAPI_(AutoPtr<IZenModeConfig>) GetConfig();

    CARAPI_(Boolean) SetConfig(
        /* [in] */ IZenModeConfig* config);

    CARAPI_(Boolean) IsCall(
        /* [in] */ NotificationRecord* record);

    /**
     * @param extras extras of the notification with EXTRA_PEOPLE populated
     * @param contactsTimeoutMs timeout in milliseconds to wait for contacts response
     * @param timeoutAffinity affinity to return when the timeout specified via
     *                        <code>contactsTimeoutMs</code> is hit
     */
    CARAPI_(Boolean) MatchesCallFilter(
        /* [in] */ IUserHandle* userHandle,
        /* [in] */ IBundle* extras,
        /* [in] */ ValidateNotificationPeople* validator,
        /* [in] */ Int32 contactsTimeoutMs,
        /* [in] */ Float timeoutAffinity);

private:
    static CARAPI_(Int32) ZenModeFromListenerInterruptionFilter(
        /* [in] */ Int32 listenerInterruptionFilter,
        /* [in] */ Int32 defValue);

    CARAPI_(Boolean) ShouldInterceptAudience(
        /* [in] */ NotificationRecord* record);

    CARAPI_(void) HandleRingerModeChanged();

    CARAPI_(void) DispatchOnConfigChanged();

    CARAPI_(void) DispatchOnZenModeChanged();

    static CARAPI_(Boolean) IsSystem(
        /* [in] */ NotificationRecord* record);

    static CARAPI_(Boolean) IsAlarm(
        /* [in] */ NotificationRecord* record);

    static CARAPI_(Boolean) IsEvent(
        /* [in] */ NotificationRecord* record);

    CARAPI_(Boolean) IsDefaultPhoneApp(
        /* [in] */ const String& pkg);

    CARAPI_(Boolean) IsDefaultMessagingApp(
        /* [in] */ NotificationRecord* record);

    CARAPI_(Boolean) IsMessage(
        /* [in] */ NotificationRecord* record);

    CARAPI_(Boolean) AudienceMatches(
        /* [in] */ Float contactAffinity);

private:
    static const String TAG;
    static const Boolean DEBUG;// Log.isLoggable(TAG, Log.DEBUG);

    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mHandler;
    AutoPtr<SettingsObserver> mSettingsObserver;
    AutoPtr<IAppOpsManager> mAppOps;
    AutoPtr<IZenModeConfig> mDefaultConfig;
    /*ArrayList<Callback> mCallbacks = new ArrayList<Callback>();*/
    AutoPtr<IArrayList> mCallbacks;

    AutoPtr<IComponentName> mDefaultPhoneApp;
    Int32 mZenMode;
    AutoPtr<IZenModeConfig> mConfig;
    AutoPtr<IAudioManager> mAudioManager;
    Int32 mPreviousRingerMode;

    AutoPtr<IRunnable> mRingerModeChanged;
    AutoPtr<IBroadcastReceiver> mReceiver;
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_ZENMODEHELPER_H__

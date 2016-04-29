
#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEUI_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEUI_H__

#include "_Elastos_Droid_SystemUI_Volume_CVolumeUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/systemui/SystemUI.h"
#include "Elastos.Droid.Net.h"

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IIRemoteVolumeController;
using Elastos::Droid::Media::IIVolumeController;
using Elastos::Droid::Media::Session::IISessionController;
using Elastos::Droid::Media::Session::IMediaSessionManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::SystemUI::SystemUI;
using Elastos::Droid::SystemUI::StatusBar::Policy::IZenModeController;
using Elastos::Droid::SystemUI::Volume::IVolumePanel;
using Elastos::Droid::SystemUI::Volume::IVolumePanelCallback;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Volume {

/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
CarClass(CVolumeUI)
    , public SystemUI
{
private:
    class MyObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CVolumeUI::MyObserver")

        MyObserver(
            /* [in] */ CVolumeUI* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        CVolumeUI* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ CVolumeUI* host);

        CARAPI Run();

    private:
        CVolumeUI* mHost;
    };

    class MyVolumePanelCallback
        : public Object
        , public IVolumePanelCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyVolumePanelCallback(
            /* [in] */ CVolumeUI* host);

        // @Override
        CARAPI OnZenSettings();

        // @Override
        CARAPI OnInteraction();
        // @Override
        CARAPI OnVisible(
            /* [in] */ Boolean visible);

    private:
        CVolumeUI* mHost;
    };

public:
    /** For now, simply host an unmodified base volume panel in this process. */
    class VolumeController
        : public Object
        , public IIVolumeController
        , public IBinder
        , public IVolumeComponent
    {
    public:
        CAR_INTERFACE_DECL();

        VolumeController();

        CARAPI constructor(
            /* [in] */ Handle32 host);

        // @Override
        CARAPI DisplaySafeVolumeWarning(
            /* [in] */ Int32 flags);

        // @Override
        CARAPI VolumeChanged(
            /* [in] */ Int32 streamType,
            /* [in] */ Int32 flags);

        // @Override
        CARAPI MasterVolumeChanged(
            /* [in] */ Int32 flags);

        // @Override
        CARAPI MasterMuteChanged(
            /* [in] */ Int32 flags);

        // @Override
        CARAPI SetLayoutDirection(
            /* [in] */ Int32 layoutDirection);

        // @Override
        CARAPI Dismiss();

        // @Override
        CARAPI GetZenController(
            /* [out] */ IZenModeController** zmc);

        VolumeController(
            /* [in] */ CVolumeUI* host);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        CVolumeUI* mHost;
    };

    class RemoteVolumeController
        : public Object
        , public IIRemoteVolumeController
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        RemoteVolumeController();

        CARAPI constructor(
            /* [in] */ Handle32 host);

        // @Override
        CARAPI RemoteVolumeChanged(
            /* [in] */ IISessionController* binder,
            /* [in] */ Int32 flags);

        // @Override
        CARAPI UpdateRemoteController(
            /* [in] */ IISessionController* session);

        RemoteVolumeController(
            /* [in] */ CVolumeUI* host);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        CVolumeUI* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CVolumeUI();

    // @Override
    CARAPI Start();

protected:
    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

private:
    CARAPI_(void) UpdateController();

    CARAPI_(void) InitPanel();

private:
    const static String TAG;
    const static String SETTING;  // for testing
    const static AutoPtr<IUri> SETTING_URI;
    const static Int32 DEFAULT = 1;  // enabled by default

    AutoPtr<IHandler> mHandler;

    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<IMediaSessionManager> mMediaSessionManager;
    AutoPtr<VolumeController> mVolumeController;
    AutoPtr<RemoteVolumeController> mRemoteVolumeController;

    AutoPtr<IVolumePanel> mPanel;
    Int32 mDismissDelay;

    AutoPtr<MyObserver> mObserver;
    AutoPtr<MyRunnable> mStartZenSettings;
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEUI_H__

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

#ifndef __ELASTOS_DROID_SETTINGS_WFD_CWIFIDISPLAYSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_WFD_CWIFIDISPLAYSETTINGS_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Wifi.h"
#include "_Elastos_Droid_Settings_Wfd_CWifiDisplaySettings.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/media/MediaRouterSimpleCallback.h"
#include "elastos/droid/preference/CheckBoxPreference.h"
#include "elastos/droid/preference/ListPreference.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Hardware::Display::IWifiDisplay;
using Elastos::Droid::Hardware::Display::IWifiDisplayStatus;
using Elastos::Droid::Media::IMediaRouter;
using Elastos::Droid::Media::IMediaRouterRouteInfo;
using Elastos::Droid::Media::IMediaRouterCallback;
using Elastos::Droid::Media::MediaRouterSimpleCallback;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Wifi::P2p::IWifiP2pManager;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerActionListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerChannel;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Preference::CheckBoxPreference;
using Elastos::Droid::Preference::ListPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceOnPreferenceClickListener;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerActionListener;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wfd {

/**
 * The Settings screen for WifiDisplay configuration and connection management.
 *
 * The wifi display routes are integrated together with other remote display routes
 * from the media router.  It may happen that wifi display isn't actually available
 * on the system.  In that case, the enable option will not be shown but other
 * remote display routes will continue to be made available.
 */
CarClass(CWifiDisplaySettings)
    , public SettingsPreferenceFragment
{
public:
    class RoutePreference
        : public Elastos::Droid::Preference::Preference
    {
    private:
        class RoutePreferenceInnerListener
            : public Object
            , public IPreferenceOnPreferenceClickListener
        {
        public:
            TO_STRING_IMPL("CWifiDisplaySettings::RoutePreference::RoutePreferenceInnerListener")

            CAR_INTERFACE_DECL()

            RoutePreferenceInnerListener(
                /* [in] */ RoutePreference* host);

            //@Override
            CARAPI OnPreferenceClick(
                /* [in] */ IPreference* preference,
                /* [out] */ Boolean* result);

        private:
            RoutePreference* mHost;
        };

    public:
        TO_STRING_IMPL("CWifiDisplaySettings::RoutePreference")

        RoutePreference();

        CARAPI constructor(
            /* [in] */ ISettingsPreferenceFragment* host,
            /* [in] */ IContext* context,
            /* [in] */ IMediaRouterRouteInfo* route);

        //@Override
        CARAPI OnPreferenceClick(
            /* [in] */ IPreference* preference,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IMediaRouterRouteInfo> mRoute;
        CWifiDisplaySettings* mHost;
    };

    class WifiDisplayRoutePreference
        : public RoutePreference
    {
    private:
        class WifiDisplayRoutePreferenceInnerListener
            : public Object
            , public IViewOnClickListener
        {
        public:
            TO_STRING_IMPL("CWifiDisplaySettings::WifiDisplayRoutePreference::WifiDisplayRoutePreferenceInnerListener")

            CAR_INTERFACE_DECL()

            WifiDisplayRoutePreferenceInnerListener(
                /* [in] */ WifiDisplayRoutePreference* host);

            //@Override
            CARAPI OnClick(
                /* [in] */ IView* v);

        private:
            WifiDisplayRoutePreference* mHost;
        };

    public:
        TO_STRING_IMPL("CWifiDisplaySettings::WifiDisplayRoutePreference")

        WifiDisplayRoutePreference();

        CARAPI constructor(
            /* [in] */ ISettingsPreferenceFragment* host,
            /* [in] */ IContext* context,
            /* [in] */ IMediaRouterRouteInfo* route,
            /* [in] */ IWifiDisplay* display);

        using RoutePreference::OnClick;

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    protected:
        //@Override
        CARAPI OnBindView(
            /* [in] */ IView* view);

    private:
        AutoPtr<IWifiDisplay> mDisplay;
        CWifiDisplaySettings* mHost;
    };

    class UnpairedWifiDisplayPreference
        : public Elastos::Droid::Preference::Preference
    {
    private:
        class UnpairedWifiDisplayPreferenceInnerListener
            : public Object
            , public IPreferenceOnPreferenceClickListener
        {
        public:
            TO_STRING_IMPL("CWifiDisplaySettings::UnpairedWifiDisplayPreference::UnpairedWifiDisplayPreferenceInnerListener")

            CAR_INTERFACE_DECL()

            UnpairedWifiDisplayPreferenceInnerListener(
                /* [in] */ UnpairedWifiDisplayPreference* host);

            //@Override
            CARAPI OnPreferenceClick(
                /* [in] */ IPreference* preference,
                /* [out] */ Boolean* result);

        private:
            UnpairedWifiDisplayPreference* mHost;
        };

    public:
        TO_STRING_IMPL("CWifiDisplaySettings::UnpairedWifiDisplayPreference")

        UnpairedWifiDisplayPreference();

        CARAPI constructor(
            /* [in] */ ISettingsPreferenceFragment* host,
            /* [in] */ IContext* context,
            /* [in] */ IWifiDisplay* display);

        //@Override
        CARAPI OnPreferenceClick(
            /* [in] */ IPreference* preference,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IWifiDisplay> mDisplay;
        CWifiDisplaySettings* mHost;
    };

private:
    class InitUpdateRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CWifiDisplaySettings::InitUpdateRunnable")

        InitUpdateRunnable(
            /* [in] */ CWifiDisplaySettings* host);

        //@Override
        CARAPI Run();

    private:
        CWifiDisplaySettings* mHost;
    };

    class InitBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CWifiDisplaySettings::InitBroadcastReceiver")

        InitBroadcastReceiver(
            /* [in] */ CWifiDisplaySettings* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CWifiDisplaySettings* mHost;
    };

    class InitContentObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CWifiDisplaySettings::InitContentObserver")

        InitContentObserver(
            /* [in] */ CWifiDisplaySettings* host);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        CWifiDisplaySettings* mHost;
    };

    class InitMediaRouterCallback
        : public MediaRouterSimpleCallback
    {
    public:
        TO_STRING_IMPL("CWifiDisplaySettings::InitMediaRouterCallback")

        InitMediaRouterCallback(
            /* [in] */ CWifiDisplaySettings* host);

        //@Override
        CARAPI OnRouteAdded(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info);

        //@Override
        CARAPI OnRouteChanged(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info);

        //@Override
        CARAPI OnRouteRemoved(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info);

        //@Override
        CARAPI OnRouteSelected(
            /* [in] */ IMediaRouter* router,
            /* [in] */ Int32 type,
            /* [in] */ IMediaRouterRouteInfo* info);

        //@Override
        CARAPI OnRouteUnselected(
            /* [in] */ IMediaRouter* router,
            /* [in] */ Int32 type,
            /* [in] */ IMediaRouterRouteInfo* info);

    private:
        CWifiDisplaySettings* mHost;
    };

    class BCMPreference
        : public Elastos::Droid::Preference::Preference
    {
    public:
        TO_STRING_IMPL("CWifiDisplaySettings::BCMPreference")

        BCMPreference();

        CARAPI constructor(
            /* [in] */ CWifiDisplaySettings* host,
            /* [in] */ IContext* context);

        //@Override
        CARAPI GetView(
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

    private:
        CWifiDisplaySettings* mHost;
    };

    class BCMOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CWifiDisplaySettings::BCMOnClickListener");

        CAR_INTERFACE_DECL()

        BCMOnClickListener(
            /* [in] */ CWifiDisplaySettings* host,
            /* [in] */ Int32 id);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CWifiDisplaySettings* mHost;
        Int32 mId;
    };

    class BCMCheckBoxPreference
        : public CheckBoxPreference
    {
    public:
        TO_STRING_IMPL("CWifiDisplaySettings::BCMCheckBoxPreference")

        BCMCheckBoxPreference();

        CARAPI constructor(
            /* [in] */ CWifiDisplaySettings* host,
            /* [in] */ IContext* context,
            /* [in] */ Int32 id);

    protected:
        CARAPI OnClick();

    private:
        CWifiDisplaySettings* mHost;
        Int32 mId;
    };

    class BCMListPreference
        : public ListPreference
    {
    public:
        TO_STRING_IMPL("CWifiDisplaySettings::BCMListPreference")

        BCMListPreference();

        CARAPI constructor(
            /* [in] */ CWifiDisplaySettings* host,
            /* [in] */ IContext* context,
            /* [in] */ Int32 id);

    protected:
        //@Override
        CARAPI OnDialogClosed(
            /* [in] */ Boolean positiveResult);

    private:
        CWifiDisplaySettings* mHost;
        Int32 mId;
    };

    class ActionListener
        : public Object
        , public IWifiP2pManagerActionListener
    {
    public:
        TO_STRING_IMPL("CWifiDisplaySettings::ActionListener")

        CAR_INTERFACE_DECL()

        ActionListener(
            /* [in] */ Int32 id,
            /* [in] */ Boolean enable);

        ~ActionListener();

        //@Override
        CARAPI OnSuccess();

        //@Override
        CARAPI OnFailure(
            /* [in] */ Int32 reason);

    private:
        Int32 mId;
        Boolean mEnable;
    };

    class DialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CWifiDisplaySettings::DialogInterfaceOnClickListener")

        CAR_INTERFACE_DECL()

        DialogInterfaceOnClickListener(
            /* [in] */ CWifiDisplaySettings* host,
            /* [in] */ Int32 id,
            /* [in] */ IWifiDisplay* display,
            /* [in] */ IEditText* nameEditText);

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CWifiDisplaySettings* mHost;
        Int32 mId;
        AutoPtr<IWifiDisplay> mDisplay;
        AutoPtr<IEditText> mNameEditText;
    };

public:
    CAR_OBJECT_DECL()

    CWifiDisplaySettings();

    ~CWifiDisplaySettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnStart();

    //@Override
    CARAPI OnStop();

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuInflater* inflater);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI_(Int32) GetHelpResource();

private:
    CARAPI_(void) ScheduleUpdate(
        /* [in] */ Int32 changes);

    CARAPI_(void) UnscheduleUpdate();

    CARAPI_(void) Update(
        /* [in] */ Int32 changes);

    CARAPI_(AutoPtr<IPreference> /*RoutePreference*/) CreateRoutePreference(
        /* [in] */ IMediaRouterRouteInfo* route);

    CARAPI_(AutoPtr<IWifiDisplay>) FindWifiDisplay(
        /* [in] */ const String& deviceAddress);

    CARAPI_(void) BuildCertificationMenu(
        /* [in] */ IPreferenceScreen* preferenceScreen);

    CARAPI_(void) StartAutoGO();

    CARAPI_(void) StopAutoGO();

    CARAPI_(void) SetListenMode(
        /* [in] */ Boolean enable);

    CARAPI_(void) SetWifiP2pChannels(
        /* [in] */ Int32 lc,
        /* [in] */ Int32 oc);

    CARAPI_(void) ToggleRoute(
        /* [in] */ IMediaRouterRouteInfo* route);

    CARAPI_(void) PairWifiDisplay(
        /* [in] */ IWifiDisplay* display);

    CARAPI_(void) ShowWifiDisplayOptionsDialog(
        /* [in] */ IWifiDisplay* display);

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const Int32 MENU_ID_ENABLE_WIFI_DISPLAY;

    static const Int32 CHANGE_SETTINGS;
    static const Int32 CHANGE_ROUTES;
    static const Int32 CHANGE_WIFI_DISPLAY_STATUS;
    static const Int32 CHANGE_ALL;

    static const Int32 ORDER_CERTIFICATION;
    static const Int32 ORDER_CONNECTED;
    static const Int32 ORDER_AVAILABLE;
    static const Int32 ORDER_UNAVAILABLE;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IMediaRouter> mRouter;
    AutoPtr<IDisplayManager> mDisplayManager;

    Boolean mStarted;
    Int32 mPendingChanges;

    Boolean mWifiDisplayOnSetting;
    AutoPtr<IWifiDisplayStatus> mWifiDisplayStatus;

    AutoPtr<ITextView> mEmptyView;

    /* certification */
    Boolean mWifiDisplayCertificationOn;
    AutoPtr<IWifiP2pManager> mWifiP2pManager;
    AutoPtr<IWifiP2pManagerChannel> mWifiP2pChannel;
    AutoPtr<IPreferenceGroup> mCertCategory;
    Boolean mListen;
    Boolean mAutoGO;
    Int32 mWpsConfig;
    Int32 mListenChannel;
    Int32 mOperatingChannel;

    AutoPtr<Runnable> mUpdateRunnable;
    AutoPtr<BroadcastReceiver> mReceiver;

    AutoPtr<ContentObserver> mSettingsObserver;

    AutoPtr<InitMediaRouterCallback> mRouterCallback;
};

} // namespace Wfd
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WFD_CWIFIDISPLAYSETTINGS_H__
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

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/settings/wfd/CWifiDisplaySettings.h"
#include "elastos/droid/settings/wfd/CWifiDisplaySettingsRoutePreference.h"
#include "elastos/droid/settings/wfd/CWifiDisplaySettingsWifiDisplayRoutePreference.h"
#include "elastos/droid/settings/wfd/CWifiDisplaySettingsUnpairedWifiDisplayPreference.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IService;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Hardware::Display::IWifiDisplaySessionInfo;
using Elastos::Droid::Internal::App::CMediaRouteDialogPresenter;
using Elastos::Droid::Internal::App::IMediaRouteDialogPresenter;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Preference::CPreference;
using Elastos::Droid::Preference::CPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pManagerActionListener;
using Elastos::Droid::Wifi::IWpsInfo;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wfd {

const String CWifiDisplaySettings::TAG("CWifiDisplaySettings");
const Boolean CWifiDisplaySettings::DEBUG = FALSE;

const Int32 CWifiDisplaySettings::MENU_ID_ENABLE_WIFI_DISPLAY = IMenu::FIRST;

const Int32 CWifiDisplaySettings::CHANGE_SETTINGS = 1 << 0;
const Int32 CWifiDisplaySettings::CHANGE_ROUTES = 1 << 1;
const Int32 CWifiDisplaySettings::CHANGE_WIFI_DISPLAY_STATUS = 1 << 2;
const Int32 CWifiDisplaySettings::CHANGE_ALL = -1;

const Int32 CWifiDisplaySettings::ORDER_CERTIFICATION = 1;
const Int32 CWifiDisplaySettings::ORDER_CONNECTED = 2;
const Int32 CWifiDisplaySettings::ORDER_AVAILABLE = 3;
const Int32 CWifiDisplaySettings::ORDER_UNAVAILABLE = 4;

//=============================================================================================
//                  CWifiDisplaySettings::RoutePreference::RoutePreferenceInnerListener
//=============================================================================================

CAR_INTERFACE_IMPL(CWifiDisplaySettings::RoutePreference::RoutePreferenceInnerListener, Object, IPreferenceOnPreferenceClickListener)

CWifiDisplaySettings::RoutePreference::RoutePreferenceInnerListener::RoutePreferenceInnerListener(
    /* [in] */ RoutePreference* host)
    : mHost(host)
{}

ECode CWifiDisplaySettings::RoutePreference::RoutePreferenceInnerListener::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    return mHost->OnPreferenceClick(preference, result);
}

//=============================================================================================
//                  CWifiDisplaySettings::RoutePreference
//=============================================================================================

CWifiDisplaySettings::RoutePreference::RoutePreference()
{}

ECode CWifiDisplaySettings::RoutePreference::constructor(
    /* [in] */ ISettingsPreferenceFragment* host,
    /* [in] */ IContext* context,
    /* [in] */ IMediaRouterRouteInfo* route)
{
    mHost = (CWifiDisplaySettings*) host;
    Preference::constructor(context);

    mRoute = route;
    AutoPtr<ICharSequence> cs;
    route->GetName((ICharSequence**)&cs);
    SetTitle(cs);
    cs = NULL;
    route->GetDescription((ICharSequence**)&cs);
    SetSummary(cs);
    Boolean res;
    route->IsEnabled(&res);
    SetEnabled(res);
    if (route->IsSelected(&res), res) {
        SetOrder(ORDER_CONNECTED);
        if (route->IsConnecting(&res), res) {
            SetSummary(R::string::wifi_display_status_connecting);
        }
        else {
            SetSummary(R::string::wifi_display_status_connected);
        }
    }
    else {
        if (IsEnabled(&res), res) {
            SetOrder(ORDER_AVAILABLE);
        }
        else {
            SetOrder(ORDER_UNAVAILABLE);
            Int32 code;
            if ((route->GetStatusCode(&code), code) == IMediaRouterRouteInfo::STATUS_IN_USE) {
                SetSummary(R::string::wifi_display_status_in_use);
            }
            else {
                SetSummary(R::string::wifi_display_status_not_available);
            }
        }
    }

    AutoPtr<RoutePreferenceInnerListener> listener = new RoutePreferenceInnerListener(this);
    SetOnPreferenceClickListener(listener);
    return NOERROR;
}

ECode CWifiDisplaySettings::RoutePreference::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mHost->ToggleRoute(mRoute);
    *result = TRUE;
    return NOERROR;
}

//=============================================================================================
//                  CWifiDisplaySettings::WifiDisplayRoutePreference::WifiDisplayRoutePreferenceInnerListener
//=============================================================================================

CAR_INTERFACE_IMPL(CWifiDisplaySettings::WifiDisplayRoutePreference::WifiDisplayRoutePreferenceInnerListener, Object, IViewOnClickListener)

CWifiDisplaySettings::WifiDisplayRoutePreference::WifiDisplayRoutePreferenceInnerListener::WifiDisplayRoutePreferenceInnerListener(
    /* [in] */ WifiDisplayRoutePreference* host)
    : mHost(host)
{}

ECode CWifiDisplaySettings::WifiDisplayRoutePreference::WifiDisplayRoutePreferenceInnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

//=============================================================================================
//                  CWifiDisplaySettings::WifiDisplayRoutePreference
//=============================================================================================

CWifiDisplaySettings::WifiDisplayRoutePreference::WifiDisplayRoutePreference()
{}

ECode CWifiDisplaySettings::WifiDisplayRoutePreference::constructor(
    /* [in] */ ISettingsPreferenceFragment* host,
    /* [in] */ IContext* context,
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [in] */ IWifiDisplay* display)
{
    mHost = (CWifiDisplaySettings*) host;
    RoutePreference::constructor(host, context, route);

    mDisplay = display;
    SetWidgetLayoutResource(R::layout::wifi_display_preference);
    return NOERROR;
}

ECode CWifiDisplaySettings::WifiDisplayRoutePreference::OnBindView(
    /* [in] */ IView* view)
{
    RoutePreference::OnBindView(view);

    AutoPtr<IView> deviceDetailsTmp;
    view->FindViewById(R::id::deviceDetails, (IView**)&deviceDetailsTmp);
    IImageView* deviceDetails = IImageView::Probe(deviceDetailsTmp);
    if (deviceDetails != NULL) {
        AutoPtr<WifiDisplayRoutePreferenceInnerListener> listener = new WifiDisplayRoutePreferenceInnerListener(this);
        deviceDetailsTmp->SetOnClickListener(listener);
        Boolean res;
        if (IsEnabled(&res), !res) {
            AutoPtr<ITypedValue> value;
            CTypedValue::New((ITypedValue**)&value);

            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IResourcesTheme> theme;
            context->GetTheme((IResourcesTheme**)&theme);
            theme->ResolveAttribute(Elastos::Droid::R::attr::disabledAlpha,
                    value, TRUE, &res);
            Float data;
            value->GetFloat(&data);
            deviceDetails->SetImageAlpha((Int32)(data * 255));
            deviceDetailsTmp->SetEnabled(TRUE); // always allow button to be pressed
        }
    }
    return NOERROR;
}

ECode CWifiDisplaySettings::WifiDisplayRoutePreference::OnClick(
    /* [in] */ IView* v)
{
    mHost->ShowWifiDisplayOptionsDialog(mDisplay);
    return NOERROR;
}

//=============================================================================================
//                  CWifiDisplaySettings::UnpairedWifiDisplayPreference::UnpairedWifiDisplayPreferenceInnerListener
//=============================================================================================

CAR_INTERFACE_IMPL(CWifiDisplaySettings::UnpairedWifiDisplayPreference::UnpairedWifiDisplayPreferenceInnerListener, Object, IPreferenceOnPreferenceClickListener)

CWifiDisplaySettings::UnpairedWifiDisplayPreference::UnpairedWifiDisplayPreferenceInnerListener::UnpairedWifiDisplayPreferenceInnerListener(
    /* [in] */ UnpairedWifiDisplayPreference* host)
    : mHost(host)
{}

ECode CWifiDisplaySettings::UnpairedWifiDisplayPreference::UnpairedWifiDisplayPreferenceInnerListener::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    return mHost->OnPreferenceClick(preference, result);
}

//=============================================================================================
//                  CWifiDisplaySettings::UnpairedWifiDisplayPreference
//=============================================================================================

CWifiDisplaySettings::UnpairedWifiDisplayPreference::UnpairedWifiDisplayPreference()
{}

ECode CWifiDisplaySettings::UnpairedWifiDisplayPreference::constructor(
    /* [in] */ ISettingsPreferenceFragment* host,
    /* [in] */ IContext* context,
    /* [in] */ IWifiDisplay* display)
{
    mHost = (CWifiDisplaySettings*) host;
    Preference::constructor(context);

    mDisplay = display;
    String displayName;
    display->GetFriendlyDisplayName(&displayName);
    SetTitle(CoreUtils::Convert(displayName));
    SetSummary(Elastos::Droid::R::string::wireless_display_route_description);
    Boolean res;
    display->CanConnect(&res);
    SetEnabled(res);
    if (IsEnabled(&res), res) {
        SetOrder(ORDER_AVAILABLE);
    }
    else {
        SetOrder(ORDER_UNAVAILABLE);
        SetSummary(R::string::wifi_display_status_in_use);
    }
    AutoPtr<UnpairedWifiDisplayPreferenceInnerListener> listener = new UnpairedWifiDisplayPreferenceInnerListener(this);
    SetOnPreferenceClickListener(listener);
    return NOERROR;
}

ECode CWifiDisplaySettings::UnpairedWifiDisplayPreference::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mHost->PairWifiDisplay(mDisplay);
    *result = TRUE;
    return NOERROR;
}

//=============================================================================================
//                  CWifiDisplaySettings::InitUpdateRunnable
//=============================================================================================

CWifiDisplaySettings::InitUpdateRunnable::InitUpdateRunnable(
    /* [in] */ CWifiDisplaySettings* host)
    : mHost(host)
{}

ECode CWifiDisplaySettings::InitUpdateRunnable::Run()
{
    const Int32 changes = mHost->mPendingChanges;
    mHost->mPendingChanges = 0;
    mHost->Update(changes);
    return NOERROR;
}

//=============================================================================================
//                  CWifiDisplaySettings::InitBroadcastReceiver
//=============================================================================================

CWifiDisplaySettings::InitBroadcastReceiver::InitBroadcastReceiver(
    /* [in] */ CWifiDisplaySettings* host)
    : mHost(host)
{}

ECode CWifiDisplaySettings::InitBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IDisplayManager::ACTION_WIFI_DISPLAY_STATUS_CHANGED)) {
        mHost->ScheduleUpdate(CHANGE_WIFI_DISPLAY_STATUS);
    }
    return NOERROR;
}

//=============================================================================================
//                  CWifiDisplaySettings::InitContentObserver
//=============================================================================================

CWifiDisplaySettings::InitContentObserver::InitContentObserver(
    /* [in] */ CWifiDisplaySettings* host)
    : mHost(host)
{}

ECode CWifiDisplaySettings::InitContentObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    mHost->ScheduleUpdate(CHANGE_SETTINGS);
    return NOERROR;
}

//=============================================================================================
//                  CWifiDisplaySettings::InitMediaRouterCallback
//=============================================================================================

CWifiDisplaySettings::InitMediaRouterCallback::InitMediaRouterCallback(
    /* [in] */ CWifiDisplaySettings* host)
    : mHost(host)
{}

ECode CWifiDisplaySettings::InitMediaRouterCallback::OnRouteAdded(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    mHost->ScheduleUpdate(CHANGE_ROUTES);
    return NOERROR;
}

ECode CWifiDisplaySettings::InitMediaRouterCallback::OnRouteChanged(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    mHost->ScheduleUpdate(CHANGE_ROUTES);
    return NOERROR;
}

ECode CWifiDisplaySettings::InitMediaRouterCallback::OnRouteRemoved(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    mHost->ScheduleUpdate(CHANGE_ROUTES);
    return NOERROR;
}

ECode CWifiDisplaySettings::InitMediaRouterCallback::OnRouteSelected(
    /* [in] */ IMediaRouter* router,
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    mHost->ScheduleUpdate(CHANGE_ROUTES);
    return NOERROR;
}

ECode CWifiDisplaySettings::InitMediaRouterCallback::OnRouteUnselected(
    /* [in] */ IMediaRouter* router,
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    mHost->ScheduleUpdate(CHANGE_ROUTES);
    return NOERROR;
}

//=============================================================================================
//                  CWifiDisplaySettings::BCMPreference
//=============================================================================================

CWifiDisplaySettings::BCMPreference::BCMPreference()
{}

ECode CWifiDisplaySettings::BCMPreference::constructor(
    /* [in] */ CWifiDisplaySettings* host,
    /* [in] */ IContext* context)
{
    mHost = host;
    return Preference::constructor(context);
}

ECode CWifiDisplaySettings::BCMPreference::GetView(
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IView> v;
    if (convertView == NULL) {
        AutoPtr<IActivity> activity;
        mHost->GetActivity((IActivity**)&activity);
        AutoPtr<IInterface> obj;
        IContext::Probe(activity)->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
        ILayoutInflater* li = ILayoutInflater::Probe(obj);
        li->Inflate(R::layout::two_buttons_panel, NULL, (IView**)&v);
    }
    else {
        v = convertView;
    }

    AutoPtr<IView> bTmp;
    v->FindViewById(R::id::left_button, (IView**)&bTmp);
    IButton* b = IButton::Probe(bTmp);
    ITextView::Probe(b)->SetText(R::string::wifi_display_pause);

    AutoPtr<IViewOnClickListener> listener = new BCMOnClickListener(mHost, 0);
    bTmp->SetOnClickListener(listener);

    bTmp = NULL;
    v->FindViewById(R::id::right_button, (IView**)&bTmp);
    b = IButton::Probe(bTmp);
    ITextView::Probe(b)->SetText(R::string::wifi_display_resume);
    AutoPtr<IViewOnClickListener> listener1 = new BCMOnClickListener(mHost, 1);
    bTmp->SetOnClickListener(listener1);

    *result = v;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//=============================================================================================
//                  CWifiDisplaySettings::BCMOnClickListener
//=============================================================================================

CAR_INTERFACE_IMPL(CWifiDisplaySettings::BCMOnClickListener, Object, IViewOnClickListener)

CWifiDisplaySettings::BCMOnClickListener::BCMOnClickListener(
    /* [in] */ CWifiDisplaySettings* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

ECode CWifiDisplaySettings::BCMOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    switch (mId) {
        case 0:
            mHost->mDisplayManager->PauseWifiDisplay();
            break;
        case 1:
            mHost->mDisplayManager->ResumeWifiDisplay();
            break;
    }
    return NOERROR;
}

//=============================================================================================
//                  CWifiDisplaySettings::BCMCheckBoxPreference
//=============================================================================================

CWifiDisplaySettings::BCMCheckBoxPreference::BCMCheckBoxPreference()
{}

ECode CWifiDisplaySettings::BCMCheckBoxPreference::constructor(
    /* [in] */ CWifiDisplaySettings* host,
    /* [in] */ IContext* context,
    /* [in] */ Int32 id)
{
    mHost = host;
    mId = id;
    return CheckBoxPreference::constructor(context);
}

ECode CWifiDisplaySettings::BCMCheckBoxPreference::OnClick()
{
    switch (mId) {
        case 0:
            mHost->mListen = !mHost->mListen;
            mHost->SetListenMode(mHost->mListen);
            SetChecked(mHost->mListen);
            break;
        case 1:
            mHost->mAutoGO = !mHost->mAutoGO;
            if (mHost->mAutoGO) {
                mHost->StartAutoGO();
            }
            else {
                mHost->StopAutoGO();
            }
            SetChecked(mHost->mAutoGO);
            break;
    }
    return NOERROR;
}

//=============================================================================================
//                  CWifiDisplaySettings::BCMListPreference
//=============================================================================================

CWifiDisplaySettings::BCMListPreference::BCMListPreference()
{}

ECode CWifiDisplaySettings::BCMListPreference::constructor(
    /* [in] */ CWifiDisplaySettings* host,
    /* [in] */ IContext* context,
    /* [in] */ Int32 id)
{
    mHost = host;
    mId = id;
    return ListPreference::constructor(context);
}

ECode CWifiDisplaySettings::BCMListPreference::OnDialogClosed(
    /* [in] */ Boolean positiveResult)
{
    ListPreference::OnDialogClosed(positiveResult);
    if (positiveResult) {
        String value;
        GetValue(&value);
        AutoPtr<IActivity> activity;
        mHost->GetActivity((IActivity**)&activity);
        switch (mId) {
            case 0: {
                mHost->mWpsConfig = StringUtils::ParseInt32(value);
                SetSummary(CoreUtils::Convert("%1$s"));
                activity->InvalidateOptionsMenu();
                AutoPtr<ISettingsGlobal> global;
                CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
                AutoPtr<IContentResolver> resolver;
                IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);
                Boolean res;
                global->PutInt32(resolver,
                        ISettingsGlobal::WIFI_DISPLAY_WPS_CONFIG, mHost->mWpsConfig, &res);
                break;
            }
            case 1: {
                mHost->mListenChannel = StringUtils::ParseInt32(value);
                SetSummary(CoreUtils::Convert("%1$s"));
                activity->InvalidateOptionsMenu();
                mHost->SetWifiP2pChannels(mHost->mListenChannel, mHost->mOperatingChannel);
                break;
            }
            case 2: {
                mHost->mOperatingChannel = StringUtils::ParseInt32(value);
                SetSummary(CoreUtils::Convert("%1$s"));
                activity->InvalidateOptionsMenu();
                mHost->SetWifiP2pChannels(mHost->mListenChannel, mHost->mOperatingChannel);
                break;
            }
        }
    }
    return NOERROR;
}

//=============================================================================================
//                  CWifiDisplaySettings::ActionListener
//=============================================================================================

CAR_INTERFACE_IMPL(CWifiDisplaySettings::ActionListener, Object, IWifiP2pManagerActionListener)

CWifiDisplaySettings::ActionListener::ActionListener(
    /* [in] */ Int32 id,
    /* [in] */ Boolean enable)
    : mId(id)
    , mEnable(enable)
{}

CWifiDisplaySettings::ActionListener::~ActionListener()
{}

ECode CWifiDisplaySettings::ActionListener::OnSuccess()
{
    if (DEBUG) {
        switch (mId) {
            case 0:
                Slogger::D(TAG, "Successfully started AutoGO.");
                break;
            case 1:
                Slogger::D(TAG, "Successfully stopped AutoGO.");
                break;
            case 2:
                Slogger::D(TAG, "Successfully %s listen mode.", (mEnable ? "entered" : "exited"));
                break;
            case 3:
                Slogger::D(TAG, "Successfully set wifi p2p channels.");
                break;
        }
    }
    return NOERROR;
}

ECode CWifiDisplaySettings::ActionListener::OnFailure(
    /* [in] */ Int32 reason)
{
    switch (mId) {
        case 0:
            Slogger::E(TAG, "Failed to start AutoGO with reason %d.", reason);
            break;
        case 1:
            Slogger::E(TAG, "Failed to stop AutoGO with reason %d.", reason);
            break;
        case 2:
            Slogger::E(TAG, "Failed to %s listen mode with reason %d.", (mEnable ? "entered" : "exited"), reason);
            break;
        case 3:
            Slogger::E(TAG, "Failed to set wifi p2p channels with reason %d.", reason);
            break;
    }
    return NOERROR;
}

//=============================================================================================
//                  CWifiDisplaySettings::DialogInterfaceOnClickListener
//=============================================================================================

CAR_INTERFACE_IMPL(CWifiDisplaySettings::DialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener)

CWifiDisplaySettings::DialogInterfaceOnClickListener::DialogInterfaceOnClickListener(
    /* [in] */ CWifiDisplaySettings* host,
    /* [in] */ Int32 id,
    /* [in] */ IWifiDisplay* display,
    /* [in] */ IEditText* nameEditText)
    : mHost(host)
    , mId(id)
    , mDisplay(display)
    , mNameEditText(nameEditText)
{}

ECode CWifiDisplaySettings::DialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    String address;
    mDisplay->GetDeviceAddress(&address);

    switch (mId) {
        case 0: {
            AutoPtr<ICharSequence> cs;
            ITextView::Probe(mNameEditText)->GetText((ICharSequence**)&cs);
            String str;
            cs->ToString(&str);
            String name = str.Trim();
            if (name.IsEmpty() || name.Equals((mDisplay->GetDeviceName(&str), str))) {
                name = NULL;
            }
            mHost->mDisplayManager->RenameWifiDisplay(address, name);
            break;
        }
        case 1:
            mHost->mDisplayManager->ForgetWifiDisplay(address);
            break;
    }
    return NOERROR;
}

//=============================================================================================
//                  CWifiDisplaySettings
//=============================================================================================

CAR_OBJECT_IMPL(CWifiDisplaySettings)

CWifiDisplaySettings::CWifiDisplaySettings()
    : mStarted(FALSE)
    , mPendingChanges(0)
    , mWifiDisplayOnSetting(FALSE)
    , mWifiDisplayCertificationOn(FALSE)
    , mListen(FALSE)
    , mAutoGO(FALSE)
    , mWpsConfig(IWpsInfo::INVALID)
    , mListenChannel(0)
    , mOperatingChannel(0)
{}

CWifiDisplaySettings::~CWifiDisplaySettings()
{}

ECode CWifiDisplaySettings::constructor()
{
    SettingsPreferenceFragment::constructor();

    mUpdateRunnable = new InitUpdateRunnable(this);
    mReceiver = new InitBroadcastReceiver(this);
    mReceiver->constructor();

    mSettingsObserver = new InitContentObserver(this);
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    mSettingsObserver->constructor(handler);

    mRouterCallback = new InitMediaRouterCallback(this);
    mRouterCallback->constructor();

    CHandler::New((IHandler**)&mHandler);
    return NOERROR;
}

ECode CWifiDisplaySettings::OnCreate(
    /* [in] */ IBundle* icicle)
{
    SettingsPreferenceFragment::OnCreate(icicle);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::MEDIA_ROUTER_SERVICE, (IInterface**)&obj);
    mRouter = IMediaRouter::Probe(obj);
    obj = NULL;
    context->GetSystemService(IContext::DISPLAY_SERVICE, (IInterface**)&obj);
    mDisplayManager = IDisplayManager::Probe(obj);
    obj = NULL;
    context->GetSystemService(IContext::WIFI_P2P_SERVICE, (IInterface**)&obj);
    mWifiP2pManager = IWifiP2pManager::Probe(obj);
    mWifiP2pManager->Initialize(context, Looper::GetMainLooper(), NULL, (IWifiP2pManagerChannel**)&mWifiP2pChannel);

    AddPreferencesFromResource(R::xml::wifi_display_settings);
    SetHasOptionsMenu(TRUE);
    return NOERROR;
}

Int32 CWifiDisplaySettings::GetHelpResource()
{
    return R::string::help_url_remote_display;
}

ECode CWifiDisplaySettings::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnActivityCreated(savedInstanceState);

    AutoPtr<IView> tmp;
    GetView((IView**)&tmp);
    AutoPtr<IView> emptyViewTmp;
    tmp->FindViewById(Elastos::Droid::R::id::empty, (IView**)&emptyViewTmp);
    mEmptyView = ITextView::Probe(emptyViewTmp);
    mEmptyView->SetText(R::string::wifi_display_no_devices_found);
    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    IAdapterView::Probe(listView)->SetEmptyView(emptyViewTmp);
    return NOERROR;
}

ECode CWifiDisplaySettings::OnStart()
{
    SettingsPreferenceFragment::OnStart();
    mStarted = TRUE;

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IDisplayManager::ACTION_WIFI_DISPLAY_STATUS_CHANGED);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mReceiver, filter, (IIntent**)&intent);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

    AutoPtr<IContentResolver> resolver = GetContentResolver();

    AutoPtr<IUri> uri;
    global->GetUriFor(ISettingsGlobal::WIFI_DISPLAY_ON, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, mSettingsObserver);
    uri = NULL;
    global->GetUriFor(ISettingsGlobal::WIFI_DISPLAY_CERTIFICATION_ON, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, mSettingsObserver);
    uri = NULL;
    global->GetUriFor(ISettingsGlobal::WIFI_DISPLAY_WPS_CONFIG, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, mSettingsObserver);

    mRouter->AddCallback(IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY, mRouterCallback,
            IMediaRouter::CALLBACK_FLAG_PERFORM_ACTIVE_SCAN);

    Update(CHANGE_ALL);
    return NOERROR;
}

ECode CWifiDisplaySettings::OnStop()
{
    SettingsPreferenceFragment::OnStop();
    mStarted = FALSE;

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);

    context->UnregisterReceiver(mReceiver);

    GetContentResolver()->UnregisterContentObserver(mSettingsObserver);

    mRouter->RemoveCallback(mRouterCallback);

    UnscheduleUpdate();
    return NOERROR;
}

ECode CWifiDisplaySettings::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuInflater* inflater)
{
    Int32 state;
    if (mWifiDisplayStatus != NULL && (mWifiDisplayStatus->GetFeatureState(&state), state)
            != IWifiDisplayStatus::FEATURE_STATE_UNAVAILABLE) {
        AutoPtr<IMenuItem> item;
        menu->Add(IMenu::NONE, MENU_ID_ENABLE_WIFI_DISPLAY, 0,
                R::string::wifi_display_enable_menu_item, (IMenuItem**)&item);
        item->SetCheckable(TRUE);
        item->SetChecked(mWifiDisplayOnSetting);
    }
    return SettingsPreferenceFragment::OnCreateOptionsMenu(menu, inflater);
}

ECode CWifiDisplaySettings::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case MENU_ID_ENABLE_WIFI_DISPLAY: {
            Boolean res;
            item->IsChecked(&res);
            mWifiDisplayOnSetting = !res;
            item->SetChecked(mWifiDisplayOnSetting);
            AutoPtr<ISettingsGlobal> global;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
            global->PutInt32(GetContentResolver(),
                    ISettingsGlobal::WIFI_DISPLAY_ON, mWifiDisplayOnSetting ? 1 : 0, &res);
            *result = TRUE;
            return NOERROR;
        }
    }
    return SettingsPreferenceFragment::OnOptionsItemSelected(item, result);
}

void CWifiDisplaySettings::ScheduleUpdate(
    /* [in] */ Int32 changes)
{
    if (mStarted) {
        if (mPendingChanges == 0) {
            Boolean res;
            mHandler->Post(mUpdateRunnable, &res);
        }
        mPendingChanges |= changes;
    }
}

void CWifiDisplaySettings::UnscheduleUpdate()
{
    if (mPendingChanges != 0) {
        mPendingChanges = 0;
        mHandler->RemoveCallbacks(mUpdateRunnable);
    }
}

void CWifiDisplaySettings::Update(
    /* [in] */ Int32 changes)
{
    Boolean invalidateOptions = FALSE;

    // Update settings.
    if ((changes & CHANGE_SETTINGS) != 0) {
        AutoPtr<ISettingsGlobal> global;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

        AutoPtr<IContentResolver> resolver = GetContentResolver();
        Int32 data;
        global->GetInt32(resolver, ISettingsGlobal::WIFI_DISPLAY_ON, 0, &data);
        mWifiDisplayOnSetting = data != 0;
        global->GetInt32(resolver, ISettingsGlobal::WIFI_DISPLAY_CERTIFICATION_ON, 0, &data);
        mWifiDisplayCertificationOn = data != 0;
        global->GetInt32(resolver, ISettingsGlobal::WIFI_DISPLAY_WPS_CONFIG,
                IWpsInfo::INVALID, &mWpsConfig);

        // The wifi display enabled setting may have changed.
        invalidateOptions = TRUE;
    }

    // Update wifi display state.
    if ((changes & CHANGE_WIFI_DISPLAY_STATUS) != 0) {
        mWifiDisplayStatus = NULL;
        mDisplayManager->GetWifiDisplayStatus((IWifiDisplayStatus**)&mWifiDisplayStatus);

        // The wifi display feature state may have changed.
        invalidateOptions = TRUE;
    }

    // Rebuild the routes.
    AutoPtr<IPreferenceScreen> preferenceScreen;
    GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
    IPreferenceGroup::Probe(preferenceScreen)->RemoveAll();

    // Add all known remote display routes.
    Int32 routeCount;
    mRouter->GetRouteCount(&routeCount);
    Boolean res;
    for (Int32 i = 0; i < routeCount; i++) {
        AutoPtr<IMediaRouterRouteInfo> route;
        mRouter->GetRouteAt(i, (IMediaRouterRouteInfo**)&route);
        if (route->MatchesTypes(IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY, &res), res) {
            IPreferenceGroup::Probe(preferenceScreen)->AddPreference(CreateRoutePreference(route), &res);
        }
    }

    // Additional features for wifi display routes.
    Int32 state;
    if (mWifiDisplayStatus != NULL
            && (mWifiDisplayStatus->GetFeatureState(&state), state) == IWifiDisplayStatus::FEATURE_STATE_ON) {
        // Add all unpaired wifi displays.
        AutoPtr< ArrayOf<IWifiDisplay*> > displays;
        mWifiDisplayStatus->GetDisplays((ArrayOf<IWifiDisplay*>**)&displays);
        for (Int32 i = 0; i < displays->GetLength(); ++i) {
            AutoPtr<IWifiDisplay> display = (*displays)[i];
            Boolean available;
            if ((display->IsRemembered(&res), !res) && (display->IsAvailable(&available), available)) {
                AutoPtr<IWifiDisplay> activeDiaplay;
                mWifiDisplayStatus->GetActiveDisplay((IWifiDisplay**)&activeDiaplay);
                if (!Object::Equals(display, activeDiaplay)) {
                    AutoPtr<IActivity> activity;
                    GetActivity((IActivity**)&activity);
                    AutoPtr<IPreference> pref;
                    CWifiDisplaySettingsUnpairedWifiDisplayPreference::New(this, IContext::Probe(activity), display, (IPreference**)&pref);
                    IPreferenceGroup::Probe(preferenceScreen)->AddPreference(pref, &res);
                }
            }
        }

        // Add the certification menu if enabled in developer options.
        if (mWifiDisplayCertificationOn) {
            BuildCertificationMenu(preferenceScreen);
        }
    }

    // Invalidate menu options if needed.
    if (invalidateOptions) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        activity->InvalidateOptionsMenu();
    }
}

AutoPtr<IPreference> /*RoutePreference*/ CWifiDisplaySettings::CreateRoutePreference(
    /* [in] */ IMediaRouterRouteInfo* route)
{
    String address;
    route->GetDeviceAddress(&address);
    AutoPtr<IWifiDisplay> display = FindWifiDisplay(address);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);

    AutoPtr<IPreference> pref;
    if (display != NULL) {
        CWifiDisplaySettingsWifiDisplayRoutePreference::New(this, context, route, display, (IPreference**)&pref);
        return pref;
    }
    else {
        CWifiDisplaySettingsRoutePreference::New(this, context, route, (IPreference**)&pref);
        return pref;
    }
}

AutoPtr<IWifiDisplay> CWifiDisplaySettings::FindWifiDisplay(
    /* [in] */ const String& deviceAddress)
{
    if (mWifiDisplayStatus != NULL && !deviceAddress.IsNull()) {
        AutoPtr< ArrayOf<IWifiDisplay*> > displays;
        mWifiDisplayStatus->GetDisplays((ArrayOf<IWifiDisplay*>**)&displays);
        for (Int32 i = 0; i < displays->GetLength(); ++i) {
            AutoPtr<IWifiDisplay> display = (*displays)[i];
            String address;
            display->GetDeviceAddress(&address);
            if (address.Equals(deviceAddress)) {
                return display;
            }
        }
    }
    return NULL;
}

void CWifiDisplaySettings::BuildCertificationMenu(
    /* [in] */ IPreferenceScreen* preferenceScreen)
{
    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);

    if (mCertCategory == NULL) {
        CPreferenceCategory::New(activity, (IPreferenceGroup**)&mCertCategory);
        IPreference* certCategory = IPreference::Probe(mCertCategory);
        certCategory->SetTitle(R::string::wifi_display_certification_heading);
        certCategory->SetOrder(ORDER_CERTIFICATION);
    }
    else {
        mCertCategory->RemoveAll();
    }
    Boolean res;
    IPreferenceGroup::Probe(preferenceScreen)->AddPreference(IPreference::Probe(mCertCategory), &res);

    // display session info if there is an active p2p session
    AutoPtr<IWifiDisplaySessionInfo> wdsi;
    mWifiDisplayStatus->GetSessionInfo((IWifiDisplaySessionInfo**)&wdsi);
    String gId;
    wdsi->GetGroupId(&gId);
    if (!gId.IsEmpty()) {
        AutoPtr<IPreference> p;
        CPreference::New(activity, (IPreference**)&p);
        p->SetTitle(R::string::wifi_display_session_info);
        String str;
        wdsi->ToString(&str);
        p->SetSummary(CoreUtils::Convert(str));
        mCertCategory->AddPreference(p, &res);

        // show buttons for Pause/Resume when a WFD session is established
        Int32 id;
        wdsi->GetSessionId(&id);
        if (id != 0) {
            AutoPtr<BCMPreference> pre = new BCMPreference();
            pre->constructor(this, activity);
            mCertCategory->AddPreference(pre, &res);
        }
    }

    // switch for Listen Mode
    AutoPtr<BCMCheckBoxPreference> cbp = new BCMCheckBoxPreference();
    cbp->constructor(this, activity, 0);
    cbp->SetTitle(R::string::wifi_display_listen_mode);
    cbp->SetChecked(mListen);
    mCertCategory->AddPreference(cbp, &res);

    // switch for Autonomous GO
    cbp = NULL;
    cbp = new BCMCheckBoxPreference();
    cbp->constructor(this, activity, 1);
    cbp->SetTitle(R::string::wifi_display_autonomous_go);
    cbp->SetChecked(mAutoGO);
    mCertCategory->AddPreference(cbp, &res);

    // Drop down list for choosing WPS method (PBC/KEYPAD/DISPLAY)
    AutoPtr<BCMListPreference> lp = new BCMListPreference();
    lp->constructor(this, activity, 0);

    AutoPtr<IContentResolver> resolver;
    activity->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    global->GetInt32(resolver,
            ISettingsGlobal::WIFI_DISPLAY_WPS_CONFIG, IWpsInfo::INVALID, &mWpsConfig);

    AutoPtr< ArrayOf<ICharSequence*> > wpsEntries = ArrayOf<ICharSequence*>::Alloc(4);
    (*wpsEntries)[0] = CoreUtils::Convert("Default");
    (*wpsEntries)[1] = CoreUtils::Convert("PBC");
    (*wpsEntries)[2] = CoreUtils::Convert("KEYPAD");
    (*wpsEntries)[3] = CoreUtils::Convert("DISPLAY");

    AutoPtr< ArrayOf<ICharSequence*> > wpsValues = ArrayOf<ICharSequence*>::Alloc(4);
    (*wpsValues)[0] = CoreUtils::Convert(String("") + StringUtils::ToString(IWpsInfo::INVALID));
    (*wpsValues)[1] = CoreUtils::Convert(String("") + StringUtils::ToString(IWpsInfo::PBC));
    (*wpsValues)[2] = CoreUtils::Convert(String("") + StringUtils::ToString(IWpsInfo::KEYPAD));
    (*wpsValues)[3] = CoreUtils::Convert(String("") + StringUtils::ToString(IWpsInfo::DISPLAY));

    lp->SetTitle(R::string::wifi_display_wps_config);
    lp->SetEntries(wpsEntries);
    lp->SetEntryValues(wpsValues);
    lp->SetValue(String("") + StringUtils::ToString(mWpsConfig));
    lp->SetSummary(CoreUtils::Convert("%1$s"));
    mCertCategory->AddPreference(lp, &res);

    // Drop down list for choosing listen channel
    lp = NULL;
    lp = new BCMListPreference();
    lp->constructor(this, activity, 1);
    AutoPtr< ArrayOf<ICharSequence*> > lcEntries = ArrayOf<ICharSequence*>::Alloc(4);
    (*lcEntries)[0] = CoreUtils::Convert("Auto");
    (*lcEntries)[1] = CoreUtils::Convert("1");
    (*lcEntries)[2] = CoreUtils::Convert("6");
    (*lcEntries)[3] = CoreUtils::Convert("11");
    AutoPtr< ArrayOf<ICharSequence*> > lcValues = ArrayOf<ICharSequence*>::Alloc(4);
    (*lcValues)[0] = CoreUtils::Convert("0");
    (*lcValues)[1] = CoreUtils::Convert("1");
    (*lcValues)[2] = CoreUtils::Convert("6");
    (*lcValues)[3] = CoreUtils::Convert("11");
    lp->SetTitle(R::string::wifi_display_listen_channel);
    lp->SetEntries(lcEntries);
    lp->SetEntryValues(lcValues);
    lp->SetValue(String("") + StringUtils::ToString(mListenChannel));
    lp->SetSummary(CoreUtils::Convert("%1$s"));
    mCertCategory->AddPreference(lp, &res);

    // Drop down list for choosing operating channel
    lp = NULL;
    lp = new BCMListPreference();
    lp->constructor(this, activity, 2);

    AutoPtr< ArrayOf<ICharSequence*> > ocEntries = ArrayOf<ICharSequence*>::Alloc(5);
    (*ocEntries)[0] = CoreUtils::Convert("Auto");
    (*ocEntries)[1] = CoreUtils::Convert("1");
    (*ocEntries)[2] = CoreUtils::Convert("6");
    (*ocEntries)[3] = CoreUtils::Convert("11");
    (*ocEntries)[4] = CoreUtils::Convert("36");
    AutoPtr< ArrayOf<ICharSequence*> > ocValues = ArrayOf<ICharSequence*>::Alloc(5);
    (*ocValues)[0] = CoreUtils::Convert("0");
    (*ocValues)[1] = CoreUtils::Convert("1");
    (*ocValues)[2] = CoreUtils::Convert("6");
    (*ocValues)[3] = CoreUtils::Convert("11");
    (*ocValues)[4] = CoreUtils::Convert("36");

    lp->SetTitle(R::string::wifi_display_operating_channel);
    lp->SetEntries(ocEntries);
    lp->SetEntryValues(ocValues);
    lp->SetValue(String("") + StringUtils::ToString(mOperatingChannel));
    lp->SetSummary(CoreUtils::Convert("%1$s"));
    mCertCategory->AddPreference(lp, &res);
}

void CWifiDisplaySettings::StartAutoGO()
{
    if (DEBUG) {
        Slogger::D(TAG, "Starting Autonomous GO...");
    }

    AutoPtr<IWifiP2pManagerActionListener> listener = new ActionListener(0, FALSE);
    mWifiP2pManager->CreateGroup(mWifiP2pChannel, listener);
}

void CWifiDisplaySettings::StopAutoGO()
{
    if (DEBUG) {
        Slogger::D(TAG, "Stopping Autonomous GO...");
    }
    AutoPtr<IWifiP2pManagerActionListener> listener = new ActionListener(1, FALSE);
    mWifiP2pManager->RemoveGroup(mWifiP2pChannel, listener);
}

void CWifiDisplaySettings::SetListenMode(
    /* [in] */ Boolean enable)
{
    if (DEBUG) {
        Slogger::D(TAG, "Setting listen mode to: %d", enable);
    }
    AutoPtr<IWifiP2pManagerActionListener> listener = new ActionListener(2, enable);
    mWifiP2pManager->Listen(mWifiP2pChannel, enable, listener);
}

void CWifiDisplaySettings::SetWifiP2pChannels(
    /* [in] */ Int32 lc,
    /* [in] */ Int32 oc)
{
    if (DEBUG) {
        Slogger::D(TAG, "Setting wifi p2p channel: lc=%d, oc=%d", lc, oc);
    }
    AutoPtr<IWifiP2pManagerActionListener> listener = new ActionListener(3, FALSE);
    mWifiP2pManager->SetWifiP2pChannels(mWifiP2pChannel, lc, oc, listener);
}

void CWifiDisplaySettings::ToggleRoute(
    /* [in] */ IMediaRouterRouteInfo* route)
{
    Boolean res;
    if (route->IsSelected(&res), res) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IMediaRouteDialogPresenter> mrdp;
        CMediaRouteDialogPresenter::AcquireSingleton((IMediaRouteDialogPresenter**)&mrdp);
        AutoPtr<IDialogFragment> df;
        mrdp->ShowDialogFragment(activity,
                IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY, NULL, (IDialogFragment**)&df);
    }
    else {
        route->Select();
    }
}

void CWifiDisplaySettings::PairWifiDisplay(
    /* [in] */ IWifiDisplay* display)
{
    Boolean res;
    if (display->CanConnect(&res), res) {
        String address;
        display->GetDeviceAddress(&address);
        mDisplayManager->ConnectWifiDisplay(address);
    }
}

void CWifiDisplaySettings::ShowWifiDisplayOptionsDialog(
    /* [in] */ IWifiDisplay* display)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<ILayoutInflater> inflater;
    activity->GetLayoutInflater((ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::wifi_display_options, NULL, (IView**)&view);
    AutoPtr<IView> tmp;
    view->FindViewById(R::id::name, (IView**)&tmp);
    IEditText* nameEditText = IEditText::Probe(tmp);
    String displayName;
    display->GetFriendlyDisplayName(&displayName);
    ITextView::Probe(nameEditText)->SetText(CoreUtils::Convert(displayName));

    AutoPtr<IDialogInterfaceOnClickListener> done = new DialogInterfaceOnClickListener(this, 0, display, nameEditText);
    AutoPtr<IDialogInterfaceOnClickListener> forget = new DialogInterfaceOnClickListener(this, 1, display, NULL);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
    builder->SetCancelable(TRUE);
    builder->SetTitle(R::string::wifi_display_options_title);
    builder->SetView(view);
    builder->SetPositiveButton(R::string::wifi_display_options_done, done);
    builder->SetNegativeButton(R::string::wifi_display_options_forget, forget);
    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);
    IDialog::Probe(dialog)->Show();
}

} // namespace Wfd
} // namespace Settings
} // namespace Droid
} // namespace Elastos
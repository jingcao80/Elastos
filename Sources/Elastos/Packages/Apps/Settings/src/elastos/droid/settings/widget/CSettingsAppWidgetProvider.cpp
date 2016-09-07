
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/settings/widget/CSettingsAppWidgetProvider.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "../R.h"

using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::AppWidget::CAppWidgetManagerHelper;
using Elastos::Droid::AppWidget::IAppWidgetManagerHelper;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Widget::CRemoteViews;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Widget {

const String CSettingsAppWidgetProvider::TAG("CSettingsAppWidgetProvider");

static const AutoPtr<IComponentName> InitTHIS_APPWIDGET()
{
    AutoPtr<IComponentName> component;
    CComponentName::New(String("Elastos.Droid.Settings"),
            String("Elastos.Droid.Settings.Widget.CSettingsAppWidgetProvider"),
            (IComponentName**)&component);
    return component;
}

const AutoPtr<IComponentName> CSettingsAppWidgetProvider::THIS_APPWIDGET = InitTHIS_APPWIDGET();

// static LocalBluetoothAdapter CSettingsAppWidgetProvider::sLocalBluetoothAdapter;

const Int32 CSettingsAppWidgetProvider::BUTTON_WIFI = 0;
const Int32 CSettingsAppWidgetProvider::BUTTON_BRIGHTNESS = 1;
const Int32 CSettingsAppWidgetProvider::BUTTON_SYNC = 2;
const Int32 CSettingsAppWidgetProvider::BUTTON_LOCATION = 3;
const Int32 CSettingsAppWidgetProvider::BUTTON_BLUETOOTH = 4;

const Int32 CSettingsAppWidgetProvider::STATE_DISABLED;
const Int32 CSettingsAppWidgetProvider::STATE_ENABLED;
const Int32 CSettingsAppWidgetProvider::STATE_TURNING_ON;
const Int32 CSettingsAppWidgetProvider::STATE_TURNING_OFF;
const Int32 CSettingsAppWidgetProvider::STATE_UNKNOWN = 4;
const Int32 CSettingsAppWidgetProvider::STATE_INTERMEDIATE;

const Int32 CSettingsAppWidgetProvider::POS_LEFT = 0;
const Int32 CSettingsAppWidgetProvider::POS_CENTER = 1;
const Int32 CSettingsAppWidgetProvider::POS_RIGHT = 2;

static const AutoPtr< ArrayOf<Int32> > InitIND_DRAWABLE_OFF()
{
    AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(3);
    (*args)[0] = R::drawable::appwidget_settings_ind_off_l_holo;
    (*args)[1] = R::drawable::appwidget_settings_ind_off_c_holo;
    (*args)[2] = R::drawable::appwidget_settings_ind_off_r_holo;
    return args;
}

const AutoPtr< ArrayOf<Int32> > CSettingsAppWidgetProvider::IND_DRAWABLE_OFF = InitIND_DRAWABLE_OFF();

static const AutoPtr< ArrayOf<Int32> > InitIND_DRAWABLE_MID()
{
    AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(3);
    (*args)[0] = R::drawable::appwidget_settings_ind_mid_l_holo;
    (*args)[1] = R::drawable::appwidget_settings_ind_mid_c_holo;
    (*args)[2] = R::drawable::appwidget_settings_ind_mid_r_holo;
    return args;
}

const AutoPtr< ArrayOf<Int32> > CSettingsAppWidgetProvider::IND_DRAWABLE_MID = InitIND_DRAWABLE_MID();

static const AutoPtr< ArrayOf<Int32> > InitIND_DRAWABLE_ON()
{
    AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(3);
    (*args)[0] = R::drawable::appwidget_settings_ind_on_l_holo;
    (*args)[1] = R::drawable::appwidget_settings_ind_on_c_holo;
    (*args)[2] = R::drawable::appwidget_settings_ind_on_r_holo;
    return args;
}

const AutoPtr< ArrayOf<Int32> > CSettingsAppWidgetProvider::IND_DRAWABLE_ON = InitIND_DRAWABLE_ON();

const Float CSettingsAppWidgetProvider::HALF_BRIGHTNESS_THRESHOLD = 0.3f;
const Float CSettingsAppWidgetProvider::FULL_BRIGHTNESS_THRESHOLD = 0.8f;

const AutoPtr<CSettingsAppWidgetProvider::StateTracker> CSettingsAppWidgetProvider::sWifiState = new WifiStateTracker();
//  const StateTracker sBluetoothState = new BluetoothStateTracker();
//  const StateTracker sLocationState = new LocationStateTracker();
//  const StateTracker sSyncState = new SyncStateTracker();
AutoPtr<CSettingsAppWidgetProvider::SettingsObserver> CSettingsAppWidgetProvider::sSettingsObserver;

//===============================================================================
//                  CSettingsAppWidgetProvider::StateTracker
//===============================================================================

CSettingsAppWidgetProvider::StateTracker::StateTracker()
    : mInTransition(FALSE)
    , mDeferredStateChangeRequestNeeded(FALSE)
{}

CSettingsAppWidgetProvider::StateTracker::~StateTracker()
{}

ECode CSettingsAppWidgetProvider::StateTracker::ToggleState(
    /* [in] */ IContext* context)
{
    Int32 currentState = GetTriState(context);
    Boolean newState = FALSE;
    switch (currentState) {
        case STATE_ENABLED:
            newState = FALSE;
            break;
        case STATE_DISABLED:
            newState = TRUE;
            break;
        case STATE_INTERMEDIATE:
            if (mIntendedState != NULL) {
                Boolean res;
                mIntendedState->GetValue(&res);
                newState = !res;
            }
            break;
    }
    mIntendedState = CoreUtils::Convert(newState);
    if (mInTransition) {
        // We don't send off a transition request if we're
        // already transitioning.  Makes our state tracking
        // easier, and is probably nicer on lower levels.
        // (even though they should be able to take it...)
        mDeferredStateChangeRequestNeeded = TRUE;
    }
    else {
        mInTransition = TRUE;
        RequestStateChange(context, newState);
    }
    return NOERROR;
}

ECode CSettingsAppWidgetProvider::StateTracker::SetImageViewResources(
    /* [in] */ IContext* context,
    /* [in] */ IRemoteViews* views)
{
    Int32 containerId = GetContainerId();
    Int32 buttonId = GetButtonId();
    Int32 indicatorId = GetIndicatorId();
    Int32 pos = GetPosition();
    switch (GetTriState(context)) {
        case STATE_DISABLED:
            views->SetContentDescription(containerId,
                CoreUtils::Convert(GetContentDescription(context, R::string::gadget_state_off)));
            views->SetImageViewResource(buttonId, GetButtonImageId(FALSE));
            views->SetImageViewResource(
                indicatorId, (*IND_DRAWABLE_OFF)[pos]);
            break;
        case STATE_ENABLED:
            views->SetContentDescription(containerId,
                CoreUtils::Convert(GetContentDescription(context, R::string::gadget_state_on)));
            views->SetImageViewResource(buttonId, GetButtonImageId(TRUE));
            views->SetImageViewResource(
                indicatorId, (*IND_DRAWABLE_ON)[pos]);
            break;
        case STATE_INTERMEDIATE:
            // In the transitional state, the bottom green bar
            // shows the tri-state (on, off, transitioning), but
            // the top dark-gray-or-bright-white logo shows the
            // user's intent.  This is much easier to see in
            // sunlight.
            if (IsTurningOn()) {
                views->SetContentDescription(containerId,
                    CoreUtils::Convert(GetContentDescription(context, R::string::gadget_state_turning_on)));
                views->SetImageViewResource(buttonId, GetButtonImageId(TRUE));
                views->SetImageViewResource(
                    indicatorId, (*IND_DRAWABLE_MID)[pos]);
            }
            else {
                views->SetContentDescription(containerId,
                    CoreUtils::Convert(GetContentDescription(context, R::string::gadget_state_turning_off)));
                views->SetImageViewResource(buttonId, GetButtonImageId(FALSE));
                views->SetImageViewResource(
                    indicatorId, (*IND_DRAWABLE_OFF)[pos]);
            }
            break;
    }
    return NOERROR;
}

Boolean CSettingsAppWidgetProvider::StateTracker::IsTurningOn()
{
    Boolean res;
    return mIntendedState != NULL && (mIntendedState->GetValue(&res), res);
}

Int32 CSettingsAppWidgetProvider::StateTracker::GetTriState(
    /* [in] */ IContext* context)
{
    if (mInTransition) {
        // If we know we just got a toggle request recently
        // (which set mInTransition), don't even ask the
        // underlying interface for its state.  We know we're
        // changing.  This avoids blocking the UI thread
        // during UI refresh post-toggle if the underlying
        // service state accessor has coarse locking on its
        // state (to be fixed separately).
        return STATE_INTERMEDIATE;
    }
    switch (GetActualState(context)) {
        case STATE_DISABLED:
            return STATE_DISABLED;
        case STATE_ENABLED:
            return STATE_ENABLED;
        default:
            return STATE_INTERMEDIATE;
    }
}

Int32 CSettingsAppWidgetProvider::StateTracker::GetPosition()
{
    return POS_CENTER;
}

void CSettingsAppWidgetProvider::StateTracker::SetCurrentState(
    /* [in] */ IContext* context,
    /* [in] */ Int32 newState)
{
    Boolean wasInTransition = mInTransition;
    switch (newState) {
        case STATE_DISABLED:
            mInTransition = FALSE;
            mActualState = CoreUtils::Convert(FALSE);
            break;
        case STATE_ENABLED:
            mInTransition = FALSE;
            mActualState = CoreUtils::Convert(TRUE);
            break;
        case STATE_TURNING_ON:
            mInTransition = TRUE;
            mActualState = CoreUtils::Convert(FALSE);
            break;
        case STATE_TURNING_OFF:
            mInTransition = TRUE;
            mActualState = CoreUtils::Convert(TRUE);
            break;
    }

    if (wasInTransition && !mInTransition) {
        if (mDeferredStateChangeRequestNeeded) {
            Logger::V(TAG, "processing deferred state change");
            Boolean res1, res2;
            if (mActualState != NULL && mIntendedState != NULL &&
                    (mIntendedState->GetValue(&res1), res1) == (mActualState->GetValue(&res2), res2)) {
                Logger::V(TAG, "... but intended state matches, so no changes.");
            }
            else if (mIntendedState != NULL) {
                mInTransition = TRUE;
                Boolean res;
                RequestStateChange(context, (mIntendedState->GetValue(&res), res));
            }
            mDeferredStateChangeRequestNeeded = FALSE;
        }
    }
}

String CSettingsAppWidgetProvider::StateTracker::GetContentDescription(
    /* [in] */ IContext* context,
    /* [in] */ Int32 stateResId)
{
    String gadget;
    context->GetString(GetButtonDescription(), &gadget);
    String state;
    context->GetString(stateResId, &state);

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, CoreUtils::Convert(gadget));
    args->Set(1, CoreUtils::Convert(state));
    String str;
    context->GetString(R::string::gadget_state_template, args, &str);
    return str;
}

//===============================================================================
//                  CSettingsAppWidgetProvider::WifiStateTracker
//===============================================================================

Int32 CSettingsAppWidgetProvider::WifiStateTracker::GetContainerId()
{
    return R::id::btn_wifi;
}

Int32 CSettingsAppWidgetProvider::WifiStateTracker::GetButtonId()
{
    return R::id::img_wifi;
}

Int32 CSettingsAppWidgetProvider::WifiStateTracker::GetIndicatorId()
{
    return R::id::ind_wifi;
}

Int32 CSettingsAppWidgetProvider::WifiStateTracker::GetButtonDescription()
{
    return R::string::gadget_wifi;
}

Int32 CSettingsAppWidgetProvider::WifiStateTracker::GetButtonImageId(
    /* [in] */ Boolean on)
{
    return on ? R::drawable::ic_appwidget_settings_wifi_on_holo
            : R::drawable::ic_appwidget_settings_wifi_off_holo;
}

Int32 CSettingsAppWidgetProvider::WifiStateTracker::GetPosition()
{
    return POS_LEFT;
}

Int32 CSettingsAppWidgetProvider::WifiStateTracker::GetActualState(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    IWifiManager* wifiManager = IWifiManager::Probe(obj);
    if (wifiManager != NULL) {
        Int32 state;
        wifiManager->GetWifiState(&state);
        return WifiStateToFiveState(state);
    }
    return STATE_UNKNOWN;
}

ECode CSettingsAppWidgetProvider::WifiStateTracker::RequestStateChange(
    /* [in] */ IContext* context,
    /* [in] */ Boolean desiredState)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    IWifiManager* wifiManager = IWifiManager::Probe(obj);

    if (wifiManager == NULL) {
        Logger::D(TAG, "No wifiManager.");
        return NOERROR;
    }

    AutoPtr<WifiStateTrackerTask> task = new WifiStateTrackerTask(this, wifiManager, desiredState);
    AutoPtr< ArrayOf<IInterface*> > args;
    task->Execute(args);
    return NOERROR;
}

ECode CSettingsAppWidgetProvider::WifiStateTracker::OnActualStateChange(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (!IWifiManager::WIFI_STATE_CHANGED_ACTION.Equals(action)) {
        return NOERROR;
    }
    Int32 wifiState;
    intent->GetInt32Extra(IWifiManager::EXTRA_WIFI_STATE, -1, &wifiState);
    SetCurrentState(context, WifiStateToFiveState(wifiState));
    return NOERROR;
}

Int32 CSettingsAppWidgetProvider::WifiStateTracker::WifiStateToFiveState(
    /* [in] */ Int32 wifiState)
{
    switch (wifiState) {
        case IWifiManager::WIFI_STATE_DISABLED:
            return STATE_DISABLED;
        case IWifiManager::WIFI_STATE_ENABLED:
            return STATE_ENABLED;
        case IWifiManager::WIFI_STATE_DISABLING:
            return STATE_TURNING_OFF;
        case IWifiManager::WIFI_STATE_ENABLING:
            return STATE_TURNING_ON;
        default:
            return STATE_UNKNOWN;
    }
}


//===============================================================================
//                  CSettingsAppWidgetProvider::WifiStateTrackerTask
//===============================================================================

CSettingsAppWidgetProvider::WifiStateTrackerTask::WifiStateTrackerTask(
    /* [in] */ WifiStateTracker* host,
    /* [in] */ IWifiManager* wifiManager,
    /* [in] */ Boolean desiredState)
    : mHost(host)
    , mWifiManager(wifiManager)
    , mDesiredState(desiredState)
{}

CSettingsAppWidgetProvider::WifiStateTrackerTask::~WifiStateTrackerTask()
{}

ECode CSettingsAppWidgetProvider::WifiStateTrackerTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    /**
     * Disable tethering if enabling Wifi
     */
    Boolean res;
    Int32 wifiApState;
    mWifiManager->GetWifiApState(&wifiApState);
    if (mDesiredState && ((wifiApState == IWifiManager::WIFI_AP_STATE_ENABLING) ||
                         (wifiApState == IWifiManager::WIFI_AP_STATE_ENABLED))) {
        mWifiManager->SetWifiApEnabled(NULL, FALSE, &res);
    }

    mWifiManager->SetWifiEnabled(mDesiredState, &res);
    *result = NULL;
    return NOERROR;
}

//===============================================================================
//                  CSettingsAppWidgetProvider::SettingsObserver
//===============================================================================

ECode CSettingsAppWidgetProvider::SettingsObserver::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ IContext* context)
{
    mContext = context;
    return ContentObserver::constructor(handler);
}

void CSettingsAppWidgetProvider::SettingsObserver::StartObserving()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    // Listen to brightness and brightness mode
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    AutoPtr<IUri> uri1, uri2;
    ss->GetUriFor(ISettingsSystem::SCREEN_BRIGHTNESS, (IUri**)&uri1);
    ss->GetUriFor(ISettingsSystem::SCREEN_BRIGHTNESS_MODE, (IUri**)&uri2);
    resolver->RegisterContentObserver(uri1, FALSE, this);
    resolver->RegisterContentObserver(uri2, FALSE, this);
}

void CSettingsAppWidgetProvider::SettingsObserver::StopObserving()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    resolver->UnregisterContentObserver(this);
}

ECode CSettingsAppWidgetProvider::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    UpdateWidget(mContext);
    return NOERROR;
}

//===============================================================================
//                  CSettingsAppWidgetProvider
//===============================================================================

CAR_OBJECT_IMPL(CSettingsAppWidgetProvider)

CAR_INTERFACE_IMPL(CSettingsAppWidgetProvider, AppWidgetProvider, ISettingsAppWidgetProvider);

CSettingsAppWidgetProvider::CSettingsAppWidgetProvider()
{}

CSettingsAppWidgetProvider::~CSettingsAppWidgetProvider()
{}

ECode CSettingsAppWidgetProvider::constructor()
{
    return NOERROR;
}

ECode CSettingsAppWidgetProvider::OnUpdate(
    /* [in] */ IContext* context,
    /* [in] */ IAppWidgetManager* appWidgetManager,
    /* [in] */ ArrayOf<Int32>* appWidgetIds)
{
    // Update each requested appWidgetId
    AutoPtr<IRemoteViews> view = BuildUpdate(context);

    for (Int32 i = 0; i < appWidgetIds->GetLength(); i++) {
        appWidgetManager->UpdateAppWidget((*appWidgetIds)[i], view);
    }
    return NOERROR;
}

ECode CSettingsAppWidgetProvider::OnEnabled(
    /* [in] */ IContext* context)
{
    CheckObserver(context);
    return NOERROR;
}

ECode CSettingsAppWidgetProvider::OnDisabled(
    /* [in] */ IContext* context)
{
    if (sSettingsObserver != NULL) {
        sSettingsObserver->StopObserving();
        sSettingsObserver = NULL;
    }
    return NOERROR;
}

AutoPtr<IRemoteViews> CSettingsAppWidgetProvider::BuildUpdate(
    /* [in] */ IContext* context)
{
    String pkgName;
    context->GetPackageName(&pkgName);
    AutoPtr<IRemoteViews> views;
    CRemoteViews::New(pkgName, R::layout::widget, (IRemoteViews**)&views);
    views->SetOnClickPendingIntent(R::id::btn_wifi, GetLaunchPendingIntent(context, BUTTON_WIFI));
    // TODO
    Logger::D("CSettingsAppWidgetProvider::BuildUpdate", "other Settings except wifi");
    // views->SetOnClickPendingIntent(R::id::btn_brightness,
    //         GetLaunchPendingIntent(context,
    //                 BUTTON_BRIGHTNESS));
    // views->SetOnClickPendingIntent(R::id::btn_sync,
    //         GetLaunchPendingIntent(context,
    //                 BUTTON_SYNC));
    // views->SetOnClickPendingIntent(R::id::btn_location,
    //         GetLaunchPendingIntent(context, BUTTON_LOCATION));
    // views->SetOnClickPendingIntent(R::id::btn_bluetooth,
    //         GetLaunchPendingIntent(context,
    //                 BUTTON_BLUETOOTH));

    UpdateButtons(views, context);
    return views;
}

void CSettingsAppWidgetProvider::UpdateWidget(
    /* [in] */ IContext* context)
{
    AutoPtr<IRemoteViews> views = BuildUpdate(context);
    // Update specific list of appWidgetIds if given, otherwise default to all
    AutoPtr<IAppWidgetManagerHelper> helper;
    CAppWidgetManagerHelper::AcquireSingleton((IAppWidgetManagerHelper**)&helper);
    AutoPtr<IAppWidgetManager> gm;
    helper->GetInstance(context, (IAppWidgetManager**)&gm);
    gm->UpdateAppWidget(THIS_APPWIDGET, views);
    CheckObserver(context);
}

void CSettingsAppWidgetProvider::CheckObserver(
    /* [in] */ IContext* context)
{
    if (sSettingsObserver == NULL) {
        AutoPtr<IHandler> handler;
        CHandler::New((IHandler**)&handler);
        AutoPtr<IContext> cxt;
        context->GetApplicationContext((IContext**)&cxt);
        sSettingsObserver = new SettingsObserver();
        sSettingsObserver->constructor(handler, cxt);
        sSettingsObserver->StartObserving();
    }
}

ECode CSettingsAppWidgetProvider::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AppWidgetProvider::OnReceive(context, intent);

    Boolean hasCategory;

    String action;
    intent->GetAction(&action);

    if (IWifiManager::WIFI_STATE_CHANGED_ACTION.Equals(action)) {
        sWifiState->OnActualStateChange(context, intent);
    }
    // else if (IBluetoothAdapter::ACTION_STATE_CHANGED.Equals(action)) {
    //     assert(0 && "TODO");
    //     // sBluetoothState->OnActualStateChange(context, intent);
    // }
    // else if (ILocationManager::MODE_CHANGED_ACTION.Equals(action)) {
    //     assert(0 && "TODO");
    //     // sLocationState->OnActualStateChange(context, intent);
    // }
    // else if (ContentResolver::ACTION_SYNC_CONN_STATUS_CHANGED.Equals(action)) {
    //     assert(0 && "TODO");
    //     // sSyncState->OnActualStateChange(context, intent);
    // }
    else if (intent->HasCategory(IIntent::CATEGORY_ALTERNATIVE, &hasCategory), hasCategory) {
        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);
        String str;
        data->GetSchemeSpecificPart(&str);
        Int32 buttonId = StringUtils::ParseInt32(str);
        if (buttonId == BUTTON_WIFI) {
            sWifiState->ToggleState(context);
        }
        else if (buttonId == BUTTON_BRIGHTNESS) {
            assert(0 && "TODO");
            // ToggleBrightness(context);
        }
        else if (buttonId == BUTTON_SYNC) {
            assert(0 && "TODO");
            // sSyncState->ToggleState(context);
        }
        else if (buttonId == BUTTON_LOCATION) {
            assert(0 && "TODO");
            // sLocationState->ToggleState(context);
        }
        else if (buttonId == BUTTON_BLUETOOTH) {
            assert(0 && "TODO");
            // sBluetoothState->ToggleState(context);
        }
    }
    else {
        // Don't fall-through to updating the widget.  The Intent
        // was something unrelated or that our super class took
        // care of.
        return NOERROR;
    }

    // State changes fall through
    UpdateWidget(context);
    return NOERROR;
}

void CSettingsAppWidgetProvider::UpdateButtons(
    /* [in] */ IRemoteViews* views,
    /* [in] */ IContext* context)
{
    sWifiState->SetImageViewResources(context, views);
    // TODO
    Logger::D("CSettingsAppWidgetProvider::UpdateButtons", "other Settings except wifi");
    // sBluetoothState->SetImageViewResources(context, views);
    // sLocationState->SetImageViewResources(context, views);
    // sSyncState->SetImageViewResources(context, views);

    if (GetBrightnessMode(context)) {
        String str;
        context->GetString(R::string::gadget_brightness_state_auto, &str);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(str));

        context->GetString(R::string::gadget_brightness_template, args, &str);

        views->SetContentDescription(R::id::btn_brightness, CoreUtils::Convert(str));
        views->SetImageViewResource(R::id::img_brightness,
                R::drawable::ic_appwidget_settings_brightness_auto_holo);
        views->SetImageViewResource(R::id::ind_brightness,
                R::drawable::appwidget_settings_ind_on_r_holo);
    }
    else {
        Int32 brightness = GetBrightness(context);
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
        IPowerManager* pm = IPowerManager::Probe(obj);
        // Set the icon
        Int32 screenBrightness;
        pm->GetMaximumScreenBrightnessSetting(&screenBrightness);
        const Int32 full = (Int32)(screenBrightness * FULL_BRIGHTNESS_THRESHOLD);
        const Int32 half = (Int32)(screenBrightness * HALF_BRIGHTNESS_THRESHOLD);
        if (brightness > full) {
            String str;
            context->GetString(R::string::gadget_brightness_state_full, &str);
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, CoreUtils::Convert(str));

            context->GetString(R::string::gadget_brightness_template, args, &str);

            views->SetContentDescription(R::id::btn_brightness, CoreUtils::Convert(str));
            views->SetImageViewResource(R::id::img_brightness,
                    R::drawable::ic_appwidget_settings_brightness_full_holo);
        }
        else if (brightness > half) {
            String str;
            context->GetString(R::string::gadget_brightness_state_half, &str);
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, CoreUtils::Convert(str));

            context->GetString(R::string::gadget_brightness_template, args, &str);

            views->SetContentDescription(R::id::btn_brightness, CoreUtils::Convert(str));
            views->SetImageViewResource(R::id::img_brightness,
                    R::drawable::ic_appwidget_settings_brightness_half_holo);
        }
        else {
            String str;
            context->GetString(R::string::gadget_brightness_state_off, &str);
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, CoreUtils::Convert(str));

            context->GetString(R::string::gadget_brightness_template, args, &str);

            views->SetContentDescription(R::id::btn_brightness, CoreUtils::Convert(str));
            views->SetImageViewResource(R::id::img_brightness,
                    R::drawable::ic_appwidget_settings_brightness_off_holo);
        }
        // Set the ON state
        if (brightness > half) {
            views->SetImageViewResource(R::id::ind_brightness,
                    R::drawable::appwidget_settings_ind_on_r_holo);
        }
        else {
            views->SetImageViewResource(R::id::ind_brightness,
                    R::drawable::appwidget_settings_ind_off_r_holo);
        }
    }
}

AutoPtr<IPendingIntent> CSettingsAppWidgetProvider::GetLaunchPendingIntent(
    /* [in] */ IContext* context,
    /* [in] */ Int32 buttonId)
{
    AutoPtr<IIntent> launchIntent;
    CIntent::New((IIntent**)&launchIntent);
    launchIntent->SetClass(context, ECLSID_CSettingsAppWidgetProvider);
    launchIntent->AddCategory(IIntent::CATEGORY_ALTERNATIVE);

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> baseUri;
    helper->Parse(String("custom:") + StringUtils::ToString(buttonId), (IUri**)&baseUri);

    launchIntent->SetData(baseUri);
    AutoPtr<IPendingIntentHelper> piHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);
    AutoPtr<IPendingIntent> pi;
    piHelper->GetBroadcast(context, 0 /* no requestCode */,
            launchIntent, 0 /* no flags */, (IPendingIntent**)&pi);
    return pi;
}

Int32 CSettingsAppWidgetProvider::GetBrightness(
    /* [in] */ IContext* context)
{
    // try {
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    Int32 brightness;
    ECode ec = ss->GetInt32(resolver, ISettingsSystem::SCREEN_BRIGHTNESS, &brightness);
    if (SUCCEEDED(ec))  return brightness;
    // } catch (Exception e) {
    // }
    return 0;
}

Boolean CSettingsAppWidgetProvider::GetBrightnessMode(
    /* [in] */ IContext* context)
{
    // try {
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    Int32 brightnessMode;
    ECode ec = ss->GetInt32(resolver, ISettingsSystem::SCREEN_BRIGHTNESS_MODE, &brightnessMode);
    if (SUCCEEDED(ec)) return brightnessMode == ISettingsSystem::SCREEN_BRIGHTNESS_MODE_AUTOMATIC;
    // } catch (Exception e) {
    Logger::D(TAG, "getBrightnessMode: %08x ", ec);
    // }
    return FALSE;
}

} // namespace Widget
} // namespace Settings
} // namespace Droid
} // namespace Elastos

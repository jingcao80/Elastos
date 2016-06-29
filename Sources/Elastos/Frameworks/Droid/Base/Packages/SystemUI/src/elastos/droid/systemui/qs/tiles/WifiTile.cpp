
#include "elastos/droid/systemui/qs/tiles/WifiTile.h"
#include "elastos/droid/systemui/qs/CQSDetailItems.h"
#include "elastos/droid/systemui/qs/SignalTileView.h"
#include "elastos/droid/systemui/statusbar/policy/NetworkController.h"
#include "../R.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::SystemUI::StatusBar::Policy::AccessPoint;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_INetworkControllerAccessPointCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_INetworkSignalChangedCallback;
using Elastos::Core::CBoolean;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

static const String TAG("WifiTile");

WifiTile::CallbackInfo::CallbackInfo()
    : mEnabled(FALSE)
    , mConnected(FALSE)
    , mWifiSignalIconId(0)
    , mActivityIn(FALSE)
    , mActivityOut(FALSE)
{}

ECode WifiTile::CallbackInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("CallbackInfo[");
    sb.Append("enabled=");
    sb.Append(mEnabled);
    sb.Append(",connected=");
    sb.Append(mConnected);
    sb.Append(",wifiSignalIconId=");
    sb.Append(mWifiSignalIconId);
    sb.Append(",enabledDesc=");
    sb.Append(mEnabledDesc);
    sb.Append(",activityIn=");
    sb.Append(mActivityIn);
    sb.Append(",activityOut=");
    sb.Append(mActivityOut);
    sb.Append(",wifiSignalContentDescription=");
    sb.Append(mWifiSignalContentDescription);
    sb.Append(']');
    return sb.ToString(str);
}

CAR_INTERFACE_IMPL_3(WifiTile::WifiDetailAdapter, Object, IQSTileDetailAdapter \
    , INetworkControllerAccessPointCallback, IQSDetailItemsCallback);

WifiTile::WifiDetailAdapter::WifiDetailAdapter(
    /* [in] */ WifiTile* host)
    : mHost(host)
{}

ECode WifiTile::WifiDetailAdapter::GetTitle(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = R::string::quick_settings_wifi_label;
    return NOERROR;
}

ECode WifiTile::WifiDetailAdapter::GetSettingsIntent(
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result);
    *result = WIFI_SETTINGS;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiTile::WifiDetailAdapter::GetToggleState(
    /* [out] */ IBoolean** result)
{
    VALIDATE_NOT_NULL(result);
    return CBoolean::New(((QSTile::SignalState*)mHost->mState.Get())->mEnabled, result);
}

ECode WifiTile::WifiDetailAdapter::SetToggleState(
    /* [in] */ IBoolean* state)
{
    if (DEBUG) Logger::D(TAG, "setToggleState %s", TO_CSTR(state));
    Boolean tmp = FALSE;
    state->GetValue(&tmp);
    mHost->mController->SetWifiEnabled(tmp);
    mHost->ShowDetail(FALSE);
    return NOERROR;
}

ECode WifiTile::WifiDetailAdapter::CreateDetailView(
    /* [in] */ IContext* context,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    if (DEBUG) Logger::D(TAG, "createDetailView convertView=%d", (convertView != NULL));
    mAccessPoints = NULL;
    mHost->mController->ScanForAccessPoints();
    mHost->FireScanStateChanged(TRUE);
    mItems = CQSDetailItems::ConvertOrInflate(context, convertView, parent);
    mItems->SetTagSuffix(String("Wifi"));
    mItems->SetCallback(this);
    mItems->SetEmptyState(R::drawable::ic_qs_wifi_detail_empty,
            R::string::quick_settings_wifi_detail_empty_text);
    UpdateItems();
    SetItemsVisible(((QSTile::SignalState*)mHost->mState.Get())->mEnabled);
    *view = IView::Probe(mItems);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode WifiTile::WifiDetailAdapter::OnAccessPointsChanged(
    /* [in] */ ArrayOf<INetworkControllerAccessPoint*>* accessPoints)
{
    mAccessPoints = accessPoints;
    UpdateItems();
    if (accessPoints != NULL && accessPoints->GetLength() > 0) {
        mHost->FireScanStateChanged(FALSE);
    }
    return NOERROR;
}

ECode WifiTile::WifiDetailAdapter::OnDetailItemClick(
    /* [in] */ IQSDetailItemsItem* item)
{
    AutoPtr<IInterface> tag;
    if (item == NULL || ((CQSDetailItems::Item*)item)->mTag == NULL) return NOERROR;
    AutoPtr<INetworkControllerAccessPoint> ap = INetworkControllerAccessPoint::Probe(((CQSDetailItems::Item*)item)->mTag);
    Boolean tmp = FALSE;
    if (ap->IsConnected(&tmp), !tmp) {
        mHost->mController->Connect(ap);
    }
    mHost->ShowDetail(FALSE);
    return NOERROR;
}

ECode WifiTile::WifiDetailAdapter::OnDetailItemDisconnect(
    /* [in] */ IQSDetailItemsItem* item)
{
    // noop
    return NOERROR;
}

ECode WifiTile::WifiDetailAdapter::SetItemsVisible(
    /* [in] */ Boolean visible)
{
    if (mItems == NULL) return NOERROR;
    mItems->SetItemsVisible(visible);
    return NOERROR;
}

void WifiTile::WifiDetailAdapter::UpdateItems()
{
    if (mItems == NULL) return;
    AutoPtr<IArrayList> items;
    if (mAccessPoints != NULL) {
        Boolean tmp = FALSE;
        CArrayList::New(mAccessPoints->GetLength(), (IArrayList**)&items);
        for (Int32 i = 0; i < mAccessPoints->GetLength(); i++) {
            AccessPoint* ap = (AccessPoint*)(*mAccessPoints)[i];
            AutoPtr<CQSDetailItems::Item> item = new CQSDetailItems::Item();
            item->mTag = ap->Probe(EIID_IInterface);
            ap->GetIconId(&item->mIcon);
            ap->GetSsid(&item->mLine1);
            if (ap->IsConnected(&tmp), tmp) {
                mHost->mContext->GetString(R::string::quick_settings_connected, &item->mLine2);
            }
            items->Add(i, item->Probe(EIID_IInterface));
        }
    }
    mItems->SetItems(items);
}

CAR_INTERFACE_IMPL(WifiTile::Callback, Object, INetworkSignalChangedCallback)
WifiTile::Callback::Callback(
    /* [in] */ WifiTile* host)
    : mHost(host)
{}

ECode WifiTile::Callback::OnWifiSignalChanged(
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean connected,
    /* [in] */ Int32 wifiSignalIconId,
    /* [in] */ Boolean activityIn,
    /* [in] */ Boolean activityOut,
    /* [in] */ const String& wifiSignalContentDescriptionId,
    /* [in] */ const String& description)
{
    if (DEBUG) Logger::D(TAG, "onWifiSignalChanged enabled=%d", enabled);
    AutoPtr<CallbackInfo> info = new CallbackInfo();
    info->mEnabled = enabled;
    info->mConnected = connected;
    info->mWifiSignalIconId = wifiSignalIconId;
    info->mEnabledDesc = description;
    info->mActivityIn = activityIn;
    info->mActivityOut = activityOut;
    info->mWifiSignalContentDescription = wifiSignalContentDescriptionId;
    mHost->RefreshState(info->Probe(EIID_IInterface));
    return NOERROR;
}

ECode WifiTile::Callback::OnMobileDataSignalChanged(
    /* [in] */ Boolean enabled,
    /* [in] */ Int32 mobileSignalIconId,
    /* [in] */ const String& mobileSignalContentDescriptionId,
    /* [in] */ Int32 dataTypeIconId,
    /* [in] */ Boolean activityIn,
    /* [in] */ Boolean activityOut,
    /* [in] */ const String& dataTypeContentDescriptionId,
    /* [in] */ const String& description,
    /* [in] */ Boolean noSim,
    /* [in] */ Boolean isDataTypeIconWide)
{
    // noop
    return NOERROR;
}

ECode WifiTile::Callback::OnAirplaneModeChanged(
    /* [in] */ Boolean enabled)
{
    // noop
    return NOERROR;
}

ECode WifiTile::Callback::OnMobileDataEnabled(
    /* [in] */ Boolean enabled)
{
    // noop
    return NOERROR;
}

AutoPtr<IIntent> WifiTile::WIFI_SETTINGS = InitStatic();
WifiTile::WifiTile(
    /* [in] */ IQSTileHost* host)
{
    QSTile::constructor(host);
    mStateBeforeClick = NewTileState();
    mCallback = new Callback(this);
    host->GetNetworkController((INetworkController**)&mController);
    mDetailAdapter = new WifiDetailAdapter(this);
}

AutoPtr<IIntent> WifiTile::InitStatic()
{
    CIntent::New(ISettings::ACTION_WIFI_SETTINGS, (IIntent**)&WIFI_SETTINGS);
    return WIFI_SETTINGS;
}

ECode WifiTile::SupportsDualTargets(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

AutoPtr<QSTile::State> WifiTile::NewTileState()
{
    return new SignalState();
}

ECode WifiTile::SetListening(
    /* [in] */ Boolean listening)
{
    if (listening) {
        mController->AddNetworkSignalChangedCallback(mCallback);
        mController->AddAccessPointCallback(mDetailAdapter);
    }
    else {
        mController->RemoveNetworkSignalChangedCallback(mCallback);
        mController->RemoveAccessPointCallback(mDetailAdapter);
    }
    return NOERROR;
}

ECode WifiTile::GetDetailAdapter(
    /* [out] */ IQSTileDetailAdapter** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDetailAdapter;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiTile::CreateTileView(
    /* [in] */ IContext* context,
    /* [out] */ IQSTileView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = new SignalTileView(context);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

void WifiTile::HandleClick()
{
    Boolean tmp = FALSE;
    mState->CopyTo(mStateBeforeClick, &tmp);
    mController->SetWifiEnabled(!((QSTile::SignalState*)mState.Get())->mEnabled);
}

void WifiTile::HandleSecondaryClick()
{
    mHost->StartSettingsActivity(WIFI_SETTINGS);
}

void WifiTile::HandleUpdateState(
    /* [in] */ State* state,
    /* [in] */ IInterface* arg)
{
    state->mVisible = TRUE;
    if (DEBUG) Logger::D(TAG, "handleUpdateState arg=%s", TO_CSTR(arg));
    if (arg == NULL) return;
    CallbackInfo* cb = (CallbackInfo*) IObject::Probe(arg);

    Boolean wifiConnected = cb->mEnabled && (cb->mWifiSignalIconId > 0) && (cb->mEnabledDesc != NULL);
    Boolean wifiNotConnected = (cb->mWifiSignalIconId > 0) && (cb->mEnabledDesc == NULL);
    Boolean enabledChanging = ((SignalState*)state)->mEnabled != cb->mEnabled;
    if (enabledChanging) {
        mDetailAdapter->SetItemsVisible(cb->mEnabled);
        FireToggleStateChanged(cb->mEnabled);
    }
    ((SignalState*)state)->mEnabled = cb->mEnabled;
    ((SignalState*)state)->mConnected = wifiConnected;
    ((SignalState*)state)->mActivityIn = cb->mEnabled && cb->mActivityIn;
    ((SignalState*)state)->mActivityOut = cb->mEnabled && cb->mActivityOut;
    ((SignalState*)state)->mFilter = TRUE;
    String signalContentDescription;
    AutoPtr<IResources> r;
    mContext->GetResources((IResources**)&r);
    if (!((SignalState*)state)->mEnabled) {
        state->mIconId = R::drawable::ic_qs_wifi_disabled;
        r->GetString(R::string::quick_settings_wifi_label, &state->mLabel);
        r->GetString(R::string::accessibility_wifi_off, &signalContentDescription);
    }
    else if (wifiConnected) {
        state->mIconId = cb->mWifiSignalIconId;
        state->mLabel = RemoveDoubleQuotes(cb->mEnabledDesc);
        signalContentDescription = cb->mWifiSignalContentDescription;
    }
    else if (wifiNotConnected) {
        state->mIconId = R::drawable::ic_qs_wifi_0;
        r->GetString(R::string::quick_settings_wifi_label, &state->mLabel);
        r->GetString(R::string::accessibility_no_wifi, &signalContentDescription);
    }
    else {
        state->mIconId = R::drawable::ic_qs_wifi_no_network;
        r->GetString(R::string::quick_settings_wifi_label, &state->mLabel);
        r->GetString(R::string::accessibility_wifi_off, &signalContentDescription);
    }
    AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> obj;
    CString::New(signalContentDescription, (ICharSequence**)&obj);
    objs->Set(0, obj);
    mContext->GetString(R::string::accessibility_quick_settings_wifi,
            objs, &state->mContentDescription);
    String wifiName = state->mLabel;
    if (((SignalState*)state)->mConnected) {
        obj = NULL;
        CString::New(state->mLabel, (ICharSequence**)&obj);
        r->GetString(R::string::accessibility_wifi_name, objs, &wifiName);
    }
    state->mDualLabelContentDescription = wifiName;
}

Boolean WifiTile::ShouldAnnouncementBeDelayed()
{
    return ((QSTile::SignalState*)mStateBeforeClick.Get())->mEnabled
        == ((QSTile::SignalState*)mState.Get())->mEnabled;
}

String WifiTile::ComposeChangeAnnouncement()
{
    String v;
    if (((QSTile::SignalState*)mState.Get())->mEnabled) {
        mContext->GetString(R::string::accessibility_quick_settings_wifi_changed_on, &v);
    }
    else {
        mContext->GetString(R::string::accessibility_quick_settings_wifi_changed_off, &v);
    }
    return v;
}

String WifiTile::RemoveDoubleQuotes(
    /* [in] */ const String& string)
{
    if (string == NULL) return String(NULL);
    const Int32 length = string.GetLength();
    if ((length > 1) && (string.GetChar(0) == '"') && (string.GetChar(length - 1) == '"')) {
        return string.Substring(1, length - 1);
    }
    return string;
}

String WifiTile::GetSimpleName()
{
    return String("WifiTile");
}

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos


#include "elastos/droid/systemui/qs/tiles/CellularTile.h"
#include "elastos/droid/systemui/qs/SignalTileView.h"
#include "../R.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/view/LayoutInflater.h>

using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_INetworkSignalChangedCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::INetworkControllerDataUsageInfo;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Core::CBoolean;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

static AutoPtr<IIntent> InitCELLULAR_SETTINGS()
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);

    AutoPtr<IComponentName> cn;
    CComponentName::New(
        String("Elastos.Droid.Settings"),
        String("Elastos.Droid.Settings.CSettingsDataUsageSummaryActivity"), (IComponentName**)&cn);
    intent->SetComponent(cn);
    return intent;
}

AutoPtr<IIntent> CellularTile::CELLULAR_SETTINGS = InitCELLULAR_SETTINGS();

CellularTile::CallbackInfo::CallbackInfo()
    : mEnabled(FALSE)
    , mWifiEnabled(FALSE)
    , mWifiConnected(FALSE)
    , mAirplaneModeEnabled(FALSE)
    , mMobileSignalIconId(0)
    , mDataTypeIconId(0)
    , mActivityIn(FALSE)
    , mActivityOut(FALSE)
    , mNoSim(FALSE)
    , mIsDataTypeIconWide(FALSE)
{}

CAR_INTERFACE_IMPL(CellularTile::NetworkSignalChangedCallback, Object, INetworkSignalChangedCallback)

CellularTile::NetworkSignalChangedCallback::NetworkSignalChangedCallback(
    /* [in] */ CellularTile* host)
    : mHost(host)
    , mWifiEnabled(FALSE)
    , mWifiConnected(FALSE)
    , mAirplaneModeEnabled(FALSE)
{}

ECode CellularTile::NetworkSignalChangedCallback::OnWifiSignalChanged(
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean connected,
    /* [in] */ Int32 wifiSignalIconId,
    /* [in] */ Boolean activityIn,
    /* [in] */ Boolean activityOut,
    /* [in] */ const String& wifiSignalContentDescriptionId,
    /* [in] */ const String& description)
{
    mWifiEnabled = enabled;
    mWifiConnected = connected;
    return NOERROR;
}

ECode CellularTile::NetworkSignalChangedCallback::OnMobileDataSignalChanged(
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
    AutoPtr<CallbackInfo> info = new CallbackInfo();  // TODO pool?
    info->mEnabled = enabled;
    info->mWifiEnabled = mWifiEnabled;
    info->mWifiConnected = mWifiConnected;
    info->mAirplaneModeEnabled = mAirplaneModeEnabled;
    info->mMobileSignalIconId = mobileSignalIconId;
    info->mSignalContentDescription = mobileSignalContentDescriptionId;
    info->mDataTypeIconId = dataTypeIconId;
    info->mDataContentDescription = dataTypeContentDescriptionId;
    info->mActivityIn = activityIn;
    info->mActivityOut = activityOut;
    info->mEnabledDesc = description;
    info->mNoSim = noSim;
    info->mIsDataTypeIconWide = isDataTypeIconWide;
    mHost->RefreshState(info->Probe(EIID_IInterface));
    return NOERROR;
}

ECode CellularTile::NetworkSignalChangedCallback::OnAirplaneModeChanged(
    /* [in] */ Boolean enabled)
{
    mAirplaneModeEnabled = enabled;
    return NOERROR;
}

ECode CellularTile::NetworkSignalChangedCallback::OnMobileDataEnabled(
    /* [in] */ Boolean enabled)
{
    mHost->mDetailAdapter->SetMobileDataEnabled(enabled);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CellularTile::CellularDetailAdapter, Object, IQSTileDetailAdapter)
CellularTile::CellularDetailAdapter::CellularDetailAdapter(
    /* [in] */ CellularTile* host)
    : mHost(host)
{}

ECode CellularTile::CellularDetailAdapter::GetTitle(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = R::string::quick_settings_cellular_detail_title;
    return NOERROR;
}

ECode CellularTile::CellularDetailAdapter::GetToggleState(
    /* [out] */ IBoolean** result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    mHost->mController->IsMobileDataSupported(&tmp);
    if (tmp) {
        mHost->mController->IsMobileDataEnabled(&tmp);
        return CBoolean::New(tmp, result);
    }
    *result = NULL;

    return NOERROR;
}

ECode CellularTile::CellularDetailAdapter::GetSettingsIntent(
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CELLULAR_SETTINGS;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CellularTile::CellularDetailAdapter::SetToggleState(
    /* [in] */ IBoolean* state)
{
    Boolean v = FALSE;
    state->GetValue(&v);
    mHost->mController->SetMobileDataEnabled(v);
    return NOERROR;
}

ECode CellularTile::CellularDetailAdapter::CreateDetailView(
    /* [in] */ IContext* context,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IDataUsageDetailView> v;
    if (convertView != NULL) {
        v = IDataUsageDetailView::Probe(convertView);
    }
    else {
        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(mHost->mContext, (ILayoutInflater**)&inflater);
        AutoPtr<IView> view;
        inflater->Inflate(R::layout::data_usage, parent, FALSE, (IView**)&view);
        v = IDataUsageDetailView::Probe(view);
    }

    AutoPtr<INetworkControllerDataUsageInfo> info;
    mHost->mController->GetDataUsageInfo((INetworkControllerDataUsageInfo**)&info);
    if (info == NULL) {
        *result = IView::Probe(v);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    v->Bind(info);
    *result = IView::Probe(v);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CellularTile::CellularDetailAdapter::SetMobileDataEnabled(
    /* [in] */ Boolean enabled)
{
    mHost->FireToggleStateChanged(enabled);
    return NOERROR;
}

ECode CellularTile::constructor(
    /* [in] */ IQSTileHost* host)
{
    QSTile::constructor(host);
    mCallback = new NetworkSignalChangedCallback(this);
    host->GetNetworkController((INetworkController**)&mController);
    mDetailAdapter = new CellularDetailAdapter(this);
    return NOERROR;
}

AutoPtr<QSTile::State> CellularTile::NewTileState()
{
    return new SignalState();
}

ECode CellularTile::GetDetailAdapter(
    /* [out] */ IQSTileDetailAdapter** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDetailAdapter;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CellularTile::SetListening(
    /* [in] */ Boolean listening)
{
    if (listening) {
        mController->AddNetworkSignalChangedCallback(mCallback);
    }
    else {
        mController->RemoveNetworkSignalChangedCallback(mCallback);
    }
    return NOERROR;
}

ECode CellularTile::CreateTileView(
    /* [in] */ IContext* context,
    /* [out] */ IQSTileView** result)
{
    VALIDATE_NOT_NULL(result);
    *result = new SignalTileView(context);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void CellularTile::HandleClick()
{
    Boolean tmp = FALSE;
    if (mController->IsMobileDataSupported(&tmp), tmp) {
        ShowDetail(TRUE);
    }
    else {
        mHost->StartSettingsActivity(CELLULAR_SETTINGS);
    }
}

void CellularTile::HandleUpdateState(
    /* [in] */ State* state,
    /* [in] */ IInterface* arg)
{
    mController->HasMobileDataFeature(&state->mVisible);
    if (!state->mVisible) return;
    CallbackInfo* cb = (CallbackInfo*)IObject::Probe(arg);
    if (cb == NULL) return;

    AutoPtr<IResources> r;
    mContext->GetResources((IResources**)&r);
    state->mIconId = cb->mNoSim ? R::drawable::ic_qs_no_sim
            : !cb->mEnabled || cb->mAirplaneModeEnabled ? R::drawable::ic_qs_signal_disabled
            : cb->mMobileSignalIconId > 0 ? cb->mMobileSignalIconId
            : R::drawable::ic_qs_signal_no_signal;
    ((SignalState*)state)->mIsOverlayIconWide = cb->mIsDataTypeIconWide;
    state->mAutoMirrorDrawable = !cb->mNoSim;
    ((SignalState*)state)->mOverlayIconId = cb->mEnabled && (cb->mDataTypeIconId > 0) && !cb->mWifiConnected
            ? cb->mDataTypeIconId
            : 0;
    ((SignalState*)state)->mFilter = state->mIconId != R::drawable::ic_qs_no_sim;
    ((SignalState*)state)->mActivityIn = cb->mEnabled && cb->mActivityIn;
    ((SignalState*)state)->mActivityOut = cb->mEnabled && cb->mActivityOut;

    String str;
    state->mLabel = cb->mEnabled
            ? RemoveTrailingPeriod(cb->mEnabledDesc)
            : (r->GetString(R::string::quick_settings_rssi_emergency_only, &str), str);

    String signalContentDesc = cb->mEnabled && (cb->mMobileSignalIconId > 0)
            ? cb->mSignalContentDescription
            : (r->GetString(R::string::accessibility_no_signal, &str), str);
    String dataContentDesc = cb->mEnabled && (cb->mDataTypeIconId > 0) && !cb->mWifiEnabled
            ? cb->mDataContentDescription
            : (r->GetString(R::string::accessibility_no_data, &str), str);

    AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(3);
    AutoPtr<ICharSequence> obj;
    CString::New(signalContentDesc, (ICharSequence**)&obj);
    objs->Set(0, obj);
    obj = NULL;

    CString::New(dataContentDesc, (ICharSequence**)&obj);
    objs->Set(1, obj);
    obj = NULL;

    CString::New(state->mLabel, (ICharSequence**)&obj);
    objs->Set(2, obj);

    r->GetString(R::string::accessibility_quick_settings_mobile, objs, &state->mContentDescription);
}

// Remove the period from the network name
String CellularTile::RemoveTrailingPeriod(
    /* [in] */ const String& string)
{
    if (string == NULL) return String(NULL);
    const Int32 length = string.GetLength();
    if (string.EndWith(".")) {
        return string.Substring(0, length - 1);
    }
    return string;
}

String CellularTile::GetSimpleName()
{
    return String("CellularTile");
}

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

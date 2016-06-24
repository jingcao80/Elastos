
#include "Elastos.Droid.View.h"
#include "elastos/droid/settings/wifi/p2p/CWifiP2pPeer.h"
#include "../../R.h"
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Wifi::CWifiManagerHelper;
using Elastos::Droid::Wifi::IWifiManagerHelper;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {
namespace P2p {

static AutoPtr< ArrayOf<Int32> > InitSTATE_SECURED()
{
    AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(1);
    (*args)[0] = R::attr::state_encrypted;
    return args;
}

const AutoPtr< ArrayOf<Int32> > CWifiP2pPeer::STATE_SECURED = InitSTATE_SECURED();

const Int32 CWifiP2pPeer::SIGNAL_LEVELS = 4;

CAR_OBJECT_IMPL(CWifiP2pPeer)

CAR_INTERFACE_IMPL(CWifiP2pPeer, Preference, IWifiP2pPeer)

CWifiP2pPeer::CWifiP2pPeer()
    : mRssi(0)
{}

CWifiP2pPeer::~CWifiP2pPeer()
{}

ECode CWifiP2pPeer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IWifiP2pDevice* dev)
{
    Preference::constructor(context);
    mDevice = dev;
    SetWidgetLayoutResource(R::layout::preference_widget_wifi_signal);
    mRssi = 60; //TODO: fix
    return NOERROR;
}

ECode CWifiP2pPeer::OnBindView(
    /* [in] */ IView* view)
{
    String deviceName;
    mDevice->GetDeviceName(&deviceName);
    if (TextUtils::IsEmpty(deviceName)) {
        String deviceAddress;
        mDevice->GetDeviceAddress(&deviceAddress);
        SetTitle(CoreUtils::Convert(deviceAddress));
    }
    else {
        SetTitle(CoreUtils::Convert(deviceName));
    }

    AutoPtr<IView> _view;
    view->FindViewById(R::id::signal, (IView**)&_view);
    mSignal = IImageView::Probe(_view);
    if (mRssi == Elastos::Core::Math::INT32_MAX_VALUE) {
        mSignal->SetImageDrawable(NULL);
    }
    else {
        mSignal->SetImageResource(R::drawable::wifi_signal_dark);
        mSignal->SetImageState(STATE_SECURED,  TRUE);
    }
    Refresh();
    return Preference::OnBindView(view);
}

ECode CWifiP2pPeer::CompareTo(
    /* [in] */ IInterface* preference,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IWifiP2pPeer> wp2p = IWifiP2pPeer::Probe(preference);
    if (wp2p == NULL) {
        *result = 1;
        return NOERROR;
    }

    CWifiP2pPeer* other = (CWifiP2pPeer*)wp2p.Get();
    // devices go in the order of the status
    Int32 status, otherStatus;
    mDevice->GetStatus(&status);
    other->mDevice->GetStatus(&otherStatus);

    if (status != otherStatus) {
        *result = status < otherStatus ? -1 : 1;
        return NOERROR;
    }

    // Sort by name/address
    String deviceName;
    mDevice->GetDeviceName(&deviceName);
    if (!deviceName.IsNull()) {
        String otherDeviceName;
        other->mDevice->GetDeviceName(&otherDeviceName);
        *result = deviceName.CompareIgnoreCase(otherDeviceName);
        return NOERROR;
    }

    String deviceAddress, otherDeviceAddress;
    mDevice->GetDeviceAddress(&deviceAddress);
    other->mDevice->GetDeviceAddress(&otherDeviceAddress);

    *result = deviceAddress.CompareIgnoreCase(otherDeviceAddress);
    return NOERROR;
}

Int32 CWifiP2pPeer::GetLevel()
{
    if (mRssi == Elastos::Core::Math::INT32_MAX_VALUE) {
        return -1;
    }
    AutoPtr<IWifiManagerHelper> helper;
    CWifiManagerHelper::AcquireSingleton((IWifiManagerHelper**)&helper);
    Int32 result;
    helper->CalculateSignalLevel(mRssi, SIGNAL_LEVELS, &result);
    return result;
}

void CWifiP2pPeer::Refresh()
{
    if (mSignal == NULL) {
        return;
    }

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    mSignal->SetImageLevel(GetLevel());
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr< ArrayOf<String> > statusArray;
    resources->GetStringArray(R::array::wifi_p2p_status, (ArrayOf<String>**)&statusArray);
    Int32 status;
    mDevice->GetStatus(&status);
    SetSummary(CoreUtils::Convert((*statusArray)[status]));
}

} // namespace P2p
} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos


#include "elastos/droid/systemui/qs/tiles/BluetoothTile.h"
#include "elastos/droid/systemui/qs//CQSDetailItems.h"
#include "elastos/droid/systemui/statusbar/policy/PairedDevice.h"
#include "../../R.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/text/TextUtils.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IBluetoothControllerCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::IBluetoothControllerPairedDevice;
using Elastos::Droid::SystemUI::StatusBar::Policy::PairedDevice;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CBoolean;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

CAR_INTERFACE_IMPL(BluetoothTile::Callback, Object, IBluetoothControllerCallback)
BluetoothTile::Callback::Callback(
    /* [in] */ BluetoothTile* host)
    : mHost(host)
{}

ECode BluetoothTile::Callback::OnBluetoothStateChange(
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean connecting)
{
    mHost->RefreshState();
    return NOERROR;
}

ECode BluetoothTile::Callback::OnBluetoothPairedDevicesChanged()
{
    Boolean tmp = FALSE;
    AutoPtr<Runnable1> run = new Runnable1(mHost);
    mHost->mUiHandler->Post(run, &tmp);
    mHost->RefreshState();
    return NOERROR;
}

BluetoothTile::Runnable1::Runnable1(
    /* [in] */ BluetoothTile* host)
    : mHost(host)
{}

ECode BluetoothTile::Runnable1::Run()
{
    mHost->mDetailAdapter->UpdateItems();
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(BluetoothTile::BluetoothDetailAdapter, Object, IQSTileDetailAdapter, IQSDetailItemsCallback);
BluetoothTile::BluetoothDetailAdapter::BluetoothDetailAdapter(
    /* [in] */ BluetoothTile* host)
    : mHost(host)
{}

ECode BluetoothTile::BluetoothDetailAdapter::GetTitle(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = R::string::quick_settings_bluetooth_label;
    return NOERROR;
}

ECode BluetoothTile::BluetoothDetailAdapter::GetToggleState(
    /* [out] */ IBoolean** result)
{
    VALIDATE_NOT_NULL(result);
    return CBoolean::New(((BooleanState*)mHost->mState.Get())->mValue, result);
}

ECode BluetoothTile::BluetoothDetailAdapter::GetSettingsIntent(
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result);
    *result = BLUETOOTH_SETTINGS;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothTile::BluetoothDetailAdapter::SetToggleState(
    /* [in] */ IBoolean* state)
{
    Boolean s = FALSE;
    state->GetValue(&s);
    mHost->mController->SetBluetoothEnabled(s);
    mHost->ShowDetail(FALSE);
    return NOERROR;
}

ECode BluetoothTile::BluetoothDetailAdapter::CreateDetailView(
    /* [in] */ IContext* context,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    mItems = CQSDetailItems::ConvertOrInflate(context, convertView, parent);
    mItems->SetTagSuffix(String("Bluetooth"));
    mItems->SetEmptyState(R::drawable::ic_qs_bluetooth_detail_empty,
            R::string::quick_settings_bluetooth_detail_empty_text);
    mItems->SetCallback(this);
    UpdateItems();
    SetItemsVisible(((BooleanState*)mHost->mState.Get())->mValue);
    *view = IView::Probe(mItems);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode BluetoothTile::BluetoothDetailAdapter::SetItemsVisible(
    /* [in] */ Boolean visible)
{
    if (mItems == NULL) return NOERROR;
    mItems->SetItemsVisible(visible);
    return NOERROR;
}

void BluetoothTile::BluetoothDetailAdapter::UpdateItems()
{
    if (mItems == NULL) return;
    AutoPtr<IArrayList> items;
    AutoPtr<ISet> devices;  /*<IBluetoothControllerPairedDevice*/
    mHost->mController->GetPairedDevices((ISet**)&devices);
    if (devices != NULL) {
        Int32 size = 0;
        devices->GetSize(&size);
        CArrayList::New(size, (IArrayList**)&items);
        Int32 i = 0;

        AutoPtr<IIterator> ator;
        devices->GetIterator((IIterator**)&ator);
        Boolean hasNext = FALSE;
        while (ator->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            ator->GetNext((IInterface**)&obj);
            PairedDevice* device = (PairedDevice*)IBluetoothControllerPairedDevice::Probe(obj);
            AutoPtr<CQSDetailItems::Item> item = new CQSDetailItems::Item();
            item->mIcon = R::drawable::ic_qs_bluetooth_on;
            item->mLine1 = device->mName;
            if (device->mState == PairedDevice::STATE_CONNECTED) {
                item->mIcon = R::drawable::ic_qs_bluetooth_connected;
                mHost->mContext->GetString(R::string::quick_settings_connected, &item->mLine2);
                item->mCanDisconnect = TRUE;
            }
            else if (device->mState == PairedDevice::STATE_CONNECTING) {
                item->mIcon = R::drawable::ic_qs_bluetooth_connecting;
                mHost->mContext->GetString(R::string::quick_settings_connecting, &item->mLine2);
            }
            item->mTag = device->Probe(EIID_IInterface);
            items->Add(i++, item->Probe(EIID_IQSDetailItemsItem));
        }
    }
    mItems->SetItems(items);
}

ECode BluetoothTile::BluetoothDetailAdapter::OnDetailItemClick(
    /* [in] */ IQSDetailItemsItem* item)
{
    AutoPtr<IInterface> tag = ((CQSDetailItems::Item*)item)->mTag;
    if (item == NULL || tag == NULL) return NOERROR;
    PairedDevice* device = (PairedDevice*) IBluetoothControllerPairedDevice::Probe(tag);
    if (device != NULL && device->mState == PairedDevice::STATE_DISCONNECTED) {
        mHost->mController->Connect(device);
    }
    return NOERROR;
}

ECode BluetoothTile::BluetoothDetailAdapter::OnDetailItemDisconnect(
    /* [in] */ IQSDetailItemsItem* item)
{
    AutoPtr<IInterface> tag = ((CQSDetailItems::Item*)item)->mTag;
    if (item == NULL || tag == NULL) return NOERROR;
    PairedDevice* device = (PairedDevice*) IBluetoothControllerPairedDevice::Probe(tag);
    if (device != NULL) {
        mHost->mController->Disconnect(device);
    }
    return NOERROR;
}

AutoPtr<IIntent> BluetoothTile::BLUETOOTH_SETTINGS = InitStatic();
BluetoothTile::BluetoothTile(
    /* [in] */ IQSTileHost* host)
{
    QSTile::constructor(host);
    mCallback = new Callback(this);
    host->GetBluetoothController((IBluetoothController**)&mController);
    mDetailAdapter = new BluetoothDetailAdapter(this);
}

AutoPtr<IIntent> BluetoothTile::InitStatic()
{
    CIntent::New(ISettings::ACTION_BLUETOOTH_SETTINGS, (IIntent**)&BLUETOOTH_SETTINGS);
    return BLUETOOTH_SETTINGS;
}

ECode BluetoothTile::SupportsDualTargets(
    /* [in] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode BluetoothTile::GetDetailAdapter(
    /* [out] */ IQSTileDetailAdapter** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDetailAdapter;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<QSTile::State> BluetoothTile::NewTileState()
{
    return new BooleanState();
}

ECode BluetoothTile::SetListening(
    /* [in] */ Boolean listening)
{
    if (listening) {
        mController->AddStateChangedCallback(mCallback);
    }
    else {
        mController->RemoveStateChangedCallback(mCallback);
    }
    return NOERROR;
}

void BluetoothTile::HandleClick()
{
    Boolean isEnabled = (Boolean)((BooleanState*)mState.Get())->mValue;
    mController->SetBluetoothEnabled(!isEnabled);
}

void BluetoothTile::HandleSecondaryClick()
{
    mHost->StartSettingsActivity(BLUETOOTH_SETTINGS);
}

void BluetoothTile::HandleUpdateState(
    /* [in] */ State* state,
    /* [in] */ IInterface* arg)
{
    Boolean supported = FALSE;
    mController->IsBluetoothSupported(&supported);
    Boolean enabled = FALSE;
    mController->IsBluetoothEnabled(&enabled);
    Boolean connected = FALSE;
    mController->IsBluetoothConnected(&connected);
    Boolean connecting = FALSE;
    mController->IsBluetoothConnecting(&connecting);
    state->mVisible = supported;
    ((BooleanState*)mState.Get())->mValue = enabled;
    state->mAutoMirrorDrawable = FALSE;
    if (enabled) {
        state->mLabel = NULL;
        if (connected) {
            state->mIconId = R::drawable::ic_qs_bluetooth_connected;
            mContext->GetString(R::string::accessibility_quick_settings_bluetooth_connected
                    , &state->mContentDescription);
            mController->GetLastDeviceName(&state->mLabel);
        }
        else if (connecting) {
            state->mIconId = R::drawable::ic_qs_bluetooth_connecting;
            mContext->GetString(R::string::accessibility_quick_settings_bluetooth_connecting,
                &state->mContentDescription);
            mContext->GetString(R::string::quick_settings_bluetooth_label, &state->mLabel);
        }
        else {
            state->mIconId = R::drawable::ic_qs_bluetooth_on;
            mContext->GetString(R::string::accessibility_quick_settings_bluetooth_on,
                &state->mContentDescription);
        }
        if (TextUtils::IsEmpty(state->mLabel)) {
            mContext->GetString(R::string::quick_settings_bluetooth_label, &state->mLabel);
        }
    }
    else {
        state->mIconId = R::drawable::ic_qs_bluetooth_off;
        mContext->GetString(R::string::quick_settings_bluetooth_label, &state->mLabel);
        mContext->GetString(R::string::accessibility_quick_settings_bluetooth_off,
            &state->mContentDescription);
    }

    String bluetoothName = state->mLabel;
    if (connected) {
        AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(1);
        AutoPtr<ICharSequence> v;
        CString::New(state->mLabel, (ICharSequence**)&v);
        objs->Set(0, v);
        mContext->GetString(R::string::accessibility_bluetooth_name, objs, &bluetoothName);
        state->mDualLabelContentDescription = bluetoothName;
    }
    state->mDualLabelContentDescription = bluetoothName;
}

String BluetoothTile::ComposeChangeAnnouncement()
{
    String v;
    if (((BooleanState*)mState.Get())->mValue) {
        mContext->GetString(R::string::accessibility_quick_settings_bluetooth_changed_on, &v);
    }
    else {
        mContext->GetString(R::string::accessibility_quick_settings_bluetooth_changed_off, &v);
    }
    return v;
}

String BluetoothTile::GetSimpleName()
{
    return String("BluetoothTile");
}

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

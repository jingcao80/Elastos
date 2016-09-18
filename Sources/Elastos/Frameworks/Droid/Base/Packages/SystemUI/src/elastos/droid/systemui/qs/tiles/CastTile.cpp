
#include "elastos/droid/systemui/qs/tiles/CastTile.h"
#include "elastos/droid/systemui/qs/CQSDetailItems.h"
#include "elastos/droid/systemui/statusbar/policy/CastDevice.h"
#include "../R.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::SystemUI::StatusBar::Policy::CastDevice;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_ICastControllerCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IKeyguardMonitorCallback;
using Elastos::Droid::View::EIID_IViewOnAttachStateChangeListener;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CLinkedHashMap;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

static const String TAG("CastTile");

CAR_INTERFACE_IMPL_2(CastTile::Callback, Object, ICastControllerCallback, IKeyguardMonitorCallback);

CastTile::Callback::Callback(
    /* [in] */ CastTile* host)
    : mHost(host)
{}

ECode CastTile::Callback::OnCastDevicesChanged()
{
    mHost->RefreshState();
    return NOERROR;
}

ECode CastTile::Callback::OnKeyguardChanged()
{
    mHost->RefreshState();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CastTile::CastDetailAdapter::QSDetailItemsCallback,
    Object, IQSDetailItemsCallback)

CastTile::CastDetailAdapter::QSDetailItemsCallback::QSDetailItemsCallback(
    /* [in] */ CastDetailAdapter* host)
    : mHost(host)
{}

// @Override
ECode CastTile::CastDetailAdapter::QSDetailItemsCallback::OnDetailItemClick(
    /* [in] */ IQSDetailItemsItem* item)
{
    return mHost->OnDetailItemClick(item);
}

// @Override
ECode CastTile::CastDetailAdapter::QSDetailItemsCallback::OnDetailItemDisconnect(
    /* [in] */ IQSDetailItemsItem* item)
{
    return mHost->OnDetailItemDisconnect(item);
}

CAR_INTERFACE_IMPL(CastTile::CastDetailAdapter::MyListener, Object, IViewOnAttachStateChangeListener)

CastTile::CastDetailAdapter::MyListener::MyListener(
    /* [in] */ CastDetailAdapter* host)
    : mAdapter(host)
{}

ECode CastTile::CastDetailAdapter::MyListener::OnViewAttachedToWindow(
    /* [in] */ IView* v)
{
    if (DEBUG) Logger::D(TAG, "onViewAttachedToWindow");
    return NOERROR;
}

ECode CastTile::CastDetailAdapter::MyListener::OnViewDetachedFromWindow(
    /* [in] */ IView* v)
{
    if (DEBUG) Logger::D(TAG, "onViewDetachedFromWindow");
    mAdapter->mVisibleOrder->Clear();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CastTile::CastDetailAdapter, Object, IQSTileDetailAdapter);

CastTile::CastDetailAdapter::CastDetailAdapter(
    /* [in] */ CastTile* host)
    : mHost(host)
{
    CLinkedHashMap::New((ILinkedHashMap**)&mVisibleOrder);
}

ECode CastTile::CastDetailAdapter::GetTitle(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = R::string::quick_settings_cast_title;
    return NOERROR;
}

ECode CastTile::CastDetailAdapter::GetToggleState(
    /* [out] */ IBoolean** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode CastTile::CastDetailAdapter::GetSettingsIntent(
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CAST_SETTINGS;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CastTile::CastDetailAdapter::SetToggleState(
    /* [in] */ IBoolean* state)
{
    // noop
    return NOERROR;
}

ECode CastTile::CastDetailAdapter::CreateDetailView(
    /* [in] */ IContext* context,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    mItems = CQSDetailItems::ConvertOrInflate(context, convertView, parent);
    mItems->SetTagSuffix(String("Cast"));
    if (convertView == NULL) {
        if (DEBUG) Logger::D(TAG, "addOnAttachStateChangeListener");
        AutoPtr<MyListener> listener = new MyListener(this);
        IView::Probe(mItems)->AddOnAttachStateChangeListener(listener);
    }
    mItems->SetEmptyState(R::drawable::ic_qs_cast_detail_empty,
            R::string::quick_settings_cast_detail_empty_text);

    AutoPtr<QSDetailItemsCallback> cb = new QSDetailItemsCallback(this);
    mItems->SetCallback(cb);
    AutoPtr<ISet> devices;
    mHost->mController->GetCastDevices((ISet**)&devices);
    UpdateItems(devices);
    mHost->mController->SetDiscovering(TRUE);
    *result = IView::Probe(mItems);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void CastTile::CastDetailAdapter::UpdateItems(
    /* [in] */ ISet/*<ICastControllerCastDevice>*/* devices)
{
    if (mItems == NULL) return;
    AutoPtr<IArrayList> items;
    Boolean tmp = FALSE;
    if (devices != NULL && (devices->IsEmpty(&tmp), !tmp)) {
        // if we are connected, simply show that device
        AutoPtr<IIterator> ator;
        devices->GetIterator((IIterator**)&ator);
        Boolean hasNext = FALSE;
        while (ator->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            ator->GetNext((IInterface**)&obj);
            CastDevice* device = (CastDevice*)ICastControllerCastDevice::Probe(obj);
            if (device->mState == ICastControllerCastDevice::STATE_CONNECTED) {
                AutoPtr<CQSDetailItems::Item> item = new CQSDetailItems::Item();
                item->mIcon = R::drawable::ic_qs_cast_on;
                item->mLine1 = mHost->GetDeviceName(device);
                mHost->mContext->GetString(R::string::quick_settings_connected, &item->mLine2);
                item->mTag = device->Probe(EIID_IInterface);
                item->mCanDisconnect = TRUE;
                CArrayList::New(1, (IArrayList**)&items);
                items->Add(0, item->Probe(EIID_IInterface));
                break;
            }
        }

        // otherwise list all available devices, and don't move them around
        if (items == NULL) {
            ator = NULL;
            devices->GetIterator((IIterator**)&ator);
            while (ator->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                ator->GetNext((IInterface**)&obj);
                CastDevice* device = (CastDevice*)ICastControllerCastDevice::Probe(obj);
                AutoPtr<ICharSequence> k;
                CString::New(device->mId, (ICharSequence**)&k);
                mVisibleOrder->Put(k, device->Probe(EIID_IInterface));
            }

            Int32 size = 0;
            devices->GetSize(&size);
            CArrayList::New(size, (IArrayList**)&items);
            Int32 i = 0;

            AutoPtr<ISet> keySet;
            mVisibleOrder->GetKeySet((ISet**)&keySet);
            ator = NULL;
            keySet->GetIterator((IIterator**)&ator);
            while (ator->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> id;
                ator->GetNext((IInterface**)&id);

                AutoPtr<IInterface> obj;
                mVisibleOrder->Get(id, (IInterface**)&obj);
                AutoPtr<ICastControllerCastDevice> device = ICastControllerCastDevice::Probe(obj);
                if (devices->Contains(device, &tmp), !tmp) continue;
                AutoPtr<CQSDetailItems::Item> item = new CQSDetailItems::Item();
                item->mIcon = R::drawable::ic_qs_cast_off;
                item->mLine1 = mHost->GetDeviceName(device);
                if (((CastDevice*)device.Get())->mState == ICastControllerCastDevice::STATE_CONNECTING) {
                    mHost->mContext->GetString(R::string::quick_settings_connecting, &item->mLine2);
                }
                item->mTag = device;
                items->Add(i++, item->Probe(EIID_IInterface));
            }
        }
    }
    mItems->SetItems(items);
}

ECode CastTile::CastDetailAdapter::OnDetailItemClick(
    /* [in] */ IQSDetailItemsItem* item)
{
    AutoPtr<IInterface> tag = ((CQSDetailItems::Item*)item)->mTag;
    if (item == NULL || tag == NULL) return NOERROR;
    AutoPtr<ICastControllerCastDevice> device = ICastControllerCastDevice::Probe(tag);
    mHost->mController->StartCasting(device);
    return NOERROR;
}

ECode CastTile::CastDetailAdapter::OnDetailItemDisconnect(
    /* [in] */ IQSDetailItemsItem* item)
{
    AutoPtr<IInterface> tag = ((CQSDetailItems::Item*)item)->mTag;
    if (item == NULL || tag == NULL) return NOERROR;
    AutoPtr<ICastControllerCastDevice> device = ICastControllerCastDevice::Probe(tag);
    mHost->mController->StopCasting(device);
    return NOERROR;
}

AutoPtr<IIntent> CastTile::CAST_SETTINGS = InitStatic();
CastTile::CastTile(
    /* [in] */ IQSTileHost* host)
{
    QSTile::constructor(host);
    mCallback = new Callback(this);
    host->GetCastController((ICastController**)&mController);
    mDetailAdapter = new CastDetailAdapter(this);
    host->GetKeyguardMonitor((IKeyguardMonitor**)&mKeyguard);
}

AutoPtr<IIntent> CastTile::InitStatic()
{
    CIntent::New(ISettings::ACTION_CAST_SETTINGS, (IIntent**)&CAST_SETTINGS);
    return CAST_SETTINGS;
}

ECode CastTile::GetDetailAdapter(
    /* [out] */ IQSTileDetailAdapter** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDetailAdapter;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<QSTile::State> CastTile::NewTileState()
{
    return new BooleanState();
}

ECode CastTile::SetListening(
    /* [in] */ Boolean listening)
{
    if (mController == NULL) return NOERROR;
    if (DEBUG) Logger::D(TAG, "setListening %d", listening);
    if (listening) {
        mController->AddCallback(mCallback);
        mKeyguard->AddCallback(mCallback);
    }
    else {
        mController->SetDiscovering(FALSE);
        mController->RemoveCallback(mCallback);
        mKeyguard->RemoveCallback(mCallback);
    }
    return NOERROR;
}

void CastTile::HandleUserSwitch(
    /* [in] */ Int32 newUserId)
{
    QSTile::HandleUserSwitch(newUserId);
    if (mController == NULL) return;
    mController->SetCurrentUserId(newUserId);
}

void CastTile::HandleClick()
{
    ShowDetail(TRUE);
}

void CastTile::HandleUpdateState(
    /* [in] */ State* state,
    /* [in] */ IInterface* arg)
{
    Boolean t1 = FALSE, t2 = FALSE;
    state->mVisible = !((mKeyguard->IsSecure(&t1), t1) && (mKeyguard->IsShowing(&t2), t2));
    mContext->GetString(R::string::quick_settings_cast_title, &state->mLabel);
    ((BooleanState*)state)->mValue = FALSE;
    state->mAutoMirrorDrawable = FALSE;
    AutoPtr<ISet> devices;  /*<ICastControllerCastDevice*/
    mController->GetCastDevices((ISet**)&devices);
    Boolean connecting = FALSE;

    AutoPtr<IIterator> ator;
    devices->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        CastDevice* device = (CastDevice*)ICastControllerCastDevice::Probe(obj);
        if (device->mState == ICastControllerCastDevice::STATE_CONNECTED) {
            ((BooleanState*)state)->mValue = TRUE;
            state->mLabel = GetDeviceName(device);
        }
        else if (device->mState == ICastControllerCastDevice::STATE_CONNECTING) {
            connecting = TRUE;
        }
    }

    if (!((BooleanState*)state)->mValue && connecting) {
        mContext->GetString(R::string::quick_settings_connecting, &state->mLabel);
    }
    state->mIconId = ((BooleanState*)state)->mValue ? R::drawable::ic_qs_cast_on : R::drawable::ic_qs_cast_off;
    mDetailAdapter->UpdateItems(devices);
}

String CastTile::ComposeChangeAnnouncement()
{
    if (!((BooleanState*)mState.Get())->mValue) {
        String v;
        // We only announce when it's turned off to avoid vocal overflow.
        mContext->GetString(R::string::accessibility_casting_turned_off, &v);
        return v;
    }
    return String(NULL);
}

String CastTile::GetDeviceName(
    /* [in] */ ICastControllerCastDevice* device)
{
    String name = ((CastDevice*)device)->mName;
    if (name != NULL) {
        return name;
    }
    mContext->GetString(R::string::quick_settings_cast_device_default_name, &name);
    return name;
}

String CastTile::GetSimpleName()
{
    return String("CastTile");
}

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

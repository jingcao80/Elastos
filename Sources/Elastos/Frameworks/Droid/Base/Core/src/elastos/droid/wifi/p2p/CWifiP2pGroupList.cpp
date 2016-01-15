
#include "elastos/droid/wifi/p2p/CWifiP2pGroupList.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::CInteger32;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

//===============================================================
//                CWifiP2pGroupList::GroupLruCache
//===============================================================

CWifiP2pGroupList::GroupLruCache::GroupLruCache(
    /* [in] */ Int32 size,
    /* [in] */ CWifiP2pGroupList* host)
    // TODO : LruCache(size)
    : mHost(host)
{
}

void CWifiP2pGroupList::GroupLruCache::EntryRemoved(
    /* [in] */ Boolean evicted,
    /* [in] */ IInteger32* netId,
    /* [in] */ AutoPtr<IWifiP2pGroup> oldValue,
    /* [in] */ AutoPtr<IWifiP2pGroup> newValue)
{
    if (mHost->mListener != NULL && !mHost->mIsClearCalled) {
        Int32 networkId;
        oldValue->GetNetworkId(&networkId);
        mHost->mListener->OnDeleteGroup(networkId);
    }
}

//===============================================================
//                    CWifiP2pGroupList
//===============================================================

const Int32 CWifiP2pGroupList::CREDENTIAL_MAX_NUM = 32;

CAR_INTERFACE_IMPL_2(CWifiP2pGroupList, Object, IWifiP2pGroupList, IParcelable)

CAR_OBJECT_IMPL(CWifiP2pGroupList)

CWifiP2pGroupList::CWifiP2pGroupList()
    : mGroups(NULL)
    , mIsClearCalled(FALSE)
{
}

CWifiP2pGroupList::~CWifiP2pGroupList()
{
}

ECode CWifiP2pGroupList::constructor()
{
    return constructor(NULL, NULL);
}

ECode CWifiP2pGroupList::constructor(
    /* [in] */ IWifiP2pGroupList* source,
    /* [in] */ IWifiP2pGroupListGroupDeleteListener* listener)
{
    // assert(0);
    // TODO
    // mListener = listener;
    // mGroups = new GroupLruCache(CREDENTIAL_MAX_NUM, this);

    // if (source != NULL) {
    //     CWifiP2pGroupList* wgl = (CWifiP2pGroupList*)source;
    //     AutoPtr<HashMap< AutoPtr<IInteger32>, AutoPtr<IWifiP2pGroup> > > snapshot = wgl->mGroups->Snapshot();

    //     HashMap< AutoPtr<IInteger32>, AutoPtr<IWifiP2pGroup> >::Iterator it;
    //     for (it = snapshot->Begin(); it != snapshot->End(); ++it) {
    //         mGroups->Put(it->mFirst, it->mSecond);
    //     }
    // }

    // return NOERROR;

    return E_NOT_IMPLEMENTED;
}

ECode CWifiP2pGroupList::GetGroupList(
    /* [out] */ ICollection** result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiP2pGroupList::GetGroupList(
    /* [out, callee] */ ArrayOf<IWifiP2pGroup*>** list)
{
    VALIDATE_NOT_NULL(list);

    assert(0);
    // TODO
    // AutoPtr<HashMap< AutoPtr<IInteger32>, AutoPtr<IWifiP2pGroup> > > snapshot = mGroups->Snapshot();
    // AutoPtr<ArrayOf<IWifiP2pGroup*> > array = ArrayOf<IWifiP2pGroup*>::Alloc(snapshot->GetSize());

    // Int32 i = 0;
    // HashMap< AutoPtr<IInteger32>, AutoPtr<IWifiP2pGroup> >::Iterator it;
    // for (it = snapshot->Begin(); it != snapshot->End(); ++it, ++i) {
    //     array->Set(i, it->mSecond);
    // }

    // *list = array;
    // REFCOUNT_ADD(*list);

    // return NOERROR;

    return E_NOT_IMPLEMENTED;
}

ECode CWifiP2pGroupList::Add(
    /* [in] */ IWifiP2pGroup* group)
{
    VALIDATE_NOT_NULL(group);
    Int32 networkId;
    group->GetNetworkId(&networkId);
    AutoPtr<IInteger32> key;
    CInteger32::New(networkId, (IInteger32**)&key);
    assert(0);
    // TODO
    // mGroups->Put(key, group);
    return NOERROR;
}

ECode CWifiP2pGroupList::Remove(
    /* [in] */ Int32 netId)
{
    AutoPtr<IInteger32> key;
    CInteger32::New(netId, (IInteger32**)&key);
    assert(0);
    // TODO
    // mGroups->Remove(key);
    return NOERROR;
}

ECode CWifiP2pGroupList::Remove(
    /* [in] */ const String& deviceAddress)
{
    Int32 networkId;
    GetNetworkId(deviceAddress, &networkId);
    return Remove(networkId);
}

ECode CWifiP2pGroupList::Clear(
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = FALSE;

    assert(0);
    // TODO
    // if (mGroups->Size() == 0) {
    //     return NOERROR;
    // }
    mIsClearCalled = TRUE;
    assert(0);
    // TODO
    // mGroups->EvictAll();
    mIsClearCalled = FALSE;
    *ret = TRUE;
    return NOERROR;
}

ECode CWifiP2pGroupList::GetNetworkId(
    /* [in] */ const String& deviceAddress,
    /* [out] */ Int32* networkId)
{
    VALIDATE_NOT_NULL(networkId);
    *networkId = -1;

    if (deviceAddress.IsNull()) {
        return NOERROR;
    }

    AutoPtr<IWifiP2pGroup> group;
    String address;
    Int32 netId;

    assert(0);
    // TODO
    // AutoPtr<HashMap< AutoPtr<IInteger32>, AutoPtr<IWifiP2pGroup> > > snapshot = mGroups->Snapshot();
    // HashMap< AutoPtr<IInteger32>, AutoPtr<IWifiP2pGroup> >::Iterator it;
    // for (it = snapshot->Begin(); it != snapshot->End(); ++it) {
    //     group = it->mSecond;
    //     AutoPtr<IWifiP2pDevice> owner;
    //     group->GetOwner((IWifiP2pDevice**)&owner);
    //     owner->GetDeviceAddress(&address);

    //     if (deviceAddress.Equals(address)) {
    //         group->GetNetworkId(&netId);
    //         // update cache ordered.
    //         AutoPtr<IInteger32> key;
    //         CInteger32::New(netId, (IInteger32**)&key);
    //         mGroups->Get(key);
    //         *networkId = netId;
    //         break;
    //     }
    // }

    return NOERROR;
}

ECode CWifiP2pGroupList::GetNetworkId(
    /* [in] */ const String& deviceAddress,
    /* [in] */ const String& ssid,
    /* [out] */ Int32 * networkId)
{
    VALIDATE_NOT_NULL(networkId);
    *networkId = -1;

    if (deviceAddress.IsNull() || ssid.IsNull()) {
        return NOERROR;
    }

    AutoPtr<IWifiP2pGroup> group;
    String address, networkName;
    Int32 netId;

    assert(0);
    // TODO
    // AutoPtr<HashMap< AutoPtr<IInteger32>, AutoPtr<IWifiP2pGroup> > > snapshot = mGroups->Snapshot();
    // HashMap< AutoPtr<IInteger32>, AutoPtr<IWifiP2pGroup> >::Iterator it;
    // for (it = snapshot->Begin(); it != snapshot->End(); ++it) {
    //     group = it->mSecond;
    //     group->GetNetworkName(&networkName);
    //     if (ssid.Equals(networkName)) {
    //         AutoPtr<IWifiP2pDevice> owner;
    //         group->GetOwner((IWifiP2pDevice**)&owner);
    //         owner->GetDeviceAddress(&address);

    //         if (deviceAddress.Equals(address)) {
    //             group->GetNetworkId(&netId);
    //             // update cache ordered.
    //             AutoPtr<IInteger32> key;
    //             CInteger32::New(netId, (IInteger32**)&key);
    //             mGroups->Get(key);
    //             *networkId = netId;
    //             break;
    //         }
    //     }
    // }

    return NOERROR;
}

ECode CWifiP2pGroupList::GetOwnerAddr(
    /* [in] */ Int32 netId,
    /* [out] */ String* address)
{
    VALIDATE_NOT_NULL(address);
    *address = NULL;

    AutoPtr<IInteger32> key;
    CInteger32::New(netId, (IInteger32**)&key);
    AutoPtr<IWifiP2pGroup> group;// TODO = mGroups->Get(key);
    if (group != NULL) {
        AutoPtr<IWifiP2pDevice> owner;
        group->GetOwner((IWifiP2pDevice**)&owner);
        return owner->GetDeviceAddress(address);
    }

    return NOERROR;
}

ECode CWifiP2pGroupList::Contains(
    /* [in] */ Int32 networkId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    assert(0);
    // TODO
    // AutoPtr< HashMap< AutoPtr<IInteger32>, AutoPtr<IWifiP2pGroup> > > snapshot = mGroups->Snapshot();
    // HashMap< AutoPtr<IInteger32>, AutoPtr<IWifiP2pGroup> >::Iterator it;

    // Int32 netId;
    // AutoPtr<IWifiP2pGroup> group;
    // for (it = snapshot->Begin(); it != snapshot->End(); ++it) {
    //     group = it->mSecond;
    //     group->GetNetworkId(&netId);
    //     if (netId == networkId) {
    //         *result = TRUE;
    //         break;
    //     }

    // }

    // return NOERROR;

    return E_NOT_IMPLEMENTED;
}

ECode CWifiP2pGroupList::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb("");
    String info;
    AutoPtr<IWifiP2pGroup> group;

    assert(0);
    // TODO
    // AutoPtr<HashMap< AutoPtr<IInteger32>, AutoPtr<IWifiP2pGroup> > > snapshot = mGroups->Snapshot();
    // HashMap< AutoPtr<IInteger32>, AutoPtr<IWifiP2pGroup> >::Iterator it;
    // for (it = snapshot->Begin(); it != snapshot->End(); ++it) {
    //     group = it->mSecond;
    //     assert(0);
    //     // TODO
    //     // group->ToString(&info);
    //     sb += info;
    //     sb += "\n";
    // }

    *str = sb.ToString();
    return NOERROR;
}

ECode CWifiP2pGroupList::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);
    Int32 deviceCount;
    source->ReadInt32(&deviceCount);
    for (Int32 i = 0; i < deviceCount; ++i) {
        AutoPtr<IWifiP2pGroup> group;
        source->ReadInterfacePtr((Handle32*)&group);
        Add(group);
    }

    return NOERROR;
}

ECode CWifiP2pGroupList::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    assert(0);
    // TODO
    // AutoPtr<HashMap< AutoPtr<IInteger32>, AutoPtr<IWifiP2pGroup> > > snapshot = mGroups->Snapshot();
    // dest->WriteInt32(snapshot->GetSize());

    // AutoPtr<IWifiP2pGroup> group;
    // HashMap< AutoPtr<IInteger32>, AutoPtr<IWifiP2pGroup> >::Iterator it;
    // for (it = snapshot->Begin(); it != snapshot->End(); ++it) {
    //     group = it->mSecond;
    //     dest->WriteInterfacePtr(group.Get());
    // }

    // return NOERROR;

    return E_NOT_IMPLEMENTED;
}

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

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

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/wifi/p2p/CWifiP2pGroupList.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::CInteger32;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IList;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::IMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

//===============================================================
//                CWifiP2pGroupList::GroupLruCache
//===============================================================

ECode CWifiP2pGroupList::GroupLruCache::constructor(
    /* [in] */ Int32 size,
    /* [in] */ IWifiP2pGroupList* host)
{
    mHost = (CWifiP2pGroupList*)host;
    return LruCache::constructor(size);
}

ECode CWifiP2pGroupList::GroupLruCache::EntryRemoved(
    /* [in] */ Boolean evicted,
    /* [in] */ IInterface* netId,
    /* [in] */ IInterface* oldValue,
    /* [in] */ IInterface* newValue)
{
    if (mHost->mListener != NULL && !mHost->mIsClearCalled) {
        Int32 networkId;
        IWifiP2pGroup::Probe(oldValue)->GetNetworkId(&networkId);
        mHost->mListener->OnDeleteGroup(networkId);
    }
    return NOERROR;
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
    mListener = listener;
    mGroups = new GroupLruCache();
    mGroups->constructor(CREDENTIAL_MAX_NUM, this);

    if (source != NULL) {
        CWifiP2pGroupList* wgl = (CWifiP2pGroupList*)source;
        AutoPtr<IMap> map;
        wgl->mGroups->Snapshot((IMap**)&map);

        AutoPtr<ISet> entries;
        map->GetEntrySet((ISet**)&entries);
        AutoPtr<IIterator> it;
        entries->GetIterator((IIterator**)&it);
        Boolean hasNext;
        IMapEntry* entry;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj, key, value;
            it->GetNext((IInterface**)&obj);
            entry = IMapEntry::Probe(obj);
            entry->GetKey((IInterface**)&key);
            entry->GetValue((IInterface**)&value);
            mGroups->Put(key, value, NULL);
        }
    }

    return NOERROR;
}

ECode CWifiP2pGroupList::GetGroupList(
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IMap> map;
    mGroups->Snapshot((IMap**)&map);
    return map->GetValues(result);
}

ECode CWifiP2pGroupList::GetGroupList(
    /* [out, callee] */ ArrayOf<IWifiP2pGroup*>** list)
{
    VALIDATE_NOT_NULL(list);

    AutoPtr<IMap> map;
    mGroups->Snapshot((IMap**)&map);
    AutoPtr<ICollection> values;
    map->GetValues((ICollection**)&values);

    Int32 size = 0;
    if (values) {
        values->GetSize(&size);
    }
    AutoPtr<ArrayOf<IWifiP2pGroup*> > array = ArrayOf<IWifiP2pGroup*>::Alloc(size);

    if (size > 0) {
        AutoPtr<IIterator> it;
        values->GetIterator((IIterator**)&it);
        Boolean hasNext;
        Int32 i = 0;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            array->Set(i, IWifiP2pGroup::Probe(obj));
        }
    }

    *list = array;
    REFCOUNT_ADD(*list);
    return NOERROR;

}

ECode CWifiP2pGroupList::Add(
    /* [in] */ IWifiP2pGroup* group)
{
    VALIDATE_NOT_NULL(group);
    Int32 networkId;
    group->GetNetworkId(&networkId);
    AutoPtr<IInteger32> key;
    CInteger32::New(networkId, (IInteger32**)&key);
    mGroups->Put(key, group, NULL);
    return NOERROR;
}

ECode CWifiP2pGroupList::Remove(
    /* [in] */ Int32 netId)
{
    AutoPtr<IInteger32> key;
    CInteger32::New(netId, (IInteger32**)&key);
    mGroups->Remove(key, NULL);
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

    Int32 size;
    mGroups->GetSize(&size);
    if (size == 0) {
         return NOERROR;
    }
    mIsClearCalled = TRUE;
    mGroups->EvictAll();
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

    AutoPtr<IMap> map;
    mGroups->Snapshot((IMap**)&map);
    AutoPtr<ICollection> values;
    map->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        group = IWifiP2pGroup::Probe(obj);
        AutoPtr<IWifiP2pDevice> owner;
        group->GetOwner((IWifiP2pDevice**)&owner);
        owner->GetDeviceAddress(&address);

        if (deviceAddress.Equals(address)) {
            group->GetNetworkId(&netId);
            // update cache ordered.
            AutoPtr<IInteger32> key;
            CInteger32::New(netId, (IInteger32**)&key);
            AutoPtr<IInterface> value;
            mGroups->Get(key, (IInterface**)&value);
            *networkId = netId;
            break;
        }
    }

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

    AutoPtr<IMap> map;
    mGroups->Snapshot((IMap**)&map);
    AutoPtr<ICollection> values;
    map->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        group = IWifiP2pGroup::Probe(obj);
        group->GetNetworkName(&networkName);
        if (ssid.Equals(networkName)) {
            AutoPtr<IWifiP2pDevice> owner;
            group->GetOwner((IWifiP2pDevice**)&owner);
            owner->GetDeviceAddress(&address);

            if (deviceAddress.Equals(address)) {
                group->GetNetworkId(&netId);
                // update cache ordered.
                AutoPtr<IInteger32> key;
                CInteger32::New(netId, (IInteger32**)&key);
                AutoPtr<IInterface> value;
                mGroups->Get(key, (IInterface**)&value);
                *networkId = netId;
                break;
            }
        }
    }

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
    AutoPtr<IInterface> value;
    mGroups->Get(key, (IInterface**)&value);
    if (value != NULL) {
        AutoPtr<IWifiP2pDevice> owner;
        IWifiP2pGroup::Probe(value)->GetOwner((IWifiP2pDevice**)&owner);
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

    Int32 netId;
    AutoPtr<IMap> map;
    mGroups->Snapshot((IMap**)&map);
    AutoPtr<ICollection> values;
    map->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IWifiP2pGroup::Probe(obj)->GetNetworkId(&netId);
        if (netId == networkId) {
            *result = TRUE;
            break;
        }
    }

    return NOERROR;
}

ECode CWifiP2pGroupList::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb("");

    AutoPtr<IMap> map;
    mGroups->Snapshot((IMap**)&map);
    AutoPtr<ICollection> values;
    map->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        sb += Object::ToString(obj);
        sb += "\n";
    }

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
        source->ReadInterfacePtr((HANDLE*)&group);
        Add(group);
    }

    return NOERROR;
}

ECode CWifiP2pGroupList::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    AutoPtr<IMap> map;
    mGroups->Snapshot((IMap**)&map);
    AutoPtr<ICollection> values;
    map->GetValues((ICollection**)&values);

    Int32 size;
    values->GetSize(&size);
    dest->WriteInt32(size);

    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        dest->WriteInterfacePtr(obj);
    }

    return NOERROR;
}

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

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

#include "elastos/droid/server/CNetworkPolicyListener.h"
#include "elastos/droid/server/CConnectivityService.h"
#include <elastos/core/AutoLock.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Net::INetworkRequest;
using Elastos::Droid::Net::INetworkPolicyManager;
using Elastos::Droid::Net::CConnectivityManagerHelper;
using Elastos::Droid::Net::EIID_IINetworkPolicyListener;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Utility::IHashMap;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Server {

CAR_INTERFACE_IMPL_2(CNetworkPolicyListener, Object, IINetworkPolicyListener, IBinder)

CAR_OBJECT_IMPL(CNetworkPolicyListener)

ECode CNetworkPolicyListener::constructor(
    /* [in] */ IIConnectivityManager* owner)
{
    mOwner = (CConnectivityService*)owner;
    return NOERROR;
}

ECode CNetworkPolicyListener::OnUidRulesChanged(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 uidRules)
{
    // caller is NPMS, since we only register with them
//    if (LOGD_RULES) {
//        Log("onUidRulesChanged(uid=" + String::FromInt32(uid) + ", uidRules=" + String::FromInt32(uidRules) + ")");
//    }

    {
        AutoLock lock(mOwner->mRulesLock);
        // skip update when we've already applied rules
        Int32 oldRules;
        HashMap<Int32, Int32>::Iterator iter = mOwner->mUidRules.Find(uid);
        if (iter != mOwner->mUidRules.End()) {
            oldRules = iter->mSecond;
        }
        else {
            oldRules = INetworkPolicyManager::RULE_ALLOW_ALL;
        }
        if (oldRules == uidRules) return NOERROR;

        mOwner->mUidRules[uid] = uidRules;
    }
    return NOERROR;
    // TODO: notify UID when it has requested targeted updates
}

ECode CNetworkPolicyListener::OnMeteredIfacesChanged(
    /* [in] */ ArrayOf<String>* meteredIfaces)
{
    // caller is NPMS, since we only register with them
//    if (LOGD_RULES) {
//        //Log("onMeteredIfacesChanged(ifaces=" + Arrays.toString(meteredIfaces) + ")");
//    }
    {
       AutoLock lock(mOwner->mRulesLock);
       mOwner->mMeteredIfaces.Clear();
       for (Int32 i = 0; i < meteredIfaces->GetLength(); i++) {
           mOwner->mMeteredIfaces.Insert((*meteredIfaces)[i]);
       }
    }
    return NOERROR;
}

ECode CNetworkPolicyListener::OnRestrictBackgroundChanged(
    /* [in] */ Boolean restrictBackground)
{
    // caller is NPMS, since we only register with them
//    if (LOGD_RULES) {
//        Log("onRestrictBackgroundChanged(restrictBackground=" + String::FromBoolean(restrictBackground) + ")");
//    }

    // kick off connectivity change broadcast for active network, since
    // global background policy change is radical.
    Int32 networkType = mOwner->mActiveDefaultNetwork;
    AutoPtr<IConnectivityManagerHelper> helper;
    CConnectivityManagerHelper::AcquireSingleton((IConnectivityManagerHelper**)&helper);
    Boolean isValid;
    if (helper->IsNetworkTypeValid(networkType, &isValid), isValid) {
        AutoPtr<INetworkStateTracker> tracker = (*(mOwner->mNetTrackers))[networkType];
        if (tracker != NULL) {
            AutoPtr<INetworkInfo> info;
            tracker->GetNetworkInfo((INetworkInfo**)&info);
            Boolean connected = FALSE;
            if (info != NULL && (info->IsConnected(&connected), connected)) {
                mOwner->SendConnectedBroadcast(info);
            }
        }
    }
    return NOERROR;
}

ECode CNetworkPolicyListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    Object::ToString(str);
    return NOERROR;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos

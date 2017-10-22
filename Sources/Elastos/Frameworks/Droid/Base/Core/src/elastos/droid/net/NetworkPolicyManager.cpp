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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Core.h"
#include "elastos/droid/net/NetworkPolicyManager.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/text/format/CTime.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Collect::ISets;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Text::Format::ITime;

using Elastos::IO::IPrintWriter;
using Elastos::Utility::CHashSet;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashSet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(NetworkPolicyManager, Object, INetworkPolicyManager)

const Boolean NetworkPolicyManager::ALLOW_PLATFORM_APP_POLICY = TRUE;

ECode NetworkPolicyManager::constructor(
    /* [in] */ IINetworkPolicyManager* service)
{
    if (service == NULL) {
        Logger::E("NetworkPolicyManager", "missing INetworkPolicyManager");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mService = service;
    return NOERROR;
}

ECode NetworkPolicyManager::From(
    /* [in] */ IContext* context,
    /* [out] */ INetworkPolicyManager** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IInterface> obj;
    ASSERT_SUCCEEDED(context->GetSystemService(IContext::NETWORK_POLICY_SERVICE, (IInterface**)&obj));
    *result = INetworkPolicyManager::Probe(obj);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode NetworkPolicyManager::SetUidPolicy(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 policy)
{
    return mService->SetUidPolicy(uid, policy);
}

ECode NetworkPolicyManager::AddUidPolicy(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 policy)
{
        // try {
    ECode ec = mService->AddUidPolicy(uid, policy);
        // } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) return NOERROR;
        // }
    return ec;
}

ECode NetworkPolicyManager::RemoveUidPolicy(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 policy)
{
        // try {
    ECode ec = mService->RemoveUidPolicy(uid, policy);
        // } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) return NOERROR;
        // }
    return ec;
}

ECode NetworkPolicyManager::GetUidPolicy(
    /* [in] */ Int32 uid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    ECode ec = mService->GetUidPolicy(uid, result);
    if(FAILED(ec)) *result = POLICY_NONE;
    return NOERROR;
}

ECode NetworkPolicyManager::GetUidsWithPolicy(
    /* [in] */ Int32 policy,
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr< ArrayOf<Int32> > outputarray;
    ECode ec = mService->GetUidsWithPolicy(policy, (ArrayOf<Int32>**)&outputarray);
    if(FAILED(ec)) outputarray = ArrayOf<Int32>::Alloc(0);
    *result = outputarray;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkPolicyManager::GetPowerSaveAppIdWhitelist(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
        // try {
    ECode ec =  mService->GetPowerSaveAppIdWhitelist(result);
        // } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *result = ArrayOf<Int32>::Alloc(0);
        return NOERROR;
    }
        // }
    return ec;
}

ECode NetworkPolicyManager::RegisterListener(
    /* [in] */ IINetworkPolicyListener* listener)
{
    return mService->RegisterListener(listener);
}

ECode NetworkPolicyManager::UnregisterListener(
    /* [in] */ IINetworkPolicyListener* listener)
{
    return mService->UnregisterListener(listener);
}

ECode NetworkPolicyManager::SetNetworkPolicies(
    /* [in] */ ArrayOf<INetworkPolicy*>* policies)
{
    return mService->SetNetworkPolicies(policies);
}

ECode NetworkPolicyManager::GetNetworkPolicies(
    /* [out, callee] */ ArrayOf<INetworkPolicy*>** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr< ArrayOf<INetworkPolicy*> > outputarray;
    ECode ec = mService->GetNetworkPolicies((ArrayOf<INetworkPolicy*>**)&outputarray);
    if(FAILED(ec)) outputarray = NULL;
    *result = outputarray;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkPolicyManager::SetRestrictBackground(
    /* [in] */ Boolean restrictBackground)
{
    return mService->SetRestrictBackground(restrictBackground);
}

ECode NetworkPolicyManager::GetRestrictBackground(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    ECode ec = mService->GetRestrictBackground(result);
    if(FAILED(ec)) *result = FALSE;
    return NOERROR;
}

ECode NetworkPolicyManager::ComputeLastCycleBoundary(
    /* [in] */ Int64 currentTime,
    /* [in] */ INetworkPolicy* policy,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 cycleday;
    policy->GetCycleDay(&cycleday);
    if (cycleday == INetworkPolicy::CYCLE_NONE) {
        Logger::E("NetworkPolicyManager", "Unable to compute boundary without cycleDay");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String cycleTimezone;
    policy->GetCycleTimezone(&cycleTimezone);
    AutoPtr<ITime> now;
    CTime::New(cycleTimezone, (ITime**)&now);
    now->Set(currentTime);
    //first, find cycle boundary for current month
    AutoPtr<ITime> cycle;
    CTime::New(now, (ITime**)&cycle);
    cycle->SetHour(0);
    cycle->SetMinute(0);
    cycle->SetSecond(0);
    SnapToCycleDay(cycle, cycleday);
    Int32 compRet;
    if (CTime::Compare(cycle, now, &compRet), compRet >= 0) {
        //cycle boundary is beyond now, use last cycle boundary; start by
        //pushing ourselves squarely into last month.
        AutoPtr<ITime> lastMonth;
        CTime::New(now, (ITime**)&lastMonth);
        lastMonth->SetHour(0);
        lastMonth->SetMinute(0);
        lastMonth->SetSecond(0);
        lastMonth->SetMonthDay(1);
        Int32 month;
        lastMonth->GetMonth(&month);
        lastMonth->SetMonth(month - 1);
        Int64 ret;
        lastMonth->Normalize(TRUE, &ret);
        cycle->Set(lastMonth);
        SnapToCycleDay(cycle, cycleday);
    }
    cycle->ToMillis(TRUE, result);
    return NOERROR;
}

ECode NetworkPolicyManager::ComputeNextCycleBoundary(
    /* [in] */ Int64 currentTime,
    /* [in] */ INetworkPolicy* policy,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 cycleday;
    policy->GetCycleDay(&cycleday);
    if (cycleday == INetworkPolicy::CYCLE_NONE) {
        Logger::E("NetworkPolicyManager", "Unable to compute boundary without cycleDay");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ITime> now;
    String cycleTimezone;
    policy->GetCycleTimezone(&cycleTimezone);
    CTime::New(cycleTimezone, (ITime**)&now);
    now->Set(currentTime);
    // // first, find cycle boundary for current month
    AutoPtr<ITime> cycle;
    CTime::New(now, (ITime**)&cycle);
    cycle->SetHour(0);
    cycle->SetMinute(0);
    cycle->SetSecond(0);

    SnapToCycleDay(cycle, cycleday);
    Int32 comRet;
    if (CTime::Compare(cycle, now, &comRet), comRet <= 0) {
        // cycle boundary is before now, use next cycle boundary; start by
        // pushing ourselves squarely into next month.
        AutoPtr<ITime> nextMonth;
        CTime::New(now, (ITime**)&nextMonth);
        nextMonth->SetHour(0);
        nextMonth->SetMinute(0);
        nextMonth->SetSecond(0);
        nextMonth->SetMonthDay(1);
        Int32 month;
        nextMonth->GetMonth(&month);
        nextMonth->SetMonth(month + 1);
        Int64 ret;
        nextMonth->Normalize(TRUE, &ret);
        cycle->Set(nextMonth);
        SnapToCycleDay(cycle, cycleday);
    }
    cycle->ToMillis(TRUE, result);
    return NOERROR;
}

ECode NetworkPolicyManager::SnapToCycleDay(
    /* [in] */ ITime* time,
    /* [in] */ Int32 cycleDay)
{
    Int32 maximum;
    time->GetActualMaximum(ITime::MONTH_DAY, &maximum);
    if (cycleDay > maximum) {
        // cycle day isn't valid this month; snap to last second of month
        Int32 month;
        time->GetMonth(&month);
        time->SetMonth(month + 1);
        time->SetMonthDay(1);
        Int32 second;
        time->GetSecond(&second);
        time->SetSecond(second - 1);
    }
    else {
        time->SetMonthDay(cycleDay);
    }
    Int64 ret;
    time->Normalize(TRUE, &ret);
    return NOERROR;
}

ECode NetworkPolicyManager::IsUidValidForPolicy(
    /* [in] */ IContext* context,
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    //first, quick-reject non-applications
    if (!(CUserHandle::IsApp(uid))) {
        *result = FALSE;
    }

    if (!ALLOW_PLATFORM_APP_POLICY) {
        AutoPtr<IPackageManager> pm;
        context->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IHashSet> systemSignature;
        CHashSet::New((IHashSet**)&systemSignature);
            //try {
        AutoPtr<IPackageInfo> packageinfo;
        ECode ec = pm->GetPackageInfo(String("android"), IPackageManager::GET_SIGNATURES, (IPackageInfo**)&packageinfo);
        if (FAILED(ec)) {
            if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
                Logger::E("NetworkPolicyManager", "problem finding system signature");
                return E_RUNTIME_EXCEPTION;
            }
            return NOERROR;
        }
        AutoPtr< ArrayOf<ISignature*> > signatures;
        packageinfo->GetSignatures((ArrayOf<ISignature*>**)&signatures);
        AutoPtr<ISets> sets;
        // TODO: Waiting for Sets
        assert(0);
        // CSets::AcquireSingleton((ISets**)&sets);
        // sets->NewHashSet(signatures, (IHashSet**)&systemSignature);
            // } catch (NameNotFoundException e) {
            // }
            // try {
        // reject apps signed with platform cert
        AutoPtr< ArrayOf<String> > uids;
        pm->GetPackagesForUid(uid, (ArrayOf<String>**)&uids);
        for (Int32 i = 0; i< uids->GetLength(); i++) {
            AutoPtr<IHashSet> packageSignature;
            CHashSet::New((IHashSet**)&packageSignature);
            AutoPtr<IPackageInfo> Uidspackageinfo;
            ECode ec = pm->GetPackageInfo((*uids)[i], IPackageManager::GET_SIGNATURES, (IPackageInfo**)&Uidspackageinfo);
            if (FAILED(ec) && ec != (ECode)E_NAME_NOT_FOUND_EXCEPTION) return ec;

            AutoPtr< ArrayOf<ISignature*> > Uidssignatures;
            packageinfo->GetSignatures((ArrayOf<ISignature*>**)&Uidssignatures);
            for (Int32 j = 0; j< Uidssignatures->GetLength(); j++) {
                packageSignature->Add((*Uidssignatures)[i]);
            }
            Boolean bol;
            packageSignature->ContainsAll(ICollection::Probe(systemSignature), &bol);
            if (bol) {
                *result = FALSE;
                return NOERROR;
            }
        }
            // } catch (NameNotFoundException e) {
            // }
    }

    //nothing found above; we can apply policy to UID
    *result = TRUE;
    return NOERROR;
}

ECode NetworkPolicyManager::DumpPolicy(
    /* [in] */ IPrintWriter* fout,
    /* [in] */ Int32 policy)
{
    fout->Print(String("["));
    if ((policy & POLICY_REJECT_METERED_BACKGROUND) != 0) {
        fout->Print(String("REJECT_METERED_BACKGROUND"));
    }
    fout->Print(String("]"));
    return NOERROR;
}

ECode NetworkPolicyManager::DumpRules(
    /* [in] */ IPrintWriter* fout,
    /* [in] */ Int32 rules)
{
    fout->Print(String("["));
    if ((rules & RULE_REJECT_METERED) != 0) {
        fout->Print(String("REJECT_METERED"));
    }
    fout->Print(String("]"));
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos

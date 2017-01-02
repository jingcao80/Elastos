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

#include "elastos/droid/server/content/SyncOperation.h"
#include <elastos/droid/os/SystemClock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IComparable;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Content {

const String SyncOperation::TAG("SyncManager");

const Int32 SyncOperation::REASON_BACKGROUND_DATA_SETTINGS_CHANGED = -1;
const Int32 SyncOperation::REASON_ACCOUNTS_UPDATED = -2;
const Int32 SyncOperation::REASON_SERVICE_CHANGED = -3;
const Int32 SyncOperation::REASON_PERIODIC = -4;
/** Sync started because it has just been set to isSyncable. */
const Int32 SyncOperation::REASON_IS_SYNCABLE = -5;
/** Sync started because it has just been set to sync automatically. */
const Int32 SyncOperation::REASON_SYNC_AUTO = -6;
/** Sync started because master sync automatically has been set to TRUE. */
const Int32 SyncOperation::REASON_MASTER_SYNC_AUTO = -7;
const Int32 SyncOperation::REASON_USER_START = -8;

const Int32 SyncOperation::SYNC_TARGET_UNKNOWN = 0;
const Int32 SyncOperation::SYNC_TARGET_ADAPTER = 1;
const Int32 SyncOperation::SYNC_TARGET_SERVICE = 2;

AutoPtr<ArrayOf<String> > InitREASON_NAMES()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(8);
    array->Set(0, String("DataSettingsChanged"));
    array->Set(0, String("AccountsUpdated"));
    array->Set(0, String("ServiceChanged"));
    array->Set(0, String("Periodic"));
    array->Set(0, String("IsSyncable"));
    array->Set(0, String("AutoSync"));
    array->Set(0, String("MasterSyncAuto"));
    array->Set(0, String("UserStart"));
    return array;
}

AutoPtr<ArrayOf<String> > SyncOperation::REASON_NAMES = InitREASON_NAMES();

CAR_INTERFACE_IMPL_2(SyncOperation, Object, ISyncOperation, IComparable)

SyncOperation::SyncOperation(
    /* [in] */ IAccount* account,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 reason,
    /* [in] */ Int32 source,
    /* [in] */ const String& provider,
    /* [in] */ IBundle* extras,
    /* [in] */ Int64 runTimeFromNow,
    /* [in] */ Int64 flexTime,
    /* [in] */ Int64 backoff,
    /* [in] */ Int64 delayUntil,
    /* [in] */ Boolean allowParallelSyncs)
    : mReason(reason)
    , mSyncSource(source)
    , mAllowParallelSyncs(FALSE)
    , mExpedited(FALSE)
    , mLatestRunTime(0)
    , mBackoff(0)
    , mDelayUntil(0)
    , mEffectiveRunTime(0)
    , mFlexTime(0)
{
    AutoPtr<EndPoint> ep = new EndPoint(account, provider, userId);
    Init(ep, reason, source, extras, runTimeFromNow,
        flexTime, backoff, delayUntil, allowParallelSyncs);
}

SyncOperation::SyncOperation(
    /* [in] */ IComponentName* service,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 reason,
    /* [in] */ Int32 source,
    /* [in] */ IBundle* extras,
    /* [in] */ Int64 runTimeFromNow,
    /* [in] */ Int64 flexTime,
    /* [in] */ Int64 backoff,
    /* [in] */ Int64 delayUntil)
{
    AutoPtr<EndPoint> ep = new EndPoint(service, userId);
    Init(ep, reason, source, extras,
        runTimeFromNow, flexTime, backoff, delayUntil, TRUE /* allowParallelSyncs */);
}

ECode SyncOperation::Init(
    /* [in] */ EndPoint* info,
    /* [in] */ Int32 reason,
    /* [in] */ Int32 source,
    /* [in] */ IBundle* extras,
    /* [in] */ Int64 runTimeFromNow,
    /* [in] */ Int64 flexTime,
    /* [in] */ Int64 backoff,
    /* [in] */ Int64 delayUntil,
    /* [in] */ Boolean allowParallelSyncs)
{
    mTarget = info;
    mReason = reason;
    mSyncSource = source;
    CBundle::New(extras, (IBundle**)&mExtras);
    CleanBundle(mExtras);
    mDelayUntil = delayUntil;
    mBackoff = backoff;
    mAllowParallelSyncs = allowParallelSyncs;
    Int64 now = SystemClock::GetElapsedRealtime();
    // Set expedited based on runTimeFromNow. The SyncManager specifies whether the op is
    // expedited (Not done solely based on bundle).
    if (runTimeFromNow < 0) {
        mExpedited = TRUE;
        // Sanity check: Will always be TRUE.
        Boolean bval;
        mExtras->GetBoolean(IContentResolver::SYNC_EXTRAS_EXPEDITED, FALSE, &bval);
        if (!bval) {
            mExtras->PutBoolean(IContentResolver::SYNC_EXTRAS_EXPEDITED, TRUE);
        }
        mLatestRunTime = now;
        mFlexTime = 0;
    }
    else {
        mExpedited = FALSE;
        mExtras->Remove(IContentResolver::SYNC_EXTRAS_EXPEDITED);
        mLatestRunTime = now + runTimeFromNow;
        mFlexTime = flexTime;
    }
    UpdateEffectiveRunTime();
    mKey = ToKey(info, mExtras);
    return NOERROR;
}

SyncOperation::SyncOperation(
    /* [in] */ SyncOperation* other,
    /* [in] */ Int64 newRunTimeFromNow)
{
    AutoPtr<IBundle> extras;
    CBundle::New(other->mExtras, (IBundle**)&extras);
    Init(other->mTarget, other->mReason, other->mSyncSource, extras,
        newRunTimeFromNow,
        0 /* In back-off so no flex */,
        other->mBackoff,
        other->mDelayUntil,
        other->mAllowParallelSyncs);
}

Boolean SyncOperation::MatchesAuthority(
    /* [in] */ SyncOperation* other)
{
    return mTarget->MatchesSpec(other->mTarget);
}

void SyncOperation::CleanBundle(
    /* [in] */ IBundle* bundle)
{
    RemoveFalseExtra(bundle, IContentResolver::SYNC_EXTRAS_UPLOAD);
    RemoveFalseExtra(bundle, IContentResolver::SYNC_EXTRAS_MANUAL);
    RemoveFalseExtra(bundle, IContentResolver::SYNC_EXTRAS_IGNORE_SETTINGS);
    RemoveFalseExtra(bundle, IContentResolver::SYNC_EXTRAS_IGNORE_BACKOFF);
    RemoveFalseExtra(bundle, IContentResolver::SYNC_EXTRAS_DO_NOT_RETRY);
    RemoveFalseExtra(bundle, IContentResolver::SYNC_EXTRAS_DISCARD_LOCAL_DELETIONS);
    RemoveFalseExtra(bundle, IContentResolver::SYNC_EXTRAS_EXPEDITED);
    RemoveFalseExtra(bundle, IContentResolver::SYNC_EXTRAS_OVERRIDE_TOO_MANY_DELETIONS);
    RemoveFalseExtra(bundle, IContentResolver::SYNC_EXTRAS_DISALLOW_METERED);
}

void SyncOperation::RemoveFalseExtra(
    /* [in] */ IBundle* bundle,
    /* [in] */ const String& extraName)
{
    Boolean bval;
    bundle->GetBoolean(extraName, FALSE, &bval);
    if (!bval) {
        bundle->Remove(extraName);
    }
}

Boolean SyncOperation::IsConflict(
    /* [in] */ SyncOperation* toRun)
{
    AutoPtr<EndPoint> other = toRun->mTarget;
    if (mTarget->mTarget_provider) {
        String t1, t2, n1, n2;
        mTarget->mAccount->GetType(&t1);
        mTarget->mAccount->GetName(&n1);
        other->mAccount->GetType(&t2);
        other->mAccount->GetName(&n2);
        return t1.Equals(t2)
            && mTarget->mProvider.Equals(other->mProvider)
            && mTarget->mUserId == other->mUserId
            && (!mAllowParallelSyncs || n1.Equals(n2));
    }

    // Ops that target a service default to allow parallel syncs, which is handled by the
    // service returning SYNC_IN_PROGRESS if they don't.
    return Object::Equals(mTarget->mService, other->mService) && !mAllowParallelSyncs;
}

ECode SyncOperation::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = Dump(NULL, TRUE);
    return NOERROR;
}

String SyncOperation::Dump(
    /* [in ]*/ IPackageManager* pm,
    /* [in] */ Boolean useOneLine)
{
    StringBuilder sb;
    if (mTarget->mTarget_provider) {
        String name, type;
        mTarget->mAccount->GetName(&name);
        mTarget->mAccount->GetType(&type);
        sb.Append(name);
        sb.Append(" u");
        sb.Append(mTarget->mUserId);
        sb.Append(" (");
        sb.Append(type);
        sb.Append(")");
        sb.Append(", ");
        sb.Append(mTarget->mProvider);
        sb.Append(", ");
    }
    else if (mTarget->mTarget_service) {
        String pkgName, clsName;
        mTarget->mService->GetPackageName(&pkgName);
        mTarget->mService->GetClassName(&clsName);
        sb.Append(pkgName);
        sb.Append(" u");
        sb.Append(mTarget->mUserId);
        sb.Append(" (");
        sb.Append(clsName);
        sb.Append(")");
        sb.Append(", ");
    }
    sb.Append((*SyncStorageEngine::SOURCES)[mSyncSource]);
    sb.Append(", currentRunTime ");
    sb.Append(mEffectiveRunTime);
    if (mExpedited) {
        sb.Append(", EXPEDITED");
    }
    sb.Append(", reason: ");
    sb.Append(ReasonToString(pm, mReason));
    if (!useOneLine) {
        AutoPtr<ISet> set;
        mExtras->GetKeySet((ISet**)&set);
        Boolean isEmpty;
        set->IsEmpty(&isEmpty);
        if (!isEmpty) {
            sb.Append("\n    ");
            ExtrasToStringBuilder(mExtras, sb);
        }
    }

    return sb.ToString();
}

String SyncOperation::ReasonToString(
    /* [in] */ IPackageManager* pm,
    /* [in] */ Int32 reason)
{
    if (reason >= 0) {
        if (pm != NULL) {
            AutoPtr<ArrayOf<String> > packages;
            pm->GetPackagesForUid(reason, (ArrayOf<String>**)&packages);
            if (packages != NULL && packages->GetLength() == 1) {
                return (*packages)[0];
            }
            String name;
            pm->GetNameForUid(reason, &name);
            if (name != NULL) {
                return name;
            }
        }

        return StringUtils::ToString(reason);
    }

    Int32 index = -reason - 1;
    if (index >= REASON_NAMES->GetLength()) {
        return StringUtils::ToString(reason);
    }

    return (*REASON_NAMES)[index];
}

Boolean SyncOperation::IsInitialization()
{
    Boolean bval;
    mExtras->GetBoolean(IContentResolver::SYNC_EXTRAS_INITIALIZE, FALSE, &bval);
    return bval;
}

Boolean SyncOperation::IsExpedited()
{
    return mExpedited;
}

Boolean SyncOperation::IgnoreBackoff()
{
    Boolean bval;
    mExtras->GetBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_BACKOFF, FALSE, &bval);
    return bval;
}

Boolean SyncOperation::IsNotAllowedOnMetered()
{
    Boolean bval;
    mExtras->GetBoolean(IContentResolver::SYNC_EXTRAS_DISALLOW_METERED, FALSE, &bval);
    return bval;
}

Boolean SyncOperation::IsManual()
{
    Boolean bval;
    mExtras->GetBoolean(IContentResolver::SYNC_EXTRAS_MANUAL, FALSE, &bval);
    return bval;
}

Boolean SyncOperation::IsIgnoreSettings()
{
    Boolean bval;
    mExtras->GetBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_SETTINGS, FALSE, &bval);
    return bval;
}

/** Changed in V3. */
String SyncOperation::ToKey(
    /* [in] */ EndPoint* info,
    /* [in] */ IBundle* extras)
{
    StringBuilder sb;
    if (info->mTarget_provider) {
        String name, type;
        info->mAccount->GetName(&name);
        info->mAccount->GetType(&type);

        sb.Append("provider: ");
        sb.Append(info->mProvider);
        sb.Append(" account {name=");
        sb.Append(name);
        sb.Append(", user=");
        sb.Append(info->mUserId);
        sb.Append(", type=");
        sb.Append(type);
        sb.Append("}");
    }
    else if (info->mTarget_service) {
        String pkgName, clsName;
        info->mService->GetPackageName(&pkgName);
        info->mService->GetClassName(&clsName);

        sb.Append("service {package=" );
        sb.Append(pkgName);
        sb.Append(" user=");
        sb.Append(info->mUserId);
        sb.Append(", class=");
        sb.Append(clsName);
        sb.Append("}");
    }
    else {
        Logger::V(TAG, "Converting SyncOperaton to key, invalid target: %s",
            TO_CSTR(info));
        return String("");
    }
    sb.Append(" extras: ");
    ExtrasToStringBuilder(extras, sb);
    return sb.ToString();
}

void SyncOperation::ExtrasToStringBuilder(
    /* [in] */ IBundle* bundle,
    /* [in] */ StringBuilder& sb)
{
    sb.Append("[");
    AutoPtr<ISet> set;
    bundle->GetKeySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj, value;
        it->GetNext((IInterface**)&obj);
        String key = Object::ToString(obj);
        sb.Append(key);
        sb.Append("=");
        bundle->Get(key, (IInterface**)&value);
        sb.Append(Object::ToString(value));
        sb.Append(" ");
    }
    sb.Append("]");
}

String SyncOperation::WakeLockName()
{
    if (mWakeLockName != NULL) {
        return mWakeLockName;
    }
    if (mTarget->mTarget_provider) {
        String name, type;
        mTarget->mAccount->GetName(&name);
        mTarget->mAccount->GetType(&type);

        StringBuilder sb;
        sb += mTarget->mProvider;
        sb += "/";
        sb += type;
        sb += "/";
        sb += name;
        mWakeLockName = sb.ToString();
        return mWakeLockName;
    }
    else if (mTarget->mTarget_service) {
        String pkgName, clsName;
        mTarget->mService->GetPackageName(&pkgName);
        mTarget->mService->GetClassName(&clsName);

        StringBuilder sb;
        sb += "/";
        sb += pkgName;
        sb += "/";
        sb += clsName;
        mWakeLockName = sb.ToString();
        return mWakeLockName;
    }

    Logger::W(TAG, "Invalid target getting wakelock name for operation - %s", mKey.string());
    return String(NULL);
}

void SyncOperation::UpdateEffectiveRunTime()
{
    using Elastos::Core::Math;
    // Regardless of whether we're in backoff or honouring a delayUntil, we still incorporate
    // the flex time provided by the developer.
    mEffectiveRunTime = IgnoreBackoff() ?
        mLatestRunTime : Math::Max(Math::Max(mLatestRunTime, mDelayUntil), mBackoff);
}

ECode SyncOperation::CompareTo(
    /* [in] */ IInterface* o,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    using Elastos::Core::Math;

    SyncOperation* other = (SyncOperation*)IComparable::Probe(o);
    if (mExpedited != other->mExpedited) {
        *result = mExpedited ? -1 : 1;
        return NOERROR;
    }
    Int64 thisIntervalStart = Math::Max(mEffectiveRunTime - mFlexTime, (Int64)0);
    Int64 otherIntervalStart = Math::Max(other->mEffectiveRunTime - other->mFlexTime, (Int64)0);
    if (thisIntervalStart < otherIntervalStart) {
        *result = -1;
    }
    else if (otherIntervalStart < thisIntervalStart) {
        *result = 1;
    }
    else {
        *result = 0;
    }
    return NOERROR;
}

// TODO: Test this to make sure that casting to object doesn't lose the type info for EventLog.
AutoPtr<ArrayOf<IInterface*> > SyncOperation::ToEventLog(
    /* [in] */ Int32 event)
{
    AutoPtr<ArrayOf<IInterface*> > logArray = ArrayOf<IInterface*>::Alloc(4);
    logArray->Set(1, CoreUtils::Convert(event).Get());
    logArray->Set(2, CoreUtils::Convert(mSyncSource).Get());
    if (mTarget->mTarget_provider) {
        logArray->Set(0, CoreUtils::Convert(mTarget->mProvider).Get());
        String name;
        mTarget->mAccount->GetName(&name);
        Int32 hash = name.GetHashCode();
        logArray->Set(3, CoreUtils::Convert(hash).Get());
    }
    else if (mTarget->mTarget_service) {
        String pkgName;
        mTarget->mService->GetPackageName(&pkgName);
        Int32 hash = Object::GetHashCode(mTarget->mService);
        logArray->Set(0, CoreUtils::Convert(pkgName).Get());
        logArray->Set(3, CoreUtils::Convert(hash).Get());
    }
    else {
        Logger::W(TAG, "sync op with invalid target: %s", mKey.string());
    }
    return logArray;
}

} // namespace Content
} // namespace Server
} // namespace Droid
} // namespace Elastos

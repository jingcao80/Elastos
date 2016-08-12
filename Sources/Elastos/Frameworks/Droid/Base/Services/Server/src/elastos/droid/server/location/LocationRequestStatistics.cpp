#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/server/location/LocationRequestStatistics.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::SystemClock;
using Elastos::Core::AutoLock;
using Elastos::Core::Math;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

//====================================
// LocationRequestStatistics::PackageProviderKey
//====================================

LocationRequestStatistics::PackageProviderKey::PackageProviderKey(
    /* [in] */ const String& packageName,
    /* [in] */ const String& providerName)
    : mPackageName(packageName)
    , mProviderName(providerName)
{
}

ECode LocationRequestStatistics::PackageProviderKey::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<PackageProviderKey> otherKey = (PackageProviderKey*)(IObject::Probe(other));
    if (otherKey == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    *result = mPackageName.Equals(otherKey->mPackageName)
            && mProviderName.Equals(otherKey->mProviderName);
    return NOERROR;
}

ECode LocationRequestStatistics::PackageProviderKey::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    AutoPtr<ICharSequence> p1, p2;
    CString::New(mPackageName, (ICharSequence**)&p1);
    CString::New(mProviderName, (ICharSequence**)&p2);
    Int32 i1, i2;
    IObject::Probe(p1)->GetHashCode(&i1);
    IObject::Probe(p2)->GetHashCode(&i2);
    *hashCode = i1 + 31 * i2;
    return NOERROR;
}

//====================================
// LocationRequestStatistics::PackageStatistics
//====================================

LocationRequestStatistics::PackageStatistics::PackageStatistics()
    : mInitialElapsedTimeMs(SystemClock::GetElapsedRealtime())
    , mNumActiveRequests(0)
    , mLastActivitationElapsedTimeMs(0L)
    , mFastestIntervalMs(Elastos::Core::Math::INT64_MAX_VALUE)
    , mSlowestIntervalMs(0)
    , mTotalDurationMs(0)
{
}

void LocationRequestStatistics::PackageStatistics::StartRequesting(
    /* [in] */ Int64 intervalMs)
{
    if (mNumActiveRequests == 0) {
        mLastActivitationElapsedTimeMs = SystemClock::GetElapsedRealtime();
    }

    if (intervalMs < mFastestIntervalMs) {
        mFastestIntervalMs = intervalMs;
    }

    if (intervalMs > mSlowestIntervalMs) {
        mSlowestIntervalMs = intervalMs;
    }

    mNumActiveRequests++;
}

void LocationRequestStatistics::PackageStatistics::StopRequesting() {
    if (mNumActiveRequests <= 0) {
        // Shouldn't be a possible code path
        Logger::E(TAG, "Reference counting corrupted in usage mStatistics.");
        return;
    }

    mNumActiveRequests--;
    if (mNumActiveRequests == 0) {
        Int64 lastDurationMs
                = SystemClock::GetElapsedRealtime() - mLastActivitationElapsedTimeMs;
        mTotalDurationMs += lastDurationMs;
    }
}

ECode LocationRequestStatistics::PackageStatistics::GetDurationMs(
    /* [out] */ Int64* ms)
{
    VALIDATE_NOT_NULL(ms)
    Int64 currentDurationMs = mTotalDurationMs;
    if (mNumActiveRequests > 0) {
        currentDurationMs
                += SystemClock::GetElapsedRealtime() - mLastActivitationElapsedTimeMs;
    }
    *ms = currentDurationMs;
    return NOERROR;
}

ECode LocationRequestStatistics::PackageStatistics::GetTimeSinceFirstRequestMs(
    /* [out] */ Int64* ms)
{
    VALIDATE_NOT_NULL(ms)
    *ms = SystemClock::GetElapsedRealtime() - mInitialElapsedTimeMs;
    return NOERROR;
}

ECode LocationRequestStatistics::PackageStatistics::GetFastestIntervalMs(
    /* [out] */ Int64* ms)
{
    VALIDATE_NOT_NULL(ms)
    *ms = mFastestIntervalMs;
    return NOERROR;
}

ECode LocationRequestStatistics::PackageStatistics::GetSlowestIntervalMs(
    /* [out] */ Int64* ms)
{
    VALIDATE_NOT_NULL(ms)
    *ms = mSlowestIntervalMs;
    return NOERROR;
}

ECode LocationRequestStatistics::PackageStatistics::IsActive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mNumActiveRequests > 0;
    return NOERROR;
}

ECode LocationRequestStatistics::PackageStatistics::ToString(
    /* [out] */ String* result)
{
    StringBuilder s;
    if (mFastestIntervalMs == mSlowestIntervalMs) {
        s += "Interval ";
        s += mFastestIntervalMs / 1000;
        s += " seconds";
    }
    else {
        s += "Min interval ";
        s += mFastestIntervalMs / 1000;
        s += " seconds";
        s += ": Max interval ";
        s += mSlowestIntervalMs / 1000;
        s += " seconds";
    }
    s += ": Duration requested ";
    Int64 ms;
    GetDurationMs(&ms);
    s += ms / 1000 / 60;
    s += " out of the last ";
    GetTimeSinceFirstRequestMs(&ms);
    s += ms / 1000 / 60;
    s += " minutes";
    Boolean isActive;
    if (IsActive(&isActive), isActive) {
        s += ": Currently active";
    }
    *result = s.ToString();
    return NOERROR;
}

//====================================
// LocationRequestStatistics
//====================================

const String LocationRequestStatistics::TAG("LocationStats");

LocationRequestStatistics::LocationRequestStatistics()
{
    CHashMap::New((IHashMap**)&mStatistics);
}

ECode LocationRequestStatistics::StartRequesting(
    /* [in] */ const String& packageName,
    /* [in] */ const String& providerName,
    /* [in] */ Int64 intervalMs)
{
    AutoPtr<PackageProviderKey> key = new PackageProviderKey(packageName, providerName);
    AutoPtr<IInterface> obj;
    mStatistics->Get((IObject*)key.Get(), (IInterface**)&obj);
    AutoPtr<PackageStatistics> stats = (PackageStatistics*)(IObject::Probe(obj));
    if (stats == NULL) {
        stats = new PackageStatistics();
        mStatistics->Put((IObject*)key.Get(), (IObject*)stats.Get());
    }
    stats->StartRequesting(intervalMs);
    return NOERROR;
}

ECode LocationRequestStatistics::StopRequesting(
    /* [in] */ const String& packageName,
    /* [in] */ const String& providerName)
{
    AutoPtr<PackageProviderKey> key = new PackageProviderKey(packageName, providerName);
    AutoPtr<IInterface> obj;
    mStatistics->Get((IObject*)key.Get(), (IInterface**)&obj);
    AutoPtr<PackageStatistics> stats = (PackageStatistics*)(IObject::Probe(obj));
    if (stats != NULL) {
        stats->StopRequesting();
    }
    else {
        // This shouldn't be a possible code path.
        Logger::E(TAG, "Couldn't find package mStatistics when removing location request.");
    }
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
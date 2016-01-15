
#include "elastos/droid/app/usage/CConfigurationStats.h"
#include "elastos/droid/content/res/CConfiguration.h"

using Elastos::Droid::Content::Res::CConfiguration;

namespace Elastos {
namespace Droid {
namespace App {
namespace Usage {

CAR_OBJECT_IMPL(CConfigurationStats)

CAR_INTERFACE_IMPL_2(CConfigurationStats, Object, IConfigurationStats, IParcelable)

CConfigurationStats::CConfigurationStats()
    : mBeginTimeStamp(0)
    , mEndTimeStamp(0)
    , mLastTimeActive(0)
    , mTotalTimeActive(0)
    , mActivationCount(0)
{}

ECode CConfigurationStats::constructor()
{
    return NOERROR;
}

ECode CConfigurationStats::constructor(
    /* [in] */ IConfigurationStats* cs)
{
    CConfigurationStats* stats = (CConfigurationStats*)cs;
    mConfiguration = stats->mConfiguration;
    mBeginTimeStamp = stats->mBeginTimeStamp;
    mEndTimeStamp = stats->mEndTimeStamp;
    mLastTimeActive = stats->mLastTimeActive;
    mTotalTimeActive = stats->mTotalTimeActive;
    mActivationCount = stats->mActivationCount;
    return NOERROR;
}

ECode CConfigurationStats::GetConfiguration(
    /* [out] */ IConfiguration** config)
{
    VALIDATE_NOT_NULL(config)
    *config = mConfiguration;
    REFCOUNT_ADD(*config)
    return NOERROR;
}

ECode CConfigurationStats::GetFirstTimeStamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBeginTimeStamp;
    return NOERROR;
}

ECode CConfigurationStats::GetLastTimeStamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEndTimeStamp;
    return NOERROR;
}

ECode CConfigurationStats::GetLastTimeActive(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mLastTimeActive;
    return NOERROR;
}

ECode CConfigurationStats::GetTotalTimeActive(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTotalTimeActive;
    return NOERROR;
}

ECode CConfigurationStats::GetActivationCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mActivationCount;
    return NOERROR;
}

ECode CConfigurationStats::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (mConfiguration != NULL) {
        dest->WriteInt32(1);
        IParcelable::Probe(mConfiguration)->WriteToParcel(dest);
    }
    else {
        dest->WriteInt32(0);
    }

    dest->WriteInt64(mBeginTimeStamp);
    dest->WriteInt64(mEndTimeStamp);
    dest->WriteInt64(mLastTimeActive);
    dest->WriteInt64(mTotalTimeActive);
    dest->WriteInt32(mActivationCount);
    return NOERROR;
}

ECode CConfigurationStats::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 ival;
    source->ReadInt32(&ival);
    if (ival != 0) {
        CConfiguration::New((IConfiguration**)&mConfiguration);
        IParcelable::Probe(mConfiguration)->ReadFromParcel(source);
    }
    source->ReadInt64(&mBeginTimeStamp);
    source->ReadInt64(&mEndTimeStamp);
    source->ReadInt64(&mLastTimeActive);
    source->ReadInt64(&mTotalTimeActive);
    source->ReadInt32(&mActivationCount);
    return NOERROR;
}

} // namespace Usage
} // namespace App
} // namespace Droid
} // namespace Elastos

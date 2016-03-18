
#include "Elastos.Droid.Accounts.h"
#include "elastos/droid/content/CSyncRequest.h"
#include "elastos/droid/content/CSyncRequestBuilder.h"
//#include "elastos/droid/accounts/CAccount.h"
#include "elastos/droid/os/CBundle.h"

//using Elastos::Droid::Accounts::CAccount;
using Elastos::Droid::Os::CBundle;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL_2(CSyncRequest, Object, ISyncRequest, IParcelable)

CAR_OBJECT_IMPL(CSyncRequest)

CSyncRequest::CSyncRequest()
    : mDisallowMetered(0)
    , mSyncFlexTimeSecs(0)
    , mSyncRunTimeSecs(0)
    , mIsPeriodic(FALSE)
    , mIsAuthority(FALSE)
    , mIsExpedited(FALSE)
{
}

ECode CSyncRequest:: constructor()
{
    return NOERROR;
}

ECode CSyncRequest:: constructor(
    /* [in] */ ISyncRequestBuilder* b)
{
    CSyncRequestBuilder* crb = (CSyncRequestBuilder*)b;

    mSyncFlexTimeSecs = crb->mSyncFlexTimeSecs;
    mSyncRunTimeSecs = crb->mSyncRunTimeSecs;
    mAccountToSync = crb->mAccount;
    mAuthority = crb->mAuthority;
    mIsPeriodic = (crb->mSyncType == CSyncRequestBuilder::SYNC_TYPE_PERIODIC);
    mIsAuthority = (crb->mSyncTarget == CSyncRequestBuilder::SYNC_TARGET_ADAPTER);
    mIsExpedited = crb->mExpedited;
    CBundle::New(crb->mCustomExtras, (IBundle**)&mExtras);
    // For now we merge the sync config extras & the custom extras into one bundle.
    // TODO: pass the configuration extras through separately.
    mExtras->PutAll(crb->mSyncConfigExtras);
    mDisallowMetered = crb->mDisallowMetered;
    return NOERROR;
}

ECode CSyncRequest:: IsPeriodic(
    /* [out] */ Boolean* periodic)
{
    VALIDATE_NOT_NULL(periodic)
    *periodic = mIsPeriodic;
    return NOERROR;
}

ECode CSyncRequest:: IsExpedited(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsExpedited;
    return NOERROR;
}

ECode CSyncRequest:: GetAccount(
    /* [out] */ IAccount** account)
{
    VALIDATE_NOT_NULL(account)
    *account = mAccountToSync;
    REFCOUNT_ADD(*account)
    return NOERROR;
}

ECode CSyncRequest:: GetProvider(
    /* [out] */ String* provider)
{
    VALIDATE_NOT_NULL(provider)
    *provider = mAuthority;
    return NOERROR;
}

ECode CSyncRequest:: GetBundle(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras)
    *extras = mExtras;
    REFCOUNT_ADD(*extras)
    return NOERROR;
}

ECode CSyncRequest:: GetSyncFlexTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = mSyncFlexTimeSecs;
    return NOERROR;
}

ECode CSyncRequest:: GetSyncRunTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = mSyncRunTimeSecs;
    return NOERROR;
}

ECode CSyncRequest:: WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->WriteInterfacePtr(TO_IINTERFACE(mExtras));
    parcel->WriteInt64(mSyncFlexTimeSecs);
    parcel->WriteInt64(mSyncRunTimeSecs);
    parcel->WriteInt32((mIsPeriodic ? 1 : 0));
    parcel->WriteInt32((mDisallowMetered ? 1 : 0));
    parcel->WriteInt32((mIsAuthority ? 1 : 0));
    parcel->WriteInt32((mIsExpedited? 1 : 0));
    parcel->WriteInterfacePtr(mAccountToSync);
    parcel->WriteString(mAuthority);
    return NOERROR;
}

ECode CSyncRequest:: ReadFromParcel(
    /* [in] */ IParcel* in)
{
    AutoPtr<IInterface> obj;
    in->ReadInterfacePtr((Handle32*)&obj);
    mExtras = IBundle::Probe(obj);
    in->ReadInt64(&mSyncFlexTimeSecs);
    in->ReadInt64(&mSyncRunTimeSecs);
    Int32 ival;
    in->ReadInt32(&ival);
    mIsPeriodic = (ival != 0);
    in->ReadInt32(&ival);
    mDisallowMetered = (ival != 0);
    in->ReadInt32(&ival);
    mIsAuthority = (ival != 0);
    in->ReadInt32(&ival);
    mIsExpedited = (ival != 0);
    AutoPtr<IInterface> account;
    in->ReadInterfacePtr((Handle32*)(IInterface**)&account);
    mAccountToSync = IAccount::Probe(account);
    in->ReadString(&mAuthority);
    return NOERROR;
}

}
}
}
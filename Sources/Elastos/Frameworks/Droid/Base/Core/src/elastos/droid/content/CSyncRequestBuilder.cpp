
#include "Elastos.Droid.Accounts.h"
#include "elastos/droid/content/CSyncRequestBuilder.h"
#include "elastos/droid/content/ContentResolver.h"
#include "elastos/droid/content/CSyncRequest.h"
#include "elastos/droid/os/CBundle.h"

using Elastos::Droid::Os::CBundle;

namespace Elastos {
namespace Droid {
namespace Content {

/** Unknown sync type. */
const Int32 CSyncRequestBuilder::SYNC_TYPE_UNKNOWN = 0;
/** Specify that this is a periodic sync. */
const Int32 CSyncRequestBuilder::SYNC_TYPE_PERIODIC = 1;
/** Specify that this is a one-time sync. */
const Int32 CSyncRequestBuilder::SYNC_TYPE_ONCE = 2;
/** Unknown sync target. */
const Int32 CSyncRequestBuilder::SYNC_TARGET_UNKNOWN = 0;
/** Specify that this is a sync with a provider. */
const Int32 CSyncRequestBuilder::SYNC_TARGET_ADAPTER = 2;

CAR_INTERFACE_IMPL(CSyncRequestBuilder, Object, ISyncRequestBuilder)

CAR_OBJECT_IMPL(CSyncRequestBuilder)

CSyncRequestBuilder::CSyncRequestBuilder()
    : mSyncFlexTimeSecs(0)
    , mSyncRunTimeSecs(0)
    , mDisallowMetered(FALSE)
    , mSyncType(CSyncRequestBuilder::SYNC_TYPE_UNKNOWN)
    , mSyncTarget(CSyncRequestBuilder::SYNC_TARGET_UNKNOWN)
    , mIsManual(FALSE)
    , mNoRetry(FALSE)
    , mIgnoreBackoff(FALSE)
    , mIgnoreSettings(FALSE)
    , mExpedited(FALSE)
{
}

ECode CSyncRequestBuilder::constructor()
{
    return NOERROR;
}

ECode CSyncRequestBuilder::SyncOnce()
{
    if (mSyncType != SYNC_TYPE_UNKNOWN) {
        // throw new IllegalArgumentException("Sync type has already been defined.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mSyncType = SYNC_TYPE_ONCE;
    SetupInterval(0, 0);
    return NOERROR;
}

ECode CSyncRequestBuilder::SyncPeriodic(
    /* [in] */ Int64 pollFrequency,
    /* [in] */ Int64 beforeSeconds)
{
    if (mSyncType != SYNC_TYPE_UNKNOWN) {
        //throw new IllegalArgumentException("Sync type has already been defined.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSyncType = SYNC_TYPE_PERIODIC;
    SetupInterval(pollFrequency, beforeSeconds);
    return NOERROR;
}

ECode CSyncRequestBuilder::SetupInterval(
    /* [in] */ Int64 at,
    /* [in] */ Int64 before)
{
    if (before > at) {
        // throw new IllegalArgumentException("Specified run time for the sync must be" +
        //     " after the specified flex time.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSyncRunTimeSecs = at;
    mSyncFlexTimeSecs = before;
return NOERROR;
}

ECode CSyncRequestBuilder::SetDisallowMetered(
    /* [in] */ Boolean disallow)
{
    if (mIgnoreSettings && disallow) {
        // throw new IllegalArgumentException("setDisallowMetered(TRUE) after having"
        //         + "specified that settings are ignored.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mDisallowMetered = disallow;
    return NOERROR;
}

ECode CSyncRequestBuilder::SetSyncAdapter(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority)
{
    if (mSyncTarget != SYNC_TARGET_UNKNOWN) {
        // throw new IllegalArgumentException("Sync target has already been defined.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (authority != NULL && authority.IsEmpty()) {
        // throw new IllegalArgumentException("Authority must be non-empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSyncTarget = SYNC_TARGET_ADAPTER;
    mAccount = account;
    mAuthority = authority;
    return NOERROR;
}

ECode CSyncRequestBuilder::SetExtras(
    /* [in] */ IBundle* bundle)
{
    mCustomExtras = bundle;
    return NOERROR;
}

ECode CSyncRequestBuilder::SetNoRetry(
    /* [in] */ Boolean noRetry)
{
    mNoRetry = noRetry;
    return NOERROR;
}

ECode CSyncRequestBuilder::SetIgnoreSettings(
    /* [in] */ Boolean ignoreSettings)
{
    if (mDisallowMetered && ignoreSettings) {
        // throw new IllegalArgumentException("setIgnoreSettings(TRUE) after having specified"
        //         + " sync settings with this builder.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mIgnoreSettings = ignoreSettings;
    return NOERROR;
}

ECode CSyncRequestBuilder::SetIgnoreBackoff(
    /* [in] */ Boolean ignoreBackoff)
{
    mIgnoreBackoff = ignoreBackoff;
    return NOERROR;
}

ECode CSyncRequestBuilder::SetManual(
    /* [in] */ Boolean isManual)
{
    mIsManual = isManual;
    return NOERROR;
}

ECode CSyncRequestBuilder::SetExpedited(
    /* [in] */ Boolean expedited)
{
    mExpedited = expedited;
    return NOERROR;
}

ECode CSyncRequestBuilder::Build(
    /* [out] */ ISyncRequest** request)
{
    // Validate the extras bundle
    FAIL_RETURN(ContentResolver::ValidateSyncExtrasBundle(mCustomExtras))

    if (mCustomExtras == NULL) {
        CBundle::New((IBundle**)&mCustomExtras);
    }
    // Combine builder extra flags into the config bundle.
    mSyncConfigExtras = NULL;
    CBundle::New((IBundle**)&mSyncConfigExtras);

    if (mIgnoreBackoff) {
        mSyncConfigExtras->PutBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_BACKOFF, TRUE);
    }
    if (mDisallowMetered) {
        mSyncConfigExtras->PutBoolean(IContentResolver::SYNC_EXTRAS_DISALLOW_METERED, TRUE);
    }
    if (mIgnoreSettings) {
        mSyncConfigExtras->PutBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_SETTINGS, TRUE);
    }
    if (mNoRetry) {
        mSyncConfigExtras->PutBoolean(IContentResolver::SYNC_EXTRAS_DO_NOT_RETRY, TRUE);
    }
    if (mExpedited) {
        mSyncConfigExtras->PutBoolean(IContentResolver::SYNC_EXTRAS_EXPEDITED, TRUE);
    }
    if (mIsManual) {
        mSyncConfigExtras->PutBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_BACKOFF, TRUE);
        mSyncConfigExtras->PutBoolean(IContentResolver::SYNC_EXTRAS_IGNORE_SETTINGS, TRUE);
    }
    if (mSyncType == SYNC_TYPE_PERIODIC) {
        // If this is a periodic sync ensure than invalid extras were not set.
        Boolean b1, b2;
        ContentResolver::InvalidPeriodicExtras(mCustomExtras, &b1);
        ContentResolver::InvalidPeriodicExtras(mSyncConfigExtras, &b2);
        if (b1 || b2) {
            // throw new IllegalArgumentException("Illegal extras were set");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    // Ensure that a target for the sync has been set.
    if (mSyncTarget == SYNC_TARGET_UNKNOWN) {
        // throw new IllegalArgumentException("Must specify an adapter with" +
        //         " setSyncAdapter(Account, String");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ISyncRequest> sr;
    CSyncRequest::New((ISyncRequest**)&sr);
    ((CSyncRequest*)sr.Get())->constructor(THIS_PROBE(ISyncRequestBuilder));
    *request = sr;
    REFCOUNT_ADD(*request);
    return NOERROR;
}

}
}
}

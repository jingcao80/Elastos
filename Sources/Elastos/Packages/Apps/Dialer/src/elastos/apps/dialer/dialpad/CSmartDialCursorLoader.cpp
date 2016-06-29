
#include "elastos/apps/dialer/dialpad/CSmartDialCursorLoader.h"
#include "elastos/apps/dialer/dialpad/SmartDialNameMatcher.h"
#include "elastos/apps/dialer/dialpad/CSmartDialNameMatcher.h"
#include "elastos/apps/dialer/dialpad/SmartDialPrefix.h"
#include "elastos/apps/dialerbind/CDatabaseHelperManager.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Database::IMatrixCursor;
using Elastos::Droid::Database::CMatrixCursor;
using Elastos::Core::CoreUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Apps::Dialer::Database::IDialerDatabaseHelper;
using Elastos::Apps::DialerBind::CDatabaseHelperManager;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Dialpad {

CAR_INTERFACE_IMPL(CSmartDialCursorLoader, AsyncTaskLoader, ISmartDialCursorLoader);

CAR_OBJECT_IMPL(CSmartDialCursorLoader);

CSmartDialCursorLoader::CSmartDialCursorLoader()
    : TAG("CSmartDialCursorLoader")
    , DEBUG(FALSE)
{}

ECode CSmartDialCursorLoader::constructor(
    /* [in] */ IContext* context)
{
    AsyncTaskLoader::constructor(context);
    mContext = context;
    return NOERROR;
}

ECode CSmartDialCursorLoader::ConfigureQuery(
    /* [in] */ const String& query)
{
    if (DEBUG) {
        Logger::V(TAG, "Configure new query to be %s", query.string());
    }
    mQuery = SmartDialNameMatcher::NormalizeNumber(query, SmartDialPrefix::GetMap());

    /** Constructs a name matcher object for matching names. */
    CSmartDialNameMatcher::New(mQuery,
            SmartDialPrefix::GetMap(), (ISmartDialNameMatcher**)&mNameMatcher);
    return NOERROR;
}

ECode CSmartDialCursorLoader::LoadInBackground(
    /* [out] */ IInterface** result)
{
    if (DEBUG) {
        Logger::V(TAG, "Load in background %s", mQuery.string());
    }

    /** Loads results from the database helper. */
    AutoPtr<IDialerDatabaseHelper> dialerDatabaseHelper =
            CDatabaseHelperManager::GetDatabaseHelper(mContext);
    AutoPtr<IArrayList> allMatches;
    dialerDatabaseHelper->GetLooseMatches(mQuery,
            mNameMatcher, (IArrayList**)&allMatches);

    if (DEBUG) {
        Int32 size;
        allMatches->GetSize(&size);
        Logger::V(TAG, "Loaded matches %d", size);
    }

    /** Constructs a cursor for the returned array of results. */
    assert(0 && "TODO");
    // AutoPtr<IPhoneNumberListAdapterPhoneQueryHelper> helper;
    // CPhoneNumberListAdapterPhoneQueryHelper::AcquireSingleton(
    //         (IPhoneNumberListAdapterPhoneQueryHelper)&helper);
    // AutoPtr<ArrayOf<String> > primary;
    // helper->GetPROJECTION_PRIMARY((ArrayOf<String>**)&primary);
    // AutoPtr<IMatrixCursor> cursor;
    // CMatrixCursor::New(primary, (IMatrixCursor**)&cursor);

    // AutoPtr<ArrayOf<IInterface*> > row = ArrayOf<IInterface*>::Alloc(primary.GetLength());
    // AutoPtr<IIterator> it;
    // allMatches->GetIterator((IIterator**)&it);
    // Boolean hasNext = FALSE;
    // while (it->HasNext(&hasNext), hasNext) {
    //     AutoPtr<IInterface> item;
    //     it->GetNext((IInterface**)&item);
    //     DialerDatabaseHelper::ContactNumber* contact =
    //             (DialerDatabaseHelper::ContactNumber*)item;
    //     row->Set(IPhoneNumberListAdapterPhoneQuery::PHONE_ID,
    //             CoreUtils::Convert(contact->mDataId));
    //     row->Set(IPhoneNumberListAdapterPhoneQuery::PHONE_NUMBER,
    //             CoreUtils::Convert(contact->mPhoneNumber));
    //     row->Set(IPhoneNumberListAdapterPhoneQuery::CONTACT_ID,
    //             CoreUtils::Convert(contact->mId));
    //     row->Set(IPhoneNumberListAdapterPhoneQuery::LOOKUP_KEY,
    //             CoreUtils::Convert(contact->mLookupKey));
    //     row->Set(IPhoneNumberListAdapterPhoneQuery::PHOTO_ID,
    //             CoreUtils::Convert(contact->mPhotoId));
    //     row->Set(IPhoneNumberListAdapterPhoneQuery::DISPLAY_NAME,
    //             CoreUtils::Convert(contact->mDisplayName));
    //     cursor->AddRow(row);
    // }
    // *result = cursor;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmartDialCursorLoader::DeliverResult(
    /* [in] */ IInterface* data)
{
    Boolean isReset;
    if (IsReset(&isReset), isReset) {
        /** The Loader has been reset; ignore the result and invalidate the data. */
        ReleaseResources(ICursor::Probe(data));
        return NOERROR;
    }

    /** Hold a reference to the old data so it doesn't get garbage collected. */
    AutoPtr<ICursor> oldCursor = mCursor;
    mCursor = ICursor::Probe(data);

    Boolean isStarted;
    if (IsStarted(&isStarted), isStarted) {
        /** If the Loader is in a started state, deliver the results to the client. */
        AsyncTaskLoader::DeliverResult(ICursor::Probe(data));
    }

    /** Invalidate the old data as we don't need it any more. */
    if (oldCursor != NULL && oldCursor != data) {
        ReleaseResources(oldCursor);
    }

    return NOERROR;
}

ECode CSmartDialCursorLoader::OnStartLoading()
{
    if (mCursor != NULL) {
        /** Deliver any previously loaded data immediately. */
        DeliverResult(mCursor);
    }
    if (mCursor == NULL) {
        /** Force loads every time as our results change with queries. */
        ForceLoad();
    }

    return NOERROR;
}

ECode CSmartDialCursorLoader::OnStopLoading()
{
    /** The Loader is in a stopped state, so we should attempt to cancel the current load. */
    Boolean succeeded;
    CancelLoad(&succeeded);
    return NOERROR;
}

ECode CSmartDialCursorLoader::OnReset()
{
    /** Ensure the loader has been stopped. */
    OnStopLoading();

    /** Release all previously saved query results. */
    if (mCursor != NULL) {
        ReleaseResources(mCursor);
        mCursor = NULL;
    }
    return NOERROR;
}

ECode CSmartDialCursorLoader::OnCanceled(
    /* [in] */ IInterface* data)
{
    ICursor* cursor = ICursor::Probe(data);
    AsyncTaskLoader::OnCanceled(cursor);

    /** The load has been canceled, so we should release the resources associated with 'data'.*/
    ReleaseResources(cursor);
    return NOERROR;
}

void CSmartDialCursorLoader::ReleaseResources(
    /* [in] */ ICursor* cursor)
{
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
}

} // Dialpad
} // Dialer
} // Apps
} // Elastos

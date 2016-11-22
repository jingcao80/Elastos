
#include "elastos/droid/database/BulkCursorToCursorAdaptor.h"
#include "elastos/droid/database/CBulkCursorDescriptor.h"
#include "elastos/droid/database/DatabaseUtils.h"
#include "elastos/droid/os/CBundle.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CBundle;
using Elastos::IO::ICloseable;
using Elastos::IO::EIID_ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Database {

const String BulkCursorToCursorAdaptor::TAG("BulkCursor");

BulkCursorToCursorAdaptor::BulkCursorToCursorAdaptor()
{
}

ECode BulkCursorToCursorAdaptor::constructor()
{
    mObserverBridge = new SelfContentObserver();
    mObserverBridge->constructor(this);
    return NOERROR;
}

ECode BulkCursorToCursorAdaptor::Initialize(
    /* [in] */ IBulkCursorDescriptor* d)
{
    CBulkCursorDescriptor* desc = (CBulkCursorDescriptor*)d;
    mBulkCursor = desc->mCursor;
    mColumns = desc->mColumnNames;
    mRowIdColumnIndex = DatabaseUtils::FindRowIdColumnIndex(mColumns);
    mWantsAllOnMoveCalls = desc->mWantsAllOnMoveCalls;
    mCount = desc->mCount;
    if (desc->mWindow != NULL) {
        SetWindow(desc->mWindow);
    }
    return NOERROR;
}

ECode BulkCursorToCursorAdaptor::GetObserver(
    /* [out] */ IIContentObserver** observer)
{
    VALIDATE_NOT_NULL(observer)
    return mObserverBridge->GetContentObserver(observer);
}

ECode BulkCursorToCursorAdaptor::ThrowIfCursorIsClosed()
{
    if (mBulkCursor == NULL) {
        //throw new StaleDataException("Attempted to access a cursor after it has been closed.");
        Logger::E(TAG, "Attempted to access a cursor after it has been closed.");
        return E_STALE_DATA_EXCEPTION;
    }
    return NOERROR;
}

ECode BulkCursorToCursorAdaptor::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    FAIL_RETURN(ThrowIfCursorIsClosed())
    *count = mCount;
    return NOERROR;
}

ECode BulkCursorToCursorAdaptor::OnMove(
    /* [in] */ Int32 oldPosition,
    /* [in] */ Int32 newPosition,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = FALSE;

    FAIL_RETURN(ThrowIfCursorIsClosed())
    //try {
    // Make sure we have the proper window
    Int32 pos, num;
    if (mWindow == NULL || (mWindow->GetStartPosition(&pos), newPosition < pos)
            || (mWindow->GetNumRows(&num), newPosition >= (pos + num))) {
        AutoPtr<ICursorWindow> win;
        ECode ec = mBulkCursor->GetWindow(newPosition, (ICursorWindow**)&win);
        if (FAILED(ec)) {
            // We tried to get a window and failed
            Logger::E(TAG, "Unable to get window because the remote process is dead");
            return ec;
        }
        SetWindow(win);
    }
    else if (mWantsAllOnMoveCalls) {
        ECode ec = mBulkCursor->OnMove(newPosition);
        if (FAILED(ec)) {
            // We tried to get a window and failed
            Logger::E(TAG, "Unable to get window because the remote process is dead");
            return NOERROR;
        }
    }

    if (mWindow == NULL) {
        return NOERROR;
    }

    *succeeded = TRUE;
    return NOERROR;
}

ECode BulkCursorToCursorAdaptor::Deactivate()
{
    // This will call onInvalidated(), so make sure to do it before calling release,
    // which is what actually makes the data set invalid.
    FAIL_RETURN(AbstractWindowedCursor::Deactivate())
    ECode ec = NOERROR;
    if(mBulkCursor != NULL) {
        //try {
        ec = mBulkCursor->Deactivate();
        //} catch (RemoteException ex) {
        if(FAILED(ec)) {
            Logger::W(TAG, "Remote process exception when deactivating");
        }
        //}
    }
    return ec;
}

ECode BulkCursorToCursorAdaptor::Close()
{
    FAIL_RETURN(AbstractWindowedCursor::Close())
    ECode ec = NOERROR;
    if (mBulkCursor != NULL) {
        //try {
        ec = mBulkCursor->Close();
        //} catch (RemoteException ex) {
        if(FAILED(ec)) {
            Logger::W(TAG, String("Remote process exception when closing"));
        }
        //} finally {
        mBulkCursor = NULL;
        //}
    }
    return ec;
}

ECode BulkCursorToCursorAdaptor::Requery(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = FALSE;

    FAIL_RETURN(ThrowIfCursorIsClosed())
    Boolean issuccess;

    //try {
    AutoPtr<IIContentObserver> observer;
    GetObserver((IIContentObserver**)&observer);
    ECode ec = mBulkCursor->Requery(observer, &mCount);
    if (FAILED(ec)) {
        Logger::E(TAG, "Unable to requery because the remote process exception , 0x%08x", ec);
        Deactivate();
        return ec;
    }
    if (mCount != -1) {
        mPos = -1;
        CloseWindow();

        // super.requery() will call onChanged. Do it here instead of relying on the
        // observer from the far side so that observers can see a correct value for mCount
        // when responding to onChanged.
        AbstractWindowedCursor::Requery(&issuccess);
        *succeeded = TRUE;
    }
    else {
        Deactivate();
        *succeeded = FALSE;
    }
    //} catch (Exception ex) {
        // Log.e(TAG, "Unable to requery because the remote process exception " + ex.getMessage());
        // deactivate();
        // return false;
    //}
    return NOERROR;
}

ECode BulkCursorToCursorAdaptor::GetColumnNames(
    /* [out] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names)
    *names = NULL;

    FAIL_RETURN(ThrowIfCursorIsClosed())
    *names = mColumns;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode BulkCursorToCursorAdaptor::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras)
    FAIL_RETURN(ThrowIfCursorIsClosed())
    //try {
    return mBulkCursor->GetExtras(extras);
    //} catch (RemoteException e) {
        // This should never happen because the system kills processes that are using remote
        // cursors when the provider process is killed.
        // throw new RuntimeException(e);
    //}
}

ECode BulkCursorToCursorAdaptor::Respond(
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** v)
{
    VALIDATE_NOT_NULL(v)
    *v = NULL;

    //try {
    ECode ec = mBulkCursor->Respond(extras, v);
    //} catch (RemoteException e) {
        // the system kills processes that are using remote cursors when the provider process
        // is killed, but this can still happen if this is being called from the system process,
        // so, better to log and return an empty bundle.
     if(FAILED(ec)) {
        Logger::W(TAG, "respond() threw RemoteException, returning an empty bundle. 0x%08x");
        *v =  CBundle::EMPTY;
        REFCOUNT_ADD(*v)
     }
    //}
    return ec;
}

} //Database
} //Droid
} //Elastos

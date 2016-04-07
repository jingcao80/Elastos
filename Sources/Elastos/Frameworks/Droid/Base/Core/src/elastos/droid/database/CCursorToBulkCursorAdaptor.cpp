
#include "elastos/droid/database/CCursorToBulkCursorAdaptor.h"
#include "elastos/droid/database/CCrossProcessCursorWrapper.h"
#include "elastos/droid/database/CCursorWindow.h"
#include "elastos/droid/os/Process.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::Process;
using Elastos::Droid::Database::Sqlite::ISQLiteClosable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {

CCursorToBulkCursorAdaptor::ContentObserverProxy::ContentObserverProxy()
{}

ECode CCursorToBulkCursorAdaptor::ContentObserverProxy::constructor(
    /* [in] */ IIContentObserver* remoteObserver,
    /* [in] */ IProxyDeathRecipient* recipient)
{
    FAIL_RETURN(ContentObserver::constructor(NULL))

    mRemote = remoteObserver;
    // try {
    AutoPtr<IProxy> proxy = (IProxy*)mRemote->Probe(EIID_IProxy);
    if (proxy != NULL) {
        proxy->LinkToDeath(recipient, 0);
    }
    // } catch (RemoteException e) {
    //     // Do nothing, the far side is dead
    // }
    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::ContentObserverProxy::UnlinkToDeath(
    /* [in] */ IProxyDeathRecipient* recipient,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IProxy> proxy = (IProxy*)mRemote->Probe(EIID_IProxy);
    assert(proxy != NULL);
    return proxy->UnlinkToDeath(recipient, 0, result);
}

ECode CCursorToBulkCursorAdaptor::ContentObserverProxy::DeliverSelfNotifications(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::ContentObserverProxy::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    // try {
    return mRemote->OnChange(selfChange, uri, Process::MyUid());
    // } catch (RemoteException ex) {
    //     // Do nothing, the far side is dead
    // }
}

const String CCursorToBulkCursorAdaptor::TAG("Cursor");

CAR_INTERFACE_IMPL_3(CCursorToBulkCursorAdaptor, Object, IBinder, IBulkCursor, IProxyDeathRecipient);

CAR_OBJECT_IMPL(CCursorToBulkCursorAdaptor)

void CCursorToBulkCursorAdaptor::CloseFilledWindowLocked()
{
    if (mFilledWindow != NULL) {
        ICloseable::Probe(mFilledWindow)->Close();
        mFilledWindow = NULL;
    }
}

void CCursorToBulkCursorAdaptor::DisposeLocked()
{
    if (mCursor != NULL) {
        UnregisterObserverProxyLocked();
        ICloseable::Probe(mCursor)->Close();
        mCursor = NULL;
    }

    CloseFilledWindowLocked();
}

ECode CCursorToBulkCursorAdaptor::ThrowIfCursorIsClosed()
{
    if (mCursor == NULL) {
        //throw new StaleDataException("Attempted to access a cursor after it has been closed.");
        Slogger::E(TAG, "Attempted to access a cursor after it has been closed.");
        return E_STALE_DATA_EXCEPTION;
    }
    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::ProxyDied()
{
    synchronized(mLock) {
        DisposeLocked();
    }
    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::GetBulkCursorDescriptor(
    /* [out] */ CBulkCursorDescriptor** result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(mLock) {
        FAIL_RETURN(ThrowIfCursorIsClosed())

        AutoPtr<CBulkCursorDescriptor> d;
        CBulkCursorDescriptor::NewByFriend((CBulkCursorDescriptor**)&d);
        d->mCursor = this;
        ICursor* cursor = ICursor::Probe(mCursor);
        cursor->GetColumnNames((ArrayOf<String>**)&d->mColumnNames);
        cursor->GetWantsAllOnMoveCalls(&d->mWantsAllOnMoveCalls);
        cursor->GetCount(&d->mCount);
        mCursor->GetWindow((ICursorWindow**)&d->mWindow);
        if (d->mWindow != NULL) {
            // Acquire a reference to the window because its reference count will be
            // decremented when it is returned as part of the binder call reply parcel.
            ISQLiteClosable::Probe(d->mWindow)->AcquireReference();
        }
        *result = d;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::GetWindow(
    /* [in] */ Int32 position,
    /* [out] */ ICursorWindow** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    synchronized(mLock) {
        FAIL_RETURN(ThrowIfCursorIsClosed())

        Boolean isSuccess;
        if (ICursor::Probe(mCursor)->MoveToPosition(position, &isSuccess), !isSuccess) {
            CloseFilledWindowLocked();
            return NOERROR;
        }

        AutoPtr<ICursorWindow> window;
        mCursor->GetWindow((ICursorWindow**)&window);
        if (window != NULL) {
            CloseFilledWindowLocked();
        }
        else {
            window = mFilledWindow;
            if (window == NULL) {
                CCursorWindow::New(mProviderName, (ICursorWindow**)&mFilledWindow);
                window = mFilledWindow;
            }
            else {
                Int32 pos, rows;
                window->GetStartPosition(&pos);
                window->GetNumRows(&rows);
                if (position < pos || position >= pos + rows) {
                    window->Clear();
                }
            }
            mCursor->FillWindow(position, window);
        }

        if (window != NULL) {
            // Acquire a reference to the window because its reference count will be
            // decremented when it is returned as part of the binder call reply parcel.
            ISQLiteClosable::Probe(window)->AcquireReference();
        }
        *result = window;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::OnMove(
    /* [in] */ Int32 position)
{
    ECode ec = NOERROR;
    synchronized(mLock) {
        FAIL_RETURN(ThrowIfCursorIsClosed())
        Int32 mPosition;
        ICursor::Probe(mCursor)->GetPosition(&mPosition);
        Boolean isSuccess;
        ec = mCursor->OnMove(mPosition, position, &isSuccess);
    }
    return ec;
}

ECode CCursorToBulkCursorAdaptor::Deactivate()
{
    synchronized(mLock) {
        if (mCursor != NULL) {
            UnregisterObserverProxyLocked();
            ICursor::Probe(mCursor)->Deactivate();
        }

        CloseFilledWindowLocked();
    }
    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::Close()
{
    synchronized(mLock) {
        DisposeLocked();
    }
    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::Requery(
    /* [in] */ IIContentObserver* observer,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    ECode ec = NOERROR;
    synchronized(mLock) {
        FAIL_RETURN(ThrowIfCursorIsClosed())

        CloseFilledWindowLocked();

        //try {
        Boolean isSuccess;
        ec = ICursor::Probe(mCursor)->Requery(&isSuccess);
        if (ec == (ECode)E_ILLEGAL_STATE_EXCEPTION) {
            Boolean isClosed;
            ICursor::Probe(mCursor)->IsClosed(&isClosed);
            Slogger::E(TAG, "%s Requery misuse db, cursor isClosed:%d, 0x%08x", mProviderName.string(), isClosed, ec);
            return ec;
        }
        if (!isSuccess) {
            *result = -1;
            return NOERROR;
        }
        //} catch (IllegalStateException e) {
            // IllegalStateException leakProgram = new IllegalStateException(
            //         mProviderName + " Requery misuse db, mCursor isClosed:" +
            //         mCursor.isClosed(), e);
            // throw leakProgram;
        //}
        UnregisterObserverProxyLocked();
        CreateAndRegisterObserverProxyLocked(observer);
        ec =  ICursor::Probe(mCursor)->GetCount(result);
    }
    return ec;
}

ECode CCursorToBulkCursorAdaptor::CreateAndRegisterObserverProxyLocked(
    /* [in] */ IIContentObserver* observer)
{
    if (mObserver != NULL) {
        //throw new IllegalStateException("an observer is already registered");
        Slogger::E(TAG, "an observer is already registered");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mObserver = new ContentObserverProxy();
    mObserver->constructor(observer, this);
    return ICursor::Probe(mCursor)->RegisterContentObserver((IContentObserver*)mObserver);
}

ECode CCursorToBulkCursorAdaptor::UnregisterObserverProxyLocked()
{
    if (mObserver != NULL) {
        ICursor::Probe(mCursor)->UnregisterContentObserver(mObserver);
        Boolean result;
        mObserver->UnlinkToDeath(this, &result);
        mObserver = NULL;
    }
    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::GetExtras(
    /* [out] */ IBundle** result)
{
    ECode ec = NOERROR;
    synchronized(mLock) {
        FAIL_RETURN(ThrowIfCursorIsClosed())
        ec = ICursor::Probe(mCursor)->GetExtras(result);
    }
    return ec;
}

ECode CCursorToBulkCursorAdaptor::Respond(
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** result)
{
    ECode ec = NOERROR;
    synchronized(mLock) {
        FAIL_RETURN(ThrowIfCursorIsClosed())
        ec = ICursor::Probe(mCursor)->Respond(extras, result);
    }
    return ec;
}

ECode CCursorToBulkCursorAdaptor::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);
    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::constructor(
    /* [in] */ ICursor* cursor,
    /* [in] */ IIContentObserver* observer,
    /* [in] */ const String& providerName)
{
    if (ICrossProcessCursor::Probe(cursor) != NULL) {
        mCursor = ICrossProcessCursor::Probe(cursor);
    }
    else {
        CCrossProcessCursorWrapper::New(cursor, (ICrossProcessCursor**)&mCursor);
    }
    mProviderName = providerName;

    ECode ec = NOERROR;
    synchronized(mLock) {
        ec = CreateAndRegisterObserverProxyLocked(observer);
    }
    return ec;
}

} //Database
} //Droid
} //Elastos

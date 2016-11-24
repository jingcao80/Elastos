
#include "elastos/droid/database/CCursorToBulkCursorAdaptor.h"
#include "elastos/droid/database/CCrossProcessCursorWrapper.h"
#include "elastos/droid/database/CCursorWindow.h"
#include "elastos/droid/os/Process.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::Process;
using Elastos::Droid::Database::Sqlite::ISQLiteClosable;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

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
    if (proxy != NULL) {
        return proxy->UnlinkToDeath(recipient, 0, result);
    }
    return NOERROR;
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

const String CCursorToBulkCursorAdaptor::TAG("CCursorToBulkCursorAdaptor");

CAR_INTERFACE_IMPL_4(CCursorToBulkCursorAdaptor, Object, \
    ICursorToBulkCursorAdaptor, IBulkCursor, \
    IBinder, IProxyDeathRecipient)

CAR_OBJECT_IMPL(CCursorToBulkCursorAdaptor)

ECode CCursorToBulkCursorAdaptor::constructor(
    /* [in] */ ICursor* cursor,
    /* [in] */ IIContentObserver* observer,
    /* [in] */ const String& providerName)
{
    mCursor = ICrossProcessCursor::Probe(cursor);
    if (mCursor == NULL) {
        CCrossProcessCursorWrapper::New(cursor, (ICrossProcessCursor**)&mCursor);
    }
    mProviderName = providerName;

    AutoLock syncLock(mLock);
    return CreateAndRegisterObserverProxyLocked(observer);
}

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
        Logger::E(TAG, "Attempted to access a cursor after it has been closed.");
        return E_STALE_DATA_EXCEPTION;
    }
    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::ProxyDied()
{
    AutoLock syncLock(mLock);
    DisposeLocked();

    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::GetBulkCursorDescriptor(
    /* [out] */ IBulkCursorDescriptor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoLock syncLock(mLock);
    FAIL_RETURN(ThrowIfCursorIsClosed())

    AutoPtr<IBulkCursorDescriptor> d;
    CBulkCursorDescriptor::New((IBulkCursorDescriptor**)&d);
    d->SetCursor(this);

    ICursor* cursor = ICursor::Probe(mCursor);
    AutoPtr< ArrayOf<String> > names;
    cursor->GetColumnNames((ArrayOf<String>**)&names);
    d->SetColumnNames(names);

    Boolean wants;
    cursor->GetWantsAllOnMoveCalls(&wants);
    d->SetWantsAllOnMoveCalls(wants);

    Int32 count;
    cursor->GetCount(&count);
    d->SetCount(count);

    AutoPtr<ICursorWindow> window;
    mCursor->GetWindow((ICursorWindow**)&window);
    d->SetWindow(window);
    if (window != NULL) {
        // Acquire a reference to the window because its reference count will be
        // decremented when it is returned as part of the binder call reply parcel.
        ISQLiteClosable::Probe(window)->AcquireReference();
    }

    *result = d;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::GetWindow(
    /* [in] */ Int32 position,
    /* [out] */ ICursorWindow** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoLock syncLock(mLock);
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

    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::OnMove(
    /* [in] */ Int32 position)
{
    ECode ec = NOERROR;
    AutoLock syncLock(mLock);
    FAIL_RETURN(ThrowIfCursorIsClosed())
    Int32 mPosition;
    ICursor::Probe(mCursor)->GetPosition(&mPosition);
    Boolean isSuccess;
    return mCursor->OnMove(mPosition, position, &isSuccess);
}

ECode CCursorToBulkCursorAdaptor::Deactivate()
{
    AutoLock syncLock(mLock);
    if (mCursor != NULL) {
        UnregisterObserverProxyLocked();
        ICursor::Probe(mCursor)->Deactivate();
    }

    CloseFilledWindowLocked();
    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::Close()
{
    AutoLock syncLock(mLock);
    DisposeLocked();
    return NOERROR;
}

ECode CCursorToBulkCursorAdaptor::Requery(
    /* [in] */ IIContentObserver* observer,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    ECode ec = NOERROR;
    {    AutoLock syncLock(mLock);
        FAIL_RETURN(ThrowIfCursorIsClosed())

        CloseFilledWindowLocked();

        //try {
        Boolean isSuccess;
        ec = ICursor::Probe(mCursor)->Requery(&isSuccess);
        if (ec == (ECode)E_ILLEGAL_STATE_EXCEPTION) {
            Boolean isClosed;
            ICursor::Probe(mCursor)->IsClosed(&isClosed);
            Logger::E(TAG, "%s Requery misuse db, cursor isClosed:%d, 0x%08x", mProviderName.string(), isClosed, ec);
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
        Logger::E(TAG, "an observer is already registered");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mObserver = new ContentObserverProxy();
    mObserver->constructor(observer, this);
    return ICursor::Probe(mCursor)->RegisterContentObserver(mObserver);
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
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    AutoLock syncLock(mLock);
    FAIL_RETURN(ThrowIfCursorIsClosed())
    return ICursor::Probe(mCursor)->GetExtras(result);
}

ECode CCursorToBulkCursorAdaptor::Respond(
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoLock syncLock(mLock);
    FAIL_RETURN(ThrowIfCursorIsClosed())
    return ICursor::Probe(mCursor)->Respond(extras, result);
}


} //Database
} //Droid
} //Elastos

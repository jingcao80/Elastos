
#include "elastos/droid/app/backup/RestoreSession.h"
#include "elastos/droid/app/backup/CRestoreObserverWrapper.h"


namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

RestoreSession::RestoreSession(
    /* [in] */ IContext* context,
    /* [in] */ IRestoreSession* binder):
    mContext(context),
    mBinder(binder)
{
}

RestoreSession::~RestoreSession()
{

}

UInt32 RestoreSession::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 RestoreSession::Release()
{
    return ElRefBase::Release();
}

ECode RestoreSession::GetAvailableRestoreSets(
    /* [in] */ IRestoreObserver* observer,
    /* [out] */ Int32* error)
{
    assert(error != NULL);
    *error = -1;

    AutoPtr<IRestoreObserver> obsWrapper;
    CRestoreObserverWrapper::New(mContext, observer, (IRestoreObserver**)&obsWrapper);
    assert(obsWrapper != NULL);

//    try {
    mBinder->GetAvailableRestoreSets(obsWrapper, error);
    // } catch (RemoteException e) {
    //     Log.d(TAG, "Can't contact server to get available sets");
    // }
    return NOERROR;
}

ECode RestoreSession::RestoreAll(
    /* [in] */ Int64 token,
    /* [in] */ IRestoreObserver* observer,
    /* [out] */ Int32* error)
{
    assert(error != NULL);
    *error = -1;

    if (mObserver != NULL) {
        //Log.d(TAG, "restoreAll() called during active restore");
        return NOERROR;
    }

    CRestoreObserverWrapper::New(mContext, observer, (IRestoreObserver**)&mObserver);
    assert(mObserver != NULL);

    //try {
        mBinder->RestoreAll(token, mObserver, error);
    // } catch (RemoteException e) {
    //     Log.d(TAG, "Can't contact server to restore");
    // }

    return NOERROR;
}

ECode RestoreSession::RestoreSome(
    /* [in] */ Int64 token,
    /* [in] */ IRestoreObserver* observer,
    /* [in] */ ArrayOf<String> packages,
    /* [out] */ Int32* success)
{
    Int32 err = -1;
    if (mObserver != NULL) {
        //Log.d(TAG, "restoreAll() called during active restore");
        *success = -1;
        return NOERROR;
    }
    CRestoreObserverWrapper::New(mContext, observer, (IRestoreObserver**)&mObserver);
    //try {
    return mBinder->RestoreSome(token, mObserver, packages, success);
    //} catch (RemoteException e) {
    //    Log.d(TAG, "Can't contact server to restore packages");
    //}
}

ECode RestoreSession::RestorePackage(
    /* [in] */ const String& package,
    /* [in] */ IRestoreObserver* observer,
    /* [out] */ Int32* error)
{
    assert(error != NULL);
    *error = -1;

    if (mObserver != NULL) {
        //Log.d(TAG, "restorePackage() called during active restore");
        return NOERROR;
    }

    CRestoreObserverWrapper::New(mContext, observer, (IRestoreObserver**)&mObserver);
    assert(mObserver != NULL);

    //try {
        mBinder->RestorePackage(package, mObserver, error);
    // } catch (RemoteException e) {
    //     Log.d(TAG, "Can't contact server to restore package");
    // }

    return NOERROR;
}

ECode RestoreSession::EndRestoreSession()
{
    // try {
    //     mBinder.endRestoreSession();
    // } catch (RemoteException e) {
    //     Log.d(TAG, "Can't contact server to get available sets");
    // } finally {
    //     mBinder = NULL;
    // }

    mBinder->EndRestoreSession();
    mBinder = NULL;
    return NOERROR;
}

}
}
}
}
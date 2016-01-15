#include "elastos/droid/app/backup/CRestoreSession.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/app/backup/CRestoreObserverWrapper.h"

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::App::Backup::CRestoreObserverWrapper;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

const String CRestoreSession::TAG("CRestoreSession");

CAR_INTERFACE_IMPL(CRestoreSession, Object, IRestoreSession)

CAR_OBJECT_IMPL(CRestoreSession)

ECode CRestoreSession::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIRestoreSession* binder)
{
    mContext = context;
    mBinder = binder;

    return NOERROR;
}

ECode CRestoreSession::GetAvailableRestoreSets(
    /* [in] */ IRestoreObserver* observer,
    /* [out] */ Int32* error)
{
    VALIDATE_NOT_NULL(error);
    Int32 err = -1;
    AutoPtr<IIRestoreObserver> obs;
    CRestoreObserverWrapper::New(mContext, observer, (IIRestoreObserver**)&obs);
    //try{
    ECode ec = mBinder->GetAvailableRestoreSets(obs, &err);
    if(FAILED(ec)) {
        Logger::D(TAG, "Can't contact server to get available sets");
    }
    //} catch (RemoteException e) {
    //Log.d(TAG, "Can't contact server to get available sets");
    //}
    *error = err;
    return ec;
}

ECode CRestoreSession::RestoreAll(
    /* [in] */ Int64 token,
    /* [in] */ IRestoreObserver* observer,
    /* [out] */ Int32* error)
{
    VALIDATE_NOT_NULL(error);
    Int32 err = -1;
    if (mObserver != NULL) {
        Logger::D(TAG, "restoreAll() called during active restore");
        *error = err;
        return NOERROR;
    }
    CRestoreObserverWrapper::New(mContext, observer, (IIRestoreObserver**)&mObserver);
    //try{
    ECode ec = mBinder->RestoreAll(token, mObserver, &err);
    if(FAILED(ec)) {
        Logger::D(TAG, "Can't contact server to get available sets");
    }
    //} catch (RemoteException e) {
    //Log.d(TAG, "Can't contact server to get available sets");
    //}
    *error = err;
    return ec;
}

ECode CRestoreSession::RestoreSome(
    /* [in] */ Int64 token,
    /* [in] */ IRestoreObserver* observer,
    /* [in] */ ArrayOf<String>* packages,
    /* [out] */ Int32* success)
{
    VALIDATE_NOT_NULL(success);
    Int32 err = -1;
    if (mObserver != NULL) {
        Logger::D(TAG, "restoreAll() called during active restore");
        *success = err;
        return NOERROR;
    }
    CRestoreObserverWrapper::New(mContext, observer, (IIRestoreObserver**)&mObserver);
    //try{
    ECode ec = mBinder->RestoreSome(token, mObserver, *packages, &err);
    if(FAILED(ec)) {
        Logger::D(TAG, "Can't contact server to get available sets");
    }
    //} catch (RemoteException e) {
    //Log.d(TAG, "Can't contact server to get available sets");
    //}
    *success = err;
    return ec;
}

ECode CRestoreSession::RestorePackage(
    /* [in] */ const String& package,
    /* [in] */ IRestoreObserver* observer,
    /* [out] */ Int32* error)
{
    VALIDATE_NOT_NULL(error);
    Int32 err = -1;
    if (mObserver != NULL) {
        Logger::D(TAG, "restoreAll() called during active restore");
        *error = err;
        return NOERROR;
    }
    CRestoreObserverWrapper::New(mContext, observer, (IIRestoreObserver**)&mObserver);
    //try{
    ECode ec = mBinder->RestorePackage(package, mObserver, &err);
    if(FAILED(ec)) {
        Logger::D(TAG, "Can't contact server to get available sets");
    }
    //} catch (RemoteException e) {
    //Log.d(TAG, "Can't contact server to get available sets");
    //}
    *error = err;
    return ec;
}

ECode CRestoreSession::EndRestoreSession()
{
    //try{
    ECode ec = mBinder->EndRestoreSession();
    mBinder = NULL;
    if(FAILED(ec)) {
        Logger::D(TAG, "Can't contact server to get available sets");
    }
    //} catch (RemoteException e) {
    //Log.d(TAG, "Can't contact server to get available sets");
    //}

    return ec;
}

}
}
}
}


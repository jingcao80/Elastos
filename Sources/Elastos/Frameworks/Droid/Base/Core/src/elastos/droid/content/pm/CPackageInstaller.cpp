
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/content/pm/CPackageInstaller.h"
#include "elastos/droid/content/pm/CPackageInstallerSession.h"
#include "elastos/droid/os/CHandler.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

//=============================================================
// CPackageInstaller::SessionCallbackDelegate
//=============================================================
const Int32 CPackageInstaller::SessionCallbackDelegate::MSG_SESSION_CREATED = 1;
const Int32 CPackageInstaller::SessionCallbackDelegate::MSG_SESSION_BADGING_CHANGED = 2;
const Int32 CPackageInstaller::SessionCallbackDelegate::MSG_SESSION_ACTIVE_CHANGED = 3;
const Int32 CPackageInstaller::SessionCallbackDelegate::MSG_SESSION_PROGRESS_CHANGED = 4;
const Int32 CPackageInstaller::SessionCallbackDelegate::MSG_SESSION_FINISHED = 5;

CAR_INTERFACE_IMPL_3(CPackageInstaller::SessionCallbackDelegate, Object, IPackageInstallerCallback, IHandlerCallback, IBinder)

CPackageInstaller::SessionCallbackDelegate::SessionCallbackDelegate(
    /* [in] */ IPackageInstallerSessionCallback* callback,
    /* [in] */ ILooper* looper)
    : mCallback(callback)
{
    CHandler::New(looper, this, FALSE, (IHandler**)&mHandler);
}

ECode CPackageInstaller::SessionCallbackDelegate::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    Int32 sessionId, what;
    msg->GetArg1(&sessionId);
    msg->GetWhat(&what);
    switch (what) {
        case MSG_SESSION_CREATED: {
            mCallback->OnCreated(sessionId);
            *result = TRUE;
            return NOERROR;
        }
        case MSG_SESSION_BADGING_CHANGED: {
            mCallback->OnBadgingChanged(sessionId);
            *result = TRUE;
            return NOERROR;
        }
        case MSG_SESSION_ACTIVE_CHANGED: {
            Int32 arg2;
            msg->GetArg2(&arg2);
            Boolean active = arg2 != 0;
            mCallback->OnActiveChanged(sessionId, active);
            *result = TRUE;
            return NOERROR;
        }
        case MSG_SESSION_PROGRESS_CHANGED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            Float fv;
            IFloat::Probe(obj)->GetValue(&fv);
            mCallback->OnProgressChanged(sessionId, fv);
            *result = TRUE;
            return NOERROR;
        }
        case MSG_SESSION_FINISHED: {
            Int32 arg2;
            msg->GetArg2(&arg2);
            mCallback->OnFinished(sessionId, arg2 != 0);
            *result = TRUE;
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CPackageInstaller::SessionCallbackDelegate::OnSessionCreated(
    /* [in] */ Int32 sessionId)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_SESSION_CREATED, sessionId, 0, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode CPackageInstaller::SessionCallbackDelegate::OnSessionBadgingChanged(
    /* [in] */ Int32 sessionId)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_SESSION_BADGING_CHANGED, sessionId, 0, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode CPackageInstaller::SessionCallbackDelegate::OnSessionActiveChanged(
    /* [in] */ Int32 sessionId,
    /* [in] */ Boolean active)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_SESSION_ACTIVE_CHANGED, sessionId, active ? 1 : 0, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode CPackageInstaller::SessionCallbackDelegate::OnSessionProgressChanged(
    /* [in] */ Int32 sessionId,
    /* [in] */ Float progress)
{
    AutoPtr<IFloat> fo;
    CFloat::New(progress, (IFloat**)&fo);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_SESSION_PROGRESS_CHANGED, sessionId, 0, TO_IINTERFACE(fo), (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode CPackageInstaller::SessionCallbackDelegate::OnSessionFinished(
    /* [in] */ Int32 sessionId,
    /* [in] */ Boolean success)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_SESSION_FINISHED, sessionId, success ? 1 : 0, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode CPackageInstaller::SessionCallbackDelegate::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("CPackageInstaller::SessionCallbackDelegate");
    return NOERROR;
}

//=============================================================
// CPackageInstaller
//=============================================================
const String CPackageInstaller::TAG("CPackageInstaller");

CAR_INTERFACE_IMPL(CPackageInstaller, Object, IPackageInstaller)

CAR_OBJECT_IMPL(CPackageInstaller)

CPackageInstaller::CPackageInstaller()
    : mUserId(0)
{}

CPackageInstaller::~CPackageInstaller()
{}

ECode CPackageInstaller::constructor()
{
    return NOERROR;
}

ECode CPackageInstaller::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IPackageManager* pm,
    /* [in] */ IIPackageInstaller* installer,
    /* [in] */ const String& installerPackageName,
    /* [in] */ Int32 userId)
{
    mContext = context;
    mPm = pm;
    mInstaller = installer;
    mInstallerPackageName = installerPackageName;
    mUserId = userId;
    return NOERROR;
}

ECode CPackageInstaller::CreateSession(
    /* [in] */ IPackageInstallerSessionParams* params,
    /* [out] */ Int32* sessionId)
{
    // try {
    return mInstaller->CreateSession(params, mInstallerPackageName, mUserId, sessionId);
    // } catch (RuntimeException e) {
    //     ExceptionUtils.maybeUnwrapIOException(e);
    //     throw e;
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
    return NOERROR;
}

ECode CPackageInstaller::OpenSession(
    /* [in] */ Int32 sessionId,
    /* [out] */ IPackageInstallerSession** session)
{
    VALIDATE_NOT_NULL(session)
    // try {
    AutoPtr<IIPackageInstallerSession> ss;
    mInstaller->OpenSession(sessionId, (IIPackageInstallerSession**)&ss);
    CPackageInstallerSession::New(session);
    return ((CPackageInstallerSession*)(*session))->constructor(ss);
    // } catch (RuntimeException e) {
    //     ExceptionUtils.maybeUnwrapIOException(e);
    //     throw e;
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
}

ECode CPackageInstaller::UpdateSessionAppIcon(
    /* [in] */ Int32 sessionId,
    /* [in] */ IBitmap* appIcon)
{
    // try {
    return mInstaller->UpdateSessionAppIcon(sessionId, appIcon);
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
}

ECode CPackageInstaller::UpdateSessionAppLabel(
    /* [in] */ Int32 sessionId,
    /* [in] */ ICharSequence* appLabel)
{
    // try {
        String val = (appLabel != NULL) ? Object::ToString(appLabel) : String(NULL);
        return mInstaller->UpdateSessionAppLabel(sessionId, val);
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
}

ECode CPackageInstaller::AbandonSession(
    /* [in] */ Int32 sessionId)
{
    // try {
    return mInstaller->AbandonSession(sessionId);
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
}

ECode CPackageInstaller::GetSessionInfo(
    /* [in] */ Int32 sessionId,
    /* [out] */ IPackageInstallerSessionInfo** session)
{
    // try {
    return mInstaller->GetSessionInfo(sessionId, session);
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
    return NOERROR;
}

ECode CPackageInstaller::GetAllSessions(
    /* [out] */ IList** sessions)
{
    VALIDATE_NOT_NULL(sessions)
    AutoPtr<IApplicationInfo> info;
    mContext->GetApplicationInfo((IApplicationInfo**)&info);
    String packageName;
    IPackageItemInfo::Probe(info)->GetPackageName(&packageName);
    Int32 versionCode;
    info->GetVersionCode(&versionCode);
    if (packageName.Equals("com.google.android.googlequicksearchbox")
            && versionCode <= 300400110) {
        Logger::D(TAG, "Ignoring callback request from old prebuilt");

        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        return collections->GetEmptyList(sessions);
    }

    // try {

    AutoPtr<IParceledListSlice> pls;
    mInstaller->GetAllSessions(mUserId, (IParceledListSlice**)&pls);
    return pls->GetList(sessions);
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
}

ECode CPackageInstaller::GetMySessions(
    /* [out] */ IList** sessions)
{
    VALIDATE_NOT_NULL(sessions)
    // try {
    AutoPtr<IParceledListSlice> pls;
    mInstaller->GetMySessions(mInstallerPackageName, mUserId, (IParceledListSlice**)&pls);
    return pls->GetList(sessions);
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
}

ECode CPackageInstaller::Uninstall(
    /* [in] */ const String& packageName,
    /* [in] */ IIntentSender* statusReceiver)
{
    // try {
    return mInstaller->Uninstall(packageName, 0, statusReceiver, mUserId);
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
}

ECode CPackageInstaller::SetPermissionsResult(
    /* [in] */ Int32 sessionId,
    /* [in] */ Boolean accepted)
{
    // try {
    return mInstaller->SetPermissionsResult(sessionId, accepted);
    // } catch (RemoteException e) {
    //     throw e.rethrowAsRuntimeException();
    // }
}

ECode CPackageInstaller::AddSessionCallback(
    /* [in] */ IPackageInstallerSessionCallback* callback)
{
    return RegisterSessionCallback(callback);
}

ECode CPackageInstaller::RegisterSessionCallback(
    /* [in] */ IPackageInstallerSessionCallback* callback)
{
    AutoPtr<IHandler> h;
    CHandler::New((IHandler**)&h);
    return RegisterSessionCallback(callback, h);
}

ECode CPackageInstaller::AddSessionCallback(
    /* [in] */ IPackageInstallerSessionCallback* callback,
    /* [in] */ IHandler* handler)
{
    return RegisterSessionCallback(callback, handler);
}

ECode CPackageInstaller::RegisterSessionCallback(
    /* [in] */ IPackageInstallerSessionCallback* callback,
    /* [in] */ IHandler* handler)
{
    // TODO: remove this temporary guard once we have new prebuilts
    AutoPtr<IApplicationInfo> info;
    mContext->GetApplicationInfo((IApplicationInfo**)&info);
    String packageName;
    IPackageItemInfo::Probe(info)->GetPackageName(&packageName);
    Int32 versionCode;
    info->GetVersionCode(&versionCode);
    if (packageName.Equals("com.google.android.googlequicksearchbox")
            && versionCode <= 300400110) {
        Logger::D(TAG, "Ignoring callback request from old prebuilt");

        return NOERROR;
    }

    ISynchronize* sync = ISynchronize::Probe(mDelegates);
    {    AutoLock syncLock(sync);
        AutoPtr<ILooper> looper;
        handler->GetLooper((ILooper**)&looper);
        AutoPtr<SessionCallbackDelegate> delegate = new SessionCallbackDelegate(callback, looper);
        // try {
            mInstaller->RegisterCallback(IPackageInstallerCallback::Probe(delegate), mUserId);
        // } catch (RemoteException e) {
        //     throw e.rethrowAsRuntimeException();
        // }
        mDelegates->Add(TO_IINTERFACE(delegate));
    }
    return NOERROR;
}

ECode CPackageInstaller::RemoveSessionCallback(
    /* [in] */ IPackageInstallerSessionCallback* callback)
{
    return UnregisterSessionCallback(callback);
}

ECode CPackageInstaller::UnregisterSessionCallback(
    /* [in] */ IPackageInstallerSessionCallback* callback)
{
    ISynchronize* sync = ISynchronize::Probe(mDelegates);
    {    AutoLock syncLock(sync);
        AutoPtr<IIterator> it;
        mDelegates->GetIterator((IIterator**)&it);
        SessionCallbackDelegate* delegate;
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            delegate = (SessionCallbackDelegate*)IPackageInstallerCallback::Probe(obj);
            if (delegate->mCallback.Get() == callback) {
                // try {
                    mInstaller->UnregisterCallback(delegate);
                // } catch (RemoteException e) {
                //     throw e.rethrowAsRuntimeException();
                // }
                it->Remove();
            }
        }
    }
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
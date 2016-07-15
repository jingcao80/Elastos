
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/app/NotificationManager.h"
#include "elastos/droid/app/CNotificationBuilder.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/CStrictMode.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CStrictMode;
using Elastos::Droid::Os::IStrictMode;
using Elastos::Core::ICloneable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace App {

const String NotificationManager::TAG("NotificationManager");
const Boolean NotificationManager::DEBUG = FALSE;
const Boolean NotificationManager::localLOGV = FALSE;

AutoPtr<IINotificationManager> NotificationManager::sService;

CAR_INTERFACE_IMPL(NotificationManager, Object, INotificationManager);

NotificationManager::NotificationManager()
{
}

NotificationManager::~NotificationManager()
{}

/** @hide */
AutoPtr<IINotificationManager> NotificationManager::GetService()
{
    if (sService != NULL) {
        return sService;
    }

    sService = IINotificationManager::Probe(
            Elastos::Droid::Os::ServiceManager::GetService(String("notification")).Get());
    return sService;
}

AutoPtr<INotificationManager> NotificationManager::From(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
    AutoPtr<INotificationManager> mgr = INotificationManager::Probe(obj.Get());
    return mgr;
}

ECode NotificationManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler)
{
    mContext = context;
    return NOERROR;
}

ECode NotificationManager::Notify(
    /* [in] */ Int32 id,
    /* [in] */ INotification* notification)
{
    return Notify(String(NULL), id, notification);
}

ECode NotificationManager::Notify(
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ INotification* notification)
{
    VALIDATE_NOT_NULL(notification);

    AutoPtr<ArrayOf<Int32> > idOut = ArrayOf<Int32>::Alloc(1);
    AutoPtr<IINotificationManager> service = GetService();
    String pkgName;
    mContext->GetPackageName(&pkgName);

    AutoPtr<IUri> sound;
    notification->GetSound((IUri**)&sound);
    if (sound != NULL) {
        AutoPtr<IUri> newSound;
        sound->GetCanonicalUri((IUri**)&newSound);
        notification->SetSound(newSound);
        assert(0 && "TODO");
        // if (StrictMode.vmFileUriExposureEnabled()) {
        //     newSound->CheckFileUriExposed("Notification.sound");
        // }
    }

    if (localLOGV) {
        String notificationStr = Object::ToString(notification);
        Slogger::V(TAG, "%s: notify(%d, %s)", pkgName.string(), id, notificationStr.string());
    }

    AutoPtr<IInterface> tmp;
    ICloneable::Probe(notification)->Clone((IInterface**)&tmp);
    AutoPtr<INotification> stripped = INotification::Probe(tmp);
    CNotificationBuilder::StripForDelivery(stripped);

    // try {
    ECode ec;
    do {
        Int32 userId = UserHandle::GetMyUserId();
        AutoPtr<ArrayOf<Int32> > idOutTmp;
        ec = service->EnqueueNotificationWithTag(
            pkgName, pkgName, tag, id, stripped, idOut, userId, (ArrayOf<Int32>**)&idOutTmp);
        idOut = idOutTmp;
        if (FAILED(ec)) break;
        if (idOut == NULL || idOut->GetLength() == 0 || id != (*idOut)[0]) {
            Slogger::W(TAG, "notify: id corrupted: sent %d, got back %d",
                id, (idOut && idOut->GetLength() > 0) ? (*idOut)[0] : -1);
        }
    } while(FALSE);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            ec = NOERROR;
        }
    }
    // }
    return ec;
}

ECode NotificationManager::NotifyAsUser(
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ INotification* notification,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<ArrayOf<Int32> > idOut = ArrayOf<Int32>::Alloc(1);
    AutoPtr<IINotificationManager> service = GetService();
    String pkgName;
    mContext->GetPackageName(&pkgName);
    AutoPtr<IUri> sound;
    notification->GetSound((IUri**)&sound);
    if (sound != NULL) {
        AutoPtr<IUri> newSound;
        sound->GetCanonicalUri((IUri**)&newSound);
        notification->SetSound(newSound);
        AutoPtr<IStrictMode> sm;
        CStrictMode::AcquireSingleton((IStrictMode**)&sm);
        Boolean bval;
        if (sm->VmFileUriExposureEnabled(&bval), bval) {
            newSound->CheckFileUriExposed(String("Notification.sound"));
        }
    }

    if (localLOGV) {
        String notificationStr = Object::ToString(notification);
        Slogger::V(TAG, "%s: notify(%d, %s)", pkgName.string(), id, notificationStr.string());
    }

    AutoPtr<IInterface> tmp;
    ICloneable::Probe(notification)->Clone((IInterface**)&tmp);
    AutoPtr<INotification> stripped = INotification::Probe(tmp);
    CNotificationBuilder::StripForDelivery(stripped);

    Int32 identifier;
    user->GetIdentifier(&identifier);

    AutoPtr<ArrayOf<Int32> > idOutTmp;
    ECode ec = service->EnqueueNotificationWithTag(
        pkgName, pkgName, tag, id, stripped, idOut, identifier, (ArrayOf<Int32>**)&idOutTmp);
    idOut = idOutTmp;
    if (FAILED(ec)) return ec;
    if (idOut == NULL || idOut->GetLength() == 0 || id != (*idOut)[0]) {
        Slogger::W(TAG, "notify: id corrupted: sent %d, got back %d",
            id, (idOut && idOut->GetLength() > 0) ? (*idOut)[0] : -1);
    }
    return NOERROR;
}

ECode NotificationManager::Cancel(
    /* [in] */ Int32 id)
{
    return Cancel(String(NULL), id);
}

ECode NotificationManager::Cancel(
    /* [in] */ const String& tag,
    /* [in] */ Int32 id)
{
    AutoPtr<IINotificationManager> service = GetService();
    String cap;
    mContext->GetPackageName(&cap);
//    if (localLOGV) Log.v(TAG, pkg + ": cancel(" + id + ")");
    Int32 userId = UserHandle::GetMyUserId();
    return service->CancelNotificationWithTag(cap, tag, id, userId);
}

ECode NotificationManager::CancelAsUser(
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<IINotificationManager> service = GetService();
    String cap;
    mContext->GetPackageName(&cap);
//    if (localLOGV) Log.v(TAG, pkg + ": cancel(" + id + ")");
    Int32 identifier;
    user->GetIdentifier(&identifier);
    return service->CancelNotificationWithTag(cap, tag, id, identifier);
}

ECode NotificationManager::CancelAll()
{
    AutoPtr<IINotificationManager> service = GetService();
    String cap;
    mContext->GetPackageName(&cap);
//    if (localLOGV) Log.v(TAG, pkg + ": cancelAll()");
    Int32 userId = UserHandle::GetMyUserId();
    return service->CancelAllNotifications(cap, userId);
}

ECode NotificationManager::GetEffectsSuppressor(
    /* [out] */ IComponentName** cn)
{
    VALIDATE_NOT_NULL(cn)
    AutoPtr<IINotificationManager> service = GetService();
    return service->GetEffectsSuppressor(cn);
}

ECode NotificationManager::MatchesCallFilter(
    /* [in] */ IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IINotificationManager> service = GetService();
    return service->MatchesCallFilter(extras, result);
}

ECode NotificationManager::GetShowNotificationForPackageOnKeyguard(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IINotificationManager> service = GetService();
    // try {
    ECode ec = service->GetShowNotificationForPackageOnKeyguard(pkg, uid, result);
    if (FAILED(ec)) {
        *result = INotification::SHOW_ALL_NOTI_ON_KEYGUARD;
    }
    return NOERROR;
}

}
}
}

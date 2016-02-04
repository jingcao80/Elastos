#ifndef __ELASTOS_DROID_SERVICE_NOTIFICATION_STATUSBARNOTIFICATION_H__
#define __ELASTOS_DROID_SERVICE_NOTIFICATION_STATUSBARNOTIFICATION_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::INotification;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Core::ICloneable;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

/**
 * Class encapsulating a Notification. Sent by the NotificationManagerService to clients including
 * the status bar and any {@link android.service.notification.NotificationListenerService}s.
 */
class StatusBarNotification
    : public Object
    , public IStatusBarNotification
    , public IParcelable
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    StatusBarNotification();

    /** @hide */
    CARAPI constructor(
        /* [in] */ const String& pkg,
        /* [in] */ const String& opPkg,
        /* [in] */ Int32 id,
        /* [in] */ const String& tag,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 initialPid,
        /* [in] */ Int32 score,
        /* [in] */ INotification* notification,
        /* [in] */ IUserHandle* user);

    CARAPI constructor(
        /* [in] */ const String& pkg,
        /* [in] */ const String& opPkg,
        /* [in] */ Int32 id,
        /* [in] */ const String& tag,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 initialPid,
        /* [in] */ Int32 score,
        /* [in] */ INotification* notification,
        /* [in] */ IUserHandle* user,
        /* [in] */ Int64 postTime);

    CARAPI constructor(
        /* [in] */ IParcelable* in);

public:
    // @Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    /**
     * @hide
     */
    CARAPI CloneLight(
        /* [out] */ IStatusBarNotification** notification);

    // @Override
    CARAPI Clone(
        /* [out] */ IInterface** obj);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    /** Convenience method to check the notification's flags for
     * {@link Notification#FLAG_ONGOING_EVENT}.
     */
    CARAPI IsOngoing(
        /* [out] */ Boolean* isongoing);

    /** Convenience method to check the notification's flags for
     * either {@link Notification#FLAG_ONGOING_EVENT} or
     * {@link Notification#FLAG_NO_CLEAR}.
     */
    CARAPI IsClearable(
        /* [out] */ Boolean* isClearable);

    /**
     * Returns a userHandle for the instance of the app that posted this notification.
     *
     * @deprecated Use {@link #getUser()} instead.
     */
    CARAPI GetUserId(
        /* [out] */ Int32* id);

    /** The package of the app that posted the notification. */
    CARAPI GetPackageName(
        /* [out] */ String* name);

    /** The id supplied to {@link android.app.NotificationManager#notify(int,Notification)}. */
    CARAPI GetId(
        /* [out] */ Int32* id);

    /** The tag supplied to {@link android.app.NotificationManager#notify(int,Notification)},
     * or null if no tag was specified. */
    CARAPI GetTag(
        /* [out] */ String* str);

    /** The notifying app's calling uid. @hide */
    CARAPI GetUid(
        /* [out] */ Int32* id);

    /** The package used for AppOps tracking. @hide */
    CARAPI GetOpPkg(
        /* [out] */ String* str);

    /** @hide */
    CARAPI GetInitialPid(
        /* [out] */ Int32* id);

    /** The {@link android.app.Notification} supplied to
     * {@link android.app.NotificationManager#notify(int,Notification)}. */
    CARAPI GetNotification(
        /* [out] */ INotification** no);

    /**
     * The {@link android.os.UserHandle} for whom this notification is intended.
     */
    CARAPI GetUser(
        /* [out] */ IUserHandle** us);

    /** The time (in {@link System#currentTimeMillis} time) the notification was posted,
     * which may be different than {@link android.app.Notification#when}.
     */
    CARAPI GetPostTime(
        /* [out] */ Int64* pt);

    /** @hide */
    CARAPI GetScore(
        /* [out] */ Int32* isongoing);

    /**
     * A unique instance key for this notification record.
     */
    CARAPI GetKey(
        /* [out] */ String* str);

    /**
     * A key that indicates the group with which this message ranks.
     */
    CARAPI GetGroupKey(
        /* [out] */ String* str);

private:
    CARAPI Key(
        /* [out] */ String* key);

    CARAPI GroupKey(
        /* [out] */ String* key);

private:
    String mPkg;
    Int32 mId;
    String mTag;
    String mKey;
    String mGroupKey;

    Int32 mUid;
    String mOpPkg;
    Int32 mInitialPid;
    AutoPtr<INotification> mNotification;
    AutoPtr<IUserHandle> mUser;
    Int64 mPostTime;

    Int32 mScore;

};

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_NOTIFICATION_STATUSBARNOTIFICATION_H__

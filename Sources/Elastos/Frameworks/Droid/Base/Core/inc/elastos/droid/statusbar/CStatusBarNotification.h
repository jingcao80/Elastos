#ifndef __ELASTOS_DROID_STATUSBAR_CSTATUSBARNOTIFICATION_H__
#define __ELASTOS_DROID_STATUSBAR_CSTATUSBARNOTIFICATION_H__

#include "_Elastos_Droid_StatusBar_CStatusBarNotification.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/app/CNotification.h"

using Elastos::Droid::App::INotification;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::UserHandle;

namespace Elastos {
namespace Droid {
namespace StatusBar {
/**
 * Class encapsulating a Notification. Sent by the NotificationManagerService to the IStatusBar (in System UI).
 */
CarClass(CStatusBarNotification)
{
public:
    CStatusBarNotification();

    CARAPI constructor();

    /** This is temporarily needed for the JB MR1 PDK. */
    //@Deprecated
    CARAPI constructor(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 id,
        /* [in] */ const String& tag,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 initialPid,
        /* [in] */ Int32 score,
        /* [in] */ INotification* notification);

    CARAPI constructor(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 id,
        /* [in] */ const String& tag,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 initialPid,
        /* [in] */ Int32 score,
        /* [in] */ INotification* notification,
        /* [in] */ IUserHandle* user);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

/*
    public static final Parcelable.Creator<StatusBarNotification> CREATOR
            = new Parcelable.Creator<StatusBarNotification>()
    {
        public StatusBarNotification createFromParcel(Parcel parcel)
        {
            return new StatusBarNotification(parcel);
        }

        public StatusBarNotification[] newArray(int size)
        {
            return new StatusBarNotification[size];
        }
    };
*/
/*
    //@Override
    public StatusBarNotification clone() {
        return new StatusBarNotification(this.pkg, this.id, this.tag, this.uid, this.initialPid,
                this.score, this.notification.clone(), this.user);
    }
*/
    CARAPI ToString(
        /* [out] */ String* result);


    CARAPI IsOngoing(
        /* [out] */ Boolean* result);

    CARAPI IsClearable(
        /* [out] */ Boolean* result);

    /** Returns a userHandle for the instance of the app that posted this notification. */
    CARAPI GetUserId(
        /* [out] */ Int32* userId);

    CARAPI GetPkg(
        /* [out] */ String* pkg);

    CARAPI GetTag(
        /* [out] */ String* tag);

    CARAPI GetId(
        /* [out] */ Int32* id);

    CARAPI GetNotification(
        /* [out] */ INotification** n);

    CARAPI GetUid(
        /* [out] */ Int32* uid);

    CARAPI GetInitialPid(
        /* [out] */ Int32* initialPid);

    CARAPI GetScore(
        /* [out] */ Int32* score);

    CARAPI GetUser(
        /* [out] */ IUserHandle** user);
public:
    String mPkg;
    Int32 mId;
    String mTag;
    Int32 mUid;
    Int32 mInitialPid;
    // TODO: make this field private and move callers to an accessor that
    // ensures sourceUser is applied.
    AutoPtr<INotification> mNotification;
    Int32 mScore;
    AutoPtr<IUserHandle> mUser;
};

}//namespace StatusBar
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_STATUSBAR_CSTATUSBARNOTIFICATION_H__

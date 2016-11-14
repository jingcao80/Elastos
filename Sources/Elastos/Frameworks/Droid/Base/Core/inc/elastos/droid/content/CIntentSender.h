
#ifndef __ELASTOS_DROID_CONTENT_CINTENTSENDER_H__
#define __ELASTOS_DROID_CONTENT_CINTENTSENDER_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Content_CIntentSender.h"
#include <elastos/core/Object.h>
#include <elastos/core/Runnable.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Runnable;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::IIIntentSender;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CIntentSender)
    , public Object
    , public IIntentSender
    , public IParcelable
{
public:
    class FinishedDispatcher
        : public Object
        , public IIntentReceiver
        , public IBinder
    {
    private:
        class MyRunnable
            : public Runnable {
        public:
            MyRunnable(
                /* [in] */ FinishedDispatcher* host);

            CARAPI Run();

        private:
            FinishedDispatcher* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        FinishedDispatcher();

        CARAPI constructor(
            /* [in] */ IIntentSender* pi,
            /* [in] */ IIntentSenderOnFinished* who,
            /* [in] */ IHandler* handler);

        virtual ~FinishedDispatcher();

        CARAPI PerformReceive(
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 resultCode,
            /* [in] */ const String& data,
            /* [in] */ IBundle* extras,
            /* [in] */ Boolean ordered,
            /* [in] */ Boolean sticky,
            /* [in] */ Int32 sendingUser);

        CARAPI Run();

        CARAPI ToString(
            /* [out] */ String* description);

    private:
        CIntentSender* mIntentSender;
        AutoPtr<IIntentSenderOnFinished> mWho;
        AutoPtr<IHandler> mHandler;
        AutoPtr<IIntent> mIntent;
        Int32 mResultCode;
        String mResultData;
        AutoPtr<IBundle> mResultExtras;

    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CIntentSender();

    virtual ~CIntentSender();

    /**
     * Perform the operation associated with this IntentSender, allowing the
     * caller to specify information about the Intent to use and be notified
     * when the send has completed.
     *
     * @param context The Context of the caller.  This may be null if
     * <var>intent</var> is also null.
     * @param code Result code to supply back to the IntentSender's target.
     * @param intent Additional Intent data.  See {@link Intent#fillIn
     * Intent.fillIn()} for information on how this is applied to the
     * original Intent.  Use null to not modify the original Intent.
     * @param onFinished The object to call back on when the send has
     * completed, or null for no callback.
     * @param handler Handler identifying the thread on which the callback
     * should happen.  If null, the callback will happen from the thread
     * pool of the process.
     *
     *
     * @throws SendIntentException Throws CanceledIntentException if the IntentSender
     * is no longer allowing more intents to be sent through it.
     */
    CARAPI SendIntent(
        /* [in] */ IContext* context,
        /* [in] */ Int32 code,
        /* [in] */ IIntent* intent,
        /* [in] */ IIntentSenderOnFinished* onFinished,
        /* [in] */ IHandler* handler);

    /**
     * Perform the operation associated with this IntentSender, allowing the
     * caller to specify information about the Intent to use and be notified
     * when the send has completed.
     *
     * @param context The Context of the caller.  This may be null if
     * <var>intent</var> is also null.
     * @param code Result code to supply back to the IntentSender's target.
     * @param intent Additional Intent data.  See {@link Intent#fillIn
     * Intent.fillIn()} for information on how this is applied to the
     * original Intent.  Use null to not modify the original Intent.
     * @param onFinished The object to call back on when the send has
     * completed, or null for no callback.
     * @param handler Handler identifying the thread on which the callback
     * should happen.  If null, the callback will happen from the thread
     * pool of the process.
     * @param requiredPermission Name of permission that a recipient of the PendingIntent
     * is required to hold.  This is only valid for broadcast intents, and
     * corresponds to the permission argument in
     * {@link Context#sendBroadcast(Intent, String) Context.sendOrderedBroadcast(Intent, String)}.
     * If null, no permission is required.
     *
     *
     * @throws SendIntentException Throws CanceledIntentException if the IntentSender
     * is no longer allowing more intents to be sent through it.
     */
    CARAPI SendIntent(
        /* [in] */ IContext* context,
        /* [in] */ Int32 code,
        /* [in] */ IIntent* intent,
        /* [in] */ IIntentSenderOnFinished* onFinished,
        /* [in] */ IHandler* handler,
        /* [in] */ const String& requiredPermission);

    /**
     * @deprecated Renamed to {@link #getCreatorPackage()}.
     */
    CARAPI GetTargetPackage(
        /* [out] */ String* str);

    /**
     * Return the package name of the application that created this
     * IntentSender, that is the identity under which you will actually be
     * sending the Intent.  The returned string is supplied by the system, so
     * that an application can not spoof its package.
     *
     * @return The package name of the PendingIntent, or null if there is
     * none associated with it.
     */
    CARAPI GetCreatorPackage(
        /* [out] */ String* str);

    /**
     * Return the uid of the application that created this
     * PendingIntent, that is the identity under which you will actually be
     * sending the Intent.  The returned integer is supplied by the system, so
     * that an application can not spoof its uid.
     *
     * @return The uid of the PendingIntent, or -1 if there is
     * none associated with it.
     */
    CARAPI GetCreatorUid(
        /* [out] */ Int32* uid);

    /**
     * Return the user handle of the application that created this
     * PendingIntent, that is the user under which you will actually be
     * sending the Intent.  The returned UserHandle is supplied by the system, so
     * that an application can not spoof its user.  See
     * {@link android.os.Process#myUserHandle() Process.myUserHandle()} for
     * more explanation of user handles.
     *
     * @return The user handle of the PendingIntent, or null if there is
     * none associated with it.
     */
    CARAPI GetCreatorUserHandle(
        /* [out] */ IUserHandle** userHandle);

    /**
     * Comparison operator on two IntentSender objects, such that true
     * is returned then they both represent the same operation from the
     * same package.
     */
    CARAPI Equals(
        /* [in] */ IInterface* otherObj,
        /* [out] */ Boolean* isEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* str);

    /** @hide */
    CARAPI GetTarget(
        /* [out] */ IIIntentSender** intentSender);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IIIntentSender* target);

    CARAPI constructor(
        /* [in] */ IBinder* target);

private:
    AutoPtr<IIIntentSender> mTarget;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CINTENTSENDER_H__

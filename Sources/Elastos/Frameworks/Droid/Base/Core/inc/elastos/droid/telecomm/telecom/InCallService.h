#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_INCALLSERVICE_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_INCALLSERVICE_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Telecomm.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * This service is implemented by any app that wishes to provide the user-interface for managing
 * phone calls. Telecom binds to this service while there exists a live (active or incoming) call,
 * and uses it to notify the in-call app of any live and and recently disconnected calls.
 *
 * {@hide}
 */
class ECO_PUBLIC InCallService
    : public Elastos::Droid::App::Service
    , public IInCallService
{
public:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("InCallService::MyHandler")

        MyHandler(
            /* [in] */ ILooper* loop,
            /* [in] */ InCallService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        InCallService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    InCallService();

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** result);

    CARAPI OnUnbind(
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* result);

    /**
     * Obtain the {@code Phone} associated with this {@code InCallService}.
     *
     * @return The {@code Phone} object associated with this {@code InCallService}, or {@code null}
     *         if the {@code InCallService} is not in a state where it has an associated
     *         {@code Phone}.
     */
    CARAPI GetPhone(
        /* [out] */ IPhone** result);

    /**
     * Invoked when the {@code Phone} has been created. This is a signal to the in-call experience
     * to start displaying in-call information to the user. Each instance of {@code InCallService}
     * will have only one {@code Phone}, and this method will be called exactly once in the lifetime
     * of the {@code InCallService}.
     *
     * @param phone The {@code Phone} object associated with this {@code InCallService}.
     */
    CARAPI OnPhoneCreated(
        /* [in] */ IPhone* phone);

    /**
     * Invoked when a {@code Phone} has been destroyed. This is a signal to the in-call experience
     * to stop displaying in-call information to the user. This method will be called exactly once
     * in the lifetime of the {@code InCallService}, and it will always be called after a previous
     * call to {@link #onPhoneCreated(Phone)}.
     *
     * @param phone The {@code Phone} object associated with this {@code InCallService}.
     */
    CARAPI OnPhoneDestroyed(
        /* [in] */ IPhone* phone);

public:
    static const Int32 MSG_SET_IN_CALL_ADAPTER;
    static const Int32 MSG_ADD_CALL;
    static const Int32 MSG_UPDATE_CALL;
    static const Int32 MSG_SET_POST_DIAL_WAIT;
    static const Int32 MSG_ON_AUDIO_STATE_CHANGED;
    static const Int32 MSG_BRING_TO_FOREGROUND;

    /** Default Handler used to consolidate binder method calls onto a single thread. */
    AutoPtr<IHandler> mHandler;

    AutoPtr<IPhone> mPhone;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_INCALLSERVICE_H__
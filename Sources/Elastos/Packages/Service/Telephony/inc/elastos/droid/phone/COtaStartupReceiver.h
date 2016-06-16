#ifndef  __ELASTOS_DROID_PHONE_COTASTARTUPRECEIVER_H__
#define  __ELASTOS_DROID_PHONE_COTASTARTUPRECEIVER_H__

#include "_Elastos_Droid_Phone_COtaStartupReceiver.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/*
 * Handles OTA Start procedure at phone power up. At phone power up, if phone is not OTA
 * provisioned (check MIN value of the Phone) and 'device_provisioned' is not set,
 * OTA Activation screen is shown that helps user activate the phone
 */
CarClass(COtaStartupReceiver)
    , public BroadcastReceiver
    , public IOtaStartupReceiver
{
private:
    class MyPhoneStateListener
        : public PhoneStateListener
    {
    public:
        TO_STRING_IMPL("COtaStartupReceiver::MyPhoneStateListener")

        MyPhoneStateListener(
            /* [in] */ COtaStartupReceiver* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnOtaspChanged(
            /* [in] */ Int32 otaspMode);

    private:
        COtaStartupReceiver* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("COtaStartupReceiver::MyHandler")

        MyHandler(
            /* [in] */ COtaStartupReceiver* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        COtaStartupReceiver* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    COtaStartupReceiver();

    CARAPI constructor();

    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    /**
     * On devices that provide a phone initialization wizard (such as Google Setup Wizard), we
     * allow delaying CDMA OTA setup so it can be done in a single wizard. The wizard is responsible
     * for (1) disabling itself once it has been run and/or (2) setting the 'device_provisioned'
     * flag to something non-zero and (3) calling the OTA Setup with the action below.
     *
     * NB: Typical phone initialization wizards will install themselves as the homescreen
     * (category "android.intent.category.HOME") with a priority higher than the default.
     * The wizard should set 'device_provisioned' when it completes, disable itself with the
     * PackageManager.setComponentEnabledSetting() and then start home screen.
     *
     * @return true if setup will be handled by wizard, false if it should be done now.
     */
    CARAPI_(Boolean) ShouldPostpone(
        /* [in] */ IContext* context);

private:
    static const String TAG;
    static const Boolean DBG;
    static const Int32 MIN_READY;
    static const Int32 SERVICE_STATE_CHANGED;
    AutoPtr<IContext> mContext;

    Int32 mOtaspMode;
    Boolean mPhoneStateListenerRegistered;
    AutoPtr<IPhoneStateListener> mPhoneStateListene;

    AutoPtr<IHandler> mHandle;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_COTASTARTUPRECEIVER_H__
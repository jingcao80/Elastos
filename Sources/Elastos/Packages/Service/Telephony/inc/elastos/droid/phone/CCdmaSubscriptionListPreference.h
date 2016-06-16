#ifndef  __ELASTOS_DROID_PHONE_CCDMASUBSCRIPTIONLISTPREFERENCE_H__
#define  __ELASTOS_DROID_PHONE_CCDMASUBSCRIPTIONLISTPREFERENCE_H__

#include "_Elastos_Droid_Phone_CCdmaSubscriptionListPreference.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CCdmaSubscriptionListPreference)
    , public ListPreference
    , public ICdmaSubscriptionListPreference
{
private:
    class CdmaSubscriptionButtonHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CCdmaSubscriptionListPreference::CdmaSubscriptionButtonHandler")

        CdmaSubscriptionButtonHandler(
            /* [in] */ CCdmaSubscriptionListPreference* mHost);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(void) HandleSetCdmaSubscriptionMode(
            /* [in] */ IMessage* msg);

    private:
        static const Int32 MESSAGE_SET_CDMA_SUBSCRIPTION;

        CCdmaSubscriptionListPreference* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CCdmaSubscriptionListPreference() {}

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    //@Override
    CARAPI ShowDialog(
        /* [in] */ IBundle* state);

    //@Override
    CARAPI OnDialogClosed(
        /* [in] */ Boolean positiveResult);

private:
    CARAPI_(void) SetCurrentCdmaSubscriptionModeValue();

private:
    static const String LOG_TAG;

    // Used for CDMA subscription mode
    static const Int32 CDMA_SUBSCRIPTION_RUIM_SIM;
    static const Int32 CDMA_SUBSCRIPTION_NV;

    //preferredSubscriptionMode  0 - RUIM/SIM, preferred
    //                           1 - NV
    static const Int32 sPreferredSubscriptionMode;

    AutoPtr<IPhone> mPhone;
    AutoPtr<IHandler> mHandler;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CCDMASUBSCRIPTIONLISTPREFERENCE_H__
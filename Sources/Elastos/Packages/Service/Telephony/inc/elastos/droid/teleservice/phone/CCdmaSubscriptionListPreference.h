#ifndef  __ELASTOS_DROID_PHONE_CCDMASUBSCRIPTIONLISTPREFERENCE_H__
#define  __ELASTOS_DROID_PHONE_CCDMASUBSCRIPTIONLISTPREFERENCE_H__

#include "_Elastos_Droid_TeleService_Phone_CCdmaSubscriptionListPreference.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/preference/ListPreference.h"
#include "Elastos.Droid.Internal.h"

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Preference::ListPreference;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

class CCdmaOptions;

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

    public:
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
    static const String TAG;

    // Used for CDMA subscription mode
    static const Int32 CDMA_SUBSCRIPTION_RUIM_SIM;
    static const Int32 CDMA_SUBSCRIPTION_NV;

    //preferredSubscriptionMode  0 - RUIM/SIM, preferred
    //                           1 - NV
    static const Int32 sPreferredSubscriptionMode;

    AutoPtr<IPhone> mPhone;
    AutoPtr<IHandler> mHandler;
    friend class CCdmaOptions;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CCDMASUBSCRIPTIONLISTPREFERENCE_H__
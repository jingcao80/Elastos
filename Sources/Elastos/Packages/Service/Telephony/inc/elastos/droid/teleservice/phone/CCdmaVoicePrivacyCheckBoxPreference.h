#ifndef  __ELASTOS_DROID_PHONE_CCDMAVOICEPRIVACYCHECKBOXPREFERENCE_H__
#define  __ELASTOS_DROID_PHONE_CCDMAVOICEPRIVACYCHECKBOXPREFERENCE_H__

#include "_Elastos_Droid_TeleService_Phone_CCdmaVoicePrivacyCheckBoxPreference.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/preference/CheckBoxPreference.h"

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Preference::CheckBoxPreference;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CarClass(CCdmaVoicePrivacyCheckBoxPreference)
    , public CheckBoxPreference
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CCdmaVoicePrivacyCheckBoxPreference::MyHandler")

        MyHandler(
            /* [in] */ CCdmaVoicePrivacyCheckBoxPreference* mHost);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(void) HandleGetVPResponse(
            /* [in] */ IMessage* msg);

        CARAPI_(void) HandleSetVPResponse(
            /* [in] */ IMessage* msg);

    public:
        static const Int32 MESSAGE_GET_VP;
        static const Int32 MESSAGE_SET_VP;

        CCdmaVoicePrivacyCheckBoxPreference* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CCdmaVoicePrivacyCheckBoxPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

protected:
    //@Override
    CARAPI OnClick();

private:
    static const String TAG;
    const Boolean DBG;

    AutoPtr<IPhone> mPhone;
    AutoPtr<IHandler> mHandler;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CCDMAVOICEPRIVACYCHECKBOXPREFERENCE_H__
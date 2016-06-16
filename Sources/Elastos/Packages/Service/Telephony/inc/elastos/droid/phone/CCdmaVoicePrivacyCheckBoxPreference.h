#ifndef  __ELASTOS_DROID_PHONE_CCDMAVOICEPRIVACYCHECKBOXPREFERENCE_H__
#define  __ELASTOS_DROID_PHONE_CCDMAVOICEPRIVACYCHECKBOXPREFERENCE_H__

#include "_Elastos_Droid_Phone_CCdmaVoicePrivacyCheckBoxPreference.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CCdmaVoicePrivacyCheckBoxPreference)
    , public CheckBoxPreference
    , public ICdmaVoicePrivacyCheckBoxPreference
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

    private:
        static const Int32 MESSAGE_GET_VP;
        static const Int32 MESSAGE_SET_VP;

        CCdmaVoicePrivacyCheckBoxPreference* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CCdmaVoicePrivacyCheckBoxPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int232 defStyle);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

protected:
    //@Override
    CARAPI OnClick();

private:
    static const String LOG_TAG;
    const Boolean DBG;

    AutoPtr<IPhone> mPhone;
    AutoPtr<IHandler> mHandler;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CCDMAVOICEPRIVACYCHECKBOXPREFERENCE_H__
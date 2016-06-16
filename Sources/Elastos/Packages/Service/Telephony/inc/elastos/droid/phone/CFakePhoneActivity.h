#ifndef  __ELASTOS_DROID_PHONE_CFACKPHONEACTIVITY_H__
#define  __ELASTOS_DROID_PHONE_CFACKPHONEACTIVITY_H__

#include "_Elastos_Droid_Phone_CFakePhoneActivity.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * A simple activity that presents you with a UI for faking incoming phone operations.
 */
CarClass(CFakePhoneActivity)
    , public Activity
    , public IFakePhoneActivity
{
private:
    class ButtonListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CFakePhoneActivity::ButtonListener")

        ButtonListener(
            /* [in] */ CFakePhoneActivity* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CFakePhoneActivity* mHost;
    };

    class MyViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CFakePhoneActivity::MyViewOnClickListener")

        MyViewOnClickListener(
            /* [in] */ CFakePhoneActivity* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CFakePhoneActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CFakePhoneActivity() {}

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

private:
    static const String TAG;

    AutoPtr<IButton> mPlaceCall;
    AutoPtr<IEditText> mPhoneNumber;
    AutoPtr<ISimulatedRadioControl> mRadioControl;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CFACKPHONEACTIVITY_H__
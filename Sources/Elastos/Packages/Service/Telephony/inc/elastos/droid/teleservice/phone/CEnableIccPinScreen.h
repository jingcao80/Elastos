#ifndef  __ELASTOS_DROID_PHONE_CENABLEICCPINSCREEN_H__
#define  __ELASTOS_DROID_PHONE_CENABLEICCPINSCREEN_H__

#include "_Elastos_Droid_TeleService_Phone_CEnableIccPinScreen.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/os/AsyncResult.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * UI to enable/disable the ICC PIN.
 */
CarClass(CEnableIccPinScreen)
    , public Activity
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CEnableIccPinScreen::MyHandler")

        MyHandler(
            /* [in] */ CEnableIccPinScreen* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CEnableIccPinScreen* mHost;
    };

    class MyViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CEnableIccPinScreen::MyViewOnClickListener")

        CAR_INTERFACE_DECL()

        MyViewOnClickListener(
            /* [in] */ CEnableIccPinScreen* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CEnableIccPinScreen* mHost;

    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CEnableIccPinScreen::MyRunnable")

        MyRunnable(
            /* [in] */ CEnableIccPinScreen* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CEnableIccPinScreen* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CEnableIccPinScreen();

    CARAPI constructor();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

private:
    CARAPI_(void) SetupView();

    CARAPI_(void) ShowStatus(
        /* [in] */ ICharSequence* statusMsg);

    CARAPI_(String) GetPin();

    CARAPI_(void) EnableIccPin();

    CARAPI_(void) HandleResult(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String TAG;

    static const Int32 ENABLE_ICC_PIN_COMPLETE = 100;
    static const Boolean DBG;

    AutoPtr<ILinearLayout> mPinFieldContainer;
    AutoPtr<IEditText> mPinField;
    AutoPtr<ITextView> mStatusField;
    Boolean mEnable;
    AutoPtr<IPhone> mPhone;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IViewOnClickListener> mClicked;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CENABLEICCPINSCREEN_H__
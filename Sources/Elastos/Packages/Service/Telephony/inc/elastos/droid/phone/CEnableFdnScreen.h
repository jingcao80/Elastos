#ifndef  __ELASTOS_DROID_PHONE_CENABELFDNSCREEN_H__
#define  __ELASTOS_DROID_PHONE_CENABELFDNSCREEN_H__

#include "_Elastos_Droid_Phone_CEnableFdnScreen.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/os/AsyncResult.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * UI to enable/disable FDN.
 */
CarClass(CEnableFdnScreen)
    , public Activity
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CEnableFdnScreen::MyHandler")

        MyHandler(
            /* [in] */ CEnableFdnScreen* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CEnableFdnScreen* mHost;
    };

    class MyViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CEnableFdnScreen::MyViewOnClickListener")

        CAR_INTERFACE_DECL()

        MyViewOnClickListener(
            /* [in] */ CEnableFdnScreen* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CEnableFdnScreen* mHost;

    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CEnableFdnScreen::MyRunnable")

        MyRunnable(
            /* [in] */ CEnableFdnScreen* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CEnableFdnScreen* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CEnableFdnScreen();

    CARAPI constructor();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

private:
    CARAPI_(void) SetupView();

    CARAPI_(void) ShowStatus(
        /* [in] */ ICharSequence* statusMsg);

    CARAPI_(String) GetPin2();

    CARAPI_(void) EnableFdn();

    CARAPI_(void) HandleResult(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String TAG;
    static const Boolean DBG;

    static const Int32 ENABLE_FDN_COMPLETE = 100;

    AutoPtr<ILinearLayout> mPinFieldContainer;
    AutoPtr<IEditText> mPin2Field;
    AutoPtr<ITextView> mStatusField;
    Boolean mEnable;
    AutoPtr<IPhone> mPhone;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IViewOnClickListener> mClicked;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CENABELFDNSCREEN_H__
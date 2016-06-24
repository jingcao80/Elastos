#ifndef  __ELASTOS_DROID_PHONE_CENABELFDNSCREEN_H__
#define  __ELASTOS_DROID_PHONE_CENABELFDNSCREEN_H__

#include "_Elastos_Droid_Phone_CEnableFdnScreen.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * UI to enable/disable FDN.
 */
CarClass(CEnableFdnScreen)
    , public Activity
    , public IEnableFdnScreen
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

        //@Override
        CARAPI Run();

    private:
        CEnableFdnScreen* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CPhoneApp() {}

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
        /* [in] */ IAsyncResult* ar);

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String TAG;
    static const Boolean DBG;

    static const Int32 ENABLE_FDN_COMPLETE;

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
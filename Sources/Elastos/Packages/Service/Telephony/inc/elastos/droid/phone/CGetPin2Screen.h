#ifndef  __ELASTOS_DROID_PHONE_CGETPIN2SCREEN_H__
#define  __ELASTOS_DROID_PHONE_CGETPIN2SCREEN_H__

#include "_Elastos_Droid_Phone_CGetPin2Screen.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Pin2 entry screen.
 */
CarClass(CGetPin2Screen)
    , public Activity
    , public IGetPin2Screen
    , public ITextViewOnEditorActionListener
{
private:
    class MyViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CGetPin2Screen::MyViewOnClickListener")

        CAR_INTERFACE_DECL()

        MyViewOnClickListener(
            /* [in] */ CGetPin2Screen* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CGetPin2Screen* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CGetPin2Screen();

    CARAPI constructor();

    //@Override
    CARAPI OnEditorAction(
        /* [in] */ ITextView* v,
        /* [in] */ Int32 actionId,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

private:
    CARAPI_(String) GetPin2();

    CARAPI_(void) ReturnResult();

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String LOG_TAG;

    AutoPtr<IEditText> mPin2Field;
    AutoPtr<IButton> mOkButton;

    AutoPtr<IViewOnClickListener> mClicked;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CGETPIN2SCREEN_H__
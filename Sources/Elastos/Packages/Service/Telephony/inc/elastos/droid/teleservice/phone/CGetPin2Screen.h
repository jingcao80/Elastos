#ifndef  __ELASTOS_DROID_PHONE_CGETPIN2SCREEN_H__
#define  __ELASTOS_DROID_PHONE_CGETPIN2SCREEN_H__

#include "_Elastos_Droid_TeleService_Phone_CGetPin2Screen.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IOnEditorActionListener;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Pin2 entry screen.
 */
CarClass(CGetPin2Screen)
    , public Activity
    , public IOnEditorActionListener
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
    static const String TAG;

    AutoPtr<IEditText> mPin2Field;
    AutoPtr<IButton> mOkButton;

    AutoPtr<IViewOnClickListener> mClicked;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CGETPIN2SCREEN_H__

#ifndef __Elastos_DevSamples_EditTextDemo_CActivityOne_H__
#define __Elastos_DevSamples_EditTextDemo_CActivityOne_H__

#include <Elastos.Droid.Widget.h>
#include <elastos/droid/app/Activity.h>
#include "_Elastos_DevSamples_EditTextDemo_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::Widget::IEditText;

namespace Elastos {
namespace DevSamples {
namespace EditTextDemo {

CarClass(CActivityOne) , public Activity
{
public:
    class MyListener
        : public Object
        , public IViewOnKeyListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ CActivityOne* host);

        ~MyListener();

        CARAPI OnKey(
            /* [in] */ IView * v,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

    private:
        CActivityOne* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    AutoPtr<IEditText> mEditText;
};

} // namespace EditTextDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_EditTextDemo_CActivityOne_H__

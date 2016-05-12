
#ifndef __Elastos_DevSamples_ScrollViewDemo_CACTIVITYONE_H__
#define __Elastos_DevSamples_ScrollViewDemo_CACTIVITYONE_H__

#include <Elastos.Droid.Widget.h>
#include "elastos/droid/app/Activity.h"
#include "_Elastos_DevSamples_ScrollViewDemo_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IScrollView;
using Elastos::Droid::Widget::IButton;

namespace Elastos {
namespace DevSamples {
namespace ScrollViewDemo {

class CActivityOne;

CarClass(CActivityOne)
    , public Activity
{
public:
    class MyListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

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
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

private:
    AutoPtr<IScrollView> mScroll;
    AutoPtr<IButton> mButton;

    friend class MyListener;
};

} // namespace EditTextDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_ScrollViewDemo_CACTIVITYONE_H__

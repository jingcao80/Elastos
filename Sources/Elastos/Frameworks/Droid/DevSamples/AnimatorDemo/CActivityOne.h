
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Animation::ITypeEvaluator;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AnimatorDemo {


class CActivityOne : public Activity
{
private:
    class MyEvaluator
        : public ITypeEvaluator
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        MyEvaluator();

        CARAPI Evaluate(
            /* [in] */ Float fraction,
            /* [in] */ IInterface* startValue,
            /* [in] */ IInterface* endValue,
            /* [out] */ IInterface** result);
    private:
        CActivityOne* mHost;
    };

    class MyListener
        : public IViewOnClickListener
        , public ElRefBase
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
protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();
private:
    AutoPtr<IButton> mButtonInter;
    AutoPtr<IButton> mButtonColor;
    AutoPtr<IButton> mButtonAlpha;
    AutoPtr<IButton> mButtonProperty;
    AutoPtr<IButton> mButtonValueHolder;
};

} // namespace AnimatorDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__

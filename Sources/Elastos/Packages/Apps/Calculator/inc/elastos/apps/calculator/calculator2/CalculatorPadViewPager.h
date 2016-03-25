#ifndef _ELASTOS_APPS_CALCULATOR_CALCULATOR2_CALCULATORPADVIEWPAGER_H__
#define _ELASTOS_APPS_CALCULATOR_CALCULATOR2_CALCULATORPADVIEWPAGER_H__

#include "_Elastos.Apps.Calculator.Calculator2.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos{
namespace Apps {
namespace Calculator {
namespace Calculator2 {

class CalculatorPadViewPager
    : public Object
    /*, public ViewPager*/
{
private:
    class MyPagerAdapter
    {
    public:
        MyPagerAdapter();
        ~MyPagerAdapter();

    };

    class MyOnPageChangeListener
    {
    public:
        MyOnPageChangeListener();
        ~MyOnPageChangeListener();

    };

    class MyPageTransformer
    {
    public:
        MyPageTransformer();
        ~MyPageTransformer();
    };

public:
    CalculatorPadViewPager();

    ~CalculatorPadViewPager();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    CARAPI OnFinishInflate();

private:
    assert(0&* "TODO");
/*    PagerAdapter mStaticPagerAdapter;
    OnPageChangeListener mOnPageChangeListener;
    PageTransformer mPageTransformer;*/
};

} // namespace Calculator2
} // namespace Calculator
} // namespace Apps
} // namespace Elastos

#endif // _ELASTOS_APPS_CALCULATOR_CALCULATOR2_CALCULATORPADVIEWPAGER_H__
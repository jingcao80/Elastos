#ifndef _ELASTOS_DROID_CALCULATOR2_CALCULATORPADVIEWPAGER_H__
#define _ELASTOS_DROID_CALCULATOR2_CALCULATORPADVIEWPAGER_H__

#include "_Elastos.Droid.Calculator2.h"
#include "elastos/droid/support/v4/view/ViewPager.h"
#include "elastos/droid/support/v4/view/PagerAdapter.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Support::V4::View::ViewPager;
using Elastos::Droid::Support::V4::View::PagerAdapter;
using Elastos::Droid::Support::V4::View::IPagerAdapter;
using Elastos::Droid::Support::V4::View::IViewPagerOnPageChangeListener;
using Elastos::Droid::Support::V4::View::IViewPagerPageTransformer;

namespace Elastos {
namespace Droid {
namespace Calculator2 {

class CalculatorPadViewPager
    : public ViewPager
    , public ICalculatorPadViewPager
{
private:
    class MyPagerAdapter : public PagerAdapter
    {
    public:
        MyPagerAdapter(
            /* [in] */ CalculatorPadViewPager* host)
            : mHost(host)
        {}

        CARAPI GetCount(
            /* [out] */ Int32* count);

        CARAPI InstantiateItem(
            /* [in] */ IViewGroup* container,
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        CARAPI DestroyItem(
            /* [in] */ IViewGroup* container,
            /* [in] */ Int32 position,
            /* [in] */ IInterface* object);

        CARAPI IsViewFromObject(
            /* [in] */ IView* view,
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI GetPageWidth(
            /* [in] */ Int32 position,
            /* [out] */ Float* width);

    private:
        CalculatorPadViewPager* mHost;
    };

    class MyOnPageChangeListener : public SimpleOnPageChangeListener
    {
    public:
        MyOnPageChangeListener(
            /* [in] */ CalculatorPadViewPager* host)
            : mHost(host)
        {}

        CARAPI OnPageSelected(
            /* [in] */ Int32 position);

    private:
        CARAPI_(void) RecursivelySetEnabled(
            /* [in] */ IView* view,
            /* [in] */ Boolean enabled);

    private:
        CalculatorPadViewPager* mHost;
    };

    class MyPageTransformer
        : public Object
        , public IViewPagerPageTransformer
    {
    public:
        MyPageTransformer(
            /* [in] */ CalculatorPadViewPager* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI TransformPage(
            /* [in] */ IView* page,
            /* [in] */ Float position);

    private:
        CalculatorPadViewPager* mHost;
    };

public:
    CalculatorPadViewPager();

    virtual ~CalculatorPadViewPager() {}

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    CARAPI OnFinishInflate();

private:
    AutoPtr<IPagerAdapter> mStaticPagerAdapter;
    AutoPtr<IViewPagerOnPageChangeListener> mOnPageChangeListener;
    AutoPtr<IViewPagerPageTransformer> mPageTransformer;

    friend class MyOnPageChangeListener;
};

} // namespace Calculator2
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_CALCULATOR2_CALCULATORPADVIEWPAGER_H__
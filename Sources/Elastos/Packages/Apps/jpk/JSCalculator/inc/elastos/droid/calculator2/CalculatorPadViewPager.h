//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
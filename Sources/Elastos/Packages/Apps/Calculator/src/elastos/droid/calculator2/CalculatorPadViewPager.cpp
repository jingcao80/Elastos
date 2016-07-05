
#include "elastos/droid/calculator2/CalculatorPadViewPager.h"
#include "R.h"
#include <elastos/core/Math.h>
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Calculator2::EIID_ICalculatorPadViewPager;
using Elastos::Droid::Support::V4::View::EIID_IViewPagerOnPageChangeListener;
using Elastos::Droid::Support::V4::View::EIID_IViewPagerPageTransformer;

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Calculator2 {

//=================================================================
// CalculatorPadViewPager::MyPagerAdapter
//=================================================================

ECode CalculatorPadViewPager::MyPagerAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    return mHost->GetChildCount(count);
}

ECode CalculatorPadViewPager::MyPagerAdapter::InstantiateItem(
    /* [in] */ IViewGroup* container,
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)
    AutoPtr<IView> child;
    mHost->GetChildAt(position, (IView**)&child);
    *item = (IInterface*)child.Get();
    REFCOUNT_ADD(*item)
    return NOERROR;
}

ECode CalculatorPadViewPager::MyPagerAdapter::DestroyItem(
    /* [in] */ IViewGroup* container,
    /* [in] */ Int32 position,
    /* [in] */ IInterface* object)
{
    return mHost->RemoveViewAt(position);
}

ECode CalculatorPadViewPager::MyPagerAdapter::IsViewFromObject(
    /* [in] */ IView* view,
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IInterface::Probe(view) == object;
    return NOERROR;
}

ECode CalculatorPadViewPager::MyPagerAdapter::GetPageWidth(
    /* [in] */ Int32 position,
    /* [out] */ Float* width)
{
    VALIDATE_NOT_NULL(width)
    *width = position == 1 ? 7.0f / 9.0f : 1.0f;
    return NOERROR;
}


//=================================================================
// CalculatorPadViewPager::MyOnPageChangeListener
//=================================================================

void CalculatorPadViewPager::MyOnPageChangeListener::RecursivelySetEnabled(
    /* [in] */ IView* view,
    /* [in] */ Boolean enabled)
{
    AutoPtr<IViewGroup> viewGroup = IViewGroup::Probe(view);
    if (viewGroup != NULL) {
        Int32 count;
        viewGroup->GetChildCount(&count);
        for (Int32 childIndex = 0; childIndex < count; ++childIndex) {
            AutoPtr<IView> child;
            viewGroup->GetChildAt(childIndex, (IView**)&child);
            RecursivelySetEnabled(child, enabled);
        }
    }
    else {
        view->SetEnabled(enabled);
    }
}

ECode CalculatorPadViewPager::MyOnPageChangeListener::OnPageSelected(
    /* [in] */ Int32 position)
{
    AutoPtr<IPagerAdapter> adapter;
    if (mHost->GetAdapter((IPagerAdapter**)&adapter), adapter == mHost->mStaticPagerAdapter) {
        Int32 count;
        mHost->GetChildCount(&count);
        for (Int32 childIndex = 0; childIndex < count; ++childIndex) {
            // Only enable subviews of the current page.
            AutoPtr<IView> child;
            mHost->GetChildAt(childIndex, (IView**)&child);
            RecursivelySetEnabled(child, childIndex == position);
        }
    }
    return NOERROR;
}


//=================================================================
// CalculatorPadViewPager::MyPageTransformer
//=================================================================

CAR_INTERFACE_IMPL(CalculatorPadViewPager::MyPageTransformer, Object, IViewPagerPageTransformer)

ECode CalculatorPadViewPager::MyPageTransformer::TransformPage(
    /* [in] */ IView* view,
    /* [in] */ Float position)
{
    if (position < 0.0f) {
        // Pin the left page to the left side.
        Int32 width;
        mHost->GetWidth(&width);
        view->SetTranslationX(width * -position);
        view->SetAlpha(Elastos::Core::Math::Max(1.0f + position, 0.0f));
    }
    else {
        // Use the default slide transition when moving to the next page.
        view->SetTranslationX(0.0f);
        view->SetAlpha(1.0f);
    }
    return NOERROR;
}


//=================================================================
// CalculatorPadViewPager
//=================================================================

CalculatorPadViewPager::CalculatorPadViewPager()
{
    Slogger::D("CalculatorPadViewPager", "~~~~~~~~~~~~viewpager create");
    mStaticPagerAdapter = (IPagerAdapter*)new MyPagerAdapter(this);
    mOnPageChangeListener = (IViewPagerOnPageChangeListener*)new MyOnPageChangeListener(this);
    mPageTransformer = (IViewPagerPageTransformer*)new MyPageTransformer(this);
}

CAR_INTERFACE_IMPL(CalculatorPadViewPager, ViewPager, ICalculatorPadViewPager)

ECode CalculatorPadViewPager::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CalculatorPadViewPager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ViewPager::constructor(context, attrs);

    SetAdapter(mStaticPagerAdapter);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 color;
    res->GetColor(Elastos::Droid::R::color::black, &color);
    SetBackgroundColor(color);
    SetOnPageChangeListener(mOnPageChangeListener);
    Int32 size;
    res->GetDimensionPixelSize(R::dimen::pad_page_margin, &size);
    SetPageMargin(size);
    SetPageTransformer(FALSE, mPageTransformer);
    return NOERROR;
}

ECode CalculatorPadViewPager::OnFinishInflate()
{
    ViewPager::OnFinishInflate();

    // Invalidate the adapter's data set since children may have been added during inflation.
    AutoPtr<IPagerAdapter> adapter;
    if (GetAdapter((IPagerAdapter**)&adapter), adapter == mStaticPagerAdapter) {
        mStaticPagerAdapter->NotifyDataSetChanged();
    }
    return NOERROR;
}

} // namespace Calculator2
} // namespace Droid
} // namespace Elastos
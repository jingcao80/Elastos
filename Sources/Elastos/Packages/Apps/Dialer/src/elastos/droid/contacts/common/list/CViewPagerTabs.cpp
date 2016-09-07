
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/contacts/common/list/CViewPagerTabs.h"
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::Support::V4::View::EIID_IViewPagerOnPageChangeListener;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOutlineProvider;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Droid::Widget::CLinearLayoutLayoutParams;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::CTextView;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

//================================================================
// CViewPagerTabs::ViewBoundsOutlineProvider
//================================================================
CAR_INTERFACE_IMPL(CViewPagerTabs::ViewBoundsOutlineProvider, Object, IViewOutlineProvider)

ECode CViewPagerTabs::ViewBoundsOutlineProvider::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    Int32 width, height;
    view->GetWidth(&width);
    view->GetHeight(&height);
    return outline->SetRect(0, 0, width, height);
}


//================================================================
// CViewPagerTabs::OnTabLongClickListener
//================================================================
CAR_INTERFACE_IMPL(CViewPagerTabs::OnTabLongClickListener, Object, IViewOnLongClickListener)

ECode CViewPagerTabs::OnTabLongClickListener::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Int32> > screenPos = ArrayOf<Int32>::Alloc(2);
    mHost->GetLocationOnScreen(screenPos);

    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    Int32 width;
    mHost->GetWidth(&width);
    Int32 height;
    mHost->GetHeight(&height);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Int32 screenWidth;
    dm->GetWidthPixels(&screenWidth);
    AutoPtr<IPagerAdapter> adapter;
    mHost->mPager->GetAdapter((IPagerAdapter**)&adapter);
    AutoPtr<ICharSequence> title;
    adapter->GetPageTitle(mPosition, (ICharSequence**)&title);
    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<IToast> toast;
    helper->MakeText(context, title, IToast::LENGTH_SHORT, (IToast**)&toast);

    // Show the toast under the tab
    toast->SetGravity(IGravity::TOP | IGravity::CENTER_HORIZONTAL,
            ((*screenPos)[0] + width / 2) - screenWidth / 2, (*screenPos)[1] + height);

    toast->Show();
    *result = TRUE;
    return NOERROR;
}


//================================================================
// CViewPagerTabs::TextViewOnClickListener
//================================================================
CAR_INTERFACE_IMPL(CViewPagerTabs::TextViewOnClickListener, Object, IViewOnClickListener)

ECode CViewPagerTabs::TextViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->mPager->SetCurrentItem(mHost->GetRtlPosition(mPosition));
}


//================================================================
// CViewPagerTabs
//================================================================
AutoPtr<IViewOutlineProvider> CViewPagerTabs::InitViewOutlineProvider()
{
    AutoPtr<IViewOutlineProvider> provider = (IViewOutlineProvider*)new ViewBoundsOutlineProvider();
    return provider;
}
const AutoPtr<IViewOutlineProvider> CViewPagerTabs::VIEW_BOUNDS_OUTLINE_PROVIDER = CViewPagerTabs::InitViewOutlineProvider();
const Int32 CViewPagerTabs::TAB_SIDE_PADDING_IN_DPS;

static AutoPtr<ArrayOf<Int32> > InitAttrs()
{
    AutoPtr<ArrayOf<Int32> > attrs = ArrayOf<Int32>::Alloc(4);
    (*attrs)[0] = Elastos::Droid::R::attr::textSize;
    (*attrs)[1] = Elastos::Droid::R::attr::textStyle;
    (*attrs)[2] = Elastos::Droid::R::attr::textColor;
    (*attrs)[3] = Elastos::Droid::R::attr::textAllCaps;
    return attrs;
}
const AutoPtr<ArrayOf<Int32> > CViewPagerTabs::ATTRS = InitAttrs();

CViewPagerTabs::CViewPagerTabs()
    : mTextStyle(0)
    , mTextSize(0)
    , mTextAllCaps(FALSE)
    , mPrevSelected(-1)
    , mSidePadding(0)
{}

CAR_INTERFACE_IMPL_2(CViewPagerTabs, HorizontalScrollView, IViewPagerTabs, IViewPagerOnPageChangeListener);

CAR_OBJECT_IMPL(CViewPagerTabs);

ECode CViewPagerTabs::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CViewPagerTabs::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CViewPagerTabs::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FAIL_RETURN(HorizontalScrollView::constructor(context, attrs, defStyle))
    SetFillViewport(TRUE);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float density;
    dm->GetDensity(&density);
    mSidePadding = (Int32)density * TAB_SIDE_PADDING_IN_DPS;

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, ATTRS, (ITypedArray**)&a);
    a->GetDimensionPixelSize(0, 0, &mTextSize);
    a->GetInt32(1, 0, &mTextStyle);
    a->GetColorStateList(2, (IColorStateList**)&mTextColor);
    a->GetBoolean(3, FALSE, &mTextAllCaps);

    mTabStrip = new ViewPagerTabStrip();
    mTabStrip->constructor(context);
    AutoPtr<IViewGroupLayoutParams> lp;
    CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::MATCH_PARENT,
            (IViewGroupLayoutParams**)&lp);
    AddView(mTabStrip, lp);
    a->Recycle();

    // enable shadow casting from view bounds
    SetOutlineProvider(VIEW_BOUNDS_OUTLINE_PROVIDER);
    return NOERROR;
}

ECode CViewPagerTabs::SetViewPager(
    /* [in] */ IViewPager* viewPager)
{
    mPager = viewPager;
    AutoPtr<IPagerAdapter> adapter;
    mPager->GetAdapter((IPagerAdapter**)&adapter);
    AddTabs(adapter);
    return NOERROR;
}

void CViewPagerTabs::AddTabs(
    /* [in] */ IPagerAdapter* adapter)
{
    mTabStrip->RemoveAllViews();

    Int32 count;
    adapter->GetCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<ICharSequence> title;
        adapter->GetPageTitle(i, (ICharSequence**)&title);
        AddTab(title, i);
    }
}

void CViewPagerTabs::AddTab(
    /* [in] */ ICharSequence* tabTitle,
    /* [in] */ Int32 position)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ITextView> textView;
    CTextView::New(context, (ITextView**)&textView);
    AutoPtr<IView> tv = IView::Probe(textView);
    textView->SetText(tabTitle);
    tv->SetBackgroundResource(Elastos::Droid::Dialer::R::drawable::view_pager_tab_background);
    textView->SetGravity(IGravity::CENTER);
    AutoPtr<IViewOnClickListener> listener = (IViewOnClickListener*)new TextViewOnClickListener(position, this);
    tv->SetOnClickListener(listener);

    AutoPtr<IViewOnLongClickListener> longClickListener = (IViewOnLongClickListener*)new OnTabLongClickListener(position, this);
    tv->SetOnLongClickListener(longClickListener);

    // Assign various text appearance related attributes to child views.
    if (mTextStyle > 0) {
        AutoPtr<ITypeface> typeface;
        textView->GetTypeface((ITypeface**)&typeface);
        textView->SetTypeface(typeface, mTextStyle);
    }
    if (mTextSize > 0) {
        textView->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, mTextSize);
    }
    if (mTextColor != NULL) {
        textView->SetTextColor(mTextColor);
    }
    textView->SetAllCaps(mTextAllCaps);
    tv->SetPadding(mSidePadding, 0, mSidePadding, 0);
    AutoPtr<IViewGroupLayoutParams> lp;
    CLinearLayoutLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::MATCH_PARENT,
            1, (IViewGroupLayoutParams**)&lp);
    mTabStrip->AddView(IView::Probe(textView), lp);
    // Default to the first child being selected
    if (position == 0) {
        mPrevSelected = 0;
        tv->SetSelected(TRUE);
    }
}

ECode CViewPagerTabs::OnPageScrolled(
    /* [in] */ Int32 position,
    /* [in] */ Float positionOffset,
    /* [in] */ Int32 positionOffsetPixels)
{
    position = GetRtlPosition(position);
    Int32 tabStripChildCount;
    mTabStrip->GetChildCount(&tabStripChildCount);
    if ((tabStripChildCount == 0) || (position < 0) || (position >= tabStripChildCount)) {
        return NOERROR;
    }

    mTabStrip->OnPageScrolled(position, positionOffset, positionOffsetPixels);
    return NOERROR;
}

ECode CViewPagerTabs::OnPageSelected(
    /* [in] */ Int32 position)
{
    position = GetRtlPosition(position);
    if (mPrevSelected >= 0) {
        AutoPtr<IView> child;
        mTabStrip->GetChildAt(mPrevSelected, (IView**)&child);
        child->SetSelected(FALSE);
    }
    AutoPtr<IView> selectedChild;
    mTabStrip->GetChildAt(position, (IView**)&selectedChild);
    selectedChild->SetSelected(TRUE);

    // Update scroll position
    Int32 left;
    selectedChild->GetLeft(&left);
    Int32 width, childWidth;
    GetWidth(&width);
    selectedChild->GetWidth(&childWidth);
    Int32 scrollPos = left - (width - childWidth) / 2;
    SmoothScrollTo(scrollPos, 0);
    mPrevSelected = position;
    return NOERROR;
}

ECode CViewPagerTabs::OnPageScrollStateChanged(
    /* [in] */ Int32 state)
{
    return NOERROR;
}

Int32 CViewPagerTabs::GetRtlPosition(
    /* [in] */ Int32 position)
{
    Int32 direction;
    if (GetLayoutDirection(&direction), direction == IView::LAYOUT_DIRECTION_RTL) {
        Int32 count;
        mTabStrip->GetChildCount(&count);
        return count - 1 - position;
    }
    return position;
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

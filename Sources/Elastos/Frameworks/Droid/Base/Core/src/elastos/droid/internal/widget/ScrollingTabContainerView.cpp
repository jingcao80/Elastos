
#include "elastos/droid/internal/widget/ScrollingTabContainerView.h"
#include "elastos/droid/internal/widget/CScrollingTabContainerTabView.h"
#include "elastos/droid/animation/CObjectAnimator.h"
// #include "elastos/droid/internal/view/CActionBarPolicy.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/view/animation/CDecelerateInterpolator.h"
#include "elastos/droid/widget/CAbsListViewLayoutParams.h"
#include "elastos/droid/widget/CLinearLayout.h"
#include "elastos/droid/widget/CLinearLayoutLayoutParams.h"
#include "elastos/droid/widget/CSpinner.h"
#include "elastos/droid/widget/CImageView.h"
#include "elastos/droid/widget/CTextView.h"
#include "elastos/droid/widget/Toast.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::CObjectAnimator;
using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Internal::View::IActionBarPolicy;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::TextUtilsTruncateAt;
using Elastos::Droid::Text::TextUtilsTruncateAt_END;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::CViewGroupLayoutParams;
// using Elastos::Droid::View::Menu::CActionBarPolicy;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewParent;
using Elastos::Droid::Widget::CAbsListViewLayoutParams;
using Elastos::Droid::Widget::CImageView;
using Elastos::Droid::Widget::CTextView;
using Elastos::Droid::Widget::CLinearLayout;
using Elastos::Droid::Widget::CLinearLayoutLayoutParams;
using Elastos::Droid::Widget::CSpinner;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IAdapter;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAbsListViewLayoutParams;
using Elastos::Droid::Widget::ILinearLayoutLayoutParams;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::Toast;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

ScrollingTabContainerView::AnimateToTabRunnable::AnimateToTabRunnable(
    /* [in] */ IView* tabView,
    /* [in] */ ScrollingTabContainerView* host)
{
    mTabView = tabView;
    mHost = host;
}

ECode ScrollingTabContainerView::AnimateToTabRunnable::Run()
{
    Int32 tLeft, width, tWidth;
    mTabView->GetLeft(&tLeft);
    mHost->GetWidth(&width);
    mTabView->GetWidth(&tWidth);
    Int32 scrollPos = tLeft - (width - tWidth) / 2;
    mHost->SmoothScrollTo(scrollPos, 0);
    mHost->mTabSelector = NULL;
    return NOERROR;
}


CAR_INTERFACE_IMPL_2(ScrollingTabContainerView::TabView, LinearLayout, ITabView, IViewOnLongClickListener);
ECode ScrollingTabContainerView::TabView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IActionBarTab* tab,
    /* [in] */ Boolean forList,
    /* [in] */ IScrollingTabContainerView* view)
{
    LinearLayout::constructor(context, NULL, R::attr::actionBarTabStyle);
    mTab = tab;

    if (forList) {
        SetGravity(IGravity::START | IGravity::CENTER_VERTICAL);
    }

    Update();
    mHost = (ScrollingTabContainerView*)view;
    return NOERROR;
}

void ScrollingTabContainerView::TabView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    LinearLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    // Re-measure if we went beyond our maximum size.
    Int32 width = 0;
    if (mHost->mMaxTabWidth > 0 && (GetMeasuredWidth(&width), width) > mHost->mMaxTabWidth) {
        LinearLayout::OnMeasure(MeasureSpec::MakeMeasureSpec(mHost->mMaxTabWidth, MeasureSpec::EXACTLY),
                heightMeasureSpec);
    }
}

ECode ScrollingTabContainerView::TabView::BindTab(
    /* [in] */ IActionBarTab* tab)
{
    mTab = tab;
    Update();

    return NOERROR;
}

ECode ScrollingTabContainerView::TabView::SetSelected(
    /* [in] */ Boolean selected)
{
    Boolean tmp = FALSE;
    Boolean changed = ((IsSelected(&tmp), tmp) != selected);
    LinearLayout::SetSelected(selected);
    if (changed && selected) {
        SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_SELECTED);
    }
    return NOERROR;
}

ECode ScrollingTabContainerView::TabView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    LinearLayout::OnInitializeAccessibilityEvent(event);
    // This view masquerades as an action bar tab.
    assert(0 && "TODO: class name is right?");
    IAccessibilityRecord::Probe(event)->SetClassName(CoreUtils::Convert(String("ActionBar.Tab.class.getName()")));
    return NOERROR;
}

ECode ScrollingTabContainerView::TabView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    LinearLayout::OnInitializeAccessibilityNodeInfo(info);
    // This view masquerades as an action bar tab.
    assert(0 && "TODO: class name is right?");
    info->SetClassName(CoreUtils::Convert(String("ActionBar.Tab.class.getName()")));
    return NOERROR;
}

ECode ScrollingTabContainerView::TabView::Update()
{
    AutoPtr<IActionBarTab> tab = mTab;
    AutoPtr<IView> custom;
    tab->GetCustomView((IView**)&custom);
    if (custom != NULL) {
        AutoPtr<IViewParent> customParent;
        custom->GetParent((IViewParent**)&customParent);
        if (customParent.Get() != (IViewParent*)this) {
            if (customParent != NULL)
                IViewManager::Probe(customParent)->RemoveView(custom);
            AddView(custom);
        }
        mCustomView = custom;
        if (mTextView != NULL) IView::Probe(mTextView)->SetVisibility(IView::GONE);
        if (mIconView != NULL) {
            IView::Probe(mIconView)->SetVisibility(IView::GONE);
            mIconView->SetImageDrawable(NULL);
        }
    }
    else {
        if (mCustomView != NULL) {
            RemoveView(mCustomView);
            mCustomView = NULL;
        }

        AutoPtr<IDrawable> icon;
        tab->GetIcon((IDrawable**)&icon);
        AutoPtr<ICharSequence> text;
        tab->GetText((ICharSequence**)&text);

        if (icon != NULL) {
            if (mIconView == NULL) {
                AutoPtr<IImageView> iconView;
                AutoPtr<IContext> ctx;
                GetContext((IContext**)&ctx);
                CImageView::New(ctx, (IImageView**)&iconView);
                AutoPtr<ILinearLayoutLayoutParams> lp;
                CLinearLayoutLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
                        IViewGroupLayoutParams::WRAP_CONTENT, (ILinearLayoutLayoutParams**)&lp);
                lp->SetGravity(IGravity::CENTER_VERTICAL);
                IView::Probe(iconView)->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
                ViewGroup::AddView(IView::Probe(iconView), 0);
                mIconView = iconView;
            }
            mIconView->SetImageDrawable(icon);
            IView::Probe(mIconView)->SetVisibility(IView::VISIBLE);
        }
        else if (mIconView != NULL) {
            IView::Probe(mIconView)->SetVisibility(IView::GONE);
            mIconView->SetImageDrawable(NULL);
        }

        Boolean hasText = !TextUtils::IsEmpty(text);
        if (hasText) {
            if (mTextView == NULL) {
                AutoPtr<ITextView> textView;
                AutoPtr<IContext> ctx;
                GetContext((IContext**)&ctx);
                CTextView::New(ctx, NULL, R::attr::actionBarTabTextStyle, (ITextView**)&textView);
                textView->SetEllipsize(TextUtilsTruncateAt_END);
                AutoPtr<ILinearLayoutLayoutParams> lp;
                CLinearLayoutLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
                        IViewGroupLayoutParams::WRAP_CONTENT, (ILinearLayoutLayoutParams**)&lp);
                lp->SetGravity(IGravity::CENTER_VERTICAL);
                IView::Probe(textView)->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
                AddView(IView::Probe(textView));
                mTextView = textView;
            }
            mTextView->SetText(text);
            IView::Probe(mTextView)->SetVisibility(IView::VISIBLE);
        }
        else if (mTextView != NULL) {
            IView::Probe(mTextView)->SetVisibility(IView::GONE);
            mTextView->SetText((ICharSequence*)NULL);
        }

        AutoPtr<ICharSequence> cs;
        tab->GetContentDescription((ICharSequence**)&cs);
        if (mIconView != NULL) {
            IView::Probe(mIconView)->SetContentDescription(cs);
        }

        if (!hasText && !TextUtils::IsEmpty(cs)) {
            SetOnLongClickListener(this);
        }
        else {
            SetOnLongClickListener(NULL);
            SetLongClickable(FALSE);
        }
    }
    return NOERROR;
}

ECode ScrollingTabContainerView::TabView::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* rst)
{
    AutoPtr<ArrayOf<Int32> > screenPos = ArrayOf<Int32>::Alloc(2);
    GetLocationOnScreen(screenPos);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 width;
    GetWidth(&width);
    Int32 height;
    GetHeight(&height);
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    AutoPtr<IDisplayMetrics> dm;
    r->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Int32 screenWidth;
    dm->GetWidthPixels(&screenWidth);

    AutoPtr<IToast> cheatSheet;
    AutoPtr<ICharSequence> cs;
    mTab->GetContentDescription((ICharSequence**)&cs);
    Toast::MakeText(context, cs, IToast::LENGTH_SHORT, (IToast**)&cheatSheet);
    // Show under the tab
    cheatSheet->SetGravity(IGravity::TOP | IGravity::CENTER_HORIZONTAL,
            ((*screenPos)[0] + width / 2) - screenWidth / 2, height);

    cheatSheet->Show();
    *rst = TRUE;
    return NOERROR;
}

ECode ScrollingTabContainerView::TabView::GetTab(
    /* [out] */ IActionBarTab** tab)
{
    VALIDATE_NOT_NULL(tab);
    *tab = mTab;
    REFCOUNT_ADD(*tab);
    return NOERROR;
}

ScrollingTabContainerView::TabAdapter::TabAdapter(
    /* [in] */ ScrollingTabContainerView* host)
{
    mHost = host;
}

ECode ScrollingTabContainerView::TabAdapter::GetCount(
    /* [out] */ Int32* count)
{
    return IViewGroup::Probe(mHost->mTabLayout)->GetChildCount(count);
}

ECode ScrollingTabContainerView::TabAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    AutoPtr<IView> v;
    IViewGroup::Probe(mHost->mTabLayout)->GetChildAt(position, (IView**)&v);
    AutoPtr<IActionBarTab> abt;
    ((TabView*)ITabView::Probe(v))->GetTab((IActionBarTab**)&abt);
    *item = abt.Get();
    REFCOUNT_ADD(*item)
    return NOERROR;
}

ECode ScrollingTabContainerView::TabAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = position;
    return NOERROR;
}

ECode ScrollingTabContainerView::TabAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* _convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> convertView = _convertView;
    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    if (convertView == NULL) {
        AutoPtr<ITabView> v = mHost->CreateTabView(IActionBarTab::Probe(item) , TRUE);
        convertView = IView::Probe(v);
    }
    else {
        ITabView::Probe(convertView)->BindTab(IActionBarTab::Probe(item));
    }
    *view = convertView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}


CAR_INTERFACE_IMPL(ScrollingTabContainerView::TabClickListener, Object, IViewOnClickListener)
ScrollingTabContainerView::TabClickListener::TabClickListener(
    /* [in] */ ScrollingTabContainerView* host)
    : mHost(host)
{
}

ECode ScrollingTabContainerView::TabClickListener::OnClick(
    /* [in] */ IView* view)
{
    AutoPtr<ITabView> tabView = ITabView::Probe(view);
    AutoPtr<IActionBarTab> tab;
    tabView->GetTab((IActionBarTab**)&tab);
    tab->Select();
    Int32 tabCount = 0;
    IViewGroup::Probe(mHost->mTabLayout)->GetChildCount(&tabCount);
    for (Int32 i = 0; i < tabCount; i++) {
        AutoPtr<IView> child;
        IViewGroup::Probe(mHost->mTabLayout)->GetChildAt(i, (IView**)&child);
        child->SetSelected(child.Get() == view);
    }

    return NOERROR;
}


CAR_INTERFACE_IMPL(ScrollingTabContainerView::VisibilityAnimListener, Object, IAnimatorListener)
ScrollingTabContainerView::VisibilityAnimListener::VisibilityAnimListener(
    /* [in] */ ScrollingTabContainerView* host)
    : mHost(host)
    , mCanceled(FALSE)
    , mFinalVisibility(0)
{
}

AutoPtr<ScrollingTabContainerView::VisibilityAnimListener> ScrollingTabContainerView::VisibilityAnimListener::WithFinalVisibility(
    /* [in] */ Int32 visibility)
{
    mFinalVisibility = visibility;
    return this;
}

ECode ScrollingTabContainerView::VisibilityAnimListener::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mHost->SetVisibility(IView::VISIBLE);
    mHost->mVisibilityAnim = animation;
    mCanceled = FALSE;
    return NOERROR;
}

ECode ScrollingTabContainerView::VisibilityAnimListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mCanceled) return NOERROR;

    mHost->mVisibilityAnim = NULL;
    mHost->SetVisibility(mFinalVisibility);
    return NOERROR;
}

ECode ScrollingTabContainerView::VisibilityAnimListener::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mCanceled = TRUE;
    return NOERROR;
}

ECode ScrollingTabContainerView::VisibilityAnimListener::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

static AutoPtr<ITimeInterpolator> InitIpt()
{
    AutoPtr<ITimeInterpolator> ipt;
    CDecelerateInterpolator::New((ITimeInterpolator**)&ipt);
    return ipt;
}

const String ScrollingTabContainerView::TAG("ScrollingTabContainerView");
AutoPtr<ITimeInterpolator> ScrollingTabContainerView::sAlphaInterpolator = InitIpt();
const Int32 ScrollingTabContainerView::FADE_DURATION = 200;

CAR_INTERFACE_IMPL_2(ScrollingTabContainerView, HorizontalScrollView, IScrollingTabContainerView, IAdapterViewOnItemClickListener);
ScrollingTabContainerView::ScrollingTabContainerView()
    : mAllowCollapse(FALSE)
    , mMaxTabWidth(0)
    , mStackedTabMaxWidth(0)
    , mContentHeight(0)
    , mSelectedTabIndex(0)
{
    mVisAnimListener = new VisibilityAnimListener(this);
}

ECode ScrollingTabContainerView::constructor(
    /* [in] */ IContext* context)
{
    HorizontalScrollView::constructor(context);
    SetHorizontalScrollBarEnabled(FALSE);

    AutoPtr<IActionBarPolicy> abp;
    assert(0 && "TODO");
    // CActionBarPolicy::New(context, (IActionBarPolicy**)&abp);
    Int32 height;
    abp->GetTabContainerHeight(&height);
    SetContentHeight(height);
    abp->GetStackedTabMaxWidth(&mStackedTabMaxWidth);

    mTabLayout = CreateTabLayout();
    AutoPtr<IViewGroupLayoutParams> params;
    CViewGroupLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::MATCH_PARENT, (IViewGroupLayoutParams**)&params);
    AddView(IView::Probe(mTabLayout), params);
    return NOERROR;
}

void ScrollingTabContainerView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 widthMode = MeasureSpec::GetMode(widthMeasureSpec);
    Boolean lockedExpanded = widthMode == MeasureSpec::EXACTLY;
    SetFillViewport(lockedExpanded);

    Int32 childCount;
    IViewGroup::Probe(mTabLayout)->GetChildCount(&childCount);
    if (childCount > 1 &&
            (widthMode == MeasureSpec::EXACTLY || widthMode == MeasureSpec::AT_MOST)) {
        if (childCount > 2) {
            mMaxTabWidth = (Int32) (MeasureSpec::GetSize(widthMeasureSpec) * 0.4f);
        }
        else {
            mMaxTabWidth = MeasureSpec::GetSize(widthMeasureSpec) / 2;
        }
        mMaxTabWidth = Elastos::Core::Math::Min(mMaxTabWidth, mStackedTabMaxWidth);
    }
    else {
        mMaxTabWidth = -1;
    }

    heightMeasureSpec = MeasureSpec::MakeMeasureSpec(mContentHeight, MeasureSpec::EXACTLY);

    Boolean canCollapse = !lockedExpanded && mAllowCollapse;

    if (canCollapse) {
        // See if we should expand
        IView::Probe(mTabLayout)->Measure(MeasureSpec::UNSPECIFIED, heightMeasureSpec);

        Int32 width = 0;
        IView::Probe(mTabLayout)->GetMeasuredWidth(&width);
        if (width > MeasureSpec::GetSize(widthMeasureSpec)) {
            PerformCollapse();
        }
        else {
            PerformExpand();
        }
    }
    else {
        PerformExpand();
    }

    Int32 oldWidth = 0;
    GetMeasuredWidth(&oldWidth);
    HorizontalScrollView::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    Int32 newWidth = 0;
    GetMeasuredWidth(&newWidth);

    if (lockedExpanded && oldWidth != newWidth) {
        // Recenter the tab display if we're at a new (scrollable) size.
        SetTabSelected(mSelectedTabIndex);
    }
}

Boolean ScrollingTabContainerView::IsCollapsed()
{
    AutoPtr<IViewParent> p;
    return mTabSpinner != NULL && (IView::Probe(mTabSpinner)->GetParent((IViewParent**)&p), p.Get() == THIS_PROBE(IViewParent));
}

ECode ScrollingTabContainerView::SetAllowCollapse(
    /* [in] */ Boolean allowCollapse)
{
    mAllowCollapse = allowCollapse;

    return NOERROR;
}

void ScrollingTabContainerView::PerformCollapse()
{
    if (IsCollapsed()) return;

    if (mTabSpinner == NULL) {
        mTabSpinner = CreateSpinner();
    }
    RemoveView(IView::Probe(mTabLayout));
    AutoPtr<IViewGroupLayoutParams> params;
    CViewGroupLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::MATCH_PARENT, (IViewGroupLayoutParams**)&params);
    AddView(IView::Probe(mTabSpinner), params);
    AutoPtr<IAdapter> apt;
    IAdapterView::Probe(mTabSpinner)->GetAdapter((IAdapter**)&apt);
    if (apt == NULL) {
        AutoPtr<TabAdapter> adapter = new TabAdapter(this);
        IAdapterView::Probe(mTabSpinner)->SetAdapter((IAdapter*)adapter->Probe(EIID_IAdapter));
    }
    if (mTabSelector != NULL) {
        Boolean result = FALSE;
        RemoveCallbacks(mTabSelector, &result);
        mTabSelector = NULL;
    }
    IAdapterView::Probe(mTabSpinner)->SetSelection(mSelectedTabIndex);
}

Boolean ScrollingTabContainerView::PerformExpand()
{
    if (!IsCollapsed()) return FALSE;

    RemoveView(IView::Probe(mTabSpinner));
    AutoPtr<IViewGroupLayoutParams> params;
    CViewGroupLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::MATCH_PARENT, (IViewGroupLayoutParams**)&params);
    AddView(IView::Probe(mTabLayout), params);
    Int32 position = 0;
    IAdapterView::Probe(mTabSpinner)->GetSelectedItemPosition(&position);
    SetTabSelected(position);
    return FALSE;
}

ECode ScrollingTabContainerView::SetTabSelected(
    /* [in] */ Int32 position)
{
    mSelectedTabIndex = position;
    Int32 tabCount;
    IViewGroup::Probe(mTabLayout)->GetChildCount(&tabCount);

    for (Int32 i = 0; i < tabCount; i++) {
        AutoPtr<IView> child;
        IViewGroup::Probe(mTabLayout)->GetChildAt(i, (IView**)&child);
        Boolean isSelected = i == position;
        child->SetSelected(isSelected);
        if (isSelected) {
            AnimateToTab(position);
        }
    }
    if (mTabSpinner != NULL && position >= 0) {
        IAdapterView::Probe(mTabSpinner)->SetSelection(position);
    }

    return NOERROR;
}

ECode ScrollingTabContainerView::SetContentHeight(
    /* [in] */ Int32 contentHeight)
{
    mContentHeight = contentHeight;
    RequestLayout();

    return NOERROR;
}

AutoPtr<ILinearLayout> ScrollingTabContainerView::CreateTabLayout()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ILinearLayout> tabLayout;
    CLinearLayout::New(context, NULL,
        R::attr::actionBarTabBarStyle, (ILinearLayout**)&tabLayout);
    tabLayout->SetMeasureWithLargestChildEnabled(TRUE);
    tabLayout->SetGravity(IGravity::CENTER);

    AutoPtr<ILinearLayoutLayoutParams> lp;
    CLinearLayoutLayoutParams::New(
        IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::MATCH_PARENT, (ILinearLayoutLayoutParams**)&lp);

    IView::Probe(tabLayout)->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
    return tabLayout;
}

AutoPtr<ISpinner> ScrollingTabContainerView::CreateSpinner()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ISpinner> spinner;
    CSpinner::New(context, NULL, R::attr::actionDropDownStyle, (ISpinner**)&spinner);

    AutoPtr<ILinearLayoutLayoutParams> lp;
    CLinearLayoutLayoutParams::New(
        IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::MATCH_PARENT, (ILinearLayoutLayoutParams**)&lp);

    IView::Probe(spinner)->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
    IAdapterView::Probe(spinner)->SetOnItemClickListener(THIS_PROBE(IAdapterViewOnItemClickListener));
    return spinner;
}

void ScrollingTabContainerView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    HorizontalScrollView::OnConfigurationChanged(newConfig);

    AutoPtr<IActionBarPolicy> abp;
    assert(0 && "TODO");
    // CActionBarPolicy::New(GetContext(), (IActionBarPolicy**)&abp);
    // Action bar can change size on configuration changes.
    // Reread the desired height from the theme-specified style.
    Int32 height;
    abp->GetTabContainerHeight(&height);
    SetContentHeight(height);
    abp->GetStackedTabMaxWidth(&mStackedTabMaxWidth);
}

ECode ScrollingTabContainerView::AnimateToVisibility(
    /* [in] */ Int32 visibility)
{
    if (mVisibilityAnim != NULL) {
        mVisibilityAnim->Cancel();
    }
    if (visibility == IView::VISIBLE) {
        Int32 v = 0;
        if ((GetVisibility(&v), v) != IView::VISIBLE) {
            SetAlpha(0);
        }
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = 1.f;
        AutoPtr<IObjectAnimator> anim = CObjectAnimator::OfFloat(THIS_PROBE(IScrollingTabContainerView), String("alpha"), array);
        IAnimator::Probe(anim)->SetDuration(FADE_DURATION);
        IAnimator::Probe(anim)->SetInterpolator(sAlphaInterpolator);

        IAnimator::Probe(anim)->AddListener(mVisAnimListener->WithFinalVisibility(visibility));
        IAnimator::Probe(anim)->Start();
    }
    else {
        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = 0.f;
        AutoPtr<IObjectAnimator> anim = CObjectAnimator::OfFloat(THIS_PROBE(IScrollingTabContainerView), String("alpha"), array);
        IAnimator::Probe(anim)->SetDuration(FADE_DURATION);
        IAnimator::Probe(anim)->SetInterpolator(sAlphaInterpolator);

        IAnimator::Probe(anim)->AddListener(mVisAnimListener->WithFinalVisibility(visibility));
        IAnimator::Probe(anim)->Start();
    }

    return NOERROR;
}

ECode ScrollingTabContainerView::AnimateToTab(
    /* [in] */ Int32 position)
{
    AutoPtr<IView> tabView;
    IViewGroup::Probe(mTabLayout)->GetChildAt(position, (IView**)&tabView);
    Boolean result = FALSE;
    if (mTabSelector != NULL) {
        RemoveCallbacks(mTabSelector, &result);
    }
    mTabSelector = new AnimateToTabRunnable(tabView, this);
    Post(mTabSelector, &result);
    return NOERROR;
}

ECode ScrollingTabContainerView::OnAttachedToWindow()
{
    HorizontalScrollView::OnAttachedToWindow();
    if (mTabSelector != NULL) {
        // Re-post the selector we saved
        Boolean result = FALSE;
        Post(mTabSelector, &result);
    }
    return NOERROR;
}

ECode ScrollingTabContainerView::OnDetachedFromWindow()
{
    HorizontalScrollView::OnDetachedFromWindow();
    if (mTabSelector != NULL) {
        Boolean result = FALSE;
        RemoveCallbacks(mTabSelector, &result);
    }
    return NOERROR;
}

AutoPtr<ITabView> ScrollingTabContainerView::CreateTabView(
    /* [in] */ IActionBarTab* tab,
    /* [in] */ Boolean forAdapter)
{
    AutoPtr<ITabView> tabView;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    CScrollingTabContainerTabView::New(context, tab, forAdapter,
            THIS_PROBE(IScrollingTabContainerView), (ITabView**)&tabView);
    if (forAdapter) {
        IView::Probe(tabView)->SetBackgroundDrawable(NULL);

        AutoPtr<IAbsListViewLayoutParams> lp;
        CAbsListViewLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
            mContentHeight, (IAbsListViewLayoutParams**)&lp);
        IView::Probe(tabView)->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
    }
    else {
        IView::Probe(tabView)->SetFocusable(TRUE);

        if (mTabClickListener == NULL) {
            mTabClickListener = new TabClickListener(this);
        }
        IView::Probe(tabView)->SetOnClickListener(mTabClickListener);
    }
    return tabView;
}

ECode ScrollingTabContainerView::AddTab(
    /* [in] */ IActionBarTab* tab,
    /* [in] */ Boolean setSelected)
{
    AutoPtr<ITabView> tabView = CreateTabView(tab, FALSE);
    AutoPtr<ILinearLayoutLayoutParams> lp;
    CLinearLayoutLayoutParams::New(0,
        IViewGroupLayoutParams::MATCH_PARENT, 1, (ILinearLayoutLayoutParams**)&lp);
    IViewGroup::Probe(mTabLayout)->AddView(IView::Probe(tabView), IViewGroupLayoutParams::Probe(lp));

    if (mTabSpinner != NULL) {
        AutoPtr<IAdapter> tabAdapter;
        IAdapterView::Probe(mTabSpinner)->GetAdapter((IAdapter**)&tabAdapter);

        IBaseAdapter::Probe(tabAdapter)->NotifyDataSetChanged();
    }
    if (setSelected) {
        IView::Probe(tabView)->SetSelected(TRUE);
    }
    if (mAllowCollapse) {
        RequestLayout();
    }

    return NOERROR;
}

ECode ScrollingTabContainerView::AddTab(
    /* [in] */ IActionBarTab* tab,
    /* [in] */ Int32 position,
    /* [in] */ Boolean setSelected)
{
    AutoPtr<ITabView> tabView = CreateTabView(tab, FALSE);
    AutoPtr<ILinearLayoutLayoutParams> lp;
    CLinearLayoutLayoutParams::New(0,
        IViewGroupLayoutParams::MATCH_PARENT, 1, (ILinearLayoutLayoutParams**)&lp);
    IViewGroup::Probe(mTabLayout)->AddView((IView*)tabView->Probe(EIID_IView), position, IViewGroupLayoutParams::Probe(lp));

    if (mTabSpinner != NULL) {
        AutoPtr<IAdapter> tabAdapter;
        IAdapterView::Probe(mTabSpinner)->GetAdapter((IAdapter**)&tabAdapter);
        IBaseAdapter::Probe(tabAdapter)->NotifyDataSetChanged();
    }
    if (setSelected) {
        IView::Probe(tabView)->SetSelected(TRUE);
    }
    if (mAllowCollapse) {
        RequestLayout();
    }

    return NOERROR;
}

ECode ScrollingTabContainerView::UpdateTab(
    /* [in] */ Int32 position)
{
    AutoPtr<IView> temp;
    IViewGroup::Probe(mTabLayout)->GetChildAt(position, (IView**)&temp);
    AutoPtr<TabView> tabView = (TabView*)(temp.Get());
    tabView->Update();

    if (mTabSpinner != NULL) {
        AutoPtr<IAdapter> adater;
        IAdapterView::Probe(mTabSpinner)->GetAdapter((IAdapter**)&adater);
        IBaseAdapter::Probe(adater)->NotifyDataSetChanged();
    }
    if (mAllowCollapse) {
        RequestLayout();
    }
    return NOERROR;
}

ECode ScrollingTabContainerView::RemoveTabAt(
    /* [in] */ Int32 position)
{
    IViewGroup::Probe(mTabLayout)->RemoveViewAt(position);
    if (mTabSpinner != NULL) {
        AutoPtr<IAdapter> adater;
        IAdapterView::Probe(mTabSpinner)->GetAdapter((IAdapter**)&adater);
        IBaseAdapter::Probe(adater)->NotifyDataSetChanged();
    }
    if (mAllowCollapse) {
        RequestLayout();
    }

    return NOERROR;
}

ECode ScrollingTabContainerView::RemoveAllTabs()
{
    IViewGroup::Probe(mTabLayout)->RemoveAllViews();
    if (mTabSpinner != NULL) {
        AutoPtr<IAdapter> adater;
        IAdapterView::Probe(mTabSpinner)->GetAdapter((IAdapter**)&adater);
        IBaseAdapter::Probe(adater)->NotifyDataSetChanged();
    }
    if (mAllowCollapse) {
        RequestLayout();
    }

    return NOERROR;
}

ECode ScrollingTabContainerView::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<ITabView> tabView = ITabView::Probe(view);
    AutoPtr<IActionBarTab> tab;
    tabView->GetTab((IActionBarTab**)&tab);
    tab->Select();

    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos


#include "elastos/apps/packageinstaller/CaffeinatedScrollView.h"
#include "elastos/apps/packageinstaller/TabsAdapter.h"

using Elastos::Droid::View::CView;
using Elastos::Droid::Widget::EIID_ITabHostOnTabChangeListener;
using Elastos::Droid::Widget::EIID_ITabHostTabContentFactory;
using Elastos::Droid::Widget::ITabWidget;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

//=====================================================================
//                         TabsAdapter::TabInfo
//=====================================================================
TabsAdapter::TabInfo::TabInfo(
    /* [in] */ const String& _tag,
    /* [in] */ IView* _view)
    : mTag(_tag)
    , mView(_view)
{
    // ==================before translated======================
    // tag = _tag;
    // view = _view;
}

AutoPtr<IView> TabsAdapter::TabInfo::GetView()
{
    return mView;
}

//=====================================================================
//                     TabsAdapter::DummyTabFactory
//=====================================================================
CAR_INTERFACE_IMPL(TabsAdapter::DummyTabFactory, Object, ITabHostTabContentFactory)

TabsAdapter::DummyTabFactory::DummyTabFactory(
    /* [in] */ IContext* context)
    : mContext(context)
{
    // ==================before translated======================
    // mContext = context;
}

ECode TabsAdapter::DummyTabFactory::CreateTabContent(
    /* [in] */ const String& tag,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // View v = new View(mContext);
    // v.setMinimumWidth(0);
    // v.setMinimumHeight(0);
    // return v;

    AutoPtr<IView> v;
    CView::New(mContext, (IView**)&v);

    v->SetMinimumWidth(0);
    v->SetMinimumHeight(0);

    *result = v;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//                             TabsAdapter
//=====================================================================
CAR_INTERFACE_IMPL(TabsAdapter, Object, ITabHostOnTabChangeListener)

TabsAdapter::TabsAdapter(
    /* [in] */ IActivity* activity,
    /* [in] */ ITabHost* tabHost,
    /* [in] */ IInterface/*IViewPager*/* pager)
{
    // ==================before translated======================
    // mContext = activity;
    // mTabHost = tabHost;
    // mViewPager = pager;
    // mTabHost.setOnTabChangedListener(this);
    // mViewPager.setAdapter(this);
    // mViewPager.setOnPageChangeListener(this);

    mContext = IContext::Probe(activity);
    mTabHost = tabHost;
    //mViewPager = pager;
    mTabHost->SetOnTabChangedListener(this);
    //mViewPager->SetAdapter(this);
    //mViewPager->SetOnPageChangeListener(this);
}

ECode TabsAdapter::AddTab(
    /* [in] */ ITabSpec* tabSpec,
    /* [in] */ IView* view)
{
    VALIDATE_NOT_NULL(tabSpec);
    VALIDATE_NOT_NULL(view);
    // ==================before translated======================
    // tabSpec.setContent(new DummyTabFactory(mContext));
    // String tag = tabSpec.getTag();
    //
    // TabInfo info = new TabInfo(tag, view);
    // mTabs.add(info);
    // mTabHost.addTab(tabSpec);
    // notifyDataSetChanged();

    AutoPtr<DummyTabFactory> dummy = new DummyTabFactory(mContext);
    tabSpec->SetContent(dummy);
    String tag;
    tabSpec->GetTag(&tag);

    AutoPtr<TabInfo> info = new TabInfo(tag, view);
    mTabs->Add(TO_IINTERFACE(info));
    mTabHost->AddTab(tabSpec);
    //NotifyDataSetChanged();
    return NOERROR;
}

ECode TabsAdapter::GetCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mTabs.size();

    return mTabs->GetSize(result);
}

ECode TabsAdapter::InstantiateItem(
    /* [in] */ IViewGroup* container,
    /* [in] */ Int32 position,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // View view = mTabs.get(position).view;
    // container.addView(view);
    // return view;

    AutoPtr<IInterface> interfaceTmp;
    mTabs->Get(position, (IInterface**)&interfaceTmp);
    IObject* objTmp = IObject::Probe(interfaceTmp);
    TabInfo* tabInfo = (TabInfo*)objTmp;
    AutoPtr<IView> view = tabInfo->GetView();
    container->AddView(view);

    *result = TO_IINTERFACE(view);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TabsAdapter::DestroyItem(
    /* [in] */ IViewGroup* container,
    /* [in] */ Int32 position,
    /* [in] */ IInterface* object)
{
    VALIDATE_NOT_NULL(container);
    VALIDATE_NOT_NULL(object);
    // ==================before translated======================
    // container.removeView((View)object);

    return container->RemoveView(IView::Probe(object));
}

ECode TabsAdapter::IsViewFromObject(
    /* [in] */ IView* view,
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(view);
    VALIDATE_NOT_NULL(object);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return view == object;

    *result = TO_IINTERFACE(view) == TO_IINTERFACE(object);
    return NOERROR;
}

ECode TabsAdapter::SetOnTabChangedListener(
    /* [in] */ ITabHostOnTabChangeListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // mOnTabChangeListener = listener;

    mOnTabChangeListener = listener;
    return NOERROR;
}

ECode TabsAdapter::OnTabChanged(
    /* [in] */ const String& tabId)
{
    // ==================before translated======================
    // int position = mTabHost.getCurrentTab();
    // mViewPager.setCurrentItem(position);
    // if (mOnTabChangeListener != null) {
    //     mOnTabChangeListener.onTabChanged(tabId);
    // }

    Int32 position = 0;
    mTabHost->GetCurrentTab(&position);
    //mViewPager->SetCurrentItem(position);
    if (mOnTabChangeListener != NULL) {
        mOnTabChangeListener->OnTabChanged(tabId);
    }
    return NOERROR;
}

ECode TabsAdapter::OnPageScrolled(
    /* [in] */ Int32 position,
    /* [in] */ Float positionOffset,
    /* [in] */ Int32 positionOffsetPixels)
{
    return NOERROR;
}

ECode TabsAdapter::OnPageSelected(
    /* [in] */ Int32 position)
{
    // ==================before translated======================
    // // Unfortunately when TabHost changes the current tab, it kindly
    // // also takes care of putting focus on it when not in touch mode.
    // // The jerk.
    // // This hack tries to prevent this from pulling focus out of our
    // // ViewPager.
    // TabWidget widget = mTabHost.getTabWidget();
    // int oldFocusability = widget.getDescendantFocusability();
    // widget.setDescendantFocusability(ViewGroup.FOCUS_BLOCK_DESCENDANTS);
    // mTabHost.setCurrentTab(position);
    // widget.setDescendantFocusability(oldFocusability);
    //
    // // Scroll the current tab into visibility if needed.
    // View tab = widget.getChildTabViewAt(position);
    // mTempRect.set(tab.getLeft(), tab.getTop(), tab.getRight(), tab.getBottom());
    // widget.requestRectangleOnScreen(mTempRect, false);
    //
    // // Make sure the scrollbars are visible for a moment after selection
    // final View contentView = mTabs.get(position).view;
    // if (contentView instanceof CaffeinatedScrollView) {
    //     ((CaffeinatedScrollView) contentView).awakenScrollBars();
    // }

    AutoPtr<ITabWidget> widget;
    mTabHost->GetTabWidget((ITabWidget**)&widget);
    Int32 oldFocusability = 0;
    IViewGroup::Probe(widget)->GetDescendantFocusability(&oldFocusability);
    IViewGroup::Probe(widget)->SetDescendantFocusability(IViewGroup::FOCUS_BLOCK_DESCENDANTS);
    mTabHost->SetCurrentTab(position);
    IViewGroup::Probe(widget)->SetDescendantFocusability(oldFocusability);

    // Scroll the current tab into visibility if needed.
    AutoPtr<IView> tab;
    widget->GetChildTabViewAt(position, (IView**)&tab);

    Int32 left, top, right, bottom;
    tab->GetLeft(&left);
    tab->GetTop(&top);
    tab->GetRight(&right);
    tab->GetBottom(&bottom);
    mTempRect->Set(left, top, right, bottom);
    IView::Probe(widget)->RequestRectangleOnScreen(mTempRect, FALSE);

    // Make sure the scrollbars are visible for a moment after selection
    AutoPtr<IInterface> interfaceTmp;
    mTabs->Get(position, (IInterface**)&interfaceTmp);
    IObject* objTmp = IObject::Probe(interfaceTmp.Get());
    TabInfo* tabInfo = (TabInfo*)objTmp;
    AutoPtr<IView> contentView = tabInfo->GetView();
    CaffeinatedScrollView* isInstanceOf = (CaffeinatedScrollView*)contentView.Get();
    if (isInstanceOf) {
        Boolean resTmp;
        isInstanceOf->AwakenScrollBars(&resTmp);
    }
    return NOERROR;
}

ECode TabsAdapter::OnPageScrollStateChanged(
    /* [in] */ Int32 state)
{
    return NOERROR;
}

} // namespace PackageInstaller
} // namespace Apps
} // namespace Elastos



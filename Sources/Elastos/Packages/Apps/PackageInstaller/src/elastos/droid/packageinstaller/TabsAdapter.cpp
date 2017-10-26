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

#include "elastos/droid/packageinstaller/CCaffeinatedScrollView.h"
#include "elastos/droid/packageinstaller/TabsAdapter.h"

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::View::CView;
using Elastos::Droid::Widget::EIID_ITabHostOnTabChangeListener;
using Elastos::Droid::Widget::EIID_ITabHostTabContentFactory;
using Elastos::Droid::Widget::ITabWidget;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
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
}

ECode TabsAdapter::DummyTabFactory::CreateTabContent(
    /* [in] */ const String& tag,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);

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
    /* [in] */ IViewPager* pager)
{
    CArrayList::New((IList**)&mTabs);
    mContext = IContext::Probe(activity);
    mTabHost = tabHost;
    mViewPager = pager;
    mTabHost->SetOnTabChangedListener(this);
    mViewPager->SetAdapter(this);
    mViewPager->SetOnPageChangeListener(this);
    CRect::New((IRect**)&mTempRect);
}

ECode TabsAdapter::AddTab(
    /* [in] */ ITabSpec* tabSpec,
    /* [in] */ IView* view)
{
    AutoPtr<DummyTabFactory> dummy = new DummyTabFactory(mContext);
    tabSpec->SetContent(dummy);
    String tag;
    tabSpec->GetTag(&tag);

    AutoPtr<TabInfo> info = new TabInfo(tag, view);
    mTabs->Add(TO_IINTERFACE(info));
    mTabHost->AddTab(tabSpec);
    NotifyDataSetChanged();
    return NOERROR;
}

ECode TabsAdapter::GetCount(
    /* [out] */ Int32* result)
{
    return mTabs->GetSize(result);
}

ECode TabsAdapter::InstantiateItem(
    /* [in] */ IViewGroup* container,
    /* [in] */ Int32 position,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

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
    return container->RemoveView(IView::Probe(object));
}

ECode TabsAdapter::IsViewFromObject(
    /* [in] */ IView* view,
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = TO_IINTERFACE(view) == TO_IINTERFACE(object);
    return NOERROR;
}

ECode TabsAdapter::SetOnTabChangedListener(
    /* [in] */ ITabHostOnTabChangeListener* listener)
{
    mOnTabChangeListener = listener;
    return NOERROR;
}

ECode TabsAdapter::OnTabChanged(
    /* [in] */ const String& tabId)
{
    Int32 position = 0;
    mTabHost->GetCurrentTab(&position);
    mViewPager->SetCurrentItem(position);
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
    Boolean result;
    IView::Probe(widget)->RequestRectangleOnScreen(mTempRect, FALSE, &result);

    // Make sure the scrollbars are visible for a moment after selection
    AutoPtr<IInterface> interfaceTmp;
    mTabs->Get(position, (IInterface**)&interfaceTmp);
    IObject* objTmp = IObject::Probe(interfaceTmp.Get());
    TabInfo* tabInfo = (TabInfo*)objTmp;
    AutoPtr<IView> contentView = tabInfo->GetView();
    ICaffeinatedScrollView* isInstanceOf = ICaffeinatedScrollView::Probe(contentView);
    if (isInstanceOf) {
        ((CCaffeinatedScrollView*)isInstanceOf)->AwakenScrollBars();
    }
    return NOERROR;
}

ECode TabsAdapter::OnPageScrollStateChanged(
    /* [in] */ Int32 state)
{
    return NOERROR;
}

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos

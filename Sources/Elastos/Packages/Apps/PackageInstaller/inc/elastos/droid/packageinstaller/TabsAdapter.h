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

#ifndef __ELASTOS_APPS_PACKAGEINSTALLER_TABSADAPTER_H__
#define __ELASTOS_APPS_PACKAGEINSTALLER_TABSADAPTER_H__

#include "elastos/droid/support/v4/view/PagerAdapter.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include "elastos/core/Object.h"
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Support::V4::View::PagerAdapter;
using Elastos::Droid::Support::V4::View::IPagerAdapter;
using Elastos::Droid::Support::V4::View::IViewPager;
using Elastos::Droid::Support::V4::View::IViewPagerOnPageChangeListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Droid::Widget::ITabHostOnTabChangeListener;
using Elastos::Droid::Widget::ITabHostTabContentFactory;
using Elastos::Droid::Widget::ITabSpec;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

/**
  * This is a helper class that implements the management of tabs and all
  * details of connecting a ViewPager with associated TabHost.  It relies on a
  * trick.  Normally a tab host has a simple API for supplying a View or
  * Intent that each tab will show.  This is not sufficient for switching
  * between pages.  So instead we make the content part of the tab host
  * 0dp high (it is not shown) and the TabsAdapter supplies its own dummy
  * view to show as the tab content.  It listens to changes in tabs, and takes
  * care of switch to the correct paged in the ViewPager whenever the selected
  * tab changes.
  */
class TabsAdapter
    : public PagerAdapter
    , public ITabHostOnTabChangeListener
    , public IViewPagerOnPageChangeListener
{
public:
    class TabInfo
        : public Object
    {
    public:
        TabInfo(
            /* [in] */ const String& _tag,
            /* [in] */ IView* _view);

        CARAPI_(AutoPtr<IView>) GetView();

    private:
        /*const*/ String mTag;
        /*const*/ AutoPtr<IView> mView;
    };

    class DummyTabFactory
        : public Object
        , public ITabHostTabContentFactory
    {
    public:
        CAR_INTERFACE_DECL()

        DummyTabFactory(
            /* [in] */ IContext* context);

        // @Override
        CARAPI CreateTabContent(
            /* [in] */ const String& tag,
            /* [out] */ IView** result);

    private:
        /*const*/ AutoPtr<IContext> mContext;
    };

public:
    CAR_INTERFACE_DECL()

    TabsAdapter(
        /* [in] */ IActivity* activity,
        /* [in] */ ITabHost* tabHost,
        /* [in] */ IViewPager* pager);

    virtual CARAPI AddTab(
        /* [in] */ ITabSpec* tabSpec,
        /* [in] */ IView* view);

    // @Override
    CARAPI GetCount(
        /* [out] */ Int32* result);

    // @Override
    CARAPI InstantiateItem(
        /* [in] */ IViewGroup* container,
        /* [in] */ Int32 position,
        /* [out] */ IInterface** result);

    // @Override
    CARAPI DestroyItem(
        /* [in] */ IViewGroup* container,
        /* [in] */ Int32 position,
        /* [in] */ IInterface* object);

    // @Override
    CARAPI IsViewFromObject(
        /* [in] */ IView* view,
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    virtual CARAPI SetOnTabChangedListener(
        /* [in] */ ITabHostOnTabChangeListener* listener);

    // @Override
    CARAPI OnTabChanged(
        /* [in] */ const String& tabId);

    // @Override
    CARAPI OnPageScrolled(
        /* [in] */ Int32 position,
        /* [in] */ Float positionOffset,
        /* [in] */ Int32 positionOffsetPixels);

    // @Override
    CARAPI OnPageSelected(
        /* [in] */ Int32 position);

    // @Override
    CARAPI OnPageScrollStateChanged(
        /* [in] */ Int32 state);

private:
    /*const*/ AutoPtr<IContext> mContext;
    /*const*/ AutoPtr<ITabHost> mTabHost;
    /*const*/ AutoPtr<IViewPager> mViewPager;
    /*const*/ AutoPtr<IList> mTabs;
    /*const*/ AutoPtr<IRect> mTempRect;
    AutoPtr<ITabHostOnTabChangeListener> mOnTabChangeListener;
};

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_APPS_PACKAGEINSTALLER_TABSADAPTER_H__


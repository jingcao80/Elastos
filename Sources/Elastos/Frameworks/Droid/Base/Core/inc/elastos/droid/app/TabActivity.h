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

#ifndef __ELASTOS_DROID_APP_TAG_ACTIVITY_H__
#define __ELASTOS_DROID_APP_TAG_ACTIVITY_H__

#include "elastos/droid/app/ActivityGroup.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Droid::Widget::ITabWidget;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * <p>For apps developing against {@link android.os.Build.VERSION_CODES#HONEYCOMB}
 * or later, tabs are typically presented in the UI using the new
 * {@link ActionBar#newTab() ActionBar.newTab()} and
 * related APIs for placing tabs within their action bar area.</p>
 *
 * <p>A replacement for TabActivity can also be implemented by directly using
 * TabHost.  You will need to define a layout that correctly uses a TabHost
 * with a TabWidget as well as an area in which to display your tab content.
 * A typical example would be:</p>
 *
 * {@sample development/samples/Support4Demos/res/layout/fragment_tabs.xml complete}
 *
 * <p>The implementation needs to take over responsibility for switching
 * the shown content when the user switches between tabs.
 *
 * {@sample development/samples/Support4Demos/src/com/example/android/supportv4/app/FragmentTabs.java
 *      complete}
 *
 * <p>Also see the <a href="{@docRoot}resources/samples/Support4Demos/src/com/example/android/supportv4/app/FragmentTabsPager.html">
 * Fragment Tabs Pager</a> sample for an example of using the support library's ViewPager to
 * allow the user to swipe the content to switch between tabs.</p>
 *
 * @deprecated New applications should use Fragments instead of this class;
 * to continue to run on older devices, you can use the v4 support library
 * which provides a version of the Fragment API that is compatible down to
 * {@link android.os.Build.VERSION_CODES#DONUT}.
 */

class ECO_PUBLIC TabActivity
    : public ActivityGroup
    , public ITabActivity
{
public:
    CAR_INTERFACE_DECL()

    TabActivity();

    virtual ~TabActivity();

    /**
     * Sets the default tab that is the first tab highlighted.
     *
     * @param tag the name of the default tab
     */
    CARAPI SetDefaultTab(
        /* [in] */ const String& tag);

    /**
     * Sets the default tab that is the first tab highlighted.
     *
     * @param index the index of the default tab
     */
    CARAPI SetDefaultTab(
        /* [in] */ Int32 index);

    /**
     * Returns the {@link TabHost} the activity is using to host its tabs.
     *
     * @return the {@link TabHost} the activity is using to host its tabs.
     */
    CARAPI GetTabHost(
        /* [out] */ ITabHost** tab);

    /**
     * Returns the {@link TabWidget} the activity is using to draw the actual tabs.
     *
     * @return the {@link TabWidget} the activity is using to draw the actual tabs.
     */
    CARAPI GetTabWidget(
        /* [out] */ ITabWidget** tab);

    /**
     * Updates the screen state (current list and other views) when the
     * content changes.
     *
     *@see Activity#OnContentChanged()
     */
    //@Override
    CARAPI OnContentChanged();

protected:
    CARAPI OnChildTitleChanged(
        /* [in] */ IActivity* childActivity,
        /* [in] */ ICharSequence* title);

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IBundle* state);

    //@Override
    CARAPI OnPostCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

private:
    CARAPI EnsureTabHost();

private:
    AutoPtr<ITabHost> mTabHost;
    String mDefaultTab;
    Int32 mDefaultTabIndex;// = -1;
};


} // namespace App
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_APP_TAG_ACTIVITY_H__


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

/**
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package com.android.settings.applications;

using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreferenceFrameLayout;
using Elastos::Droid::Support::V13::App::IFragmentPagerAdapter;
using Elastos::Droid::Support::V4::View::IPagerTabStrip;
using Elastos::Droid::Support::V4::View::IViewPager;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

using Elastos::Droid::Settings::IR;

public class AppOpsSummary extends Fragment {
    // layout inflater object used to inflate views
    private LayoutInflater mInflater;
    
    private ViewGroup mContentContainer;
    private View mRootView;
    private ViewPager mViewPager;

    CharSequence[] mPageNames;
    static AppOpsState.OpsTemplate[] sPageTemplates = new AppOpsState.OpsTemplate[] {
        AppOpsState.LOCATION_TEMPLATE,
        AppOpsState.PERSONAL_TEMPLATE,
        AppOpsState.MESSAGING_TEMPLATE,
        AppOpsState.MEDIA_TEMPLATE,
        AppOpsState.DEVICE_TEMPLATE
    };

    Int32 mCurPos;

    class MyPagerAdapter extends FragmentPagerAdapter implements ViewPager.OnPageChangeListener {

        public MyPagerAdapter(FragmentManager fm) {
            Super(fm);
        }

        //@Override
        public Fragment GetItem(Int32 position) {
            return new AppOpsCategory(sPageTemplates[position]);
        }

        //@Override
        public Int32 GetCount() {
            return sPageTemplates.length;
        }

        //@Override
        public CharSequence GetPageTitle(Int32 position) {
            return mPageNames[position];
        }

        //@Override
        CARAPI OnPageScrolled(Int32 position, Float positionOffset, Int32 positionOffsetPixels) {
        }

        //@Override
        CARAPI OnPageSelected(Int32 position) {
            mCurPos = position;
        }

        //@Override
        CARAPI OnPageScrollStateChanged(Int32 state) {
            if (state == ViewPager.SCROLL_STATE_IDLE) {
                //UpdateCurrentTab(mCurPos);
            }
        }
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        // initialize the inflater
        mInflater = inflater;

        View rootView = mInflater->Inflate(R.layout.app_ops_summary,
                container, FALSE);
        mContentContainer = container;
        mRootView = rootView;

        mPageNames = GetResources()->GetTextArray(R.array.app_ops_categories);

        mViewPager = (ViewPager) rootView->FindViewById(R.id.pager);
        MyPagerAdapter adapter = new MyPagerAdapter(GetChildFragmentManager());
        mViewPager->SetAdapter(adapter);
        mViewPager->SetOnPageChangeListener(adapter);
        PagerTabStrip tabs = (PagerTabStrip) rootView->FindViewById(R.id.tabs);
        tabs->SetTabIndicatorColorResource(R::Color::theme_accent);

        // We have to do this now because PreferenceFrameLayout looks at it
        // only when the view is added.
        if (container instanceof PreferenceFrameLayout) {
            ((PreferenceFrameLayout.LayoutParams) rootView->GetLayoutParams()).removeBorders = TRUE;
        }

        return rootView;
    }
}

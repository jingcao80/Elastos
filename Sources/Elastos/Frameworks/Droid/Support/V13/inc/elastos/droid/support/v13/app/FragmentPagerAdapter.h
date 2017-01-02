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

#ifndef __ELASTOS_DROID_SUPPORT_V13_APP_FRAGMENTPAGERADAPTER_H__
#define __ELASTOS_DROID_SUPPORT_V13_APP_FRAGMENTPAGERADAPTER_H__

#include "Elastos.Droid.App.h"
#include "elastos/droid/support/v4/view/PagerAdapter.h"

using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::Support::V4::View::PagerAdapter;

namespace Elastos {
namespace Droid {
namespace Support {
namespace V13 {
namespace App {

/**
 * Implementation of {@link android.support.v4.view.PagerAdapter} that
 * represents each page as a {@link android.app.Fragment} that is persistently
 * kept in the fragment manager as long as the user can return to the page.
 *
 * <p>This version of the pager is best for use when there are a handful of
 * typically more static fragments to be paged through, such as a set of tabs.
 * The fragment of each page the user visits will be kept in memory, though its
 * view hierarchy may be destroyed when not visible.  This can result in using
 * a significant amount of memory since fragment instances can hold on to an
 * arbitrary amount of state.  For larger sets of pages, consider
 * {@link FragmentStatePagerAdapter}.
 *
 * <p>When using FragmentPagerAdapter the host ViewPager must have a
 * valid ID set.</p>
 *
 * <p>Subclasses only need to implement {@link #getItem(int)}
 * and {@link #getCount()} to have a working adapter.
 *
 * <p>Here is an example implementation of a pager containing fragments of
 * lists:
 *
 * {@sample development/samples/Support13Demos/src/com/example/android/supportv13/app/FragmentPagerSupport.java
 *      complete}
 *
 * <p>The <code>R.layout.fragment_pager</code> resource of the top-level fragment is:
 *
 * {@sample development/samples/Support13Demos/res/layout/fragment_pager.xml
 *      complete}
 *
 * <p>The <code>R.layout.fragment_pager_list</code> resource containing each
 * individual fragment's layout is:
 *
 * {@sample development/samples/Support13Demos/res/layout/fragment_pager_list.xml
 *      complete}
 */
class FragmentPagerAdapter
    : public PagerAdapter
{
public:
    FragmentPagerAdapter(
        /* [in] */ IFragmentManager* fm)
        : mFragmentManager(fm)
    {}

    /**
     * Return the Fragment associated with a specified position.
     */
    virtual CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IFragment** item) = 0;

    // @Override
    CARAPI StartUpdate(
        /* [in] */ IViewGroup* container);

    // @Override
    CARAPI InstantiateItem(
        /* [in] */ IViewGroup* container,
        /* [in] */ Int32 position,
        /* [out] */ IInterface** newPage);

    // @Override
    CARAPI DestroyItem(
        /* [in] */ IViewGroup* container,
        /* [in] */ Int32 position,
        /* [in] */ IInterface* object);

    // @Override
    CARAPI SetPrimaryItem(
        /* [in] */ IViewGroup* container,
        /* [in] */ Int32 position,
        /* [in] */ IInterface* object);

    // @Override
    CARAPI FinishUpdate(
        /* [in] */ IViewGroup* container);

    // @Override
    CARAPI IsViewFromObject(
        /* [in] */ IView* view,
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SaveState(
        /* [out] */ IParcelable** state);

    // @Override
    CARAPI RestoreState(
        /* [in] */ IParcelable* state,
        /* [in] */ IClassLoader* loader);

    /**
     * Return a unique identifier for the item at the given position.
     *
     * <p>The default implementation returns the given position.
     * Subclasses should override this method if the positions of items can change.</p>
     *
     * @param position Position within this adapter
     * @return Unique identifier for the item at position
     */
    virtual CARAPI_(Int64) GetItemId(
        /* [in] */ Int32 position);

private:
    static CARAPI_(String) MakeFragmentName(
        /* [in] */ Int32 viewId,
        /* [in] */ Int64 id);

private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IFragmentManager> mFragmentManager;
    AutoPtr<IFragmentTransaction> mCurTransaction;
    AutoPtr<IFragment> mCurrentPrimaryItem;
};

} // namespace App
} // namespace V13
} // namespace Support
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SUPPORT_V13_APP_FRAGMENTPAGERADAPTER_H__

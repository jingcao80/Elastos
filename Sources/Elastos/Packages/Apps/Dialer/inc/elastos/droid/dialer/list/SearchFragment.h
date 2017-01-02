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

#ifndef __ELASTOS_DROID_DIALER_LIST_SEARCHFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_LIST_SEARCHFRAGMENT_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/contacts/common/list/PhoneNumberPickerFragment.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Contacts::Common::List::PhoneNumberPickerFragment;
using Elastos::Droid::Contacts::Common::List::IContactEntryListAdapter;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::IAbsListViewOnScrollListener;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

class SearchFragment
    : public PhoneNumberPickerFragment
    , public ISearchFragment
{
private:
    class MyOnScrollListener
        : public Object
        , public IAbsListViewOnScrollListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyOnScrollListener(
            /* [in] */ SearchFragment* host);

        // @Override
        CARAPI OnScrollStateChanged(
            /* [in] */ IAbsListView* view,
            /* [in] */ Int32 scrollState);

        // @Override
        CARAPI OnScroll(
            /* [in] */ IAbsListView* view,
            /* [in] */ Int32 firstVisibleItem,
            /* [in] */ Int32 visibleItemCount,
            /* [in] */ Int32 totalItemCount);

    private:
        SearchFragment* mHost;
    };

public:
    SearchFragment();

    CAR_INTERFACE_DECL();

    CARAPI constructor();

    // @Override
    CARAPI OnAttach(
        /* [in] */ IActivity* activity);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnViewCreated(
        /* [in] */ IView* view,
        /* [in] */ IBundle* savedInstanceState);

    CARAPI SetAddToContactNumber(
        /* [in] */ const String& addToContactNumber);

    /**
     * Updates the position and padding of the search fragment, depending on whether the dialpad is
     * shown. This can be optionally animated.
     * @param animate
     */
    CARAPI UpdatePosition(
        /* [in] */ Boolean animate);

protected:
    // @Override
    CARAPI SetSearchMode(
        /* [in] */ Boolean flag);

    // @Override
    CARAPI_(AutoPtr<IContactEntryListAdapter>) CreateListAdapter();

    // @Override
    CARAPI_(void) OnItemClick(
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

private:
    AutoPtr<IOnListFragmentScrolledListener> mActivityScrollListener;

    /*
     * Stores the untouched user-entered string that is used to populate the add to contacts
     * intent.
     */
    String mAddToContactNumber;
    Int32 mActionBarHeight;
    Int32 mShadowHeight;
    Int32 mPaddingTop;
    Int32 mShowDialpadDuration;
    Int32 mHideDialpadDuration;

    AutoPtr<ISearchFragmentHostInterface> mActivity;
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_SEARCHFRAGMENT_H__

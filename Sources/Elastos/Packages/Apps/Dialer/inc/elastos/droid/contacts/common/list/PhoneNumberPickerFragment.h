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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_PHONENUMBERPICKERFRAGMENT_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_PHONENUMBERPICKERFRAGMENT_H__

#include "elastos/droid/contacts/common/list/ContactEntryListFragment.h"

using Elastos::Droid::Content::ILoader;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IMenuItem;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

/**
 * Fragment containing a phone number list for picking.
 */
class PhoneNumberPickerFragment
    : public ContactEntryListFragment
    , public IOnShortcutIntentCreatedListener
{
private:
    class FilterHeaderClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        FilterHeaderClickListener(
            /* [in] */ PhoneNumberPickerFragment* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* view);

    private:
        PhoneNumberPickerFragment* mHost;
    };

public:
    PhoneNumberPickerFragment();

    CARAPI constructor();

    CAR_INTERFACE_DECL()

    CARAPI_(void) SetDirectorySearchEnabled(
        /* [in] */ Boolean flag);

    CARAPI_(void) SetOnPhoneNumberPickerActionListener(
        /* [in] */ IOnPhoneNumberPickerActionListener* listener);

    CARAPI_(AutoPtr<IOnPhoneNumberPickerActionListener>) GetOnPhoneNumberPickerListener();

    // @Override
    CARAPI RestoreSavedState(
        /* [in] */ IBundle* savedState);

    // @Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    // @Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    /**
     * @param shortcutAction either {@link Intent#ACTION_CALL} or
     *            {@link Intent#ACTION_SENDTO} or null.
     */
    CARAPI_(void) SetShortcutAction(
        /* [in] */ const String& shortcutAction);

    // @Override
    CARAPI OnLoadFinished(
        /* [in] */ ILoader* loader,
        /* [in] */ IInterface* data);

    CARAPI_(void) SetUseCallableUri(
        /* [in] */ Boolean useCallableUri);

    CARAPI_(Boolean) UsesCallableUri();

    CARAPI_(void) PickPhoneNumber(
        /* [in] */ IUri* uri);

    // @Override
    CARAPI OnShortcutIntentCreated(
        /* [in] */ IUri* uri,
        /* [in] */ IIntent* shortcutIntent);

    // @Override
    CARAPI OnPickerResult(
        /* [in] */ IIntent* data);

    // @Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    CARAPI_(AutoPtr<IContactListFilter>) GetFilter();

    CARAPI_(void) SetFilter(
        /* [in] */ IContactListFilter* filter);

    CARAPI_(void) SetPhotoPosition(
        /* [in] */ PhotoPosition photoPosition);

protected:
    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container);

    CARAPI_(Boolean) GetVisibleScrollbarEnabled();

    // @Override
    CARAPI_(void) SetSearchMode(
        /* [in] */ Boolean flag);

    // @Override
    CARAPI_(void) OnItemClick(
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    CARAPI_(void) CacheContactInfo(
        /* [in] */ Int32 position);

    CARAPI_(String) GetPhoneNumber(
        /* [in] */ Int32 position);

    CARAPI_(AutoPtr<IUri>) GetPhoneUri(
        /* [in] */ Int32 position);

    // @Override
    CARAPI_(void) StartLoading();

    // @Override
    CARAPI_(AutoPtr<IContactEntryListAdapter>) CreateListAdapter();

    // @Override
    CARAPI_(void) ConfigureAdapter();

    CARAPI_(void) SetPhotoPosition(
        /* [in] */ IContactEntryListAdapter* adapter);

    // @Override
    CARAPI_(AutoPtr<IView>) InflateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container);

    CARAPI_(void) StartPhoneNumberShortcutIntent(
        /* [in] */ IUri* uri);

private:
    CARAPI_(void) UpdateFilterHeaderView();

private:
    static const String TAG;

    static const Int32 REQUEST_CODE_ACCOUNT_FILTER = 1;

    static const String KEY_SHORTCUT_ACTION;

    static const String KEY_FILTER;

    AutoPtr<IOnPhoneNumberPickerActionListener> mListener;
    String mShortcutAction;

    AutoPtr<IContactListFilter> mFilter;

    AutoPtr<IView> mAccountFilterHeader;
    /**
     * Lives as ListView's header and is shown when {@link #mAccountFilterHeader} is set
     * to View.GONE.
     */
    AutoPtr<IView> mPaddingView;

    /** true if the loader has started at least once. */
    Boolean mLoaderStarted;

    Boolean mUseCallableUri;

    PhotoPosition mPhotoPosition; //= ContactListItemView.getDefaultPhotoPosition(false /* normal/non opposite */);

    AutoPtr<IViewOnClickListener> mFilterHeaderClickListener;// = new FilterHeaderClickListener();

    friend class FilterHeaderClickListener;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_PHONENUMBERPICKERFRAGMENT_H__

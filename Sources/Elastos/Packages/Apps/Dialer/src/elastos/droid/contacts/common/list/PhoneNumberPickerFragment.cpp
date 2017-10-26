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

#include "elastos/droid/contacts/common/list/PhoneNumberPickerFragment.h"
#include "elastos/droid/contacts/common/list/ContactListItemView.h"
#include "elastos/droid/contacts/common/list/DirectoryListLoader.h"
#include "elastos/droid/contacts/common/list/PhoneNumberListAdapter.h"
#include "elastos/droid/contacts/common/list/ShortcutIntentBuilder.h"
#include "elastos/droid/contacts/common/list/ContactListFilterController.h"
#include "elastos/droid/contacts/common/util/AccountFilterUtil.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"
#include <elastos/droid/R.h>

using Elastos::Droid::Contacts::Common::List::EIID_IOnShortcutIntentCreatedListener;
using Elastos::Droid::Contacts::Common::Util::AccountFilterUtil;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

//=================================================================
// PhoneNumberPickerFragment::FilterHeaderClickListener
//=================================================================
CAR_INTERFACE_IMPL(PhoneNumberPickerFragment::FilterHeaderClickListener, Object, IViewOnClickListener)

ECode PhoneNumberPickerFragment::FilterHeaderClickListener::OnClick(
    /* [in] */ IView* view)
{
    AccountFilterUtil::StartAccountFilterActivityForResult(mHost, REQUEST_CODE_ACCOUNT_FILTER,
            mHost->mFilter);
    return NOERROR;
}


//=================================================================
// PhoneNumberPickerFragment
//=================================================================

const String PhoneNumberPickerFragment::TAG("PhoneNumberPickerFragment");
const Int32 PhoneNumberPickerFragment::REQUEST_CODE_ACCOUNT_FILTER;
const String PhoneNumberPickerFragment::KEY_SHORTCUT_ACTION("shortcutAction");
const String PhoneNumberPickerFragment::KEY_FILTER("filter");

PhoneNumberPickerFragment::PhoneNumberPickerFragment()
    : mLoaderStarted(FALSE)
    , mUseCallableUri(FALSE)
{
    mPhotoPosition = ContactListItemView::GetDefaultPhotoPosition(FALSE /* normal/non opposite */);
    mFilterHeaderClickListener = new FilterHeaderClickListener(this);

    SetQuickContactEnabled(FALSE);
    SetPhotoLoaderEnabled(TRUE);
    SetSectionHeaderDisplayEnabled(TRUE);
    SetDirectorySearchMode(DirectoryListLoader::SEARCH_MODE_NONE);

    // Show nothing instead of letting caller Activity show something.
    SetHasOptionsMenu(TRUE);
}

CAR_INTERFACE_IMPL(PhoneNumberPickerFragment, ContactEntryListFragment, IOnShortcutIntentCreatedListener)

ECode PhoneNumberPickerFragment::constructor()
{
    return ContactEntryListFragment::constructor();
}

void PhoneNumberPickerFragment::SetDirectorySearchEnabled(
    /* [in] */ Boolean flag)
{
    SetDirectorySearchMode(flag ? DirectoryListLoader::SEARCH_MODE_DEFAULT : DirectoryListLoader::SEARCH_MODE_NONE);
}

void PhoneNumberPickerFragment::SetOnPhoneNumberPickerActionListener(
    /* [in] */ IOnPhoneNumberPickerActionListener* listener)
{
    mListener = listener;
}

AutoPtr<IOnPhoneNumberPickerActionListener> PhoneNumberPickerFragment::GetOnPhoneNumberPickerListener()
{
    return mListener;
}

ECode PhoneNumberPickerFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container)
{
    FAIL_RETURN(ContactEntryListFragment::OnCreateView(inflater, container))

    AutoPtr<IView> paddingView;
    inflater->Inflate(Elastos::Droid::Dialer::R::layout::contact_detail_list_padding, NULL, FALSE, (IView**)&paddingView);
    AutoPtr<IView> temp;
    paddingView->FindViewById(Elastos::Droid::Dialer::R::id::contact_detail_list_padding, (IView**)&temp);
    mPaddingView = temp;
    AutoPtr<IListView> lv;
    GetListView((IListView**)&lv);
    lv->AddHeaderView(paddingView);

    AutoPtr<IView> view;
    GetView((IView**)&view);
    temp = NULL;
    view->FindViewById(Elastos::Droid::Dialer::R::id::account_filter_header_container, (IView**)&temp);
    mAccountFilterHeader = temp;
    mAccountFilterHeader->SetOnClickListener(mFilterHeaderClickListener);
    UpdateFilterHeaderView();

    SetVisibleScrollbarEnabled(GetVisibleScrollbarEnabled());
    return NOERROR;
}

Boolean PhoneNumberPickerFragment::GetVisibleScrollbarEnabled()
{
    return TRUE;
}

void PhoneNumberPickerFragment::SetSearchMode(
    /* [in] */ Boolean flag)
{
    ContactEntryListFragment::SetSearchMode(flag);
    UpdateFilterHeaderView();
}

void PhoneNumberPickerFragment::UpdateFilterHeaderView()
{
    AutoPtr<IContactListFilter> filter = GetFilter();
    if (mAccountFilterHeader == NULL || filter == NULL) {
        return;
    }
    Boolean isSearchMode;
    Boolean shouldShowHeader =
            (IsSearchMode(&isSearchMode), !isSearchMode) &&
            AccountFilterUtil::UpdateAccountFilterTitleForPhone(mAccountFilterHeader, filter, FALSE);
    if (shouldShowHeader) {
        mPaddingView->SetVisibility(IView::GONE);
        mAccountFilterHeader->SetVisibility(IView::VISIBLE);
    }
    else {
        mPaddingView->SetVisibility(IView::VISIBLE);
        mAccountFilterHeader->SetVisibility(IView::GONE);
    }
}

ECode PhoneNumberPickerFragment::RestoreSavedState(
    /* [in] */ IBundle* savedState)
{
    FAIL_RETURN(ContactEntryListFragment::RestoreSavedState(savedState))

    if (savedState == NULL) {
        return NOERROR;
    }

    AutoPtr<IParcelable> parcel;
    savedState->GetParcelable(KEY_FILTER, (IParcelable**)&parcel);
    mFilter = IContactListFilter::Probe(parcel);
    savedState->GetString(KEY_SHORTCUT_ACTION, &mShortcutAction);
    return NOERROR;
}

ECode PhoneNumberPickerFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    FAIL_RETURN(ContactEntryListFragment::OnSaveInstanceState(outState))
    outState->PutParcelable(KEY_FILTER, IParcelable::Probe(mFilter));
    outState->PutString(KEY_SHORTCUT_ACTION, mShortcutAction);
    return NOERROR;
}

ECode PhoneNumberPickerFragment::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 itemId;
    item->GetItemId(&itemId);
    if (itemId == Elastos::Droid::R::id::home) {  // See ActionBar#setDisplayHomeAsUpEnabled()
        if (mListener != NULL) {
            mListener->OnHomeInActionBarSelected();
        }
        *result = TRUE;
    }
    return ContactEntryListFragment::OnOptionsItemSelected(item, result);
}

void PhoneNumberPickerFragment::SetShortcutAction(
    /* [in] */ const String& shortcutAction)
{
    mShortcutAction = shortcutAction;
}

void PhoneNumberPickerFragment::OnItemClick(
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IUri> phoneUri = GetPhoneUri(position);

    if (phoneUri != NULL) {
        PickPhoneNumber(phoneUri);
    }
    else {
        String number = GetPhoneNumber(position);
        if (!TextUtils::IsEmpty(number)) {
            CacheContactInfo(position);
            mListener->OnCallNumberDirectly(number);
        }
        else {
            Logger::W(TAG, "Item at %d was clicked before adapter is ready. Ignoring", position);
        }
    }
}

void PhoneNumberPickerFragment::CacheContactInfo(
    /* [in] */ Int32 position)
{
    // Not implemented. Hook for child classes
}

String PhoneNumberPickerFragment::GetPhoneNumber(
    /* [in] */ Int32 position)
{
    AutoPtr<IInterface> temp;
    GetAdapter((IInterface**)&temp);
    AutoPtr<IPhoneNumberListAdapter> adapter = IPhoneNumberListAdapter::Probe(temp);
    String number;
    adapter->GetPhoneNumber(position, &number);
    return number;
}

AutoPtr<IUri> PhoneNumberPickerFragment::GetPhoneUri(
    /* [in] */ Int32 position)
{
    AutoPtr<IInterface> temp;
    GetAdapter((IInterface**)&temp);
    AutoPtr<IPhoneNumberListAdapter> adapter = IPhoneNumberListAdapter::Probe(temp);
    AutoPtr<IUri> uri;
    adapter->GetDataUri(position, (IUri**)&uri);
    return uri;
}

void PhoneNumberPickerFragment::StartLoading()
{
    mLoaderStarted = TRUE;
    return ContactEntryListFragment::StartLoading();
}

ECode PhoneNumberPickerFragment::OnLoadFinished(
    /* [in] */ ILoader* loader,
    /* [in] */ IInterface* data)
{
    FAIL_RETURN(ContactEntryListFragment::OnLoadFinished(loader, data))

    // disable scroll bar if there is no data
    Int32 count;
    ICursor::Probe(data)->GetCount(&count);
    SetVisibleScrollbarEnabled(data != NULL && count > 0);
    return NOERROR;
}

void PhoneNumberPickerFragment::SetUseCallableUri(
    /* [in] */ Boolean useCallableUri)
{
    mUseCallableUri = useCallableUri;
}

Boolean PhoneNumberPickerFragment::UsesCallableUri()
{
    return mUseCallableUri;
}

AutoPtr<IContactEntryListAdapter> PhoneNumberPickerFragment::CreateListAdapter()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<PhoneNumberListAdapter> adapter = new PhoneNumberListAdapter();
    adapter->constructor(IContext::Probe(activity));
    adapter->SetDisplayPhotos(TRUE);
    adapter->SetUseCallableUri(mUseCallableUri);
    return IContactEntryListAdapter::Probe(adapter);
}

void PhoneNumberPickerFragment::ConfigureAdapter()
{
    ContactEntryListFragment::ConfigureAdapter();

    AutoPtr<IInterface> temp;
    GetAdapter((IInterface**)&temp);
    AutoPtr<IContactEntryListAdapter> adapter = IContactEntryListAdapter::Probe(temp);
    if (adapter == NULL) {
        return;
    }

    Boolean isSearchMode;
    if ((IsSearchMode(&isSearchMode), !isSearchMode) && mFilter != NULL) {
        adapter->SetFilter(mFilter);
    }

    SetPhotoPosition(adapter);
}

void PhoneNumberPickerFragment::SetPhotoPosition(
    /* [in] */ IContactEntryListAdapter* adapter)
{
    IPhoneNumberListAdapter::Probe(adapter)->SetPhotoPosition(mPhotoPosition);
}

AutoPtr<IView> PhoneNumberPickerFragment::InflateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container)
{
    AutoPtr<IView> view;
    inflater->Inflate(Elastos::Droid::Dialer::R::layout::contact_list_content, NULL, (IView**)&view);
    return view;
}

void PhoneNumberPickerFragment::PickPhoneNumber(
    /* [in] */ IUri* uri)
{
    if (mShortcutAction.IsNull()) {
        mListener->OnPickPhoneNumberAction(uri);
    }
    else {
        StartPhoneNumberShortcutIntent(uri);
    }
}

void PhoneNumberPickerFragment::StartPhoneNumberShortcutIntent(
    /* [in] */ IUri* uri)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<ShortcutIntentBuilder> builder
            = new ShortcutIntentBuilder(IContext::Probe(activity), this);
    builder->CreatePhoneNumberShortcutIntent(uri, mShortcutAction);
}

ECode PhoneNumberPickerFragment::OnShortcutIntentCreated(
    /* [in] */ IUri* uri,
    /* [in] */ IIntent* shortcutIntent)
{
    return mListener->OnShortcutIntentCreated(shortcutIntent);
}

ECode PhoneNumberPickerFragment::OnPickerResult(
    /* [in] */ IIntent* data)
{
    AutoPtr<IUri> uri;
    data->GetData((IUri**)&uri);
    return mListener->OnPickPhoneNumberAction(uri);
}

ECode PhoneNumberPickerFragment::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    if (requestCode == REQUEST_CODE_ACCOUNT_FILTER) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        if (activity != NULL) {
            AccountFilterUtil::HandleAccountFilterResult(
                    ContactListFilterController::GetInstance(IContext::Probe(activity)), resultCode, data);
        }
        else {
            Logger::E(TAG, "getActivity() returns null during Fragment#onActivityResult()");
        }
    }
    return NOERROR;
}

AutoPtr<IContactListFilter> PhoneNumberPickerFragment::GetFilter()
{
    return mFilter;
}

void PhoneNumberPickerFragment::SetFilter(
    /* [in] */ IContactListFilter* filter)
{
    Boolean equals;
    if ((mFilter == NULL && filter == NULL) ||
            (mFilter != NULL && (IObject::Probe(mFilter)->Equals(filter, &equals), equals))) {
        return;
    }

    mFilter = filter;
    if (mLoaderStarted) {
        ReloadData();
    }
    UpdateFilterHeaderView();
}

void PhoneNumberPickerFragment::SetPhotoPosition(
    /* [in] */ PhotoPosition photoPosition)
{
    mPhotoPosition = photoPosition;

    AutoPtr<IInterface> temp;
    GetAdapter((IInterface**)&temp);
    AutoPtr<IPhoneNumberListAdapter> adapter = IPhoneNumberListAdapter::Probe(temp);
    if (adapter != NULL) {
        adapter->SetPhotoPosition(photoPosition);
    }
}

} // namespace List
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

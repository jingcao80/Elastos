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

#include "elastos/droid/dialer/settings/DialerSettingsActivity.h"
#include "Elastos.Droid.Preference.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/dialer/DialtactsActivity.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/os/UserHandle.h"
#include "R.h"

using Elastos::Droid::Dialer::DialtactsActivity;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Preference::IPreferenceActivityHeader;
using Elastos::Droid::Preference::CPreferenceActivityHeader;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Settings {

//=================================================================
// DialerSettingsActivity::HeaderAdapter
//=================================================================

ECode DialerSettingsActivity::HeaderAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IList* objects)
{
    ArrayAdapter::constructor(context, 0, objects);
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
    mInflater = ILayoutInflater::Probe(service);
    return NOERROR;
}

ECode DialerSettingsActivity::HeaderAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<HeaderViewHolder> holder;
    AutoPtr<IView> view;

    if (convertView == NULL) {
        mInflater->Inflate(R::layout::dialer_preferences, parent, FALSE, (IView**)&view);
        holder = new HeaderViewHolder();
        AutoPtr<IView> temp;
        view->FindViewById(R::id::title, (IView**)&temp);
        holder->mTitle = ITextView::Probe(temp);
        temp = NULL;
        view->FindViewById(R::id::summary, (IView**)&temp);
        holder->mSummary = ITextView::Probe(temp);
        view->SetTag(TO_IINTERFACE(holder));
    }
    else {
        view = convertView;
        AutoPtr<IInterface> tag;
        view->GetTag((IInterface**)&tag);
        holder = (HeaderViewHolder*)(IObject*)tag.Get();
    }

    // All view fields must be updated every time, because the view may be recycled
    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    AutoPtr<IPreferenceActivityHeader> header = IPreferenceActivityHeader::Probe(item);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ICharSequence> title;
    header->GetTitle(res, (ICharSequence**)&title);
    holder->mTitle->SetText(title);
    AutoPtr<ICharSequence> summary;
    header->GetSummary(res, (ICharSequence**)&summary);
    if (!TextUtils::IsEmpty(summary)) {
        IView::Probe(holder->mSummary)->SetVisibility(IView::VISIBLE);
        holder->mSummary->SetText(summary);
    }
    else {
        IView::Probe(holder->mSummary)->SetVisibility(IView::GONE);
    }
    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//=================================================================
// DialerSettingsActivity
//=================================================================
const Int32 DialerSettingsActivity::OWNER_HANDLE_ID = 0;

// TODO:
CAR_INTERFACE_IMPL(DialerSettingsActivity, /*AnalyticsPreferenceActivity*/PreferenceActivity, IDialerSettingsActivity);

ECode DialerSettingsActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    PreferenceActivity::OnCreate(savedInstanceState);
    AutoPtr<IPreferenceManagerHelper> helper;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&helper);
    helper->GetDefaultSharedPreferences(IContext::Probe(this), (ISharedPreferences**)&mPreferences);
    return NOERROR;
}

ECode DialerSettingsActivity::OnBuildHeaders(
    /* [in] */ IList* target)
{
    AutoPtr<IPreferenceActivityHeader> generalSettingsHeader;
    CPreferenceActivityHeader::New((IPreferenceActivityHeader**)&generalSettingsHeader);
    generalSettingsHeader->SetTitleRes(R::string::general_settings_label);
    generalSettingsHeader->SetSummaryRes(R::string::general_settings_description);
    generalSettingsHeader->SetFragment(String("Elastos.Droid.Dialer.Settings.CGeneralSettingsFragment"));
    target->Add(generalSettingsHeader);

    // Only add the call settings header if the current user is the primary/owner user.
    if (IsPrimaryUser()) {
        AutoPtr<IPreferenceActivityHeader> callSettingHeader;
        CPreferenceActivityHeader::New((IPreferenceActivityHeader**)&callSettingHeader);
        callSettingHeader->SetTitleRes(R::string::call_settings_label);
        callSettingHeader->SetSummaryRes(R::string::call_settings_description);
        callSettingHeader->SetIntent(DialtactsActivity::GetCallSettingsIntent());
        target->Add(callSettingHeader);
    }

    return NOERROR;
}

#include "elastos/droid/R.h"

ECode DialerSettingsActivity::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 id;
    if (item->GetItemId(&id), id == Elastos::Droid::R::id::home) {
        OnBackPressed();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode DialerSettingsActivity::IsValidFragment(
    /* [in] */ const String& fragmentName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode DialerSettingsActivity::SetListAdapter(
    /* [in] */ IListAdapter* adapter)
{
    if (adapter == NULL) {
        //TODO
        PreferenceActivity::SetListAdapter(NULL);
        // AnalyticsPreferenceActivity::SetListAdapter(NULL);
    }
    else {
        // We don't have access to the hidden getHeaders() method, so grab the headers from
        // the intended adapter and then replace it with our own.
        Int32 headerCount = 0;
        IAdapter::Probe(adapter)->GetCount(&headerCount);
        AutoPtr<IList> headers;
        CArrayList::New((IList**)&headers);
        for (Int32 i = 0; i < headerCount; i++) {
            AutoPtr<IInterface> item;
            IAdapter::Probe(adapter)->GetItem(i, (IInterface**)&item);
            headers->Add(item);
        }
        mHeaderAdapter = new HeaderAdapter();
        mHeaderAdapter->constructor(IContext::Probe(this), headers);

        // TODO
        PreferenceActivity::SetListAdapter(mHeaderAdapter);
        // AnalyticsPreferenceActivity::SetListAdapter(mHeaderAdapter);
    }
    return NOERROR;
}

Boolean DialerSettingsActivity::IsPrimaryUser()
{
    AutoPtr<IInterface> service;
    GetSystemService(IContext::USER_SERVICE, (IInterface**)&service);
    AutoPtr<IUserManager> userManager = IUserManager::Probe(service);
    AutoPtr<IList> userHandles;
    userManager->GetUserProfiles((IList**)&userHandles);
    Int32 size;
    userHandles->GetSize(&size);
    for (Int32 i = 0; i < size; i++){
        AutoPtr<IInterface> item;
        userHandles->Get(i, (IInterface**)&item);
        // TODO
        // userHandles.get(i).myUserId()
        if (UserHandle::GetMyUserId() == OWNER_HANDLE_ID) {
            return TRUE;
        }
    }

    return FALSE;
}

} // Settings
} // Dialer
} // Droid
} // Elastos

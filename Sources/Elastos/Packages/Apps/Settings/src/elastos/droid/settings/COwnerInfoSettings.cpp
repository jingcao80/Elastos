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

#include "elastos/droid/settings/COwnerInfoSettings.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/TextUtils.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Settings {

const String COwnerInfoSettings::EXTRA_SHOW_NICKNAME("show_nickname");

//===============================================================================
//                 COwnerInfoSettings::OnCheckedChangeListener
//===============================================================================

CAR_INTERFACE_IMPL(COwnerInfoSettings::OnCheckedChangeListener, Object, ICompoundButtonOnCheckedChangeListener)

COwnerInfoSettings::OnCheckedChangeListener::OnCheckedChangeListener(
    /* [in] */ COwnerInfoSettings* host)
    : mHost(host)
{}

COwnerInfoSettings::OnCheckedChangeListener::~OnCheckedChangeListener()
{}

ECode COwnerInfoSettings::OnCheckedChangeListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    mHost->mLockPatternUtils->SetOwnerInfoEnabled(isChecked);
    IView::Probe(mHost->mOwnerInfo)->SetEnabled(isChecked); // disable text field if not enabled
    return NOERROR;
}

//===============================================================================
//                 COwnerInfoSettings
//===============================================================================

CAR_OBJECT_IMPL(COwnerInfoSettings)

COwnerInfoSettings::COwnerInfoSettings()
    : mUserId(0)
    , mShowNickname(FALSE)
{}

COwnerInfoSettings::~COwnerInfoSettings()
{}

ECode COwnerInfoSettings::constructor()
{
    return Fragment::constructor();
}

ECode COwnerInfoSettings::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Fragment::OnCreate(savedInstanceState);
    AutoPtr<IBundle> args;
    GetArguments((IBundle**)&args);
    Boolean res;
    if (args != NULL && (args->ContainsKey(EXTRA_SHOW_NICKNAME, &res), res)) {
        args->GetBoolean(EXTRA_SHOW_NICKNAME, &mShowNickname);
    }
    return NOERROR;
}

ECode COwnerInfoSettings::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)

    mView = NULL;
    inflater->Inflate(R::layout::ownerinfo, container, FALSE, (IView**)&mView);
    mUserId = UserHandle::GetMyUserId();
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CLockPatternUtils::New(IContext::Probe(activity), (ILockPatternUtils**)&mLockPatternUtils);
    InitView();
    *result = mView;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void COwnerInfoSettings::InitView()
{
    AutoPtr<IView> tmpNickname;
    mView->FindViewById(R::id::owner_info_nickname, (IView**)&tmpNickname);
    mNickname = IEditText::Probe(tmpNickname);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IUserManagerHelper> helper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
    AutoPtr<IUserManager> um;
    helper->Get(IContext::Probe(activity), (IUserManager**)&um);
    if (!mShowNickname) {
        tmpNickname->SetVisibility(IView::GONE);
    }
    else {
        String name;
        um->GetUserName(&name);
        ITextView::Probe(mNickname)->SetText(CoreUtils::Convert(name));
        tmpNickname->SetSelected(TRUE);
    }

    Boolean enabled;
    mLockPatternUtils->IsOwnerInfoEnabled(&enabled);

    AutoPtr<IView> tmpCheckbox;
    mView->FindViewById(R::id::show_owner_info_on_lockscreen_checkbox, (IView**)&tmpCheckbox);
    mCheckbox = ICheckBox::Probe(tmpCheckbox);
    ICheckable::Probe(mCheckbox)->SetChecked(enabled);
    if (UserHandle::GetMyUserId() != IUserHandle::USER_OWNER) {
        Boolean res;
        um->IsLinkedUser(&res);
        if (res) {
            ITextView::Probe(mCheckbox)->SetText(R::string::show_profile_info_on_lockscreen_label);
        }
        else {
            ITextView::Probe(mCheckbox)->SetText(R::string::show_user_info_on_lockscreen_label);
        }
    }

    AutoPtr<OnCheckedChangeListener> listener = new OnCheckedChangeListener(this);
    ICompoundButton::Probe(mCheckbox)->SetOnCheckedChangeListener(listener);

    String info;
    mLockPatternUtils->GetOwnerInfo(mUserId, &info);

    AutoPtr<IView> tmpOwnerInfo;
    mView->FindViewById(R::id::owner_info_edit_text, (IView**)&tmpOwnerInfo);
    mOwnerInfo = IEditText::Probe(tmpOwnerInfo);
    tmpOwnerInfo->SetEnabled(enabled);
    if (!TextUtils::IsEmpty(info)) {
        ITextView::Probe(mOwnerInfo)->SetText(CoreUtils::Convert(info));
    }
}

ECode COwnerInfoSettings::OnPause()
{
    Fragment::OnPause();
    SaveChanges();
    return NOERROR;
}

void COwnerInfoSettings::SaveChanges()
{
    AutoPtr<ICharSequence> cs;
    ITextView::Probe(mOwnerInfo)->GetText((ICharSequence**)&cs);
    String info;
    cs->ToString(&info);
    mLockPatternUtils->SetOwnerInfo(info, mUserId);
    if (mShowNickname) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IUserManagerHelper> helper;
        CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
        AutoPtr<IUserManager> um;
        helper->Get(IContext::Probe(activity), (IUserManager**)&um);
        String oldName;
        um->GetUserName(&oldName);
        AutoPtr<ICharSequence> newName;
        ITextView::Probe(mNickname)->GetText((ICharSequence**)&newName);
        String _newName = CoreUtils::Unbox(newName);
        if (!TextUtils::IsEmpty(_newName) && !_newName.Equals(oldName)) {
            um->SetUserName(UserHandle::GetMyUserId(), _newName);
        }
    }
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
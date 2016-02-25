/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings;

using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::CompoundButton::IOnCheckedChangeListener;

using Elastos::Droid::Internal::Widget::ILockPatternUtils;

public class OwnerInfoSettings extends Fragment {

    public static const String EXTRA_SHOW_NICKNAME = "show_nickname";

    private View mView;
    private CheckBox mCheckbox;
    private Int32 mUserId;
    private LockPatternUtils mLockPatternUtils;
    private EditText mOwnerInfo;
    private EditText mNickname;
    private Boolean mShowNickname;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        Bundle args = GetArguments();
        if (args != NULL && args->ContainsKey(EXTRA_SHOW_NICKNAME)) {
            mShowNickname = args->GetBoolean(EXTRA_SHOW_NICKNAME);
        }
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        mView = inflater->Inflate(R.layout.ownerinfo, container, FALSE);
        mUserId = UserHandle->MyUserId();
        mLockPatternUtils = new LockPatternUtils(GetActivity());
        InitView();
        return mView;
    }

    private void InitView() {
        mNickname = (EditText) mView->FindViewById(R.id.owner_info_nickname);
        if (!mShowNickname) {
            mNickname->SetVisibility(View.GONE);
        } else {
            mNickname->SetText(UserManager->Get(GetActivity()).GetUserName());
            mNickname->SetSelected(TRUE);
        }

        final Boolean enabled = mLockPatternUtils->IsOwnerInfoEnabled();

        mCheckbox = (CheckBox) mView->FindViewById(R.id.show_owner_info_on_lockscreen_checkbox);
        mCheckbox->SetChecked(enabled);
        if (UserHandle->MyUserId() != UserHandle.USER_OWNER) {
            if (UserManager->Get(GetActivity()).IsLinkedUser()) {
                mCheckbox->SetText(R::string::show_profile_info_on_lockscreen_label);
            } else {
                mCheckbox->SetText(R::string::show_user_info_on_lockscreen_label);
            }
        }
        mCheckbox->SetOnCheckedChangeListener(new OnCheckedChangeListener() {
            CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
                mLockPatternUtils->SetOwnerInfoEnabled(isChecked);
                mOwnerInfo->SetEnabled(isChecked); // disable text field if not enabled
            }
        });

        String info = mLockPatternUtils->GetOwnerInfo(mUserId);

        mOwnerInfo = (EditText) mView->FindViewById(R.id.owner_info_edit_text);
        mOwnerInfo->SetEnabled(enabled);
        if (!TextUtils->IsEmpty(info)) {
            mOwnerInfo->SetText(info);
        }
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        SaveChanges();
    }

    void SaveChanges() {
        String info = mOwnerInfo->GetText()->ToString();
        mLockPatternUtils->SetOwnerInfo(info, mUserId);
        if (mShowNickname) {
            String oldName = UserManager->Get(GetActivity()).GetUserName();
            CharSequence newName = mNickname->GetText();
            if (!TextUtils->IsEmpty(newName) && !newName->Equals(oldName)) {
                UserManager->Get(GetActivity()).SetUserName(UserHandle->MyUserId(),
                        newName->ToString());
            }
        }
    }
}

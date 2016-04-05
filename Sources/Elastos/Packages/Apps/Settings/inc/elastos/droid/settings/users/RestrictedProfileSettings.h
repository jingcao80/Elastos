/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.settings.users;

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;

using Elastos::Utility::IList;

public class RestrictedProfileSettings extends AppRestrictionsFragment
        implements EditUserInfoController.OnContentChangedCallback {

    public static const String FILE_PROVIDER_AUTHORITY = "com.android.settings.files";
    static const Int32 DIALOG_ID_EDIT_USER_INFO = 1;
    private static const Int32 DIALOG_CONFIRM_REMOVE = 2;

    private View mHeaderView;
    private ImageView mUserIconView;
    private TextView mUserNameView;
    private ImageView mDeleteButton;

    private EditUserInfoController mEditUserInfoController =
            new EditUserInfoController();

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        if (icicle != NULL) {
            mEditUserInfoController->OnRestoreInstanceState(icicle);
        }

        Init(icicle);
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        if (mHeaderView == NULL) {
            mHeaderView = LayoutInflater->From(GetActivity()).Inflate(
                    R.layout.user_info_header, NULL);
            SetPinnedHeaderView(mHeaderView);
            mHeaderView->SetOnClickListener(this);
            mUserIconView = (ImageView) mHeaderView->FindViewById(android.R.id.icon);
            mUserNameView = (TextView) mHeaderView->FindViewById(android.R.id.title);
            mDeleteButton = (ImageView) mHeaderView->FindViewById(R.id.delete);
            mDeleteButton->SetOnClickListener(this);
            GetListView()->SetFastScrollEnabled(TRUE);
        }
        // This is going to bind the preferences.
        super->OnActivityCreated(savedInstanceState);
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);
        mEditUserInfoController->OnSaveInstanceState(outState);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        // Check if user still exists
        UserInfo info = GetExistingUser(mUser);
        if (info == NULL) {
            FinishFragment();
        } else {
            ((TextView) mHeaderView->FindViewById(android.R.id.title)).SetText(info.name);
            ((ImageView) mHeaderView->FindViewById(android.R.id.icon)).SetImageDrawable(
                    GetCircularUserIcon());
        }
    }

    private UserInfo GetExistingUser(UserHandle thisUser) {
        final List<UserInfo> users = mUserManager->GetUsers(TRUE); // Only get non-dying
        for (UserInfo user : users) {
            if (user.id == thisUser->GetIdentifier()) {
                return user;
            }
        }
        return NULL;
    }

    //@Override
    CARAPI StartActivityForResult(Intent intent, Int32 requestCode) {
        mEditUserInfoController->StartingActivityForResult();
        super->StartActivityForResult(intent, requestCode);
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        super->OnActivityResult(requestCode, resultCode, data);

        mEditUserInfoController->OnActivityResult(requestCode, resultCode, data);
    }

    //@Override
    CARAPI OnClick(View view) {
        if (view == mHeaderView) {
            ShowDialog(DIALOG_ID_EDIT_USER_INFO);
        } else if (view == mDeleteButton) {
            ShowDialog(DIALOG_CONFIRM_REMOVE);
        } else {
            super->OnClick(view); // in AppRestrictionsFragment
        }
    }

    //@Override
    public Dialog OnCreateDialog(Int32 dialogId) {
        if (dialogId == DIALOG_ID_EDIT_USER_INFO) {
            return mEditUserInfoController->CreateDialog(this, mUserIconView->GetDrawable(),
                    mUserNameView->GetText(), R::string::profile_info_settings_title,
                    this, mUser);
        } else if (dialogId == DIALOG_CONFIRM_REMOVE) {
            Dialog dlg =
                    Utils->CreateRemoveConfirmationDialog(GetActivity(), mUser->GetIdentifier(),
                            new DialogInterface->OnClickListener() {
                                CARAPI OnClick(DialogInterface dialog, Int32 which) {
                                    RemoveUser();
                                }
                            }
                    );
            return dlg;
        }

        return NULL;
    }

    private void RemoveUser() {
        GetView()->Post(new Runnable() {
            CARAPI Run() {
                mUserManager->RemoveUser(mUser->GetIdentifier());
                FinishFragment();
            }
        });
    }

    //@Override
    CARAPI OnPhotoChanged(Drawable photo) {
        mUserIconView->SetImageDrawable(photo);
    }

    //@Override
    CARAPI OnLabelChanged(CharSequence label) {
        mUserNameView->SetText(label);
    }
}

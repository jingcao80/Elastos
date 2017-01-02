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

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IImageView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::Drawable::ICircleFramedDrawable;

/**
 * This class encapsulates a Dialog for editing the user nickname and photo.
 */
public class EditUserInfoController {

    private static const String KEY_AWAITING_RESULT = "awaiting_result";
    private static const String KEY_SAVED_PHOTO = "pending_photo";

    private Dialog mEditUserInfoDialog;
    private Bitmap mSavedPhoto;
    private EditUserPhotoController mEditUserPhotoController;
    private UserHandle mUser;
    private UserManager mUserManager;
    private Boolean mWaitingForActivityResult = FALSE;

    public interface OnContentChangedCallback {
        CARAPI OnPhotoChanged(Drawable photo);
        CARAPI OnLabelChanged(CharSequence label);
    }

    CARAPI Clear() {
        mEditUserInfoDialog = NULL;
        mSavedPhoto = NULL;
    }

    public Dialog GetDialog() {
        return mEditUserInfoDialog;
    }

    CARAPI OnRestoreInstanceState(Bundle icicle) {
        mSavedPhoto = (Bitmap) icicle->GetParcelable(KEY_SAVED_PHOTO);
        mWaitingForActivityResult = icicle->GetBoolean(KEY_AWAITING_RESULT, FALSE);
    }

    CARAPI OnSaveInstanceState(Bundle outState) {
        if (mEditUserInfoDialog != NULL && mEditUserInfoDialog->IsShowing()
                && mEditUserPhotoController != NULL) {
            outState->PutParcelable(KEY_SAVED_PHOTO,
                    mEditUserPhotoController->GetNewUserPhotoBitmap());
        }
        if (mWaitingForActivityResult) {
            outState->PutBoolean(KEY_AWAITING_RESULT,
                    mWaitingForActivityResult);
        }
    }

    CARAPI StartingActivityForResult() {
        mWaitingForActivityResult = TRUE;
    }

    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        mWaitingForActivityResult = FALSE;

        if (mEditUserInfoDialog != NULL && mEditUserInfoDialog->IsShowing()
                && mEditUserPhotoController->OnActivityResult(requestCode, resultCode, data)) {
            return;
        }
    }

    Drawable GetCircularUserIcon(Activity activity) {
        Bitmap userIcon = mUserManager->GetUserIcon(mUser->GetIdentifier());
        if (userIcon == NULL) {
            return NULL;
        }
        CircleFramedDrawable circularIcon =
                CircleFramedDrawable->GetInstance(activity, userIcon);
        return circularIcon;
    }

    public Dialog CreateDialog(final Fragment fragment, final Drawable currentUserIcon,
            final CharSequence currentUserName,
            Int32 titleResId, final OnContentChangedCallback callback, UserHandle user) {
        Activity activity = fragment->GetActivity();
        mUser = user;
        if (mUserManager == NULL) {
            mUserManager = UserManager->Get(activity);
        }
        LayoutInflater inflater = activity->GetLayoutInflater();
        View content = inflater->Inflate(R.layout.edit_user_info_dialog_content, NULL);

        UserInfo info = mUserManager->GetUserInfo(mUser->GetIdentifier());

        final EditText userNameView = (EditText) content->FindViewById(R.id.user_name);
        userNameView->SetText(info.name);

        final ImageView userPhotoView = (ImageView) content->FindViewById(R.id.user_photo);
        Drawable drawable = NULL;
        if (mSavedPhoto != NULL) {
            drawable = CircleFramedDrawable->GetInstance(activity, mSavedPhoto);
        } else {
            drawable = currentUserIcon;
            if (drawable == NULL) {
                drawable = GetCircularUserIcon(activity);
            }
        }
        userPhotoView->SetImageDrawable(drawable);
        mEditUserPhotoController = new EditUserPhotoController(fragment, userPhotoView,
                mSavedPhoto, drawable, mWaitingForActivityResult);
        mEditUserInfoDialog = new AlertDialog->Builder(activity)
                .SetTitle(R::string::profile_info_settings_title)
                .SetView(content)
                .SetCancelable(TRUE)
                .SetPositiveButton(android.R::string::ok, new DialogInterface->OnClickListener() {
                    //@Override
                    CARAPI OnClick(DialogInterface dialog, Int32 which) {
                        if (which == DialogInterface.BUTTON_POSITIVE) {
                            // Update the name if changed.
                            CharSequence userName = userNameView->GetText();
                            if (!TextUtils->IsEmpty(userName)) {
                                if (currentUserName == NULL
                                        || !userName->ToString()->Equals(currentUserName->ToString())) {
                                    if (callback != NULL) {
                                        callback->OnLabelChanged(userName->ToString());
                                    }
                                    mUserManager->SetUserName(mUser->GetIdentifier(),
                                            userName->ToString());
                                }
                            }
                            // Update the photo if changed.
                            Drawable drawable = mEditUserPhotoController->GetNewUserPhotoDrawable();
                            Bitmap bitmap = mEditUserPhotoController->GetNewUserPhotoBitmap();
                            if (drawable != NULL && bitmap != NULL
                                    && !drawable->Equals(currentUserIcon)) {
                                if (callback != NULL) {
                                    callback->OnPhotoChanged(drawable);
                                }
                                new AsyncTask<Void, Void, Void>() {
                                    //@Override
                                    protected Void DoInBackground(Void... params) {
                                        mUserManager->SetUserIcon(mUser->GetIdentifier(),
                                                mEditUserPhotoController->GetNewUserPhotoBitmap());
                                        return NULL;
                                    }
                                }.ExecuteOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR, (Void[]) NULL);
                            }
                            fragment->GetActivity()->RemoveDialog(
                                    RestrictedProfileSettings.DIALOG_ID_EDIT_USER_INFO);
                        }
                        Clear();
                    }
                })
                .SetNegativeButton(android.R::string::cancel, new DialogInterface->OnClickListener() {
                    //@Override
                    CARAPI OnClick(DialogInterface dialog, Int32 which) {
                        Clear();
                    }
                })
                .Create();

        // Make sure the IME is up.
        mEditUserInfoDialog->GetWindow()->SetSoftInputMode(
                WindowManager.LayoutParams.SOFT_INPUT_STATE_VISIBLE);

        return mEditUserInfoDialog;
    }
}

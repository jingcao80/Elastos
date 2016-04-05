/*
 * Copyright (C) 2012 The Android Open Source Project
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

using Elastos::Droid::Settings::IR;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;

public class UserPreference extends Preference {

    public static const Int32 USERID_UNKNOWN = -10;
    public static const Int32 USERID_GUEST_DEFAULTS = -11;

    private OnClickListener mDeleteClickListener;
    private OnClickListener mSettingsClickListener;
    private Int32 mSerialNumber = -1;
    private Int32 mUserId = USERID_UNKNOWN;
    static const Int32 SETTINGS_ID = R.id.manage_user;
    static const Int32 DELETE_ID = R.id.trash_user;

    public UserPreference(Context context, AttributeSet attrs) {
        This(context, attrs, USERID_UNKNOWN, NULL, NULL);
    }

    UserPreference(Context context, AttributeSet attrs, Int32 userId,
            OnClickListener settingsListener,
            OnClickListener deleteListener) {
        Super(context, attrs);
        if (deleteListener != NULL || settingsListener != NULL) {
            SetWidgetLayoutResource(R.layout.preference_user_delete_widget);
        }
        mDeleteClickListener = deleteListener;
        mSettingsClickListener = settingsListener;
        mUserId = userId;
    }

    //@Override
    protected void OnBindView(View view) {
        UserManager um = (UserManager) GetContext()->GetSystemService(Context.USER_SERVICE);
        View deleteDividerView = view->FindViewById(R.id.divider_delete);
        View manageDividerView = view->FindViewById(R.id.divider_manage);
        View deleteView = view->FindViewById(R.id.trash_user);
        if (deleteView != NULL) {
            if (mDeleteClickListener != NULL
                    && !um->HasUserRestriction(UserManager.DISALLOW_REMOVE_USER)) {
                deleteView->SetOnClickListener(mDeleteClickListener);
                deleteView->SetTag(this);
            } else {
                deleteView->SetVisibility(View.GONE);
                deleteDividerView->SetVisibility(View.GONE);
            }
        }
        View manageView = view->FindViewById(R.id.manage_user);
        if (manageView != NULL) {
            if (mSettingsClickListener != NULL) {
                manageView->SetOnClickListener(mSettingsClickListener);
                manageView->SetTag(this);
                if (mDeleteClickListener != NULL) {
                    manageDividerView->SetVisibility(View.GONE);
                }
            } else {
                manageView->SetVisibility(View.GONE);
                manageDividerView->SetVisibility(View.GONE);
            }
        }
        super->OnBindView(view);
    }

    private Int32 GetSerialNumber() {
        if (mUserId == UserHandle->MyUserId()) return Integer.MIN_VALUE;
        if (mSerialNumber < 0) {
            // If the userId is unknown
            if (mUserId == USERID_UNKNOWN) {
                return Integer.MAX_VALUE;
            } else if (mUserId == USERID_GUEST_DEFAULTS) {
                return Integer.MAX_VALUE - 1;
            }
            mSerialNumber = ((UserManager) GetContext()->GetSystemService(Context.USER_SERVICE))
                    .GetUserSerialNumber(mUserId);
            if (mSerialNumber < 0) return mUserId;
        }
        return mSerialNumber;
    }

    public Int32 GetUserId() {
        return mUserId;
    }

    public Int32 CompareTo(Preference another) {
        if (another instanceof UserPreference) {
            return GetSerialNumber() > ((UserPreference) another).GetSerialNumber() ? 1 : -1;
        } else {
            return 1;
        }
    }
}

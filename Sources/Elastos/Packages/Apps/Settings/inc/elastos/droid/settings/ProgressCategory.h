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

package com.android.settings;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;

/**
 * A category with a progress spinner
 */
public class ProgressCategory extends ProgressCategoryBase {

    private Int32 mEmptyTextRes;
    private Boolean mProgress = FALSE;
    private Preference mNoDeviceFoundPreference;
    private Boolean mNoDeviceFoundAdded;

    public ProgressCategory(Context context) {
        This(context, NULL);
    }

    public ProgressCategory(Context context, AttributeSet attrs) {
        Super(context, attrs, 0);
    }

    public ProgressCategory(Context context, AttributeSet attrs,
            Int32 defStyleAttr) {
        This(context, attrs, defStyleAttr, 0);
    }

    public ProgressCategory(Context context, AttributeSet attrs, Int32 defStyleAttr, Int32 defStyleRes) {
        Super(context, attrs, defStyleAttr, defStyleRes);
        SetLayoutResource(R.layout.preference_progress_category);
    }

    CARAPI SetEmptyTextRes(Int32 emptyTextRes) {
        mEmptyTextRes = emptyTextRes;
    }

    //@Override
    CARAPI OnBindView(View view) {
        super->OnBindView(view);
        final View progressBar = view->FindViewById(R.id.scanning_progress);

        Boolean noDeviceFound = (GetPreferenceCount() == 0 ||
                (GetPreferenceCount() == 1 && GetPreference(0) == mNoDeviceFoundPreference));
        progressBar->SetVisibility(mProgress ? View.VISIBLE : View.GONE);

        if (mProgress || !noDeviceFound) {
            if (mNoDeviceFoundAdded) {
                RemovePreference(mNoDeviceFoundPreference);
                mNoDeviceFoundAdded = FALSE;
            }
        } else {
            if (!mNoDeviceFoundAdded) {
                if (mNoDeviceFoundPreference == NULL) {
                    mNoDeviceFoundPreference = new Preference(GetContext());
                    mNoDeviceFoundPreference->SetLayoutResource(R.layout.preference_empty_list);
                    mNoDeviceFoundPreference->SetTitle(mEmptyTextRes);
                    mNoDeviceFoundPreference->SetSelectable(FALSE);
                }
                AddPreference(mNoDeviceFoundPreference);
                mNoDeviceFoundAdded = TRUE;
            }
        }
    }

    //@Override
    CARAPI SetProgress(Boolean progressOn) {
        mProgress = progressOn;
        NotifyChanged();
    }
}

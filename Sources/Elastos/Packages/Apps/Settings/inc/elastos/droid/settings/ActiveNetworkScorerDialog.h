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
 * Copyright (C) 2014 The Android Open Source Project
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
 * limitations under the License
 */

package com.android.settings;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::INetworkScoreManager;
using Elastos::Droid::Net::INetworkScorerAppManager;
using Elastos::Droid::Net::NetworkScorerAppManager::INetworkScorerAppData;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Internal::App::IAlertActivity;
using Elastos::Droid::Internal::App::IAlertController;

/**
 * Dialog to allow a user to select a new network scorer.
 *
 * <p>Finishes with {@link #RESULT_CANCELED} in all circumstances unless the scorer is successfully
 * changed or was already set to the new value (in which case it finishes with {@link #RESULT_OK}).
 */
public class ActiveNetworkScorerDialog extends AlertActivity implements
        DialogInterface.OnClickListener {
    private static const String TAG = "ActiveNetScorerDlg";

    private String mNewPackageName;

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        Intent intent = GetIntent();
        mNewPackageName = intent->GetStringExtra(NetworkScoreManager.EXTRA_PACKAGE_NAME);

        if (!BuildDialog()) {
            Finish();
        }
    }

    //@Override
    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        switch (which) {
            case BUTTON_POSITIVE:
                NetworkScoreManager nsm =
                    (NetworkScoreManager) GetSystemService(Context.NETWORK_SCORE_SERVICE);
                if (nsm->SetActiveScorer(mNewPackageName)) {
                    SetResult(RESULT_OK);
                }
                break;
            case BUTTON_NEGATIVE:
                break;
        }
    }

    private Boolean BuildDialog() {
        NetworkScorerAppData newScorer = NetworkScorerAppManager->GetScorer(this, mNewPackageName);
        if (newScorer == NULL) {
            Logger::E(TAG, "New package " + mNewPackageName + " is not a valid scorer.");
            return FALSE;
        }

        NetworkScorerAppData oldScorer = NetworkScorerAppManager->GetActiveScorer(this);
        if (oldScorer != NULL && TextUtils->Equals(oldScorer.mPackageName, mNewPackageName)) {
            Logger::I(TAG, "New package " + mNewPackageName + " is already the active scorer.");
            // Set RESULT_OK to indicate to the caller that the "switch" was successful.
            SetResult(RESULT_OK);
            return FALSE;
        }

        // Compose dialog.
        CharSequence newName = newScorer.mScorerName;
        final AlertController.AlertParams p = mAlertParams;
        p.mTitle = GetString(R::string::network_scorer_change_active_dialog_title);
        if (oldScorer != NULL) {
            p.mMessage = GetString(R::string::network_scorer_change_active_dialog_text, newName,
                    oldScorer.mScorerName);
        } else {
            p.mMessage = GetString(R::string::network_scorer_change_active_no_previous_dialog_text,
                    newName);
        }
        p.mPositiveButtonText = GetString(R::string::yes);
        p.mNegativeButtonText = GetString(R::string::no);
        p.mPositiveButtonListener = this;
        p.mNegativeButtonListener = this;
        SetupAlert();

        return TRUE;
    }
}

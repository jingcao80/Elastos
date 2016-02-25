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

using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Service::Persistentdata::IPersistentDataBlockManager;
using Elastos::Droid::Internal::Os::Storage::IExternalStorageFormatter;

using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;

/**
 * Confirm and execute a reset of the device to a clean "just out of the box"
 * state.  Multiple confirmations are required: first, a general "are you sure
 * you want to do this?" prompt, followed by a keyguard pattern trace if the user
 * has defined one, followed by a final strongly-worded "THIS WILL ERASE EVERYTHING
 * ON THE PHONE" prompt.  If at any time the phone is allowed to go to sleep, is
 * locked, et cetera, then the confirmation sequence is abandoned.
 *
 * This is the confirmation screen.
 */
public class MasterClearConfirm extends Fragment {

    private View mContentView;
    private Boolean mEraseSdCard;

    /**
     * The user has gone through the multiple confirmation, so now we go ahead
     * and invoke the Checkin Service to reset the device to its factory-default
     * state (rebooting in the process).
     */
    private Button.OnClickListener mFinalClickListener = new Button->OnClickListener() {

        CARAPI OnClick(View v) {
            if (Utils->IsMonkeyRunning()) {
                return;
            }

            final PersistentDataBlockManager pdbManager = (PersistentDataBlockManager)
                    GetActivity()->GetSystemService(Context.PERSISTENT_DATA_BLOCK_SERVICE);

            if (pdbManager != NULL && !pdbManager->GetOemUnlockEnabled()) {
                // if OEM unlock is enabled, this will be wiped during FR process.
                final ProgressDialog progressDialog = GetProgressDialog();
                progressDialog->Show();

                // need to prevent orientation changes as we're about to go into
                // a Int64 IO request, so we won't be able to access inflate resources on flash
                final Int32 oldOrientation = GetActivity()->GetRequestedOrientation();
                GetActivity()->SetRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LOCKED);
                new AsyncTask<Void, Void, Void>() {
                    //@Override
                    protected Void DoInBackground(Void... params) {
                        pdbManager->Wipe();
                        return NULL;
                    }

                    //@Override
                    protected void OnPostExecute(Void aVoid) {
                        progressDialog->Hide();
                        GetActivity()->SetRequestedOrientation(oldOrientation);
                        DoMasterClear();
                    }
                }.Execute();
            } else {
                DoMasterClear();
            }
        }

        private ProgressDialog GetProgressDialog() {
            final ProgressDialog progressDialog = new ProgressDialog(GetActivity());
            progressDialog->SetIndeterminate(TRUE);
            progressDialog->SetCancelable(FALSE);
            progressDialog->SetTitle(
                    GetActivity()->GetString(R::string::master_clear_progress_title));
            progressDialog->SetMessage(
                    GetActivity()->GetString(R::string::master_clear_progress_text));
            return progressDialog;
        }
    };

    private void DoMasterClear() {
        if (mEraseSdCard) {
            Intent intent = new Intent(ExternalStorageFormatter.FORMAT_AND_FACTORY_RESET);
            intent->PutExtra(Intent.EXTRA_REASON, "MasterClearConfirm");
            intent->SetComponent(ExternalStorageFormatter.COMPONENT_NAME);
            GetActivity()->StartService(intent);
        } else {
            Intent intent = new Intent(IIntent::ACTION_MASTER_CLEAR);
            intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
            intent->PutExtra(Intent.EXTRA_REASON, "MasterClearConfirm");
            GetActivity()->SendBroadcast(intent);
            // Intent handling is asynchronous -- assume it will happen soon.
        }
    }

    /**
     * Configure the UI for the final confirmation interaction
     */
    private void EstablishFinalConfirmationState() {
        mContentView->FindViewById(R.id.execute_master_clear)
                .SetOnClickListener(mFinalClickListener);
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        if (UserManager->Get(GetActivity()).HasUserRestriction(
                UserManager.DISALLOW_FACTORY_RESET)) {
            return inflater->Inflate(R.layout.master_clear_disallowed_screen, NULL);
        }
        mContentView = inflater->Inflate(R.layout.master_clear_confirm, NULL);
        EstablishFinalConfirmationState();
        return mContentView;
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        Bundle args = GetArguments();
        mEraseSdCard = args != NULL && args->GetBoolean(MasterClear.ERASE_EXTERNAL_EXTRA);
    }
}

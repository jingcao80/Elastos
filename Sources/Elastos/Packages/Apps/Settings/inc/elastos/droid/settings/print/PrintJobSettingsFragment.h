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

package com.android.settings.print;

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Print::IPrintJob;
using Elastos::Droid::Print::IPrintJobId;
using Elastos::Droid::Print::IPrintJobInfo;
using Elastos::Droid::Print::IPrintManager;
using Elastos::Droid::Print::PrintManager::IPrintJobStateChangeListener;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;

using Elastos::Text::IDateFormat;

/**
 * Fragment for management of a print job.
 */
public class PrintJobSettingsFragment extends SettingsPreferenceFragment {
    private static const Int32 MENU_ITEM_ID_CANCEL = 1;
    private static const Int32 MENU_ITEM_ID_RESTART = 2;

    private static const String EXTRA_PRINT_JOB_ID = "EXTRA_PRINT_JOB_ID";

    private static const String PRINT_JOB_PREFERENCE = "print_job_preference";
    private static const String PRINT_JOB_MESSAGE_PREFERENCE = "print_job_message_preference";

    private Drawable mListDivider;

    private final PrintJobStateChangeListener mPrintJobStateChangeListener =
            new PrintJobStateChangeListener() {
        //@Override
        CARAPI OnPrintJobStateChanged(PrintJobId printJobId) {
            UpdateUi();
        }
    };

    private PrintManager mPrintManager;

    private Preference mPrintJobPreference;
    private Preference mMessagePreference;

    private PrintJobId mPrintJobId;
    private PrintJob mPrintJob;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        AddPreferencesFromResource(R.xml.print_job_settings);
        mPrintJobPreference = FindPreference(PRINT_JOB_PREFERENCE);
        mMessagePreference = FindPreference(PRINT_JOB_MESSAGE_PREFERENCE);

        mPrintManager = ((PrintManager) GetActivity()->GetSystemService(
                Context.PRINT_SERVICE)).GetGlobalPrintManagerForUser(
                        ActivityManager->GetCurrentUser());

        GetActivity()->GetActionBar().SetTitle(R::string::print_print_job);

        ProcessArguments();

        SetHasOptionsMenu(TRUE);
    }

    //@Override
    CARAPI OnViewCreated(View view, Bundle savedInstanceState) {
        super->OnViewCreated(view, savedInstanceState);
        GetListView()->SetEnabled(FALSE);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        mPrintManager->AddPrintJobStateChangeListener(
                mPrintJobStateChangeListener);
        UpdateUi();
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        mPrintManager->RemovePrintJobStateChangeListener(
                mPrintJobStateChangeListener);
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        super->OnCreateOptionsMenu(menu, inflater);

        PrintJob printJob = GetPrintJob();
        if (printJob == NULL) {
            return;
        }

        if (!printJob->GetInfo()->IsCancelling()) {
            MenuItem cancel = menu->Add(0, MENU_ITEM_ID_CANCEL, Menu.NONE,
                    GetString(R::string::print_cancel));
            cancel->SetShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM);
        }

        if (printJob->IsFailed()) {
            MenuItem restart = menu->Add(0, MENU_ITEM_ID_RESTART, Menu.NONE,
                    GetString(R::string::print_restart));
            restart->SetShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM);
        }
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        switch (item->GetItemId()) {
            case MENU_ITEM_ID_CANCEL: {
                GetPrintJob()->Cancel();
                Finish();
                return TRUE;
            }

            case MENU_ITEM_ID_RESTART: {
                GetPrintJob()->Restart();
                Finish();
                return TRUE;
            }
        }

        return super->OnOptionsItemSelected(item);
    }

    private void ProcessArguments() {
        String printJobId = GetArguments()->GetString(EXTRA_PRINT_JOB_ID);
        mPrintJobId = PrintJobId->UnflattenFromString(printJobId);
        if (mPrintJobId == NULL) {
            Finish();
        }
    }

    private PrintJob GetPrintJob() {
        if (mPrintJob == NULL) {
            mPrintJob = mPrintManager->GetPrintJob(mPrintJobId);
        }
        return mPrintJob;
    }

    private void UpdateUi() {
        PrintJob printJob = GetPrintJob();

        if (printJob == NULL) {
            Finish();
            return;
        }

        if (printJob->IsCancelled() || printJob->IsCompleted()) {
            Finish();
            return;
        }

        PrintJobInfo info = printJob->GetInfo();

        switch (info->GetState()) {
            case PrintJobInfo.STATE_QUEUED:
            case PrintJobInfo.STATE_STARTED: {
                if (!printJob->GetInfo()->IsCancelling()) {
                    mPrintJobPreference->SetTitle(GetString(
                            R::string::print_printing_state_title_template, info->GetLabel()));
                } else {
                    mPrintJobPreference->SetTitle(GetString(
                            R::string::print_cancelling_state_title_template, info->GetLabel()));
                }
            } break;

            case PrintJobInfo.STATE_FAILED: {
                mPrintJobPreference->SetTitle(GetString(
                        R::string::print_failed_state_title_template, info->GetLabel()));
            } break;

            case PrintJobInfo.STATE_BLOCKED: {
                if (!printJob->GetInfo()->IsCancelling()) {
                    mPrintJobPreference->SetTitle(GetString(
                            R::string::print_blocked_state_title_template, info->GetLabel()));
                } else {
                    mPrintJobPreference->SetTitle(GetString(
                            R::string::print_cancelling_state_title_template, info->GetLabel()));
                }
            } break;
        }

        mPrintJobPreference->SetSummary(GetString(R::string::print_job_summary,
                info->GetPrinterName(), DateUtils->FormatSameDayTime(
                        info->GetCreationTime(), info->GetCreationTime(), DateFormat.SHORT,
                        DateFormat.SHORT)));

        switch (info->GetState()) {
            case PrintJobInfo.STATE_QUEUED:
            case PrintJobInfo.STATE_STARTED: {
                mPrintJobPreference->SetIcon(R.drawable.ic_print);
            } break;

            case PrintJobInfo.STATE_FAILED:
            case PrintJobInfo.STATE_BLOCKED: {
                mPrintJobPreference->SetIcon(R.drawable.ic_print_error);
            } break;
        }

        String stateReason = info->GetStateReason();
        if (!TextUtils->IsEmpty(stateReason)) {
            if (GetPreferenceScreen()->FindPreference(PRINT_JOB_MESSAGE_PREFERENCE) == NULL) {
                GetPreferenceScreen()->AddPreference(mMessagePreference);
            }
            mMessagePreference->SetSummary(stateReason);
            GetListView()->SetDivider(NULL);
        } else {
            GetPreferenceScreen()->RemovePreference(mMessagePreference);
            GetListView()->SetDivider(mListDivider);
        }

        GetActivity()->InvalidateOptionsMenu();
    }
}

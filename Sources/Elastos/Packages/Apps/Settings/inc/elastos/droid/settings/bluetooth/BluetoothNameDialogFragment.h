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

package com.android.settings.bluetooth;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::IInputFilter;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::Inputmethod::IEditorInfo;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;

/**
 * Dialog fragment for renaming the local Bluetooth device.
 */
public class BluetoothNameDialogFragment extends DialogFragment implements TextWatcher {
    private static const Int32 BLUETOOTH_NAME_MAX_LENGTH_BYTES = 248;

    private AlertDialog mAlertDialog;
    private Button mOkButton;

    // accessed from inner class (not private to avoid thunks)
    static const String TAG = "BluetoothNameDialogFragment";
    final LocalBluetoothAdapter mLocalAdapter;
    EditText mDeviceNameView;

    // This flag is set when the name is updated by code, to distinguish from user changes
    private Boolean mDeviceNameUpdated;

    // This flag is set when the user edits the name (preserved on rotation)
    private Boolean mDeviceNameEdited;

    // Key to save the edited name and edit status for restoring after rotation
    private static const String KEY_NAME = "device_name";
    private static const String KEY_NAME_EDITED = "device_name_edited";

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (action->Equals(BluetoothAdapter.ACTION_LOCAL_NAME_CHANGED)) {
                UpdateDeviceName();
            } else if (action->Equals(BluetoothAdapter.ACTION_STATE_CHANGED) &&
                    (intent->GetIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR) ==
                            BluetoothAdapter.STATE_ON)) {
                UpdateDeviceName();
            }
        }
    };

    public BluetoothNameDialogFragment() {
        LocalBluetoothManager localManager = LocalBluetoothManager->GetInstance(GetActivity());
        mLocalAdapter = localManager->GetBluetoothAdapter();
    }

    //@Override
    public Dialog OnCreateDialog(Bundle savedInstanceState) {
        String deviceName = mLocalAdapter->GetName();
        if (savedInstanceState != NULL) {
            deviceName = savedInstanceState->GetString(KEY_NAME, deviceName);
            mDeviceNameEdited = savedInstanceState->GetBoolean(KEY_NAME_EDITED, FALSE);
        }
        mAlertDialog = new AlertDialog->Builder(GetActivity())
                .SetTitle(R::string::bluetooth_rename_device)
                .SetView(CreateDialogView(deviceName))
                .SetPositiveButton(R::string::bluetooth_rename_button,
                        new DialogInterface->OnClickListener() {
                            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                                String deviceName = mDeviceNameView->GetText()->ToString();
                                SetDeviceName(deviceName);
                            }
                        })
                .SetNegativeButton(android.R::string::cancel, NULL)
                .Create();
        mAlertDialog->GetWindow()->SetSoftInputMode(
                WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);

        return mAlertDialog;
    }

    private void SetDeviceName(String deviceName) {
        Logger::D(TAG, "Setting device name to " + deviceName);
        mLocalAdapter->SetName(deviceName);
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        outState->PutString(KEY_NAME, mDeviceNameView->GetText()->ToString());
        outState->PutBoolean(KEY_NAME_EDITED, mDeviceNameEdited);
    }

    private View CreateDialogView(String deviceName) {
        final LayoutInflater layoutInflater = (LayoutInflater)GetActivity()
            .GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View view = layoutInflater->Inflate(R.layout.dialog_edittext, NULL);
        mDeviceNameView = (EditText) view->FindViewById(R.id.edittext);
        mDeviceNameView->SetFilters(new InputFilter[] {
                new Utf8ByteLengthFilter(BLUETOOTH_NAME_MAX_LENGTH_BYTES)
        });
        mDeviceNameView->SetText(deviceName);    // set initial value before adding listener
        mDeviceNameView->AddTextChangedListener(this);
        mDeviceNameView->SetOnEditorActionListener(new TextView->OnEditorActionListener() {
            //@Override
            public Boolean OnEditorAction(TextView v, Int32 actionId, KeyEvent event) {
                if (actionId == EditorInfo.IME_ACTION_DONE) {
                    SetDeviceName(v->GetText()->ToString());
                    mAlertDialog->Dismiss();
                    return TRUE;    // action handled
                } else {
                    return FALSE;   // not handled
                }
            }
        });
        return view;
    }

    //@Override
    CARAPI OnDestroy() {
        super->OnDestroy();
        mAlertDialog = NULL;
        mDeviceNameView = NULL;
        mOkButton = NULL;
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        if (mOkButton == NULL) {
            mOkButton = mAlertDialog->GetButton(DialogInterface.BUTTON_POSITIVE);
            mOkButton->SetEnabled(mDeviceNameEdited);    // Ok button enabled after user edits
        }
        IntentFilter filter = new IntentFilter();
        filter->AddAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        filter->AddAction(BluetoothAdapter.ACTION_LOCAL_NAME_CHANGED);
        GetActivity()->RegisterReceiver(mReceiver, filter);
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        GetActivity()->UnregisterReceiver(mReceiver);
    }

    void UpdateDeviceName() {
        if (mLocalAdapter != NULL && mLocalAdapter->IsEnabled()) {
            mDeviceNameUpdated = TRUE;
            mDeviceNameEdited = FALSE;
            mDeviceNameView->SetText(mLocalAdapter->GetName());
        }
    }

    CARAPI AfterTextChanged(Editable s) {
        if (mDeviceNameUpdated) {
            // Device name changed by code; disable Ok button until edited by user
            mDeviceNameUpdated = FALSE;
            mOkButton->SetEnabled(FALSE);
        } else {
            mDeviceNameEdited = TRUE;
            if (mOkButton != NULL) {
                mOkButton->SetEnabled(s->Length() != 0);
            }
        }
    }

    /* Not used */
    CARAPI BeforeTextChanged(CharSequence s, Int32 start, Int32 count, Int32 after) {
    }

    /* Not used */
    CARAPI OnTextChanged(CharSequence s, Int32 start, Int32 before, Int32 count) {
    }
}

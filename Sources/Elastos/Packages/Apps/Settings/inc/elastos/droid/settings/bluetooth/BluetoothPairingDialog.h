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

using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::IHtml;
using Elastos::Droid::Text::IInputFilter;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::Text::InputFilter::ILengthFilter;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::App::IAlertActivity;
using Elastos::Droid::Internal::App::IAlertController;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::View::IKeyEvent;

using Elastos::Utility::ILocale;

/**
 * BluetoothPairingDialog asks the user to enter a PIN / Passkey / simple confirmation
 * for pairing with a remote Bluetooth device. It is an activity that appears as a dialog.
 */
public class BluetoothPairingDialog extends AlertActivity implements
        CompoundButton.OnCheckedChangeListener, DialogInterface.OnClickListener, TextWatcher {
    private static const String TAG = "BluetoothPairingDialog";

    private static const Int32 BLUETOOTH_PIN_MAX_LENGTH = 16;
    private static const Int32 BLUETOOTH_PASSKEY_MAX_LENGTH = 6;

    private LocalBluetoothManager mBluetoothManager;
    private CachedBluetoothDeviceManager mCachedDeviceManager;
    private BluetoothDevice mDevice;
    private Int32 mType;
    private String mPairingKey;
    private EditText mPairingView;
    private Button mOkButton;

    /**
     * Dismiss the dialog if the bond state changes to bonded or none,
     * or if pairing was canceled for {@link #mDevice}.
     */
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (BluetoothDevice.ACTION_BOND_STATE_CHANGED->Equals(action)) {
                Int32 bondState = intent->GetIntExtra(BluetoothDevice.EXTRA_BOND_STATE,
                                                   BluetoothDevice.ERROR);
                if (bondState == BluetoothDevice.BOND_BONDED ||
                        bondState == BluetoothDevice.BOND_NONE) {
                    Dismiss();
                }
            } else if (BluetoothDevice.ACTION_PAIRING_CANCEL->Equals(action)) {
                BluetoothDevice device = intent->GetParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                if (device == NULL || device->Equals(mDevice)) {
                    Dismiss();
                }
            }
        }
    };

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        Intent intent = GetIntent();
        if (!intent->GetAction()->Equals(BluetoothDevice.ACTION_PAIRING_REQUEST))
        {
            Logger::E(TAG, "Error: this activity may be started only with intent " +
                  BluetoothDevice.ACTION_PAIRING_REQUEST);
            Finish();
            return;
        }

        mBluetoothManager = LocalBluetoothManager->GetInstance(this);
        if (mBluetoothManager == NULL) {
            Logger::E(TAG, "Error: BluetoothAdapter not supported by system");
            Finish();
            return;
        }
        mCachedDeviceManager = mBluetoothManager->GetCachedDeviceManager();

        mDevice = intent->GetParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
        mType = intent->GetIntExtra(BluetoothDevice.EXTRA_PAIRING_VARIANT, BluetoothDevice.ERROR);

        switch (mType) {
            case BluetoothDevice.PAIRING_VARIANT_PIN:
            case BluetoothDevice.PAIRING_VARIANT_PASSKEY:
                CreateUserEntryDialog();
                break;

            case BluetoothDevice.PAIRING_VARIANT_PASSKEY_CONFIRMATION:
                Int32 passkey =
                    intent->GetIntExtra(BluetoothDevice.EXTRA_PAIRING_KEY, BluetoothDevice.ERROR);
                if (passkey == BluetoothDevice.ERROR) {
                    Logger::E(TAG, "Invalid Confirmation Passkey received, not showing any dialog");
                    return;
                }
                mPairingKey = String->Format(Locale.US, "%06d", passkey);
                CreateConfirmationDialog();
                break;

            case BluetoothDevice.PAIRING_VARIANT_CONSENT:
            case BluetoothDevice.PAIRING_VARIANT_OOB_CONSENT:
                CreateConsentDialog();
                break;

            case BluetoothDevice.PAIRING_VARIANT_DISPLAY_PASSKEY:
            case BluetoothDevice.PAIRING_VARIANT_DISPLAY_PIN:
                Int32 pairingKey =
                    intent->GetIntExtra(BluetoothDevice.EXTRA_PAIRING_KEY, BluetoothDevice.ERROR);
                if (pairingKey == BluetoothDevice.ERROR) {
                    Logger::E(TAG, "Invalid Confirmation Passkey or PIN received, not showing any dialog");
                    return;
                }
                if (mType == BluetoothDevice.PAIRING_VARIANT_DISPLAY_PASSKEY) {
                    mPairingKey = String->Format("%06d", pairingKey);
                } else {
                    mPairingKey = String->Format("%04d", pairingKey);
                }
                CreateDisplayPasskeyOrPinDialog();
                break;

            default:
                Logger::E(TAG, "Incorrect pairing type received, not showing any dialog");
        }

        /*
         * Leave this registered through pause/resume since we still want to
         * finish the activity in the background if pairing is canceled.
         */
        RegisterReceiver(mReceiver, new IntentFilter(BluetoothDevice.ACTION_PAIRING_CANCEL));
        RegisterReceiver(mReceiver, new IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED));
    }

    private void CreateUserEntryDialog() {
        final AlertController.AlertParams p = mAlertParams;
        p.mTitle = GetString(R::string::bluetooth_pairing_request);
        p.mView = CreatePinEntryView();
        p.mPositiveButtonText = GetString(android.R::string::ok);
        p.mPositiveButtonListener = this;
        p.mNegativeButtonText = GetString(android.R::string::cancel);
        p.mNegativeButtonListener = this;
        SetupAlert();

        mOkButton = mAlert->GetButton(BUTTON_POSITIVE);
        mOkButton->SetEnabled(FALSE);
    }

    private View CreatePinEntryView() {
        View view = GetLayoutInflater()->Inflate(R.layout.bluetooth_pin_entry, NULL);
        TextView messageViewCaption = (TextView) view->FindViewById(R.id.message_caption);
        TextView messageViewContent = (TextView) view->FindViewById(R.id.message_subhead);
        TextView messageView2 = (TextView) view->FindViewById(R.id.message_below_pin);
        CheckBox alphanumericPin = (CheckBox) view->FindViewById(R.id.alphanumeric_pin);
        mPairingView = (EditText) view->FindViewById(R.id.text);
        mPairingView->AddTextChangedListener(this);
        alphanumericPin->SetOnCheckedChangeListener(this);

        Int32 messageId1;
        Int32 messageId2;
        Int32 maxLength;
        switch (mType) {
            case BluetoothDevice.PAIRING_VARIANT_PIN:
                messageId1 = R::string::bluetooth_enter_pin_msg;
                messageId2 = R::string::bluetooth_enter_pin_other_device;
                // Maximum of 16 characters in a PIN
                maxLength = BLUETOOTH_PIN_MAX_LENGTH;
                break;

            case BluetoothDevice.PAIRING_VARIANT_PASSKEY:
                messageId1 = R::string::bluetooth_enter_pin_msg;
                messageId2 = R::string::bluetooth_enter_passkey_other_device;
                // Maximum of 6 digits for passkey
                maxLength = BLUETOOTH_PASSKEY_MAX_LENGTH;
                alphanumericPin->SetVisibility(View.GONE);
                break;

            default:
                Logger::E(TAG, "Incorrect pairing type for createPinEntryView: " + mType);
                return NULL;
        }

        messageViewCaption->SetText(messageId1);
        messageViewContent->SetText(mCachedDeviceManager->GetName(mDevice));
        messageView2->SetText(messageId2);
        mPairingView->SetInputType(InputType.TYPE_CLASS_NUMBER);
        mPairingView->SetFilters(new InputFilter[] {
                new LengthFilter(maxLength) });

        return view;
    }

    private View CreateView() {
        View view = GetLayoutInflater()->Inflate(R.layout.bluetooth_pin_confirm, NULL);
        // Escape device name to avoid HTML injection.
        String name = Html->EscapeHtml(mCachedDeviceManager->GetName(mDevice));
        TextView messageViewCaption = (TextView) view->FindViewById(R.id.message_caption);
        TextView messageViewContent = (TextView) view->FindViewById(R.id.message_subhead);
        TextView pairingViewCaption = (TextView) view->FindViewById(R.id.pairing_caption);
        TextView pairingViewContent = (TextView) view->FindViewById(R.id.pairing_subhead);
        TextView messagePairing = (TextView) view->FindViewById(R.id.pairing_code_message);

        String messageCaption = NULL;
        String pairingContent = NULL;
        switch (mType) {
            case BluetoothDevice.PAIRING_VARIANT_DISPLAY_PASSKEY:
            case BluetoothDevice.PAIRING_VARIANT_DISPLAY_PIN:
                messagePairing->SetVisibility(View.VISIBLE);
            case BluetoothDevice.PAIRING_VARIANT_PASSKEY_CONFIRMATION:
                messageCaption = GetString(R::string::bluetooth_enter_pin_msg);
                pairingContent = mPairingKey;
                break;

            case BluetoothDevice.PAIRING_VARIANT_CONSENT:
            case BluetoothDevice.PAIRING_VARIANT_OOB_CONSENT:
                messagePairing->SetVisibility(View.VISIBLE);
                messageCaption = GetString(R::string::bluetooth_enter_pin_msg);
                break;

            default:
                Logger::E(TAG, "Incorrect pairing type received, not creating view");
                return NULL;
        }

        if (messageViewCaption != NULL) {
            messageViewCaption->SetText(messageCaption);
            messageViewContent->SetText(name);
        }

        if (pairingContent != NULL) {
            pairingViewCaption->SetVisibility(View.VISIBLE);
            pairingViewContent->SetVisibility(View.VISIBLE);
            pairingViewContent->SetText(pairingContent);
        }

        return view;
    }

    private void CreateConfirmationDialog() {
        final AlertController.AlertParams p = mAlertParams;
        p.mTitle = GetString(R::string::bluetooth_pairing_request);
        p.mView = CreateView();
        p.mPositiveButtonText = GetString(R::string::bluetooth_pairing_accept);
        p.mPositiveButtonListener = this;
        p.mNegativeButtonText = GetString(R::string::bluetooth_pairing_decline);
        p.mNegativeButtonListener = this;
        SetupAlert();
    }

    private void CreateConsentDialog() {
        final AlertController.AlertParams p = mAlertParams;
        p.mTitle = GetString(R::string::bluetooth_pairing_request);
        p.mView = CreateView();
        p.mPositiveButtonText = GetString(R::string::bluetooth_pairing_accept);
        p.mPositiveButtonListener = this;
        p.mNegativeButtonText = GetString(R::string::bluetooth_pairing_decline);
        p.mNegativeButtonListener = this;
        SetupAlert();
    }

    private void CreateDisplayPasskeyOrPinDialog() {
        final AlertController.AlertParams p = mAlertParams;
        p.mTitle = GetString(R::string::bluetooth_pairing_request);
        p.mView = CreateView();
        p.mNegativeButtonText = GetString(android.R::string::cancel);
        p.mNegativeButtonListener = this;
        SetupAlert();

        // Since its only a notification, send an OK to the framework,
        // indicating that the dialog has been displayed.
        if (mType == BluetoothDevice.PAIRING_VARIANT_DISPLAY_PASSKEY) {
            mDevice->SetPairingConfirmation(TRUE);
        } else if (mType == BluetoothDevice.PAIRING_VARIANT_DISPLAY_PIN) {
            Byte[] pinBytes = BluetoothDevice->ConvertPinToBytes(mPairingKey);
            mDevice->SetPin(pinBytes);
        }
    }

    //@Override
    protected void OnDestroy() {
        super->OnDestroy();
        UnregisterReceiver(mReceiver);
    }

    CARAPI AfterTextChanged(Editable s) {
        if (mOkButton != NULL) {
            mOkButton->SetEnabled(s->Length() > 0);
        }
    }

    private void AllowPhonebookAccess() {
        CachedBluetoothDevice cachedDevice = mCachedDeviceManager->FindDevice(mDevice);
        if (cachedDevice == NULL) {
            cachedDevice = mCachedDeviceManager->AddDevice(
                    mBluetoothManager->GetBluetoothAdapter(),
                    mBluetoothManager->GetProfileManager(),
                    mDevice);
        }
        cachedDevice->SetPhonebookPermissionChoice(CachedBluetoothDevice.ACCESS_ALLOWED);
    }

    private void OnPair(String value) {
        AllowPhonebookAccess();

        switch (mType) {
            case BluetoothDevice.PAIRING_VARIANT_PIN:
                Byte[] pinBytes = BluetoothDevice->ConvertPinToBytes(value);
                if (pinBytes == NULL) {
                    return;
                }
                mDevice->SetPin(pinBytes);
                break;

            case BluetoothDevice.PAIRING_VARIANT_PASSKEY:
                Int32 passkey = Integer->ParseInt(value);
                mDevice->SetPasskey(passkey);
                break;

            case BluetoothDevice.PAIRING_VARIANT_PASSKEY_CONFIRMATION:
            case BluetoothDevice.PAIRING_VARIANT_CONSENT:
                mDevice->SetPairingConfirmation(TRUE);
                break;

            case BluetoothDevice.PAIRING_VARIANT_DISPLAY_PASSKEY:
            case BluetoothDevice.PAIRING_VARIANT_DISPLAY_PIN:
                // Do nothing.
                break;

            case BluetoothDevice.PAIRING_VARIANT_OOB_CONSENT:
                mDevice->SetRemoteOutOfBandData();
                break;

            default:
                Logger::E(TAG, "Incorrect pairing type received");
        }
    }

    private void OnCancel() {
        mDevice->CancelPairingUserInput();
    }

    public Boolean OnKeyDown(Int32 keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            OnCancel();
        }
        return super->OnKeyDown(keyCode,event);
    }

    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        switch (which) {
            case BUTTON_POSITIVE:
                if (mPairingView != NULL) {
                    OnPair(mPairingView->GetText()->ToString());
                } else {
                    OnPair(NULL);
                }
                break;

            case BUTTON_NEGATIVE:
            default:
                OnCancel();
                break;
        }
    }

    /* Not used */
    CARAPI BeforeTextChanged(CharSequence s, Int32 start, Int32 count, Int32 after) {
    }

    /* Not used */
    CARAPI OnTextChanged(CharSequence s, Int32 start, Int32 before, Int32 count) {
    }

    CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
        // change input type for soft keyboard to numeric or alphanumeric
        if (isChecked) {
            mPairingView->SetInputType(InputType.TYPE_CLASS_TEXT);
        } else {
            mPairingView->SetInputType(InputType.TYPE_CLASS_NUMBER);
        }
    }
}

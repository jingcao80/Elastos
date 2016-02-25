

package com.android.settings.wifi;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Nfc::INdefMessage;
using Elastos::Droid::Nfc::INdefRecord;
using Elastos::Droid::Nfc::INfcAdapter;
using Elastos::Droid::Nfc::ITag;
using Elastos::Droid::Nfc::Tech::INdef;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Inputmethod::IInputMethodManager;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;


class WriteWifiConfigToNfcDialog extends AlertDialog
        implements TextWatcher, View.OnClickListener, CompoundButton.OnCheckedChangeListener {

    private static const String NFC_TOKEN_MIME_TYPE = "application/vnd.wfa.wsc";

    private static const String TAG = WriteWifiConfigToNfcDialog.class->GetName()->ToString();
    private static const String PASSWORD_FORMAT = "102700%s%s";
    private static const Int32 HEX_RADIX = 16;
    private static const Char32[] hexArray = "0123456789ABCDEF".ToCharArray();

    private final PowerManager.WakeLock mWakeLock;

    private AccessPoint mAccessPoint;
    private View mView;
    private Button mSubmitButton;
    private Button mCancelButton;
    private Handler mOnTextChangedHandler;
    private TextView mPasswordView;
    private TextView mLabelView;
    private CheckBox mPasswordCheckBox;
    private ProgressBar mProgressBar;
    private WifiManager mWifiManager;
    private String mWpsNfcConfigurationToken;
    private Context mContext;

    WriteWifiConfigToNfcDialog(Context context, AccessPoint accessPoint,
            WifiManager wifiManager) {
        Super(context);

        mContext = context;
        mWakeLock = ((PowerManager) context->GetSystemService(Context.POWER_SERVICE))
                .NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "WriteWifiConfigToNfcDialog:wakeLock");
        mAccessPoint = accessPoint;
        mOnTextChangedHandler = new Handler();
        mWifiManager = wifiManager;
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        mView = GetLayoutInflater()->Inflate(R.layout.write_wifi_config_to_nfc, NULL);

        SetView(mView);
        SetInverseBackgroundForced(TRUE);
        SetTitle(R::string::setup_wifi_nfc_tag);
        SetCancelable(TRUE);
        SetButton(DialogInterface.BUTTON_NEUTRAL,
                mContext->GetResources()->GetString(R::string::write_tag), (OnClickListener) NULL);
        SetButton(DialogInterface.BUTTON_NEGATIVE,
                mContext->GetResources()->GetString(R::string::cancel),
                (OnClickListener) NULL);

        mPasswordView = (TextView) mView->FindViewById(R.id.password);
        mLabelView = (TextView) mView->FindViewById(R.id.password_label);
        mPasswordView->AddTextChangedListener(this);
        mPasswordCheckBox = (CheckBox) mView->FindViewById(R.id.show_password);
        mPasswordCheckBox->SetOnCheckedChangeListener(this);
        mProgressBar = (ProgressBar) mView->FindViewById(R.id.progress_bar);

        super->OnCreate(savedInstanceState);

        mSubmitButton = GetButton(DialogInterface.BUTTON_NEUTRAL);
        mSubmitButton->SetOnClickListener(this);
        mSubmitButton->SetEnabled(FALSE);

        mCancelButton = GetButton(DialogInterface.BUTTON_NEGATIVE);
    }

    //@Override
    CARAPI OnClick(View v) {
        mWakeLock->Acquire();

        String password = mPasswordView->GetText()->ToString();
        String wpsNfcConfigurationToken
                = mWifiManager->GetWpsNfcConfigurationToken(mAccessPoint.networkId);
        String passwordHex = ByteArrayToHexString(password->GetBytes());

        String passwordLength = password->Length() >= HEX_RADIX
                ? Integer->ToString(password->Length(), HEX_RADIX)
                : "0" + Character->ForDigit(password->Length(), HEX_RADIX);

        passwordHex = String->Format(PASSWORD_FORMAT, passwordLength, passwordHex).ToUpperCase();

        if (wpsNfcConfigurationToken->Contains(passwordHex)) {
            mWpsNfcConfigurationToken = wpsNfcConfigurationToken;

            Activity activity = GetOwnerActivity();
            NfcAdapter nfcAdapter = NfcAdapter->GetDefaultAdapter(activity);

            nfcAdapter->EnableReaderMode(activity, new NfcAdapter->ReaderCallback() {
                //@Override
                CARAPI OnTagDiscovered(Tag tag) {
                    HandleWriteNfcEvent(tag);
                }
            }, NfcAdapter.FLAG_READER_NFC_A |
                    NfcAdapter.FLAG_READER_NFC_B |
                    NfcAdapter.FLAG_READER_NFC_BARCODE |
                    NfcAdapter.FLAG_READER_NFC_F |
                    NfcAdapter.FLAG_READER_NFC_V,
                    NULL);

            mPasswordView->SetVisibility(View.GONE);
            mPasswordCheckBox->SetVisibility(View.GONE);
            mSubmitButton->SetVisibility(View.GONE);
            InputMethodManager imm = (InputMethodManager)
                    GetOwnerActivity()->GetSystemService(Context.INPUT_METHOD_SERVICE);
            imm->HideSoftInputFromWindow(mPasswordView->GetWindowToken(), 0);

            mLabelView->SetText(R::string::status_awaiting_tap);

            mView->FindViewById(R.id.password_layout).SetTextAlignment(View.TEXT_ALIGNMENT_CENTER);
            mProgressBar->SetVisibility(View.VISIBLE);
        } else {
            mLabelView->SetText(R::string::status_invalid_password);
        }
    }

    private void HandleWriteNfcEvent(Tag tag) {
        Ndef ndef = Ndef->Get(tag);

        if (ndef != NULL) {
            if (ndef->IsWritable()) {
                NdefRecord record = NdefRecord->CreateMime(
                        NFC_TOKEN_MIME_TYPE,
                        HexStringToByteArray(mWpsNfcConfigurationToken));
                try {
                    ndef->Connect();
                    ndef->WriteNdefMessage(new NdefMessage(record));
                    GetOwnerActivity()->RunOnUiThread(new Runnable() {
                        //@Override
                        CARAPI Run() {
                            mProgressBar->SetVisibility(View.GONE);
                        }
                    });
                    SetViewText(mLabelView, R::string::status_write_success);
                    SetViewText(mCancelButton, R::string::done_label);
                } catch (IOException e) {
                    SetViewText(mLabelView, R::string::status_failed_to_write);
                    Logger::E(TAG, "Unable to write Wi-Fi config to NFC tag.", e);
                    return;
                } catch (FormatException e) {
                    SetViewText(mLabelView, R::string::status_failed_to_write);
                    Logger::E(TAG, "Unable to write Wi-Fi config to NFC tag.", e);
                    return;
                }
            } else {
                SetViewText(mLabelView, R::string::status_tag_not_writable);
                Logger::E(TAG, "Tag is not writable");
            }
        } else {
            SetViewText(mLabelView, R::string::status_tag_not_writable);
            Logger::E(TAG, "Tag does not support NDEF");
        }
    }

    //@Override
    CARAPI Dismiss() {
        if (mWakeLock->IsHeld()) {
            mWakeLock->Release();
        }

        super->Dismiss();
    }

    //@Override
    CARAPI OnTextChanged(CharSequence s, Int32 start, Int32 before, Int32 count) {
        mOnTextChangedHandler->Post(new Runnable() {
            //@Override
            CARAPI Run() {
                EnableSubmitIfAppropriate();
            }
        });
    }

    private void EnableSubmitIfAppropriate() {

        if (mPasswordView != NULL) {
            if (mAccessPoint.security == AccessPoint.SECURITY_WEP) {
                mSubmitButton->SetEnabled(mPasswordView->Length() > 0);
            } else if (mAccessPoint.security == AccessPoint.SECURITY_PSK) {
                mSubmitButton->SetEnabled(mPasswordView->Length() >= 8);
            }
        } else {
            mSubmitButton->SetEnabled(FALSE);
        }

    }

    private void SetViewText(final TextView view, final Int32 resid) {
        GetOwnerActivity()->RunOnUiThread(new Runnable() {
            //@Override
            CARAPI Run() {
                view->SetText(resid);
            }
        });
    }

    //@Override
    CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
        mPasswordView->SetInputType(
                InputType.TYPE_CLASS_TEXT |
                (isChecked
                        ? InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD
                        : InputType.TYPE_TEXT_VARIATION_PASSWORD));
    }

    private static Byte[] HexStringToByteArray(String s) {
        Int32 len = s->Length();
        Byte[] data = new Byte[len / 2];

        for (Int32 i = 0; i < len; i += 2) {
            data[i / 2] = (Byte) ((Character->Digit(s->CharAt(i), HEX_RADIX) << 4)
                    + Character->Digit(s->CharAt(i + 1), HEX_RADIX));
        }

        return data;
    }

    private static String ByteArrayToHexString(Byte[] bytes) {
        Char32[] hexChars = new Char32[bytes.length * 2];
        for ( Int32 j = 0; j < bytes.length; j++ ) {
            Int32 v = bytes[j] & 0xFF;
            hexChars[j * 2] = hexArray[v >>> 4];
            hexChars[j * 2 + 1] = hexArray[v & 0x0F];
        }
        return new String(hexChars);
    }

    //@Override
    CARAPI BeforeTextChanged(CharSequence s, Int32 start, Int32 count, Int32 after) {}

    //@Override
    CARAPI AfterTextChanged(Editable s) {}
}

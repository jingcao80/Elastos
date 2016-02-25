

package com.android.settings.bluetooth;

using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IButton;

using Elastos::Droid::Internal::App::IAlertActivity;
using Elastos::Droid::Internal::App::IAlertController;

using Elastos::Droid::Settings::IR;

/**
 * BluetoothPermissionActivity shows a dialog for accepting incoming
 * profile connection request from untrusted devices.
 * It is also used to show a dialogue for accepting incoming phonebook
 * read request. The request could be initiated by PBAP PCE or by HF AT+CPBR.
 */
public class BluetoothPermissionActivity extends AlertActivity implements
        DialogInterface.OnClickListener, Preference.OnPreferenceChangeListener {
    private static const String TAG = "BluetoothPermissionActivity";
    private static const Boolean DEBUG = Utils.D;

    private View mView;
    private TextView messageView;
    private Button mOkButton;
    private BluetoothDevice mDevice;
    private String mReturnPackage = NULL;
    private String mReturnClass = NULL;

    private Int32 mRequestType = 0;
    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (action->Equals(BluetoothDevice.ACTION_CONNECTION_ACCESS_CANCEL)) {
                Int32 requestType = intent->GetIntExtra(BluetoothDevice.EXTRA_ACCESS_REQUEST_TYPE,
                        BluetoothDevice.REQUEST_TYPE_PHONEBOOK_ACCESS);
                if (requestType != mRequestType) return;
                BluetoothDevice device = intent->GetParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                if (mDevice->Equals(device)) DismissDialog();
            }
        }
    };
    private Boolean mReceiverRegistered = FALSE;

    private void DismissDialog() {
        this->Dismiss();
    }

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        Intent i = GetIntent();
        String action = i->GetAction();
        if (!action->Equals(BluetoothDevice.ACTION_CONNECTION_ACCESS_REQUEST)) {
            Logger::E(TAG, "Error: this activity may be started only with intent "
                  + "ACTION_CONNECTION_ACCESS_REQUEST");
            Finish();
            return;
        }

        mDevice = i->GetParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
        mReturnPackage = i->GetStringExtra(BluetoothDevice.EXTRA_PACKAGE_NAME);
        mReturnClass = i->GetStringExtra(BluetoothDevice.EXTRA_CLASS_NAME);
        mRequestType = i->GetIntExtra(BluetoothDevice.EXTRA_ACCESS_REQUEST_TYPE,
                                     BluetoothDevice.REQUEST_TYPE_PHONEBOOK_ACCESS);

        If(DEBUG) Logger::I(TAG, "OnCreate() Request type: " + mRequestType);

        if (mRequestType == BluetoothDevice.REQUEST_TYPE_PROFILE_CONNECTION) {
            ShowDialog(GetString(R::string::bluetooth_connection_permission_request), mRequestType);
        } else if (mRequestType == BluetoothDevice.REQUEST_TYPE_PHONEBOOK_ACCESS) {
            ShowDialog(GetString(R::string::bluetooth_phonebook_request), mRequestType);
        } else if (mRequestType == BluetoothDevice.REQUEST_TYPE_MESSAGE_ACCESS) {
            ShowDialog(GetString(R::string::bluetooth_map_request), mRequestType);
        }
        else {
            Logger::E(TAG, "Error: bad request type: " + mRequestType);
            Finish();
            return;
        }
        RegisterReceiver(mReceiver,
                         new IntentFilter(BluetoothDevice.ACTION_CONNECTION_ACCESS_CANCEL));
        mReceiverRegistered = TRUE;
    }


    private void ShowDialog(String title, Int32 requestType)
    {
        final AlertController.AlertParams p = mAlertParams;
        p.mTitle = title;
        If(DEBUG) Logger::I(TAG, "ShowDialog() Request type: " + mRequestType + " this: " + this);
        Switch(requestType)
        {
        case BluetoothDevice.REQUEST_TYPE_PROFILE_CONNECTION:
            p.mView = CreateConnectionDialogView();
            break;
        case BluetoothDevice.REQUEST_TYPE_PHONEBOOK_ACCESS:
            p.mView = CreatePhonebookDialogView();
            break;
        case BluetoothDevice.REQUEST_TYPE_MESSAGE_ACCESS:
            p.mView = CreateMapDialogView();
            break;
        }
        p.mPositiveButtonText = GetString(R::string::yes);
        p.mPositiveButtonListener = this;
        p.mNegativeButtonText = GetString(R::string::no);
        p.mNegativeButtonListener = this;
        mOkButton = mAlert->GetButton(DialogInterface.BUTTON_POSITIVE);
        SetupAlert();

    }
    //@Override
    CARAPI OnBackPressed() {
        /*we need an answer so ignore back button presses during auth */
        If(DEBUG) Logger::I(TAG, "Back button pressed! ignoring");
        return;
    }
    private String CreateRemoteName()
    {
        String mRemoteName = mDevice != NULL ? mDevice->GetAliasName() : NULL;

        if (mRemoteName == NULL) mRemoteName = GetString(R::string::unknown);
        return mRemoteName;
    }

    // TODO(edjee): createConnectionDialogView, createPhonebookDialogView and createMapDialogView
    // are similar. Refactor them into one method.
    // Also, the string resources bluetooth_remember_choice and bluetooth_pb_remember_choice should
    // be removed.
    private View CreateConnectionDialogView() {
        String mRemoteName = CreateRemoteName();
        mView = GetLayoutInflater()->Inflate(R.layout.bluetooth_access, NULL);
        messageView = (TextView)mView->FindViewById(R.id.message);
        messageView->SetText(GetString(R::string::bluetooth_connection_dialog_text,
                mRemoteName));
        return mView;
    }

    private View CreatePhonebookDialogView() {
        String mRemoteName = CreateRemoteName();
        mView = GetLayoutInflater()->Inflate(R.layout.bluetooth_access, NULL);
        messageView = (TextView)mView->FindViewById(R.id.message);
        messageView->SetText(GetString(R::string::bluetooth_pb_acceptance_dialog_text,
                mRemoteName, mRemoteName));
        return mView;
    }

    private View CreateMapDialogView() {
        String mRemoteName = CreateRemoteName();
        mView = GetLayoutInflater()->Inflate(R.layout.bluetooth_access, NULL);
        messageView = (TextView)mView->FindViewById(R.id.message);
        messageView->SetText(GetString(R::string::bluetooth_map_acceptance_dialog_text,
                mRemoteName, mRemoteName));
        return mView;
    }

    private void OnPositive() {
        if (DEBUG) Logger::D(TAG, "onPositive");
        SendReplyIntentToReceiver(TRUE, TRUE);
        Finish();
    }

    private void OnNegative() {
        if (DEBUG) Logger::D(TAG, "onNegative");

        Boolean always = TRUE;
        if (mRequestType == BluetoothDevice.REQUEST_TYPE_MESSAGE_ACCESS) {
            LocalBluetoothManager bluetoothManager = LocalBluetoothManager->GetInstance(this);
            CachedBluetoothDeviceManager cachedDeviceManager =
                    bluetoothManager->GetCachedDeviceManager();
            CachedBluetoothDevice cachedDevice = cachedDeviceManager->FindDevice(mDevice);
            if (cachedDevice == NULL) {
                cachedDevice = cachedDeviceManager->AddDevice(bluetoothManager->GetBluetoothAdapter(),
                                                             bluetoothManager->GetProfileManager(),
                                                             mDevice);
            }
            always = cachedDevice->CheckAndIncreaseMessageRejectionCount();
        }

        SendReplyIntentToReceiver(FALSE, always);
    }

    private void SendReplyIntentToReceiver(final Boolean allowed, final Boolean always) {
        Intent intent = new Intent(BluetoothDevice.ACTION_CONNECTION_ACCESS_REPLY);

        if (mReturnPackage != NULL && mReturnClass != NULL) {
            intent->SetClassName(mReturnPackage, mReturnClass);
        }
        if (DEBUG) Logger::I(TAG, "SendReplyIntentToReceiver() Request type: " + mRequestType +
                " mReturnPackage" + mReturnPackage + " mReturnClass" + mReturnClass);

        intent->PutExtra(BluetoothDevice.EXTRA_CONNECTION_ACCESS_RESULT,
                        allowed ? BluetoothDevice.CONNECTION_ACCESS_YES
                                : BluetoothDevice.CONNECTION_ACCESS_NO);
        intent->PutExtra(BluetoothDevice.EXTRA_ALWAYS_ALLOWED, always);
        intent->PutExtra(BluetoothDevice.EXTRA_DEVICE, mDevice);
        intent->PutExtra(BluetoothDevice.EXTRA_ACCESS_REQUEST_TYPE, mRequestType);
        SendBroadcast(intent, Manifest::permission::BLUETOOTH_ADMIN);
    }

    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        switch (which) {
            case DialogInterface.BUTTON_POSITIVE:
                OnPositive();
                break;

            case DialogInterface.BUTTON_NEGATIVE:
                OnNegative();
                break;
            default:
                break;
        }
    }

    //@Override
    protected void OnDestroy() {
        super->OnDestroy();
        if (mReceiverRegistered) {
            UnregisterReceiver(mReceiver);
            mReceiverRegistered = FALSE;
        }
    }

    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        return TRUE;
    }
}

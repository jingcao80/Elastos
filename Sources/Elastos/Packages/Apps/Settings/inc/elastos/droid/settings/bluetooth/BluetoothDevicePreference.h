

package com.android.settings.bluetooth;

using static android::Os::UserManager::IDISALLOW_CONFIG_BLUETOOTH;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Bluetooth::IBluetoothClass;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Text::IHtml;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::Widget::IImageView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::Search::IIndex;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;

using Elastos::Utility::IList;

/**
 * BluetoothDevicePreference is the preference type used to display each remote
 * Bluetooth device in the Bluetooth Settings screen.
 */
public class BluetoothDevicePreference extends Preference implements
        CachedBluetoothDevice.Callback, OnClickListener {
    private static const String TAG = "BluetoothDevicePreference";

    private static Int32 sDimAlpha = Integer.MIN_VALUE;

    private final CachedBluetoothDevice mCachedDevice;

    private OnClickListener mOnSettingsClickListener;

    private AlertDialog mDisconnectDialog;

    public BluetoothDevicePreference(Context context, CachedBluetoothDevice cachedDevice) {
        Super(context);

        if (sDimAlpha == Integer.MIN_VALUE) {
            TypedValue outValue = new TypedValue();
            context->GetTheme()->ResolveAttribute(android.R.attr.disabledAlpha, outValue, TRUE);
            sDimAlpha = (Int32) (outValue->GetFloat() * 255);
        }

        mCachedDevice = cachedDevice;

        SetLayoutResource(R.layout.preference_bt_icon);

        if (cachedDevice->GetBondState() == BluetoothDevice.BOND_BONDED) {
            UserManager um = (UserManager) context->GetSystemService(Context.USER_SERVICE);
            if (! um->HasUserRestriction(DISALLOW_CONFIG_BLUETOOTH)) {
                SetWidgetLayoutResource(R.layout.preference_bluetooth);
            }
        }

        mCachedDevice->RegisterCallback(this);

        OnDeviceAttributesChanged();
    }

    CachedBluetoothDevice GetCachedDevice() {
        return mCachedDevice;
    }

    CARAPI SetOnSettingsClickListener(OnClickListener listener) {
        mOnSettingsClickListener = listener;
    }

    //@Override
    protected void OnPrepareForRemoval() {
        super->OnPrepareForRemoval();
        mCachedDevice->UnregisterCallback(this);
        if (mDisconnectDialog != NULL) {
            mDisconnectDialog->Dismiss();
            mDisconnectDialog = NULL;
        }
    }

    CARAPI OnDeviceAttributesChanged() {
        /*
         * The preference framework takes care of making sure the value has
         * changed before proceeding. It will also call NotifyChanged() if
         * any preference info has changed from the previous value.
         */
        SetTitle(mCachedDevice->GetName());

        Int32 summaryResId = GetConnectionSummary();
        if (summaryResId != 0) {
            SetSummary(summaryResId);
        } else {
            SetSummary(NULL);   // empty summary for unpaired devices
        }

        Int32 iconResId = GetBtClassDrawable();
        if (iconResId != 0) {
            SetIcon(iconResId);
        }

        // Used to gray out the item
        SetEnabled(!mCachedDevice->IsBusy());

        // This could affect ordering, so notify that
        NotifyHierarchyChanged();
    }

    //@Override
    protected void OnBindView(View view) {
        // Disable this view if the bluetooth enable/disable preference view is off
        if (NULL != FindPreferenceInHierarchy("bt_checkbox")) {
            SetDependency("bt_checkbox");
        }

        if (mCachedDevice->GetBondState() == BluetoothDevice.BOND_BONDED) {
            ImageView deviceDetails = (ImageView) view->FindViewById(R.id.deviceDetails);

            if (deviceDetails != NULL) {
                deviceDetails->SetOnClickListener(this);
                deviceDetails->SetTag(mCachedDevice);
            }
        }

        super->OnBindView(view);
    }

    CARAPI OnClick(View v) {
        // Should never be NULL by construction
        if (mOnSettingsClickListener != NULL) {
            mOnSettingsClickListener->OnClick(v);
        }
    }

    //@Override
    public Boolean Equals(Object o) {
        if ((o == NULL) || !(o instanceof BluetoothDevicePreference)) {
            return FALSE;
        }
        return mCachedDevice->Equals(
                ((BluetoothDevicePreference) o).mCachedDevice);
    }

    //@Override
    public Int32 HashCode() {
        return mCachedDevice->HashCode();
    }

    //@Override
    public Int32 CompareTo(Preference another) {
        if (!(another instanceof BluetoothDevicePreference)) {
            // Rely on default sort
            return super->CompareTo(another);
        }

        return mCachedDevice
                .CompareTo(((BluetoothDevicePreference) another).mCachedDevice);
    }

    void OnClicked() {
        Int32 bondState = mCachedDevice->GetBondState();

        if (mCachedDevice->IsConnected()) {
            AskDisconnect();
        } else if (bondState == BluetoothDevice.BOND_BONDED) {
            mCachedDevice->Connect(TRUE);
        } else if (bondState == BluetoothDevice.BOND_NONE) {
            Pair();
        }
    }

    // Show disconnect confirmation dialog for a device.
    private void AskDisconnect() {
        Context context = GetContext();
        String name = mCachedDevice->GetName();
        if (TextUtils->IsEmpty(name)) {
            name = context->GetString(R::string::bluetooth_device);
        }
        String message = context->GetString(R::string::bluetooth_disconnect_all_profiles, name);
        String title = context->GetString(R::string::bluetooth_disconnect_title);

        DialogInterface.OnClickListener disconnectListener = new DialogInterface->OnClickListener() {
            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                mCachedDevice->Disconnect();
            }
        };

        mDisconnectDialog = Utils->ShowDisconnectDialog(context,
                mDisconnectDialog, disconnectListener, title, Html->FromHtml(message));
    }

    private void Pair() {
        if (!mCachedDevice->StartPairing()) {
            Utils->ShowError(GetContext(), mCachedDevice->GetName(),
                    R::string::bluetooth_pairing_error_message);
        } else {
            final Context context = GetContext();

            SearchIndexableRaw data = new SearchIndexableRaw(context);
            data.className = BluetoothSettings.class->GetName();
            data.title = mCachedDevice->GetName();
            data.screenTitle = context->GetResources()->GetString(R::string::bluetooth_settings);
            data.iconResId = R.drawable.ic_settings_bluetooth2;
            data.enabled = TRUE;

            Index->GetInstance(context).UpdateFromSearchIndexableData(data);
        }
    }

    private Int32 GetConnectionSummary() {
        final CachedBluetoothDevice cachedDevice = mCachedDevice;

        Boolean profileConnected = FALSE;       // at least one profile is connected
        Boolean a2dpNotConnected = FALSE;       // A2DP is preferred but not connected
        Boolean headsetNotConnected = FALSE;    // Headset is preferred but not connected

        for (LocalBluetoothProfile profile : cachedDevice->GetProfiles()) {
            Int32 connectionStatus = cachedDevice->GetProfileConnectionState(profile);

            switch (connectionStatus) {
                case BluetoothProfile.STATE_CONNECTING:
                case BluetoothProfile.STATE_DISCONNECTING:
                    return Utils->GetConnectionStateSummary(connectionStatus);

                case BluetoothProfile.STATE_CONNECTED:
                    profileConnected = TRUE;
                    break;

                case BluetoothProfile.STATE_DISCONNECTED:
                    if (profile->IsProfileReady()) {
                        if (profile instanceof A2dpProfile) {
                            a2dpNotConnected = TRUE;
                        } else if (profile instanceof HeadsetProfile) {
                            headsetNotConnected = TRUE;
                        }
                    }
                    break;
            }
        }

        if (profileConnected) {
            if (a2dpNotConnected && headsetNotConnected) {
                return R::string::bluetooth_connected_no_headset_no_a2dp;
            } else if (a2dpNotConnected) {
                return R::string::bluetooth_connected_no_a2dp;
            } else if (headsetNotConnected) {
                return R::string::bluetooth_connected_no_headset;
            } else {
                return R::string::bluetooth_connected;
            }
        }

        switch (cachedDevice->GetBondState()) {
            case BluetoothDevice.BOND_BONDING:
                return R::string::bluetooth_pairing;

            case BluetoothDevice.BOND_BONDED:
            case BluetoothDevice.BOND_NONE:
            default:
                return 0;
        }
    }

    private Int32 GetBtClassDrawable() {
        BluetoothClass btClass = mCachedDevice->GetBtClass();
        if (btClass != NULL) {
            switch (btClass->GetMajorDeviceClass()) {
                case BluetoothClass.Device.Major.COMPUTER:
                    return R.drawable.ic_bt_laptop;

                case BluetoothClass.Device.Major.PHONE:
                    return R.drawable.ic_bt_cellphone;

                case BluetoothClass.Device.Major.PERIPHERAL:
                    return HidProfile->GetHidClassDrawable(btClass);

                case BluetoothClass.Device.Major.IMAGING:
                    return R.drawable.ic_bt_imaging;

                default:
                    // unrecognized device class; continue
            }
        } else {
            Logger::W(TAG, "mBtClass is NULL");
        }

        List<LocalBluetoothProfile> profiles = mCachedDevice->GetProfiles();
        for (LocalBluetoothProfile profile : profiles) {
            Int32 resId = profile->GetDrawableResource(btClass);
            if (resId != 0) {
                return resId;
            }
        }
        if (btClass != NULL) {
            if (btClass->DoesClassMatch(BluetoothClass.PROFILE_A2DP)) {
                return R.drawable.ic_bt_headphones_a2dp;

            }
            if (btClass->DoesClassMatch(BluetoothClass.PROFILE_HEADSET)) {
                return R.drawable.ic_bt_headset_hfp;
            }
        }
        return R.drawable.ic_settings_bluetooth2;
    }
}



package com.android.settings.bluetooth;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::ILog;

/**
 * LocalBluetoothManager provides a simplified interface on top of a subset of
 * the Bluetooth API. Note that {@link #getInstance} will return NULL
 * if there is no Bluetooth adapter on this device, and callers must be
 * prepared to handle this case.
 */
public class LocalBluetoothManager {
    private static const String TAG = "LocalBluetoothManager";

    /** Singleton instance. */
    private static LocalBluetoothManager sInstance;

    private final Context mContext;

    /** If a BT-related activity is in the foreground, this will be it. */
    private Context mForegroundActivity;

    private BluetoothDiscoverableEnabler mDiscoverableEnabler;

    private final LocalBluetoothAdapter mLocalAdapter;

    private final CachedBluetoothDeviceManager mCachedDeviceManager;

    /** The Bluetooth profile manager. */
    private final LocalBluetoothProfileManager mProfileManager;

    /** The broadcast receiver event manager. */
    private final BluetoothEventManager mEventManager;

    public static synchronized LocalBluetoothManager GetInstance(Context context) {
        if (sInstance == NULL) {
            LocalBluetoothAdapter adapter = LocalBluetoothAdapter->GetInstance();
            if (adapter == NULL) {
                return NULL;
            }
            // This will be around as Int64 as this process is
            Context appContext = context->GetApplicationContext();
            sInstance = new LocalBluetoothManager(adapter, appContext);
        }

        return sInstance;
    }

    CARAPI SetDiscoverableEnabler(BluetoothDiscoverableEnabler discoverableEnabler) {
        mDiscoverableEnabler = discoverableEnabler;
    }

    public BluetoothDiscoverableEnabler GetDiscoverableEnabler() {
        return mDiscoverableEnabler;
    }

    private LocalBluetoothManager(LocalBluetoothAdapter adapter, Context context) {
        mContext = context;
        mLocalAdapter = adapter;

        mCachedDeviceManager = new CachedBluetoothDeviceManager(context);
        mEventManager = new BluetoothEventManager(mLocalAdapter,
                mCachedDeviceManager, context);
        mProfileManager = new LocalBluetoothProfileManager(context,
                mLocalAdapter, mCachedDeviceManager, mEventManager);
    }

    public LocalBluetoothAdapter GetBluetoothAdapter() {
        return mLocalAdapter;
    }

    public Context GetContext() {
        return mContext;
    }

    public Context GetForegroundActivity() {
        return mForegroundActivity;
    }

    Boolean IsForegroundActivity() {
        return mForegroundActivity != NULL;
    }

    synchronized void SetForegroundActivity(Context context) {
        if (context != NULL) {
            Logger::D(TAG, "setting foreground activity to non-NULL context");
            mForegroundActivity = context;
        } else {
            if (mForegroundActivity != NULL) {
                Logger::D(TAG, "setting foreground activity to NULL");
                mForegroundActivity = NULL;
            }
        }
    }

    CachedBluetoothDeviceManager GetCachedDeviceManager() {
        return mCachedDeviceManager;
    }

    BluetoothEventManager GetEventManager() {
        return mEventManager;
    }

    LocalBluetoothProfileManager GetProfileManager() {
        return mProfileManager;
    }
}

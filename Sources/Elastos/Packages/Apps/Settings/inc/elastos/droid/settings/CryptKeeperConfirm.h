

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Storage::IIMountService;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;

using Elastos::Droid::Internal::Widget::ILockPatternUtils;

using Elastos::Utility::ILocale;

public class CryptKeeperConfirm extends Fragment {

    private static const String TAG = "CryptKeeperConfirm";

    public static class Blank extends Activity {
        private Handler mHandler = new Handler();

        //@Override
        CARAPI OnCreate(Bundle savedInstanceState) {
            super->OnCreate(savedInstanceState);

            SetContentView(R.layout.crypt_keeper_blank);

            if (Utils->IsMonkeyRunning()) {
                Finish();
            }

            StatusBarManager sbm = (StatusBarManager) GetSystemService(Context.STATUS_BAR_SERVICE);
            sbm->Disable(StatusBarManager.DISABLE_EXPAND
                    | StatusBarManager.DISABLE_NOTIFICATION_ICONS
                    | StatusBarManager.DISABLE_NOTIFICATION_ALERTS
                    | StatusBarManager.DISABLE_SYSTEM_INFO
                    | StatusBarManager.DISABLE_HOME
                    | StatusBarManager.DISABLE_SEARCH
                    | StatusBarManager.DISABLE_RECENT
                    | StatusBarManager.DISABLE_BACK);

            // Post a delayed message in 700 milliseconds to enable encryption.
            // NOTE: The animation on this activity is set for 500 milliseconds
            // I am giving it a little extra time to complete.
            mHandler->PostDelayed(new Runnable() {
                CARAPI Run() {
                    IBinder service = ServiceManager->GetService("mount");
                    if (service == NULL) {
                        Logger::E("CryptKeeper", "Failed to find the mount service");
                        Finish();
                        return;
                    }

                    IMountService mountService = IMountService.Stub->AsInterface(service);
                    try {
                        Bundle args = GetIntent()->GetExtras();
                        mountService->EncryptStorage(args->GetInt("type", -1), args->GetString("password"));
                    } catch (Exception e) {
                        Logger::E("CryptKeeper", "Error while encrypting...", e);
                    }
                }
            }, 700);
        }
    }

    private View mContentView;
    private Button mFinalButton;
    private Button.OnClickListener mFinalClickListener = new Button->OnClickListener() {

        CARAPI OnClick(View v) {
            if (Utils->IsMonkeyRunning()) {
                return;
            }

            /* WARNING - nasty hack!
               Settings for the lock screen are not available to the crypto
               screen (CryptKeeper) at boot. We duplicate the ones that
               CryptKeeper needs to the crypto key/value store when they are
               modified (see LockPatternUtils).
               However, prior to encryption, the crypto key/value store is not
               persisted across reboots, thus modified settings are lost to
               CryptKeeper.
               In order to make sure CryptKeeper had the correct settings after
               first encrypting, we thus need to rewrite them, which ensures the
               crypto key/value store is up to date. On encryption, this store
               is then persisted, and the settings will be there on future
               reboots.
             */

            // 1. The owner info.
            LockPatternUtils utils = new LockPatternUtils(GetActivity());
            utils->SetVisiblePatternEnabled(utils->IsVisiblePatternEnabled());
            if (utils->IsOwnerInfoEnabled()) {
                utils->SetOwnerInfo(utils->GetOwnerInfo(UserHandle.USER_OWNER),
                                   UserHandle.USER_OWNER);
            }
            Intent intent = new Intent(GetActivity(), Blank.class);
            intent->PutExtras(GetArguments());
            StartActivity(intent);

            // 2. The system locale.
            try {
                IBinder service = ServiceManager->GetService("mount");
                IMountService mountService = IMountService.Stub->AsInterface(service);
                mountService->SetField("SystemLocale", Locale->GetDefault()->ToLanguageTag());
            } catch (Exception e) {
                Logger::E(TAG, "Error storing locale for decryption UI", e);
            }
        }
    };

    private void EstablishFinalConfirmationState() {
        mFinalButton = (Button) mContentView->FindViewById(R.id.execute_encrypt);
        mFinalButton->SetOnClickListener(mFinalClickListener);
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        mContentView = inflater->Inflate(R.layout.crypt_keeper_confirm, NULL);
        EstablishFinalConfirmationState();
        return mContentView;
    }
}

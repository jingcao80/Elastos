

package com.android.settings;

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::IAuthenticatorDescription;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ITextView;

/**
 * Confirm and execute a reset of the device to a clean "just out of the box"
 * state.  Multiple confirmations are required: first, a general "are you sure
 * you want to do this?" prompt, followed by a keyguard pattern trace if the user
 * has defined one, followed by a final strongly-worded "THIS WILL ERASE EVERYTHING
 * ON THE PHONE" prompt.  If at any time the phone is allowed to go to sleep, is
 * locked, et cetera, then the confirmation sequence is abandoned.
 *
 * This is the initial screen.
 */
public class MasterClear extends Fragment {
    private static const String TAG = "MasterClear";

    private static const Int32 KEYGUARD_REQUEST = 55;

    static const String ERASE_EXTERNAL_EXTRA = "erase_sd";

    private View mContentView;
    private Button mInitiateButton;
    private View mExternalStorageContainer;
    private CheckBox mExternalStorage;

    /**
     * Keyguard validation is run using the standard {@link ConfirmLockPattern}
     * component as a subactivity
     * @param request the request code to be returned once confirmation finishes
     * @return TRUE if confirmation launched
     */
    private Boolean RunKeyguardConfirmation(Int32 request) {
        Resources res = GetActivity()->GetResources();
        return new ChooseLockSettingsHelper(GetActivity(), this)
                .LaunchConfirmationActivity(request,
                        res->GetText(R::string::master_clear_gesture_prompt),
                        res->GetText(R::string::master_clear_gesture_explanation));
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        super->OnActivityResult(requestCode, resultCode, data);

        if (requestCode != KEYGUARD_REQUEST) {
            return;
        }

        // If the user entered a valid keyguard trace, present the final
        // confirmation prompt; otherwise, go back to the initial state.
        if (resultCode == Activity.RESULT_OK) {
            ShowFinalConfirmation();
        } else {
            EstablishInitialState();
        }
    }

    private void ShowFinalConfirmation() {
        Preference preference = new Preference(GetActivity());
        preference->SetFragment(MasterClearConfirm.class->GetName());
        preference->SetTitle(R::string::master_clear_confirm_title);
        preference->GetExtras()->PutBoolean(ERASE_EXTERNAL_EXTRA, mExternalStorage->IsChecked());
        ((SettingsActivity) GetActivity()).OnPreferenceStartFragment(NULL, preference);
    }

    /**
     * If the user clicks to begin the reset sequence, we next require a
     * keyguard confirmation if the user has currently enabled one.  If there
     * is no keyguard available, we simply go to the final confirmation prompt.
     */
    private final Button.OnClickListener mInitiateListener = new Button->OnClickListener() {

        CARAPI OnClick(View v) {
            if (!RunKeyguardConfirmation(KEYGUARD_REQUEST)) {
                ShowFinalConfirmation();
            }
        }
    };

    /**
     * In its initial state, the activity presents a button for the user to
     * click in order to initiate a confirmation sequence.  This method is
     * called from various other points in the code to reset the activity to
     * this base state.
     *
     * <p>Reinflating views from resources is expensive and prevents us from
     * caching widget pointers, so we use a single-inflate pattern:  we lazy-
     * inflate each view, caching all of the widget pointers we'll need at the
     * time, then simply reuse the inflated views directly whenever we need
     * to change contents.
     */
    private void EstablishInitialState() {
        mInitiateButton = (Button) mContentView->FindViewById(R.id.initiate_master_clear);
        mInitiateButton->SetOnClickListener(mInitiateListener);
        mExternalStorageContainer = mContentView->FindViewById(R.id.erase_external_container);
        mExternalStorage = (CheckBox) mContentView->FindViewById(R.id.erase_external);

        /*
         * If the external storage is emulated, it will be erased with a factory
         * reset at any rate. There is no need to have a separate option until
         * we have a factory reset that only erases some directories and not
         * others. Likewise, if it's non-removable storage, it could potentially have been
         * encrypted, and will also need to be wiped.
         */
        Boolean isExtStorageEmulated = Environment->IsExternalStorageEmulated();
        if (isExtStorageEmulated
                || (!Environment->IsExternalStorageRemovable() && IsExtStorageEncrypted())) {
            mExternalStorageContainer->SetVisibility(View.GONE);

            final View externalOption = mContentView->FindViewById(R.id.erase_external_option_text);
            externalOption->SetVisibility(View.GONE);

            final View externalAlsoErased = mContentView->FindViewById(R.id.also_erases_external);
            externalAlsoErased->SetVisibility(View.VISIBLE);

            // If it's not emulated, it is on a separate partition but it means we're doing
            // a force wipe due to encryption.
            mExternalStorage->SetChecked(!isExtStorageEmulated);
        } else {
            mExternalStorageContainer->SetOnClickListener(new View->OnClickListener() {

                //@Override
                CARAPI OnClick(View v) {
                    mExternalStorage->Toggle();
                }
            });
        }

        LoadAccountList();
    }

    private Boolean IsExtStorageEncrypted() {
        String state = SystemProperties->Get("vold.decrypt");
        return !"".Equals(state);
    }

    private void LoadAccountList() {
        View accountsLabel = mContentView->FindViewById(R.id.accounts_label);
        LinearLayout contents = (LinearLayout)mContentView->FindViewById(R.id.accounts);
        contents->RemoveAllViews();

        Context context = GetActivity();

        AccountManager mgr = AccountManager->Get(context);
        Account[] accounts = mgr->GetAccounts();
        final Int32 N = accounts.length;
        if (N == 0) {
            accountsLabel->SetVisibility(View.GONE);
            contents->SetVisibility(View.GONE);
            return;
        }

        LayoutInflater inflater = (LayoutInflater)context->GetSystemService(
                Context.LAYOUT_INFLATER_SERVICE);

        AuthenticatorDescription[] descs = AccountManager->Get(context).GetAuthenticatorTypes();
        final Int32 M = descs.length;

        for (Int32 i=0; i<N; i++) {
            Account account = accounts[i];
            AuthenticatorDescription desc = NULL;
            for (Int32 j=0; j<M; j++) {
                if (account.type->Equals(descs[j].type)) {
                    desc = descs[j];
                    break;
                }
            }
            if (desc == NULL) {
                Logger::W(TAG, "No descriptor for account name=" + account.name
                        + " type=" + account.type);
                continue;
            }
            Drawable icon = NULL;
            try {
                if (desc.iconId != 0) {
                    Context authContext = context->CreatePackageContext(desc.packageName, 0);
                    icon = authContext->GetResources()->GetDrawable(desc.iconId);
                }
            } catch (PackageManager.NameNotFoundException e) {
                Logger::W(TAG, "No icon for account type " + desc.type);
            }

            TextView child = (TextView)inflater->Inflate(R.layout.master_clear_account,
                    contents, FALSE);
            child->SetText(account.name);
            if (icon != NULL) {
                child->SetCompoundDrawablesWithIntrinsicBounds(icon, NULL, NULL, NULL);
            }
            contents->AddView(child);
        }

        accountsLabel->SetVisibility(View.VISIBLE);
        contents->SetVisibility(View.VISIBLE);
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        if (!Process->MyUserHandle()->IsOwner()
                || UserManager->Get(GetActivity()).HasUserRestriction(
                UserManager.DISALLOW_FACTORY_RESET)) {
            return inflater->Inflate(R.layout.master_clear_disallowed_screen, NULL);
        }

        mContentView = inflater->Inflate(R.layout.master_clear, NULL);

        EstablishInitialState();
        return mContentView;
    }
}

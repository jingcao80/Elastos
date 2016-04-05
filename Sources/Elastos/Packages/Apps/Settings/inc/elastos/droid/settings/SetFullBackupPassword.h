

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToast;

public class SetFullBackupPassword extends Activity {
    static const String TAG = "SetFullBackupPassword";

    IBackupManager mBackupManager;
    TextView mCurrentPw, mNewPw, mConfirmNewPw;
    Button mCancel, mSet;

    OnClickListener mButtonListener = new OnClickListener() {
        //@Override
        CARAPI OnClick(View v) {
            if (v == mSet) {
                final String curPw = mCurrentPw->GetText()->ToString();
                final String newPw = mNewPw->GetText()->ToString();
                final String confirmPw = mConfirmNewPw->GetText()->ToString();

                if (!newPw->Equals(confirmPw)) {
                    // Mismatch between new pw and its confirmation re-entry
Logger::I(TAG, "password mismatch");
                    Toast->MakeText(SetFullBackupPassword.this,
                            R::string::local_backup_password_toast_confirmation_mismatch,
                            Toast.LENGTH_LONG).Show();
                    return;
                }

                // TODO: should we distinguish cases of has/hasn't set a pw before?

                if (SetBackupPassword(curPw, newPw)) {
                    // success
Logger::I(TAG, "password set successfully");
                    Toast->MakeText(SetFullBackupPassword.this,
                            R::string::local_backup_password_toast_success,
                            Toast.LENGTH_LONG).Show();
                    Finish();
                } else {
                    // failure -- bad existing pw, usually
Logger::I(TAG, "failure; password mismatch?");
                    Toast->MakeText(SetFullBackupPassword.this,
                            R::string::local_backup_password_toast_validation_failure,
                            Toast.LENGTH_LONG).Show();
                }
            } else if (v == mCancel) {
                Finish();
            } else {
                Logger::W(TAG, "Click on unknown view");
            }
        }
    };

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        mBackupManager = IBackupManager.Stub->AsInterface(ServiceManager->GetService("backup"));

        SetContentView(R.layout.set_backup_pw);

        mCurrentPw = (TextView) FindViewById(R.id.current_backup_pw);
        mNewPw = (TextView) FindViewById(R.id.new_backup_pw);
        mConfirmNewPw = (TextView) FindViewById(R.id.confirm_new_backup_pw);

        mCancel = (Button) FindViewById(R.id.backup_pw_cancel_button);
        mSet = (Button) FindViewById(R.id.backup_pw_set_button);

        mCancel->SetOnClickListener(mButtonListener);
        mSet->SetOnClickListener(mButtonListener);
    }

    private Boolean SetBackupPassword(String currentPw, String newPw) {
        try {
            return mBackupManager->SetBackupPassword(currentPw, newPw);
        } catch (RemoteException e) {
            Logger::E(TAG, "Unable to communicate with backup manager");
            return FALSE;
        }
    }
}

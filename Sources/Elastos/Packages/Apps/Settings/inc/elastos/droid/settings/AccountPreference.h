

package com.android.settings;

using Elastos::Utility::IArrayList;

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;

/**
 * AccountPreference is used to display a username, status and provider icon for an account on
 * the device.
 */
public class AccountPreference extends Preference {
    private static const String TAG = "AccountPreference";
    public static const Int32 SYNC_ENABLED = 0; // all know sync adapters are enabled and OK
    public static const Int32 SYNC_DISABLED = 1; // no sync adapters are enabled
    public static const Int32 SYNC_ERROR = 2; // one or more sync adapters have a problem
    public static const Int32 SYNC_IN_PROGRESS = 3; // currently syncing
    private Int32 mStatus;
    private Account mAccount;
    private ArrayList<String> mAuthorities;
    private ImageView mSyncStatusIcon;
    private Boolean mShowTypeIcon;

    public AccountPreference(Context context, Account account, Drawable icon,
            ArrayList<String> authorities, Boolean showTypeIcon) {
        Super(context);
        mAccount = account;
        mAuthorities = authorities;
        mShowTypeIcon = showTypeIcon;
        if (showTypeIcon) {
            SetIcon(icon);
        } else {
            SetIcon(GetSyncStatusIcon(SYNC_DISABLED));
        }
        SetTitle(mAccount.name);
        SetSummary("");
        SetPersistent(FALSE);
        SetSyncStatus(SYNC_DISABLED, FALSE);
    }

    public Account GetAccount() {
        return mAccount;
    }

    public ArrayList<String> GetAuthorities() {
        return mAuthorities;
    }

    //@Override
    protected void OnBindView(View view) {
        super->OnBindView(view);
        if (!mShowTypeIcon) {
            mSyncStatusIcon = (ImageView) view->FindViewById(android.R.id.icon);
            mSyncStatusIcon->SetImageResource(GetSyncStatusIcon(mStatus));
            mSyncStatusIcon->SetContentDescription(GetSyncContentDescription(mStatus));
        }
    }

    CARAPI SetSyncStatus(Int32 status, Boolean updateSummary) {
        mStatus = status;
        if (!mShowTypeIcon && mSyncStatusIcon != NULL) {
            mSyncStatusIcon->SetImageResource(GetSyncStatusIcon(status));
            mSyncStatusIcon->SetContentDescription(GetSyncContentDescription(mStatus));
        }
        if (updateSummary) {
            SetSummary(GetSyncStatusMessage(status));
        }
    }

    private Int32 GetSyncStatusMessage(Int32 status) {
        Int32 res;
        switch (status) {
            case SYNC_ENABLED:
                res = R::string::sync_enabled;
                break;
            case SYNC_DISABLED:
                res = R::string::sync_disabled;
                break;
            case SYNC_ERROR:
                res = R::string::sync_error;
                break;
            case SYNC_IN_PROGRESS:
                res = R::string::sync_in_progress;
                break;
            default:
                res = R::string::sync_error;
                Logger::E(TAG, "Unknown sync status: " + status);
        }
        return res;
    }

    private Int32 GetSyncStatusIcon(Int32 status) {
        Int32 res;
        switch (status) {
            case SYNC_ENABLED:
                res = R.drawable.ic_sync_green_holo;
                break;
            case SYNC_DISABLED:
                res = R.drawable.ic_sync_grey_holo;
                break;
            case SYNC_ERROR:
                res = R.drawable.ic_sync_red_holo;
                break;
            case SYNC_IN_PROGRESS:
                res = R.drawable.ic_sync_green_holo;
                break;
            default:
                res = R.drawable.ic_sync_red_holo;
                Logger::E(TAG, "Unknown sync status: " + status);
        }
        return res;
    }

    private String GetSyncContentDescription(Int32 status) {
        switch (status) {
            case SYNC_ENABLED:
                return GetContext()->GetString(R::string::accessibility_sync_enabled);
            case SYNC_DISABLED:
                return GetContext()->GetString(R::string::accessibility_sync_disabled);
            case SYNC_ERROR:
                return GetContext()->GetString(R::string::accessibility_sync_error);
            case SYNC_IN_PROGRESS:
                return GetContext()->GetString(R::string::accessibility_sync_in_progress);
            default:
                Logger::E(TAG, "Unknown sync status: " + status);
                return GetContext()->GetString(R::string::accessibility_sync_error);
        }
    }
}

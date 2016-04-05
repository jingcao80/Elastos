

package com.android.settings.accounts;

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::Widget::IAnimatedImageView;

public class SyncStateCheckBoxPreference extends CheckBoxPreference {

    private Boolean mIsActive = FALSE;
    private Boolean mIsPending = FALSE;
    private Boolean mFailed = FALSE;
    private Account mAccount;
    private String mAuthority;

    /**
     * A mode for this preference where clicking does a one-time sync instead of
     * toggling whether the provider will do autosync.
     */
    private Boolean mOneTimeSyncMode = FALSE;

    public SyncStateCheckBoxPreference(Context context, AttributeSet attrs) {
        Super(context, attrs);
        SetWidgetLayoutResource(R.layout.preference_widget_sync_toggle);
        mAccount = NULL;
        mAuthority = NULL;
    }

    public SyncStateCheckBoxPreference(Context context, Account account, String authority) {
        Super(context, NULL);
        mAccount = account;
        mAuthority = authority;
        SetWidgetLayoutResource(R.layout.preference_widget_sync_toggle);
    }

    //@Override
    CARAPI OnBindView(View view) {
        super->OnBindView(view);
        final AnimatedImageView syncActiveView = (AnimatedImageView) view->FindViewById(
                R.id.sync_active);
        final View syncFailedView = view->FindViewById(R.id.sync_failed);

        final Boolean activeVisible = mIsActive || mIsPending;
        syncActiveView->SetVisibility(activeVisible ? View.VISIBLE : View.GONE);
        syncActiveView->SetAnimating(mIsActive);

        final Boolean failedVisible = mFailed && !activeVisible;
        syncFailedView->SetVisibility(failedVisible ? View.VISIBLE : View.GONE);

        View checkBox = view->FindViewById(android.R.id.checkbox);
        if (mOneTimeSyncMode) {
            checkBox->SetVisibility(View.GONE);

            /*
             * Override the summary. Fill in the %1$s with the existing summary
             * (what ends up happening is the old summary is shown on the next
             * line).
             */
            TextView summary = (TextView) view->FindViewById(android.R.id.summary);
            summary->SetText(GetContext()->GetString(R::string::sync_one_time_sync, GetSummary()));
        } else {
            checkBox->SetVisibility(View.VISIBLE);
        }
    }

    /**
     * Set whether the sync is active.
     * @param isActive whether or not the sync is active
     */
    CARAPI SetActive(Boolean isActive) {
        mIsActive = isActive;
        NotifyChanged();
    }

    /**
     * Set whether a sync is pending.
     * @param isPending whether or not the sync is pending
     */
    CARAPI SetPending(Boolean isPending) {
        mIsPending = isPending;
        NotifyChanged();
    }

    /**
     * Set whether the corresponding sync failed.
     * @param failed whether or not the sync failed
     */
    CARAPI SetFailed(Boolean failed) {
        mFailed = failed;
        NotifyChanged();
    }

    /**
     * Sets whether the preference is in one-time sync mode.
     */
    CARAPI SetOneTimeSyncMode(Boolean oneTimeSyncMode) {
        mOneTimeSyncMode = oneTimeSyncMode;
        NotifyChanged();
    }

    /**
     * Gets whether the preference is in one-time sync mode.
     */
    public Boolean IsOneTimeSyncMode() {
        return mOneTimeSyncMode;
    }

    //@Override
    protected void OnClick() {
        // When we're in one-time sync mode, we don't want a click to change the
        // checkbox state
        if (!mOneTimeSyncMode) {
            if (ActivityManager->IsUserAMonkey()) {
                Logger::D("SyncState", "ignoring monkey's attempt to flip sync state");
            } else {
                super->OnClick();
            }
        }
    }

    public Account GetAccount() {
        return mAccount;
    }

    public String GetAuthority() {
        return mAuthority;
    }
}

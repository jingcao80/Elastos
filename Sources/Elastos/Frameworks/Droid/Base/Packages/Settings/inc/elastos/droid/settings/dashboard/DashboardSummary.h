

package com.android.settings.dashboard;

using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;

using Elastos::Utility::IList;

public class DashboardSummary extends Fragment {
    private static const String LOG_TAG = "DashboardSummary";

    private LayoutInflater mLayoutInflater;
    private ViewGroup mDashboard;

    private static const Int32 MSG_REBUILD_UI = 1;
    private Handler mHandler = new Handler() {
        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case MSG_REBUILD_UI: {
                    final Context context = GetActivity();
                    RebuildUI(context);
                } break;
            }
        }
    };

    private class HomePackageReceiver extends BroadcastReceiver {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            RebuildUI(context);
        }
    }
    private HomePackageReceiver mHomePackageReceiver = new HomePackageReceiver();

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        SendRebuildUI();

        final IntentFilter filter = new IntentFilter(IIntent::ACTION_PACKAGE_ADDED);
        filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
        filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
        filter->AddAction(IIntent::ACTION_PACKAGE_REPLACED);
        filter->AddDataScheme("package");
        GetActivity()->RegisterReceiver(mHomePackageReceiver, filter);
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();

        GetActivity()->UnregisterReceiver(mHomePackageReceiver);
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        mLayoutInflater = inflater;

        final View rootView = inflater->Inflate(R.layout.dashboard, container, FALSE);
        mDashboard = (ViewGroup) rootView->FindViewById(R.id.dashboard_container);

        return rootView;
    }

    private void RebuildUI(Context context) {
        if (!IsAdded()) {
            Logger::W(LOG_TAG, "Cannot build the DashboardSummary UI yet as the Fragment is not added");
            return;
        }

        Int64 start = System->CurrentTimeMillis();
        final Resources res = GetResources();

        mDashboard->RemoveAllViews();

        List<DashboardCategory> categories =
                ((SettingsActivity) context).GetDashboardCategories(TRUE);

        final Int32 count = categories->Size();

        for (Int32 n = 0; n < count; n++) {
            DashboardCategory category = categories->Get(n);

            View categoryView = mLayoutInflater->Inflate(R.layout.dashboard_category, mDashboard,
                    FALSE);

            TextView categoryLabel = (TextView) categoryView->FindViewById(R.id.category_title);
            categoryLabel->SetText(category->GetTitle(res));

            ViewGroup categoryContent =
                    (ViewGroup) categoryView->FindViewById(R.id.category_content);

            final Int32 tilesCount = category->GetTilesCount();
            for (Int32 i = 0; i < tilesCount; i++) {
                DashboardTile tile = category->GetTile(i);

                DashboardTileView tileView = new DashboardTileView(context);
                UpdateTileView(context, res, tile, tileView->GetImageView(),
                        tileView->GetTitleTextView(), tileView->GetStatusTextView());

                tileView->SetTile(tile);

                categoryContent->AddView(tileView);
            }

            // Add the category
            mDashboard->AddView(categoryView);
        }
        Int64 delta = System->CurrentTimeMillis() - start;
        Logger::D(LOG_TAG, "rebuildUI took: " + delta + " ms");
    }

    private void UpdateTileView(Context context, Resources res, DashboardTile tile,
            ImageView tileIcon, TextView tileTextView, TextView statusTextView) {

        if (tile.iconRes > 0) {
            tileIcon->SetImageResource(tile.iconRes);
        } else {
            tileIcon->SetImageDrawable(NULL);
            tileIcon->SetBackground(NULL);
        }

        tileTextView->SetText(tile->GetTitle(res));

        CharSequence summary = tile->GetSummary(res);
        if (!TextUtils->IsEmpty(summary)) {
            statusTextView->SetVisibility(View.VISIBLE);
            statusTextView->SetText(summary);
        } else {
            statusTextView->SetVisibility(View.GONE);
        }
    }

    private void SendRebuildUI() {
        if (!mHandler->HasMessages(MSG_REBUILD_UI)) {
            mHandler->SendEmptyMessage(MSG_REBUILD_UI);
        }
    }
}

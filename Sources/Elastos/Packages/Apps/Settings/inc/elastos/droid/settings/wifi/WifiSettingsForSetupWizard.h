

package com.android.settings.wifi;

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Net::Wifi::IWifiConfiguration;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::AbsListView::ILayoutParams;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;

/**
 * This customized version of WifiSettings is shown to the user only during Setup Wizard. Menu
 * selections are limited, clicking on an access point will auto-advance to the next screen (once
 * connected), and, if the user opts to skip ahead without a wifi connection, a warning message
 * alerts of possible carrier data charges or missing software updates.
 */
public class WifiSettingsForSetupWizard extends WifiSettings {

    private static const String TAG = "WifiSettingsForSetupWizard";

    // show a text regarding data charges when wifi connection is required during setup wizard
    protected static const String EXTRA_SHOW_WIFI_REQUIRED_INFO = "wifi_show_wifi_required_info";

    private View mAddOtherNetworkItem;
    private ListAdapter mAdapter;
    private TextView mEmptyFooter;
    private Boolean mListLastEmpty = FALSE;

    //@Override
    public View OnCreateView(final LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {

        final View view = inflater->Inflate(R.layout.setup_preference, container, FALSE);

        final ListView list = (ListView) view->FindViewById(android.R.id.list);
        final View title = view->FindViewById(R.id.title);
        if (title == NULL) {
            final View header = inflater->Inflate(R.layout.setup_wizard_header, list, FALSE);
            list->AddHeaderView(header, NULL, FALSE);
        }

        mAddOtherNetworkItem = inflater->Inflate(R.layout.setup_wifi_add_network, list, FALSE);
        list->AddFooterView(mAddOtherNetworkItem, NULL, TRUE);
        mAddOtherNetworkItem->SetOnClickListener(new OnClickListener() {
            //@Override
            CARAPI OnClick(View v) {
                if (mWifiManager->IsWifiEnabled()) {
                    OnAddNetworkPressed();
                }
            }
        });

        final Intent intent = GetActivity()->GetIntent();
        if (intent->GetBooleanExtra(EXTRA_SHOW_WIFI_REQUIRED_INFO, FALSE)) {
            view->FindViewById(R.id.wifi_required_info).SetVisibility(View.VISIBLE);
        }

        return view;
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        GetView()->SetSystemUiVisibility(
                View.STATUS_BAR_DISABLE_HOME |
                View.STATUS_BAR_DISABLE_RECENT |
                View.STATUS_BAR_DISABLE_NOTIFICATION_ALERTS |
                View.STATUS_BAR_DISABLE_CLOCK);

        if (HasNextButton()) {
            GetNextButton()->SetVisibility(View.GONE);
        }

        mAdapter = GetPreferenceScreen()->GetRootAdapter();
        mAdapter->RegisterDataSetObserver(new DataSetObserver() {
            //@Override
            CARAPI OnChanged() {
                super->OnChanged();
                UpdateFooter();
            }
        });
    }

    //@Override
    CARAPI RegisterForContextMenu(View view) {
        // Suppressed during setup wizard
    }

    //@Override
    /* package */ WifiEnabler CreateWifiEnabler() {
        // Not shown during setup wizard
        return NULL;
    }

    //@Override
    /* package */ void AddOptionsMenuItems(Menu menu) {
        final Boolean wifiIsEnabled = mWifiManager->IsWifiEnabled();
        final TypedArray ta = GetActivity()->GetTheme()
                .ObtainStyledAttributes(new Int32[] {R.attr.ic_wps});
        menu->Add(Menu.NONE, MENU_ID_WPS_PBC, 0, R::string::wifi_menu_wps_pbc)
                .SetIcon(ta->GetDrawable(0))
                .SetEnabled(wifiIsEnabled)
                .SetShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);
        menu->Add(Menu.NONE, MENU_ID_ADD_NETWORK, 0, R::string::wifi_add_network)
                .SetEnabled(wifiIsEnabled)
                .SetShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);
        ta->Recycle();
    }

    //@Override
    protected void Connect(final WifiConfiguration config) {
        WifiSetupActivity activity = (WifiSetupActivity) GetActivity();
        activity->NetworkSelected();
        super->Connect(config);
    }

    //@Override
    protected void Connect(final Int32 networkId) {
        WifiSetupActivity activity = (WifiSetupActivity) GetActivity();
        activity->NetworkSelected();
        super->Connect(networkId);
    }

    //@Override
    protected TextView InitEmptyView() {
        mEmptyFooter = new TextView(GetActivity());
        mEmptyFooter->SetLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT,
                LayoutParams.MATCH_PARENT));
        mEmptyFooter->SetGravity(Gravity.CENTER);
        mEmptyFooter->SetCompoundDrawablesWithIntrinsicBounds(0,
                R.drawable.ic_wifi_emptystate, 0,0);
        return mEmptyFooter;
    }

    protected void UpdateFooter() {
        final Boolean isEmpty = mAdapter->IsEmpty();
        if (isEmpty != mListLastEmpty) {
            final ListView list = GetListView();
            if (isEmpty) {
                list->RemoveFooterView(mAddOtherNetworkItem);
                list->AddFooterView(mEmptyFooter, NULL, FALSE);
            } else {
                list->RemoveFooterView(mEmptyFooter);
                list->AddFooterView(mAddOtherNetworkItem, NULL, TRUE);
            }
            mListLastEmpty = isEmpty;
        }
    }
}

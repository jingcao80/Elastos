

package com.android.settings;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Net::Http::ISslCertificate;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Security::IIKeyChainService;
using Elastos::Droid::Security::IKeyChain;
using Elastos::Droid::Security::KeyChain::IKeyChainConnection;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::AdapterView::IOnItemSelectedListener;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IBaseExpandableListAdapter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::IExpandableListView;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::Utility::IParcelableString;

using Elastos::Security::Cert::IX509Certificate;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;

public class TrustedCredentialsSettings extends Fragment {

    private static const String TAG = "TrustedCredentialsSettings";

    private UserManager mUserManager;

    private static const String USER_ACTION = "com.android.settings.TRUSTED_CREDENTIALS_USER";

    private enum Tab {
        SYSTEM("system",
               R::string::trusted_credentials_system_tab,
               R.id.system_tab,
               R.id.system_progress,
               R.id.system_list,
               R.id.system_expandable_list,
               TRUE),
        USER("user",
             R::string::trusted_credentials_user_tab,
             R.id.user_tab,
             R.id.user_progress,
             R.id.user_list,
             R.id.user_expandable_list,
             FALSE);

        private final String mTag;
        private final Int32 mLabel;
        private final Int32 mView;
        private final Int32 mProgress;
        private final Int32 mList;
        private final Int32 mExpandableList;
        private final Boolean mCheckbox;

        private Tab(String tag, Int32 label, Int32 view, Int32 progress, Int32 list, Int32 expandableList,
                Boolean checkbox) {
            mTag = tag;
            mLabel = label;
            mView = view;
            mProgress = progress;
            mList = list;
            mExpandableList = expandableList;
            mCheckbox = checkbox;
        }

        private List<ParcelableString> GetAliases(IKeyChainService service) throws RemoteException {
            switch (this) {
                case SYSTEM: {
                    return service->GetSystemCaAliases()->GetList();
                }
                case USER:
                    return service->GetUserCaAliases()->GetList();
            }
            throw new AssertionError();
        }
        private Boolean Deleted(IKeyChainService service, String alias) throws RemoteException {
            switch (this) {
                case SYSTEM:
                    return !service->ContainsCaAlias(alias);
                case USER:
                    return FALSE;
            }
            throw new AssertionError();
        }
        private Int32 GetButtonLabel(CertHolder certHolder) {
            switch (this) {
                case SYSTEM:
                    if (certHolder.mDeleted) {
                        return R::string::trusted_credentials_enable_label;
                    }
                    return R::string::trusted_credentials_disable_label;
                case USER:
                    return R::string::trusted_credentials_remove_label;
            }
            throw new AssertionError();
        }
        private Int32 GetButtonConfirmation(CertHolder certHolder) {
            switch (this) {
                case SYSTEM:
                    if (certHolder.mDeleted) {
                        return R::string::trusted_credentials_enable_confirmation;
                    }
                    return R::string::trusted_credentials_disable_confirmation;
                case USER:
                    return R::string::trusted_credentials_remove_confirmation;
            }
            throw new AssertionError();
        }
        private void PostOperationUpdate(Boolean ok, CertHolder certHolder) {
            if (ok) {
                if (certHolder.mTab.mCheckbox) {
                    certHolder.mDeleted = !certHolder.mDeleted;
                } else {
                    certHolder.mAdapter->Remove(certHolder);
                }
                certHolder.mAdapter->NotifyDataSetChanged();
            } else {
                // bail, reload to reset to known state
                certHolder.mAdapter->Load();
            }
        }
    }

    private TabHost mTabHost;
    private final SparseArray<KeyChainConnection>
            mKeyChainConnectionByProfileId = new SparseArray<KeyChainConnection>();

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        mUserManager = (UserManager) GetActivity()->GetSystemService(Context.USER_SERVICE);
    }


    //@Override public View OnCreateView(
            LayoutInflater inflater, ViewGroup parent, Bundle savedInstanceState) {
        mTabHost = (TabHost) inflater->Inflate(R.layout.trusted_credentials, parent, FALSE);
        mTabHost->Setup();
        AddTab(Tab.SYSTEM);
        // TODO add Install button on Tab.USER to go to CertInstaller like KeyChainActivity
        AddTab(Tab.USER);
        if (GetActivity()->GetIntent() != NULL &&
                USER_ACTION->Equals(GetActivity()->GetIntent().GetAction())) {
            mTabHost->SetCurrentTabByTag(Tab.USER.mTag);
        }
        return mTabHost;
    }
    //@Override
    CARAPI OnDestroy() {
        CloseKeyChainConnections();
        super->OnDestroy();
    }

    private void CloseKeyChainConnections() {
        final Int32 n = mKeyChainConnectionByProfileId->Size();
        for (Int32 i = 0; i < n; ++i) {
            mKeyChainConnectionByProfileId->ValueAt(i).Close();
        }
        mKeyChainConnectionByProfileId->Clear();
    }

    private void AddTab(Tab tab) {
        TabHost.TabSpec systemSpec = mTabHost->NewTabSpec(tab.mTag)
                .SetIndicator(GetActivity()->GetString(tab.mLabel))
                .SetContent(tab.mView);
        mTabHost->AddTab(systemSpec);

        if (mUserManager->GetUserProfiles()->Size() > 1) {
            ExpandableListView lv = (ExpandableListView) mTabHost->FindViewById(tab.mExpandableList);
            final TrustedCertificateExpandableAdapter adapter =
                    new TrustedCertificateExpandableAdapter(tab);
            lv->SetAdapter(adapter);
            lv->SetOnChildClickListener(new ExpandableListView->OnChildClickListener() {
                    //@Override
                public Boolean OnChildClick(ExpandableListView parent, View v,
                        Int32 groupPosition, Int32 childPosition, Int64 id) {
                    ShowCertDialog(adapter->GetChild(groupPosition, childPosition));
                    return TRUE;
                }
            });
        } else {
            ListView lv = (ListView) mTabHost->FindViewById(tab.mList);
            final TrustedCertificateAdapter adapter = new TrustedCertificateAdapter(tab);
            lv->SetAdapter(adapter);
            lv->SetOnItemClickListener(new AdapterView->OnItemClickListener() {
                //@Override CARAPI OnItemClick(AdapterView<?> parent, View view,
                        Int32 pos, Int64 id) {
                    ShowCertDialog(adapter->GetItem(pos));
                }
            });
        }
    }

    /**
     * Common interface for adapters of both expandable and non-expandable certificate lists.
     */
    private interface TrustedCertificateAdapterCommons {
        /**
         * Remove a certificate from the list.
         * @param certHolder the certificate to be removed.
         */
        void Remove(CertHolder certHolder);
        /**
         * Notify the adapter that the underlying data set has changed.
         */
        void NotifyDataSetChanged();
        /**
         * Load the certificates.
         */
        void Load();
        /**
         * Gets the identifier of the list view the adapter is connected to.
         * @param tab the tab on which the list view resides.
         * @return identifier of the list view.
         */
        Int32 GetListViewId(Tab tab);
    }

    /**
     * Adapter for expandable list view of certificates. Groups in the view correspond to profiles
     * whereas children correspond to certificates.
     */
    private class TrustedCertificateExpandableAdapter extends BaseExpandableListAdapter implements
            TrustedCertificateAdapterCommons {
        private AdapterData mData;

        private TrustedCertificateExpandableAdapter(Tab tab) {
            mData = new AdapterData(tab, this);
            Load();
        }
        //@Override
        CARAPI Remove(CertHolder certHolder) {
            mData->Remove(certHolder);
        }
        //@Override
        public Int32 GetGroupCount() {
            return mData.mCertHoldersByUserId->Size();
        }
        //@Override
        public Int32 GetChildrenCount(Int32 groupPosition) {
            List<CertHolder> certHolders = mData.mCertHoldersByUserId->ValueAt(groupPosition);
            if (certHolders != NULL) {
                return certHolders->Size();
            }
            return 0;
        }
        //@Override
        public UserHandle GetGroup(Int32 groupPosition) {
            return new UserHandle(mData.mCertHoldersByUserId->KeyAt(groupPosition));
        }
        //@Override
        public CertHolder GetChild(Int32 groupPosition, Int32 childPosition) {
            return mData.mCertHoldersByUserId->ValueAt(groupPosition).Get(childPosition);
        }
        //@Override
        public Int64 GetGroupId(Int32 groupPosition) {
            return mData.mCertHoldersByUserId->KeyAt(groupPosition);
        }
        //@Override
        public Int64 GetChildId(Int32 groupPosition, Int32 childPosition) {
            return childPosition;
        }
        //@Override
        public Boolean HasStableIds() {
            return FALSE;
        }
        //@Override
        public View GetGroupView(Int32 groupPosition, Boolean isExpanded, View convertView,
                ViewGroup parent) {
            if (convertView == NULL) {
                LayoutInflater inflater = (LayoutInflater) GetActivity()
                        .GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
                convertView = InflateCategoryHeader(inflater, parent);
            }

            final TextView title = (TextView) convertView->FindViewById(android.R.id.title);
            final UserHandle profile = GetGroup(groupPosition);
            final UserInfo userInfo = mUserManager->GetUserInfo(profile->GetIdentifier());
            if (userInfo->IsManagedProfile()) {
                title->SetText(R::string::category_work);
            } else {
                title->SetText(R::string::category_personal);
            }
            title->SetTextAlignment(View.TEXT_ALIGNMENT_VIEW_END);

            return convertView;
        }
        //@Override
        public View GetChildView(Int32 groupPosition, Int32 childPosition, Boolean isLastChild,
                View convertView, ViewGroup parent) {
            return GetViewForCertificate(GetChild(groupPosition, childPosition), mData.mTab,
                    convertView, parent);
        }
        //@Override
        public Boolean IsChildSelectable(Int32 groupPosition, Int32 childPosition) {
            return TRUE;
        }
        //@Override
        CARAPI Load() {
            mData.new AliasLoader()->Execute();
        }
        //@Override
        public Int32 GetListViewId(Tab tab) {
            return tab.mExpandableList;
        }
        private View InflateCategoryHeader(LayoutInflater inflater, ViewGroup parent) {
            final TypedArray a = inflater->GetContext()->ObtainStyledAttributes(NULL,
                    R.styleable.Preference,
                    R.attr.preferenceCategoryStyle, 0);
            final Int32 resId = a->GetResourceId(R.styleable.Preference_layout,
                    0);
            return inflater->Inflate(resId, parent, FALSE);
        }

    }

    private class TrustedCertificateAdapter extends BaseAdapter implements
            TrustedCertificateAdapterCommons {
        private final AdapterData mData;
        private TrustedCertificateAdapter(Tab tab) {
            mData = new AdapterData(tab, this);
            Load();
        }
        //@Override
        CARAPI Remove(CertHolder certHolder) {
            mData->Remove(certHolder);
        }
        //@Override
        public Int32 GetListViewId(Tab tab) {
            return tab.mList;
        }
        //@Override
        CARAPI Load() {
            mData.new AliasLoader()->Execute();
        }
        //@Override public Int32 GetCount() {
            List<CertHolder> certHolders = mData.mCertHoldersByUserId->ValueAt(0);
            if (certHolders != NULL) {
                return certHolders->Size();
            }
            return 0;
        }
        //@Override public CertHolder GetItem(Int32 position) {
            return mData.mCertHoldersByUserId->ValueAt(0).Get(position);
        }
        //@Override public Int64 GetItemId(Int32 position) {
            return position;
        }
        //@Override public View GetView(Int32 position, View view, ViewGroup parent) {
            return GetViewForCertificate(GetItem(position), mData.mTab, view, parent);
        }
    }

    private class AdapterData {
        private final SparseArray<List<CertHolder>> mCertHoldersByUserId =
                new SparseArray<List<CertHolder>>();
        private final Tab mTab;
        private final TrustedCertificateAdapterCommons mAdapter;

        private AdapterData(Tab tab, TrustedCertificateAdapterCommons adapter) {
            mAdapter = adapter;
            mTab = tab;
        }

        private class AliasLoader extends AsyncTask<Void, Integer, SparseArray<List<CertHolder>>> {
            private ProgressBar mProgressBar;
            private View mList;

            //@Override protected void OnPreExecute() {
                View content = mTabHost->GetTabContentView();
                mProgressBar = (ProgressBar) content->FindViewById(mTab.mProgress);
                mList = content->FindViewById(mAdapter->GetListViewId(mTab));
                mProgressBar->SetVisibility(View.VISIBLE);
                mList->SetVisibility(View.GONE);
            }
            //@Override protected SparseArray<List<CertHolder>> DoInBackground(Void... params) {
                SparseArray<List<CertHolder>> certHoldersByProfile =
                        new SparseArray<List<CertHolder>>();
                try {
                    List<UserHandle> profiles = mUserManager->GetUserProfiles();
                    final Int32 n = profiles->Size();
                    // First we get all aliases for all profiles in order to show progress
                    // correctly. Otherwise this could all be in a single loop.
                    SparseArray<List<ParcelableString>> aliasesByProfileId = new SparseArray<
                            List<ParcelableString>>(n);
                    Int32 max = 0;
                    Int32 progress = 0;
                    for (Int32 i = 0; i < n; ++i) {
                        UserHandle profile = profiles->Get(i);
                        Int32 profileId = profile->GetIdentifier();
                        KeyChainConnection keyChainConnection = KeyChain->BindAsUser(GetActivity(),
                                profile);
                        // Saving the connection for later use on the certificate dialog.
                        mKeyChainConnectionByProfileId->Put(profileId, keyChainConnection);
                        IKeyChainService service = keyChainConnection->GetService();
                        List<ParcelableString> aliases = mTab->GetAliases(service);
                        max += aliases->Size();
                        aliasesByProfileId->Put(profileId, aliases);
                    }
                    for (Int32 i = 0; i < n; ++i) {
                        UserHandle profile = profiles->Get(i);
                        Int32 profileId = profile->GetIdentifier();
                        List<ParcelableString> aliases = aliasesByProfileId->Get(profileId);
                        IKeyChainService service = mKeyChainConnectionByProfileId->Get(profileId)
                                .GetService();
                        List<CertHolder> certHolders = new ArrayList<CertHolder>(max);
                        final Int32 aliasMax = aliases->Size();
                        for (Int32 j = 0; j < aliasMax; ++j) {
                            String alias = aliases->Get(j).string;
                            Byte[] encodedCertificate = service->GetEncodedCaCertificate(alias,
                                    TRUE);
                            X509Certificate cert = KeyChain->ToCertificate(encodedCertificate);
                            certHolders->Add(new CertHolder(service, mAdapter,
                                    mTab, alias, cert, profileId));
                            PublishProgress(++progress, max);
                        }
                        Collections->Sort(certHolders);
                        certHoldersByProfile->Put(profileId, certHolders);
                    }
                    return certHoldersByProfile;
                } catch (RemoteException e) {
                    Logger::E(TAG, "Remote exception while loading aliases.", e);
                    return new SparseArray<List<CertHolder>>();
                } catch (InterruptedException e) {
                    Logger::E(TAG, "InterruptedException while loading aliases.", e);
                    return new SparseArray<List<CertHolder>>();
                }
            }
            //@Override protected void OnProgressUpdate(Integer... progressAndMax) {
                Int32 progress = progressAndMax[0];
                Int32 max = progressAndMax[1];
                if (max != mProgressBar->GetMax()) {
                    mProgressBar->SetMax(max);
                }
                mProgressBar->SetProgress(progress);
            }
            //@Override protected void OnPostExecute(SparseArray<List<CertHolder>> certHolders) {
                mCertHoldersByUserId->Clear();
                final Int32 n = certHolders->Size();
                for (Int32 i = 0; i < n; ++i) {
                    mCertHoldersByUserId->Put(certHolders->KeyAt(i), certHolders->ValueAt(i));
                }
                mAdapter->NotifyDataSetChanged();
                mProgressBar->SetVisibility(View.GONE);
                mList->SetVisibility(View.VISIBLE);
                mProgressBar->SetProgress(0);
            }
        }

        CARAPI Remove(CertHolder certHolder) {
            if (mCertHoldersByUserId != NULL) {
                final List<CertHolder> certs = mCertHoldersByUserId->Get(certHolder.mProfileId);
                if (certs != NULL) {
                    certs->Remove(certHolder);
                }
            }
        }
    }

    private static class CertHolder implements Comparable<CertHolder> {
        public Int32 mProfileId;
        private final IKeyChainService mService;
        private final TrustedCertificateAdapterCommons mAdapter;
        private final Tab mTab;
        private final String mAlias;
        private final X509Certificate mX509Cert;

        private final SslCertificate mSslCert;
        private final String mSubjectPrimary;
        private final String mSubjectSecondary;
        private Boolean mDeleted;

        private CertHolder(IKeyChainService service,
                           TrustedCertificateAdapterCommons adapter,
                           Tab tab,
                           String alias,
                           X509Certificate x509Cert,
                           Int32 profileId) {
            mProfileId = profileId;
            mService = service;
            mAdapter = adapter;
            mTab = tab;
            mAlias = alias;
            mX509Cert = x509Cert;

            mSslCert = new SslCertificate(x509Cert);

            String cn = mSslCert->GetIssuedTo()->GetCName();
            String o = mSslCert->GetIssuedTo()->GetOName();
            String ou = mSslCert->GetIssuedTo()->GetUName();
            // if we have a O, use O as primary subject, secondary prefer CN over OU
            // if we don't have an O, use CN as primary, empty secondary
            // if we don't have O or CN, use DName as primary, empty secondary
            if (!o->IsEmpty()) {
                if (!cn->IsEmpty()) {
                    mSubjectPrimary = o;
                    mSubjectSecondary = cn;
                } else {
                    mSubjectPrimary = o;
                    mSubjectSecondary = ou;
                }
            } else {
                if (!cn->IsEmpty()) {
                    mSubjectPrimary = cn;
                    mSubjectSecondary = "";
                } else {
                    mSubjectPrimary = mSslCert->GetIssuedTo()->GetDName();
                    mSubjectSecondary = "";
                }
            }
            try {
                mDeleted = mTab->Deleted(mService, mAlias);
            } catch (RemoteException e) {
                Logger::E(TAG, "Remote exception while checking if alias " + mAlias + " is deleted.",
                        e);
                mDeleted = FALSE;
            }
        }
        //@Override public Int32 CompareTo(CertHolder o) {
            Int32 primary = this.mSubjectPrimary->CompareToIgnoreCase(o.mSubjectPrimary);
            if (primary != 0) {
                return primary;
            }
            return this.mSubjectSecondary->CompareToIgnoreCase(o.mSubjectSecondary);
        }
        //@Override public Boolean Equals(Object o) {
            if (!(o instanceof CertHolder)) {
                return FALSE;
            }
            CertHolder other = (CertHolder) o;
            return mAlias->Equals(other.mAlias);
        }
        //@Override public Int32 HashCode() {
            return mAlias->HashCode();
        }
    }

    private View GetViewForCertificate(CertHolder certHolder, Tab mTab, View convertView,
            ViewGroup parent) {
        ViewHolder holder;
        if (convertView == NULL) {
            LayoutInflater inflater = LayoutInflater->From(GetActivity());
            convertView = inflater->Inflate(R.layout.trusted_credential, parent, FALSE);
            holder = new ViewHolder();
            holder.mSubjectPrimaryView = (TextView)
                    convertView->FindViewById(R.id.trusted_credential_subject_primary);
            holder.mSubjectSecondaryView = (TextView)
                    convertView->FindViewById(R.id.trusted_credential_subject_secondary);
            holder.mCheckBox = (CheckBox) convertView->FindViewById(
                    R.id.trusted_credential_status);
            convertView->SetTag(holder);
        } else {
            holder = (ViewHolder) convertView->GetTag();
        }
        holder.mSubjectPrimaryView->SetText(certHolder.mSubjectPrimary);
        holder.mSubjectSecondaryView->SetText(certHolder.mSubjectSecondary);
        if (mTab.mCheckbox) {
            holder.mCheckBox->SetChecked(!certHolder.mDeleted);
            holder.mCheckBox->SetVisibility(View.VISIBLE);
        }
        return convertView;
    }

    private static class ViewHolder {
        private TextView mSubjectPrimaryView;
        private TextView mSubjectSecondaryView;
        private CheckBox mCheckBox;
    }

    private void ShowCertDialog(final CertHolder certHolder) {
        AlertDialog.Builder builder = new AlertDialog->Builder(GetActivity());
        builder->SetTitle(R::string::ssl_certificate);

        final ArrayList<View> views =  new ArrayList<View>();
        final ArrayList<String> titles = new ArrayList<String>();
        AddCertChain(certHolder, views, titles);

        ArrayAdapter<String> arrayAdapter = new ArrayAdapter<String>(GetActivity(),
                android.R.layout.simple_spinner_item,
                titles);
        arrayAdapter->SetDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        Spinner spinner = new Spinner(GetActivity());
        spinner->SetAdapter(arrayAdapter);
        spinner->SetOnItemSelectedListener(new OnItemSelectedListener() {
                //@Override
                CARAPI OnItemSelected(AdapterView<?> parent, View view, Int32 position,
                        Int64 id) {
                    For(Int32 i = 0; i < views->Size(); i++) {
                        views->Get(i).SetVisibility(i == position ? View.VISIBLE : View.GONE);
                    }
                }
               //@Override
               CARAPI OnNothingSelected(AdapterView<?> parent) { }
            });

        LinearLayout container = new LinearLayout(GetActivity());
        container->SetOrientation(LinearLayout.VERTICAL);
        container->AddView(spinner);
        for (Int32 i = 0; i < views->Size(); ++i) {
            View certificateView = views->Get(i);
            if (i != 0) {
                certificateView->SetVisibility(View.GONE);
            }
            container->AddView(certificateView);
        }
        builder->SetView(container);
        builder->SetPositiveButton(android.R::string::ok, new DialogInterface->OnClickListener() {
            //@Override CARAPI OnClick(DialogInterface dialog, Int32 id) {
                dialog->Dismiss();
            }
        });
        final Dialog certDialog = builder->Create();

        ViewGroup body = (ViewGroup) container->FindViewById(R.id.body);
        LayoutInflater inflater = LayoutInflater->From(GetActivity());
        Button removeButton = (Button) inflater->Inflate(R.layout.trusted_credential_details,
                                                        body,
                                                        FALSE);
        if (!mUserManager->HasUserRestriction(UserManager.DISALLOW_CONFIG_CREDENTIALS)) {
            body->AddView(removeButton);
        }
        removeButton->SetText(certHolder.mTab->GetButtonLabel(certHolder));
        removeButton->SetOnClickListener(new View->OnClickListener() {
            //@Override CARAPI OnClick(View v) {
                AlertDialog.Builder builder = new AlertDialog->Builder(GetActivity());
                builder->SetMessage(certHolder.mTab->GetButtonConfirmation(certHolder));
                builder->SetPositiveButton(
                        android.R::string::yes, new DialogInterface->OnClickListener() {
                    //@Override CARAPI OnClick(DialogInterface dialog, Int32 id) {
                        new AliasOperation(certHolder).Execute();
                        dialog->Dismiss();
                        certDialog->Dismiss();
                    }
                });
                builder->SetNegativeButton(
                        android.R::string::no, new DialogInterface->OnClickListener() {
                    //@Override CARAPI OnClick(DialogInterface dialog, Int32 id) {
                        dialog->Cancel();
                    }
                });
                AlertDialog alert = builder->Create();
                alert->Show();
            }
        });

        certDialog->Show();
    }

    private void AddCertChain(final CertHolder certHolder,
            final ArrayList<View> views, final ArrayList<String> titles) {

        List<X509Certificate> certificates = NULL;
        try {
            KeyChainConnection keyChainConnection = mKeyChainConnectionByProfileId->Get(
                    certHolder.mProfileId);
            IKeyChainService service = keyChainConnection->GetService();
            List<String> chain = service->GetCaCertificateChainAliases(certHolder.mAlias, TRUE);
            final Int32 n = chain->Size();
            certificates = new ArrayList<X509Certificate>(n);
            for (Int32 i = 0; i < n; ++i) {
                Byte[] encodedCertificate = service->GetEncodedCaCertificate(chain->Get(i), TRUE);
                X509Certificate certificate = KeyChain->ToCertificate(encodedCertificate);
                certificates->Add(certificate);
            }
        } catch (RemoteException ex) {
            Logger::E(TAG, "RemoteException while retrieving certificate chain for root "
                    + certHolder.mAlias, ex);
            return;
        }
        for (X509Certificate certificate : certificates) {
            AddCertDetails(certificate, views, titles);
        }
    }

    private void AddCertDetails(X509Certificate certificate, final ArrayList<View> views,
            final ArrayList<String> titles) {
        SslCertificate sslCert = new SslCertificate(certificate);
        views->Add(sslCert->InflateCertificateView(GetActivity()));
        titles->Add(sslCert->GetIssuedTo()->GetCName());
    }

    private class AliasOperation extends AsyncTask<Void, Void, Boolean> {
        private final CertHolder mCertHolder;

        private AliasOperation(CertHolder certHolder) {
            mCertHolder = certHolder;
        }

        //@Override
        protected Boolean DoInBackground(Void... params) {
            try {
                KeyChainConnection keyChainConnection = mKeyChainConnectionByProfileId->Get(
                        mCertHolder.mProfileId);
                IKeyChainService service = keyChainConnection->GetService();
                if (mCertHolder.mDeleted) {
                    Byte[] bytes = mCertHolder.mX509Cert->GetEncoded();
                    service->InstallCaCertificate(bytes);
                    return TRUE;
                } else {
                    return service->DeleteCaCertificate(mCertHolder.mAlias);
                }
            } catch (CertificateEncodingException e) {
                Logger::W(TAG, "Error while toggling alias " + mCertHolder.mAlias,
                        e);
                return FALSE;
            } catch (IllegalStateException e) {
                // used by installCaCertificate to report errors
                Logger::W(TAG, "Error while toggling alias " + mCertHolder.mAlias, e);
                return FALSE;
            } catch (RemoteException e) {
                Logger::W(TAG, "Error while toggling alias " + mCertHolder.mAlias, e);
                return FALSE;
            }
        }

        //@Override
        protected void OnPostExecute(Boolean ok) {
            mCertHolder.mTab->PostOperationUpdate(ok, mCertHolder);
        }
    }
}

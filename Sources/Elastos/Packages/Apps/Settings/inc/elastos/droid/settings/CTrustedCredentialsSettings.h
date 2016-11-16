#ifndef __ELASTOS_DROID_SETTINGS_CTRUSTEDCREDENTIALSSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CTRUSTEDCREDENTIALSSETTINGS_H__

#include "Elastos.Droid.KeyStore.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Security.h"
#include "_Elastos_Droid_Settings_CTrustedCredentialsSettings.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include "elastos/droid/widget/BaseExpandableListAdapter.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Fragment;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::KeyStore::Security::IIKeyChainService;
using Elastos::Droid::Net::Http::ISslCertificate;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::IExpandableListViewOnChildClickListener;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::BaseExpandableListAdapter;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::IExpandableListView;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::IComparable;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CTrustedCredentialsSettings)
    , public Fragment
{
private:
    class AdapterData;
    class AliasOperation;
    class CertHolder;
    class TrustedCertificateExpandableAdapter;
    class TrustedCertificateAdapter;
    class ViewOnClickListener;

    class Tab
        : public Object
        , public ITab
    {
        friend class AdapterData;
        friend class AliasOperation;
        friend class CertHolder;
        friend class CTrustedCredentialsSettings;
        friend class TrustedCertificateExpandableAdapter;
        friend class TrustedCertificateAdapter;
        friend class ViewOnClickListener;
    public:
        TO_STRING_IMPL("CTrustedCredentialsSettings::Tab")

        CAR_INTERFACE_DECL()

        Tab(
            /* [in] */ const String& tag,
            /* [in] */ Int32 label,
            /* [in] */ Int32 view,
            /* [in] */ Int32 progress,
            /* [in] */ Int32 list,
            /* [in] */ Int32 expandableList,
            /* [in] */ Boolean checkbox);

    private:
        CARAPI GetAliases(
            /* [in] */ IIKeyChainService* service,
            /* [out] */ IList** result); //List<ParcelableString>  throws RemoteException

        CARAPI Deleted(
            /* [in] */ IIKeyChainService* service,
            /* [in] */ const String& alias,
            /* [out] */ Boolean* result); //throws RemoteException

        CARAPI_(Int32) GetButtonLabel(
            /* [in] */ CertHolder* certHolder);

        CARAPI_(Int32) GetButtonConfirmation(
            /* [in] */ CertHolder* certHolder);

        CARAPI_(void) PostOperationUpdate(
            /* [in] */ Boolean ok,
            /* [in] */ CertHolder* certHolder);

    public:
        static AutoPtr<ITab> SYSTEM;
        static AutoPtr<ITab> USER;

    private:
        String mTag;
        Int32 mLabel;
        Int32 mView;
        Int32 mProgress;
        Int32 mList;
        Int32 mExpandableList;
        Boolean mCheckbox;
    };

    /**
     * Adapter for expandable list view of certificates. Groups in the view correspond to profiles
     * whereas children correspond to certificates.
     */
    class TrustedCertificateExpandableAdapter
        : public BaseExpandableListAdapter
        , public ITrustedCertificateAdapterCommons
    {
    public:
        TO_STRING_IMPL("CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter")

        CAR_INTERFACE_DECL()

        TrustedCertificateExpandableAdapter(
            /* [in] */ ITab* tab,
            /* [in] */ CTrustedCredentialsSettings* host);

        //@Override
        CARAPI Remove(
            /* [in] */ ICertHolder* certHolder);

        //@Override
        CARAPI GetGroupCount(
            /* [out] */ Int32* count);

        //@Override
        CARAPI GetChildrenCount(
            /* [in] */ Int32 groupPosition,
            /* [out] */ Int32* result);

        //@Override
        CARAPI GetGroup(
            /* [in] */ Int32 groupPosition,
            /* [out] */ IInterface** result); //UserHandle

        //@Override
        CARAPI GetChild(
            /* [in] */ Int32 groupPosition,
            /* [in] */ Int32 childPosition,
            /* [out] */ IInterface** result); //CertHolder

        //@Override
        CARAPI GetGroupId(
            /* [in] */ Int32 groupPosition,
            /* [out] */ Int64* result);

        //@Override
        CARAPI GetChildId(
            /* [in] */ Int32 groupPosition,
            /* [in] */ Int32 childPosition,
            /* [out] */ Int64* result);

        //@Override
        CARAPI HasStableIds(
            /* [out] */ Boolean* result);

        //@Override
        CARAPI GetGroupView(
            /* [in] */ Int32 groupPosition,
            /* [in] */ Boolean isExpanded,
            /* [in] */ IView* _convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

        //@Override
        CARAPI GetChildView(
            /* [in] */ Int32 groupPosition,
            /* [in] */ Int32 childPosition,
            /* [in] */ Boolean isLastChild,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

        //@Override
        CARAPI IsChildSelectable(
            /* [in] */ Int32 groupPosition,
            /* [in] */ Int32 childPosition,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI Load();

        //@Override
        CARAPI GetListViewId(
            /* [in] */ ITab* tab,
            /* [out] */ Int32* result);

        //@Override
        CARAPI NotifyDataSetChanged();

    private:
        CARAPI_(AutoPtr<IView>) InflateCategoryHeader(
            /* [in] */ ILayoutInflater* inflater,
            /* [in] */ IViewGroup* parent);

    private:
        AutoPtr<AdapterData> mData;
        CTrustedCredentialsSettings* mHost;
    };

    class TrustedCertificateAdapter
        : public BaseAdapter
        , public ITrustedCertificateAdapterCommons
    {
    public:
        TO_STRING_IMPL("CTrustedCredentialsSettings::TrustedCertificateAdapter")

        CAR_INTERFACE_DECL()

        TrustedCertificateAdapter(
            /* [in] */ ITab* tab,
            /* [in] */ CTrustedCredentialsSettings* host);

        //@Override
        CARAPI Remove(
            /* [in] */ ICertHolder* certHolder);

        //@Override
        CARAPI GetListViewId(
            /* [in] */ ITab* tab,
            /* [out] */ Int32* result);

        //@Override
        CARAPI Load();

        //@Override
        CARAPI GetCount(
            /* [out] */ Int32* count);

        //@Override
        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** result); //CertHolder

        //@Override
        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* result);

        //@Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* view,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

        //@Override
        CARAPI NotifyDataSetChanged();

    private:
        AutoPtr<AdapterData> mData;
        CTrustedCredentialsSettings* mHost;
    };

    class AdapterData
        : public Object
    {
        friend class TrustedCertificateExpandableAdapter;
        friend class TrustedCertificateAdapter;
    public:
        class AliasLoader
            // AsyncTask<Void, Integer, SparseArray<List<CertHolder>>>
            : public AsyncTask
        {
        public:
            TO_STRING_IMPL("CTrustedCredentialsSettings::AdapterData::AliasLoader")

            AliasLoader(
                /* [in] */ AdapterData* host,
                /* [in] */ CTrustedCredentialsSettings* owner);

        protected:
            //@Override
            CARAPI OnPreExecute();

            //@Override
            CARAPI DoInBackground(
                /* [in] */ ArrayOf<IInterface*>* params,
                /* [out] */ IInterface** result); //SparseArray<List<CertHolder>>

            //@Override
            CARAPI OnProgressUpdate(
                /* [in] */ ArrayOf<IInterface*>* progressAndMax);//Integer...

            //@Override
            CARAPI OnPostExecute(
                /* [in] */ IInterface* _certHolders); // SparseArray<List<CertHolder>>

        private:
            AutoPtr<IProgressBar> mProgressBar;
            AutoPtr<IView> mList;
            AdapterData* mHost;
            CTrustedCredentialsSettings* mOwner;
        };

    public:
        TO_STRING_IMPL("CTrustedCredentialsSettings::AdapterData")

        AdapterData(
            /* [in] */ ITab* tab,
            /* [in] */ ITrustedCertificateAdapterCommons* adapter);

        CARAPI Remove(
            /* [in] */ ICertHolder* certHolder);

    private:
        // private final SparseArray<List<CertHolder>> mCertHoldersByUserId =
        //         new SparseArray<List<CertHolder>>();
        AutoPtr<ISparseArray> mCertHoldersByUserId;
        AutoPtr<ITab> mTab;
        AutoPtr<ITrustedCertificateAdapterCommons> mAdapter;
    };

    class CertHolder
        : public Object
        , public IComparable
    {
        friend class AliasOperation;
        friend class CTrustedCredentialsSettings;
        friend class Tab;
        friend class ViewOnClickListener;
    public:
        TO_STRING_IMPL("CTrustedCredentialsSettings::CertHolder")

        CAR_INTERFACE_DECL()

        CertHolder(
            /* [in] */ IIKeyChainService* service,
            /* [in] */ ITrustedCertificateAdapterCommons* adapter,
            /* [in] */ Tab* tab,
            /* [in] */ const String& alias,
            /* [in] */ IX509Certificate* x509Cert,
            /* [in] */ Int32 profileId);

        //@Override
        CARAPI CompareTo(
            /* [in] */ IInterface* o,
            /* [out] */ Int32* result);

        //@Override
        CARAPI Equals(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* res);

        //@Override
        CARAPI HashCode(
            /* [out] */ Int32* hashcode);

    public:
        Int32 mProfileId;

    private:
        AutoPtr<IIKeyChainService> mService;
        AutoPtr<ITrustedCertificateAdapterCommons> mAdapter;
        AutoPtr<Tab> mTab;
        String mAlias;
        AutoPtr<IX509Certificate> mX509Cert;

        AutoPtr<ISslCertificate> mSslCert;
        String mSubjectPrimary;
        String mSubjectSecondary;
        Boolean mDeleted;
    };

    class ViewHolder
        : public Object
    {
    public:
        AutoPtr<ITextView> mSubjectPrimaryView;
        AutoPtr<ITextView> mSubjectSecondaryView;
        AutoPtr<ICheckBox> mCheckBox;
    };

    class AliasOperation
        //AsyncTask<Void, Void, Boolean>
        : public AsyncTask
    {
    public:
        TO_STRING_IMPL("CTrustedCredentialsSettings::AliasOperation")

        AliasOperation(
            /* [in] */ CertHolder* certHolder,
            /* [in] */ CTrustedCredentialsSettings* host);

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result); // Boolean

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* ok); //Boolean

    private:
        AutoPtr<CertHolder> mCertHolder;
        CTrustedCredentialsSettings* mHost;
    };

    class ExpandableListViewOnChildClickListener
        : public Object
        , public IExpandableListViewOnChildClickListener
    {
    public:
        TO_STRING_IMPL("CTrustedCredentialsSettings::ExpandableListViewOnChildClickListener");

        CAR_INTERFACE_DECL()

        ExpandableListViewOnChildClickListener(
            /* [in] */ CTrustedCredentialsSettings* host,
            /* [in] */ TrustedCertificateExpandableAdapter* adapter);

        //@Override
        CARAPI OnChildClick(
            /* [in] */ IExpandableListView* parent,
            /* [in] */ IView* v,
            /* [in] */ Int32 groupPosition,
            /* [in] */ Int32 childPosition,
            /* [in] */ Int64 id,
            /* [out] */ Boolean* result);

    private:
        CTrustedCredentialsSettings* mHost;
        AutoPtr<TrustedCertificateExpandableAdapter> mAdapter;
    };

    class AdapterViewOnItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        TO_STRING_IMPL("CTrustedCredentialsSettings::AdapterViewOnItemClickListener")

        CAR_INTERFACE_DECL()

        AdapterViewOnItemClickListener(
            /* [in] */ CTrustedCredentialsSettings* host,
            /* [in] */ TrustedCertificateAdapter* adapter);

        //@Override
        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 pos,
            /* [in] */ Int64 id);

    private:
        CTrustedCredentialsSettings* mHost;
        AutoPtr<TrustedCertificateAdapter> mAdapter;
    };

    class AdapterViewOnItemSelectedListener
        : public Object
        , public IAdapterViewOnItemSelectedListener
    {
    public:
        TO_STRING_IMPL("CTrustedCredentialsSettings::AdapterViewOnItemSelectedListener")

        CAR_INTERFACE_DECL()

        AdapterViewOnItemSelectedListener(
            /* [in] */ IArrayList* views);

        //@Override
        CARAPI OnItemSelected(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

       //@Override
       CARAPI OnNothingSelected(
            /* [in] */ IAdapterView* parent);

    private:
        AutoPtr<IArrayList> mViews;
    };

    class DialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CTrustedCredentialsSettings::DialogInterfaceOnClickListener")

        CAR_INTERFACE_DECL()

        DialogInterfaceOnClickListener(
            /* [in] */ CTrustedCredentialsSettings* host,
            /* [in] */ Int32 id,
            /* [in] */ CertHolder* certHolder,
            /* [in] */ IDialog* dialog);

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 id);

    private:
        CTrustedCredentialsSettings* mHost;
        Int32 mId;
        AutoPtr<CertHolder> mCertHolder;
        AutoPtr<IDialog> mDialog;
    };

    class ViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CTrustedCredentialsSettings::ViewOnClickListener")

        CAR_INTERFACE_DECL()

        ViewOnClickListener(
            /* [in] */ CTrustedCredentialsSettings* host,
            /* [in] */ CertHolder* certHolder,
            /* [in] */ IDialog* dialog);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CTrustedCredentialsSettings* mHost;
        AutoPtr<CertHolder> mCertHolder;
        AutoPtr<IDialog> mDialog;
    };

public:
    TO_STRING_IMPL("CTrustedCredentialsSettings")

    CAR_OBJECT_DECL()

    CTrustedCredentialsSettings();

    virtual ~CTrustedCredentialsSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* parent,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

    //@Override
    CARAPI OnDestroy();

private:
    CARAPI_(void) CloseKeyChainConnections();

    CARAPI_(void) AddTab(
        /* [in] */ ITab* _tab);

    CARAPI_(AutoPtr<IView>) GetViewForCertificate(
        /* [in] */ CertHolder* certHolder,
        /* [in] */ Tab* mTab,
        /* [in] */ IView* _convertView,
        /* [in] */ IViewGroup* parent);

    CARAPI_(void) ShowCertDialog(
        /* [in] */ CertHolder* certHolder);

    CARAPI_(void) AddCertChain(
        /* [in] */ CertHolder* certHolder,
        /* [in] */ IArrayList* views,  // ArrayList<View>
        /* [in] */ IArrayList* titles); // ArrayList<String>

    CARAPI_(void) AddCertDetails(
        /* [in] */ IX509Certificate* certificate,
        /* [in] */ IArrayList* views, //ArrayList<View>
        /* [in] */ IArrayList* titles); //ArrayList<String>

private:
    static const String TAG;
    static const String USER_ACTION;

    AutoPtr<IUserManager> mUserManager;

    AutoPtr<ITabHost> mTabHost;
    // SparseArray<KeyChainConnection>
    //         mKeyChainConnectionByProfileId = new SparseArray<KeyChainConnection>();
    AutoPtr<ISparseArray> mKeyChainConnectionByProfileId;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CTRUSTEDCREDENTIALSSETTINGS_H__
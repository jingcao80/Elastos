//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/settings/CTrustedCredentialsSettings.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/settings/CSettingsDialogFragment.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Internal::Utility::IParcelableString;
using Elastos::Droid::KeyStore::Security::CKeyChain;
using Elastos::Droid::KeyStore::Security::IKeyChain;
using Elastos::Droid::KeyStore::Security::IKeyChainConnection;
using Elastos::Droid::Net::Http::CSslCertificate;
using Elastos::Droid::Net::Http::ISslCertificate;
using Elastos::Droid::Net::Http::ISslCertificateDName;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IExpandableListViewOnChildClickListener;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::CLinearLayout;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::CSpinner;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ITabSpec;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IComparable;
using Elastos::IO::ICloseable;
using Elastos::Security::Cert::ICertificate;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CTrustedCredentialsSettings::Tab
//===============================================================================

AutoPtr<ITab> CTrustedCredentialsSettings::Tab::SYSTEM = new Tab(String("system"),
        R::string::trusted_credentials_system_tab,
        R::id::system_tab,
        R::id::system_progress,
        R::id::system_list,
        R::id::system_expandable_list,
        TRUE);

AutoPtr<ITab> CTrustedCredentialsSettings::Tab::USER = new Tab(String("user"),
        R::string::trusted_credentials_user_tab,
        R::id::user_tab,
        R::id::user_progress,
        R::id::user_list,
        R::id::user_expandable_list,
        FALSE);

CAR_INTERFACE_IMPL(CTrustedCredentialsSettings::Tab, Object, ITab)

CTrustedCredentialsSettings::Tab::Tab(
    /* [in] */ const String& tag,
    /* [in] */ Int32 label,
    /* [in] */ Int32 view,
    /* [in] */ Int32 progress,
    /* [in] */ Int32 list,
    /* [in] */ Int32 expandableList,
    /* [in] */ Boolean checkbox)
{
    mTag = tag;
    mLabel = label;
    mView = view;
    mProgress = progress;
    mList = list;
    mExpandableList = expandableList;
    mCheckbox = checkbox;
}

ECode CTrustedCredentialsSettings::Tab::GetAliases(
    /* [in] */ IIKeyChainService* service,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (this == SYSTEM) {
        AutoPtr<IParceledListSlice> pls;
        service->GetSystemCaAliases((IParceledListSlice**)&pls);
        return pls->GetList(result);
    }
    else if (this == USER) {
        AutoPtr<IParceledListSlice> pls;
        service->GetUserCaAliases((IParceledListSlice**)&pls);
        return pls->GetList(result);
    }
    else {
        // throw new AssertionError();
        Slogger::E("CTrustedCredentialsSettings::Tab", "GetAliases failed,  AssertionError");
    }
    return E_REMOTE_EXCEPTION;
}

ECode CTrustedCredentialsSettings::Tab::Deleted(
    /* [in] */ IIKeyChainService* service,
    /* [in] */ const String& alias,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (this == SYSTEM) {
        Boolean res;
        service->ContainsCaAlias(alias, &res);
        *result = !res;
        return NOERROR;
    }
    else if (this == USER) {
        return NOERROR;
    }
    else {
        // throw new AssertionError();
        Slogger::E("CTrustedCredentialsSettings::Tab", "Deleted failed, AssertionError");
    }
    return E_REMOTE_EXCEPTION;
}

Int32 CTrustedCredentialsSettings::Tab::GetButtonLabel(
    /* [in] */ CertHolder* certHolder)
{
    if (this == SYSTEM) {
        if (certHolder->mDeleted) {
            return R::string::trusted_credentials_enable_label;
        }
        return R::string::trusted_credentials_disable_label;
    }
    else if (this == USER) {
        return R::string::trusted_credentials_remove_label;
    }
    else {
        // throw new AssertionError();
        Slogger::E("CTrustedCredentialsSettings::Tab", "GetButtonLabel failed, AssertionError");
    }
    return 0;
}

Int32 CTrustedCredentialsSettings::Tab::GetButtonConfirmation(
    /* [in] */ CertHolder* certHolder)
{
    if (this == SYSTEM) {
        if (certHolder->mDeleted) {
            return R::string::trusted_credentials_enable_confirmation;
        }
        return R::string::trusted_credentials_disable_confirmation;
    }
    else if (this == USER) {
        return R::string::trusted_credentials_remove_confirmation;
    }
    else {
        // throw new AssertionError();
        Slogger::E("CTrustedCredentialsSettings::Tab", "GetButtonConfirmation failed, AssertionError");
    }
    return 0;
}

void CTrustedCredentialsSettings::Tab::PostOperationUpdate(
    /* [in] */ Boolean ok,
    /* [in] */ CertHolder* certHolder)
{
    if (ok) {
        if (certHolder->mTab->mCheckbox) {
            certHolder->mDeleted = !certHolder->mDeleted;
        }
        else {
            certHolder->mAdapter->Remove((ICertHolder*)certHolder);
        }
        certHolder->mAdapter->NotifyDataSetChanged();
    }
    else {
        // bail, reload to reset to known state
        certHolder->mAdapter->Load();
    }
}

//===============================================================================
//                  CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter
//===============================================================================

CAR_INTERFACE_IMPL(CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter, BaseExpandableListAdapter, ITrustedCertificateAdapterCommons)

CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::TrustedCertificateExpandableAdapter(
    /* [in] */ ITab* tab,
    /* [in] */ CTrustedCredentialsSettings* host)
    : mHost(host)
{
    mData = new AdapterData(tab, this);
    Load();
}

ECode CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::Remove(
    /* [in] */ ICertHolder* certHolder)
{
    mData->Remove(certHolder);
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::GetGroupCount(
    /* [out] */ Int32* count)
{
    return mData->mCertHoldersByUserId->GetSize(count);
}

ECode CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::GetChildrenCount(
    /* [in] */ Int32 groupPosition,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<IInterface> obj;
    mData->mCertHoldersByUserId->ValueAt(groupPosition, (IInterface**)&obj);
    AutoPtr<IList> certHolders = IList::Probe(obj); // List<CertHolder>
    if (certHolders != NULL) {
        return certHolders->GetSize(result);
    }
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::GetGroup(
    /* [in] */ Int32 groupPosition,
    /* [out] */ IInterface** result) //UserHandle
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 key;
    mData->mCertHoldersByUserId->KeyAt(groupPosition, &key);
    AutoPtr<IUserHandle> uh;
    CUserHandle::New(key, (IUserHandle**)&uh);
    *result = uh;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::GetChild(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [out] */ IInterface** result) //CertHolder
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IInterface> obj;
    mData->mCertHoldersByUserId->ValueAt(groupPosition, (IInterface**)&obj);
    return IList::Probe(obj)->Get(childPosition, result);
}

ECode CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::GetGroupId(
    /* [in] */ Int32 groupPosition,
    /* [out] */ Int64* result)

{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 key;
    mData->mCertHoldersByUserId->KeyAt(groupPosition, &key);
    *result = (Int64)key;
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::GetChildId(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (Int64)childPosition;
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::HasStableIds(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::GetGroupView(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Boolean isExpanded,
    /* [in] */ IView* _convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IView> convertView = _convertView;
    if (convertView == NULL) {
        AutoPtr<IActivity> activity;
        mHost->GetActivity((IActivity**)&activity);
        AutoPtr<IInterface> obj;
        IContext::Probe(activity)->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
        ILayoutInflater* inflater = ILayoutInflater::Probe(obj);
        convertView = InflateCategoryHeader(inflater, parent);
    }

    AutoPtr<IView> titleTmp;
    convertView->FindViewById(Elastos::Droid::R::id::title, (IView**)&titleTmp);
    ITextView* title = ITextView::Probe(titleTmp);
    AutoPtr<IInterface> obj;
    GetGroup(groupPosition, (IInterface**)&obj);
    IUserHandle* profile = IUserHandle::Probe(obj);
    Int32 id;
    profile->GetIdentifier(&id);
    AutoPtr<IUserInfo> userInfo;
    mHost->mUserManager->GetUserInfo(id, (IUserInfo**)&userInfo);
    Boolean res;
    if (userInfo->IsManagedProfile(&res), res) {
        title->SetText(R::string::category_work);
    }
    else {
        title->SetText(R::string::category_personal);
    }
    titleTmp->SetTextAlignment(IView::TEXT_ALIGNMENT_VIEW_END);

    *result = convertView;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::GetChildView(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [in] */ Boolean isLastChild,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IInterface> obj;
    GetChild(groupPosition, childPosition, (IInterface**)&obj);

    AutoPtr<IView> view = mHost->GetViewForCertificate((CertHolder*)ICertHolder::Probe(obj), (Tab*)mData->mTab.Get(),
            convertView, parent);
    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::IsChildSelectable(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [out] */ Boolean* result)
{
    *result = TRUE;
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::Load()
{
    // mData.new AliasLoader()->Execute(args);
    AutoPtr<AdapterData::AliasLoader> al = new AdapterData::AliasLoader(mData, mHost);
    AutoPtr< ArrayOf<IInterface*> > args;
    al->Execute(args);
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::GetListViewId(
    /* [in] */ ITab* tab,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((Tab*)tab)->mExpandableList;
    return NOERROR;
}

AutoPtr<IView> CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::InflateCategoryHeader(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IContext> context;
    inflater->GetContext((IContext**)&context);
    AutoPtr< ArrayOf<Int32> > attrsId = TO_ATTRS_ARRAYOF(Elastos::Droid::R::styleable::Preference);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(NULL,
            attrsId, Elastos::Droid::R::attr::preferenceCategoryStyle, 0, (ITypedArray**)&a);
    Int32 resId;
    a->GetResourceId(Elastos::Droid::R::styleable::Preference_layout, 0, &resId);
    AutoPtr<IView> result;
    inflater->Inflate(resId, parent, FALSE, (IView**)&result);
    return result;
}

ECode CTrustedCredentialsSettings::TrustedCertificateExpandableAdapter::NotifyDataSetChanged()
{
    return NOERROR;
}

//===============================================================================
//                  CTrustedCredentialsSettings::TrustedCertificateAdapter
//===============================================================================

CAR_INTERFACE_IMPL(CTrustedCredentialsSettings::TrustedCertificateAdapter, BaseAdapter, ITrustedCertificateAdapterCommons)

CTrustedCredentialsSettings::TrustedCertificateAdapter::TrustedCertificateAdapter(
    /* [in] */ ITab* tab,
    /* [in] */ CTrustedCredentialsSettings* host)
    : mHost(host)
{
    mData = new AdapterData(tab, this);
    Load();
}

ECode CTrustedCredentialsSettings::TrustedCertificateAdapter::Remove(
    /* [in] */ ICertHolder* certHolder)
{
    mData->Remove(certHolder);
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateAdapter::GetListViewId(
    /* [in] */ ITab* tab,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    *result = ((Tab*)tab)->mList;
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateAdapter::Load()
{
    // mData.new AliasLoader()->Execute(args);
    AutoPtr<AdapterData::AliasLoader> al = new AdapterData::AliasLoader(mData, mHost);
    AutoPtr< ArrayOf<IInterface*> > args;
    al->Execute(args);
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;

    AutoPtr<IInterface> obj;
    mData->mCertHoldersByUserId->ValueAt(0, (IInterface**)&obj);
    IList* certHolders = IList::Probe(obj); //List<CertHolder>
    if (certHolders != NULL) {
        return certHolders->GetSize(count);
    }
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** result) //CertHolder
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IInterface> obj;
    mData->mCertHoldersByUserId->ValueAt(0, (IInterface**)&obj);
    return IList::Probe(obj)->Get(position, result);
}

ECode CTrustedCredentialsSettings::TrustedCertificateAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* result)
{
    *result = (Int64)position;
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* view,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    AutoPtr<IView> tmp = mHost->GetViewForCertificate((CertHolder*)ICertHolder::Probe(obj), (Tab*)mData->mTab.Get(), view, parent);
    *result = tmp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTrustedCredentialsSettings::TrustedCertificateAdapter::NotifyDataSetChanged()
{
    return NOERROR;
}

//===============================================================================
//                  CTrustedCredentialsSettings::AdapterData::AliasLoader
//===============================================================================

CTrustedCredentialsSettings::AdapterData::AliasLoader::AliasLoader(
    /* [in] */ AdapterData* host,
    /* [in] */ CTrustedCredentialsSettings* owner)
    : mHost(host)
    , mOwner(owner)
{}

ECode CTrustedCredentialsSettings::AdapterData::AliasLoader::OnPreExecute()
{
    AutoPtr<IFrameLayout> _content;
    mOwner->mTabHost->GetTabContentView((IFrameLayout**)&_content);
    AutoPtr<IView> content = IView::Probe(_content);
    AutoPtr<IView> tmp;
    content->FindViewById(((Tab*)mHost->mTab.Get())->mProgress, (IView**)&tmp);
    mProgressBar = IProgressBar::Probe(tmp);
    Int32 id;
    mHost->mAdapter->GetListViewId(mHost->mTab, &id);
    content->FindViewById(id, (IView**)&mList);
    IView::Probe(mProgressBar)->SetVisibility(IView::VISIBLE);
    mList->SetVisibility(IView::GONE);
    return NOERROR;
}

ECode CTrustedCredentialsSettings::AdapterData::AliasLoader::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result) //SparseArray<List<CertHolder>>
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<ISparseArray> certHoldersByProfile;//SparseArray<List<CertHolder>>
    CSparseArray::New((ISparseArray**)&certHoldersByProfile);
    // try {
    AutoPtr<IList> profiles; //List<UserHandle>
    mOwner->mUserManager->GetUserProfiles((IList**)&profiles);
    Int32 n;
    profiles->GetSize(&n);
    // First we get all aliases for all profiles in order to show progress
    // correctly. Otherwise this could all be in a single loop.
    AutoPtr<ISparseArray> aliasesByProfileId;//SparseArray<List<ParcelableString>>
    CSparseArray::New(n, (ISparseArray**)&aliasesByProfileId);
    Int32 max = 0;
    Int32 progress = 0;
    AutoPtr<IKeyChain> kc;
    CKeyChain::AcquireSingleton((IKeyChain**)&kc);
    AutoPtr<IActivity> activity;
    mOwner->GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    for (Int32 i = 0; i < n; ++i) {
        AutoPtr<IInterface> obj;
        profiles->Get(i, (IInterface**)&obj);
        IUserHandle* profile = IUserHandle::Probe(obj);
        Int32 profileId;
        profile->GetIdentifier(&profileId);
        AutoPtr<IKeyChainConnection> keyChainConnection;
        kc->BindAsUser(context, profile, (IKeyChainConnection**)&keyChainConnection);
        // Saving the connection for later use on the certificate dialog.
        mOwner->mKeyChainConnectionByProfileId->Put(profileId, keyChainConnection);
        AutoPtr<IIKeyChainService> service;
        keyChainConnection->GetService((IIKeyChainService**)&service);
        AutoPtr<IList> aliases;//List<ParcelableString>
        ((Tab*)mHost->mTab.Get())->GetAliases(service, (IList**)&aliases);
        Int32 size;
        aliases->GetSize(&size);
        max += size;
        aliasesByProfileId->Put(profileId, aliases);
    }
    for (Int32 i = 0; i < n; ++i) {
        AutoPtr<IInterface> obj;
        profiles->Get(i, (IInterface**)&obj);
        AutoPtr<IUserHandle> profile = IUserHandle::Probe(obj);
        Int32 profileId;
        profile->GetIdentifier(&profileId);
        obj = NULL;
        aliasesByProfileId->Get(profileId, (IInterface**)&obj);
        AutoPtr<IList> aliases = IList::Probe(obj);// List<ParcelableString>
        obj = NULL;
        mOwner->mKeyChainConnectionByProfileId->Get(profileId, (IInterface**)&obj);
        AutoPtr<IIKeyChainService> service;
        IKeyChainConnection::Probe(obj)->GetService((IIKeyChainService**)&service);
        AutoPtr<IList> certHolders;//List<CertHolder>
        CArrayList::New(max, (IList**)&certHolders);
        Int32 aliasMax;
        aliases->GetSize(&aliasMax);
        for (Int32 j = 0; j < aliasMax; ++j) {
            AutoPtr<IInterface> aliasObj;
            aliases->Get(j, (IInterface**)&aliasObj);
            String alias;
            IParcelableString::Probe(aliasObj)->GetString(&alias);
            AutoPtr< ArrayOf<Byte> > encodedCertificate;
            service->GetEncodedCaCertificate(alias, TRUE, (ArrayOf<Byte>**)&encodedCertificate);
            AutoPtr<IX509Certificate> cert;
            kc->ToCertificate(encodedCertificate, (IX509Certificate**)&cert);
            AutoPtr<CertHolder> certHolder = new CertHolder(service, mHost->mAdapter,
                    (Tab*)mHost->mTab.Get(), alias, cert, profileId);
            certHolders->Add((ICertHolder*)certHolder.Get());
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
            args->Set(0, CoreUtils::Convert(++progress));
            args->Set(1, CoreUtils::Convert(max));
            PublishProgress(args);
        }
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        coll->Sort(certHolders);
        certHoldersByProfile->Put(profileId, certHolders);
    }
    *result = certHoldersByProfile;
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (RemoteException e) {
    //     Logger::E(TAG, "Remote exception while loading aliases.", ec);
    //     return new SparseArray<List<CertHolder>>();
    // } catch (InterruptedException e) {
    //     Logger::E(TAG, "InterruptedException while loading aliases.", ec);
    //     return new SparseArray<List<CertHolder>>();
    // }
}

ECode CTrustedCredentialsSettings::AdapterData::AliasLoader::OnProgressUpdate(
    /* [in] */ ArrayOf<IInterface*>* progressAndMax)
{
    Int32 progress;
    IInteger32::Probe((*progressAndMax)[0])->GetValue(&progress);
    Int32 max;
    IInteger32::Probe((*progressAndMax)[1])->GetValue(&max);
    Int32 data;
    if (max != (mProgressBar->GetMax(&data), data)) {
        mProgressBar->SetMax(max);
    }
    mProgressBar->SetProgress(progress);
    return NOERROR;
}

ECode CTrustedCredentialsSettings::AdapterData::AliasLoader::OnPostExecute(
    /* [in] */ IInterface* _certHolders)
{
    mHost->mCertHoldersByUserId->Clear();
    AutoPtr<ISparseArray> certHolders = ISparseArray::Probe(_certHolders);
    Int32 n;
    certHolders->GetSize(&n);
    for (Int32 i = 0; i < n; ++i) {
        Int32 key;
        certHolders->KeyAt(i, &key);
        AutoPtr<IInterface> obj;
        certHolders->ValueAt(i, (IInterface**)&obj);
        mHost->mCertHoldersByUserId->Put(key, IList::Probe(obj));
    }
    mHost->mAdapter->NotifyDataSetChanged();
    IView::Probe(mProgressBar)->SetVisibility(IView::GONE);
    mList->SetVisibility(IView::VISIBLE);
    mProgressBar->SetProgress(0);
    return NOERROR;
}

//===============================================================================
//                  CTrustedCredentialsSettings::AdapterData
//===============================================================================

CTrustedCredentialsSettings::AdapterData::AdapterData(
    /* [in] */ ITab* tab,
    /* [in] */ ITrustedCertificateAdapterCommons* adapter)
{
    CSparseArray::New((ISparseArray**)&mCertHoldersByUserId);
    mAdapter = adapter;
    mTab = tab;
}

ECode CTrustedCredentialsSettings::AdapterData::Remove(
    /* [in] */ ICertHolder* certHolder)
{
    if (mCertHoldersByUserId != NULL) {
        AutoPtr<IInterface> obj;
        mCertHoldersByUserId->Get(((CertHolder*)certHolder)->mProfileId, (IInterface**)&obj);
        IList* certs = IList::Probe(obj);//List<CertHolder>
        if (certs != NULL) {
            certs->Remove(certHolder);
        }
    }
    return NOERROR;
}

//===============================================================================
//                  CTrustedCredentialsSettings::CertHolder
//===============================================================================

CAR_INTERFACE_IMPL(CTrustedCredentialsSettings::CertHolder, Object, IComparable)

CTrustedCredentialsSettings::CertHolder::CertHolder(
    /* [in] */ IIKeyChainService* service,
    /* [in] */ ITrustedCertificateAdapterCommons* adapter,
    /* [in] */ Tab* tab,
    /* [in] */ const String& alias,
    /* [in] */ IX509Certificate* x509Cert,
    /* [in] */ Int32 profileId)
    : mProfileId(0)
    , mDeleted(FALSE)
{
    mProfileId = profileId;
    mService = service;
    mAdapter = adapter;
    mTab = tab;
    mAlias = alias;
    mX509Cert = x509Cert;

    CSslCertificate::New(x509Cert, (ISslCertificate**)&mSslCert);

    AutoPtr<ISslCertificateDName> scdn;
    mSslCert->GetIssuedTo((ISslCertificateDName**)&scdn);
    String cn;
    scdn->GetCName(&cn);
    String o;
    scdn->GetOName(&o);
    String ou;
    scdn->GetUName(&ou);
    // if we have a O, use O as primary subject, secondary prefer CN over OU
    // if we don't have an O, use CN as primary, empty secondary
    // if we don't have O or CN, use DName as primary, empty secondary
    if (!o.IsEmpty()) {
        if (!cn.IsEmpty()) {
            mSubjectPrimary = o;
            mSubjectSecondary = cn;
        }
        else {
            mSubjectPrimary = o;
            mSubjectSecondary = ou;
        }
    }
    else {
        if (!cn.IsEmpty()) {
            mSubjectPrimary = cn;
            mSubjectSecondary = "";
        }
        else {
            scdn->GetDName(&mSubjectPrimary);
            mSubjectSecondary = "";
        }
    }

    ECode ec = mTab->Deleted(mService, mAlias, &mDeleted);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "Remote exception while checking if alias %s is deleted. 0x%08x", mAlias.string(), ec);
        mDeleted = FALSE;
    }
}

ECode CTrustedCredentialsSettings::CertHolder::CompareTo(
    /* [in] */ IInterface* o,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<CertHolder> ch = (CertHolder*) ICertHolder::Probe(o);
    Int32 primary = mSubjectPrimary.CompareIgnoreCase(ch->mSubjectPrimary);
    if (primary != 0) {
        *result = primary;
        return NOERROR;
    }

    *result = mSubjectSecondary.CompareIgnoreCase(ch->mSubjectSecondary);
    return NOERROR;
}

ECode CTrustedCredentialsSettings::CertHolder::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;

    AutoPtr<ICertHolder> other = ICertHolder::Probe(o);
    if (other == NULL) {
        return NOERROR;
    }
    *res = mAlias.Equals(((CertHolder*)other.Get())->mAlias);
    return NOERROR;
}

ECode CTrustedCredentialsSettings::CertHolder::HashCode(
    /* [out] */ Int32* hashcode)
{
    VALIDATE_NOT_NULL(hashcode)

    *hashcode = mAlias.GetHashCode();
    return NOERROR;
}

//===============================================================================
//                  CTrustedCredentialsSettings::AliasOperation
//===============================================================================

CTrustedCredentialsSettings::AliasOperation::AliasOperation(
    /* [in] */ CertHolder* certHolder,
    /* [in] */ CTrustedCredentialsSettings* host)
{
    mCertHolder = certHolder;
}

ECode CTrustedCredentialsSettings::AliasOperation::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // try {
        AutoPtr<IInterface> obj;
        mHost->mKeyChainConnectionByProfileId->Get(mCertHolder->mProfileId, (IInterface**)&obj);
        IKeyChainConnection* keyChainConnection = IKeyChainConnection::Probe(obj);
        AutoPtr<IIKeyChainService> service;
        keyChainConnection->GetService((IIKeyChainService**)&service);
        if (mCertHolder->mDeleted) {
            AutoPtr< ArrayOf<Byte> > bytes;
            ICertificate::Probe(mCertHolder->mX509Cert)->GetEncoded((ArrayOf<Byte>**)&bytes);
            service->InstallCaCertificate(bytes);
            AutoPtr<IBoolean> obj = CoreUtils::Convert(TRUE);
            *result = obj;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        else {
            Boolean res;
            service->DeleteCaCertificate(mCertHolder->mAlias, &res);
            AutoPtr<IBoolean> obj = CoreUtils::Convert(res);
            *result = obj;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    // } catch (CertificateEncodingException e) {
    //     Logger::W(TAG, "Error while toggling alias %s, 0x%08x", mCertHolder->mAlias.string(), ec);
    //     AutoPtr<IBoolean> obj = CoreUtils::Convert(FALSE);
    //     *result = obj;
    //     REFCOUNT_ADD(*result)
    //     return NOERROR;
    // } catch (IllegalStateException e) {
    //     // used by installCaCertificate to report errors
    //     Logger::W(TAG, "Error while toggling alias %s, 0x%08x", mCertHolder->mAlias.string(), ec);
    //     AutoPtr<IBoolean> obj = CoreUtils::Convert(FALSE);
    //     *result = obj;
    //     REFCOUNT_ADD(*result)
    //     return NOERROR;
    // } catch (RemoteException e) {
    //     Logger::W(TAG, "Error while toggling alias %s, 0x%08x", mCertHolder->mAlias.string(), ec);
    //     AutoPtr<IBoolean> obj = CoreUtils::Convert(FALSE);
    //     *result = obj;
    //     REFCOUNT_ADD(*result)
    //     return NOERROR;
    // }
}

ECode CTrustedCredentialsSettings::AliasOperation::OnPostExecute(
    /* [in] */ IInterface* ok) //Boolean
{
    Boolean res;
    IBoolean::Probe(ok)->GetValue(&res);
    mCertHolder->mTab->PostOperationUpdate(res, mCertHolder);
    return NOERROR;
}

//===============================================================================
//                  CTrustedCredentialsSettings::ExpandableListViewOnChildClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CTrustedCredentialsSettings::ExpandableListViewOnChildClickListener, Object, IExpandableListViewOnChildClickListener)

CTrustedCredentialsSettings::ExpandableListViewOnChildClickListener::ExpandableListViewOnChildClickListener(
    /* [in] */ CTrustedCredentialsSettings* host,
    /* [in] */ TrustedCertificateExpandableAdapter* adapter)
    : mHost(host)
    , mAdapter(adapter)
{}

ECode CTrustedCredentialsSettings::ExpandableListViewOnChildClickListener::OnChildClick(
    /* [in] */ IExpandableListView* parent,
    /* [in] */ IView* v,
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [in] */ Int64 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IInterface> obj;
    mAdapter->GetChild(groupPosition, childPosition, (IInterface**)&obj);
    mHost->ShowCertDialog((CertHolder*)ICertHolder::Probe(obj));
    *result = TRUE;
    return NOERROR;
}

//===============================================================================
//                  CTrustedCredentialsSettings::AdapterViewOnItemClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CTrustedCredentialsSettings::AdapterViewOnItemClickListener, Object, IAdapterViewOnItemClickListener)

CTrustedCredentialsSettings::AdapterViewOnItemClickListener::AdapterViewOnItemClickListener(
    /* [in] */ CTrustedCredentialsSettings* host,
    /* [in] */ TrustedCertificateAdapter* adapter)
    : mHost(host)
    , mAdapter(adapter)
{}

ECode CTrustedCredentialsSettings::AdapterViewOnItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 pos,
    /* [in] */ Int64 id)
{
    AutoPtr<IInterface> obj;
    mAdapter->GetItem(pos, (IInterface**)&obj);
    mHost->ShowCertDialog((CertHolder*)ICertHolder::Probe(obj));
    return NOERROR;
}

//===============================================================================
//                  CTrustedCredentialsSettings::AdapterViewOnItemSelectedListener
//===============================================================================

CAR_INTERFACE_IMPL(CTrustedCredentialsSettings::AdapterViewOnItemSelectedListener, Object, IAdapterViewOnItemSelectedListener)

CTrustedCredentialsSettings::AdapterViewOnItemSelectedListener::AdapterViewOnItemSelectedListener(
    /* [in] */ IArrayList* views)
    : mViews(views)
{}

ECode CTrustedCredentialsSettings::AdapterViewOnItemSelectedListener::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Int32 size;
    mViews->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mViews->Get(i, (IInterface**)&obj);
        IView::Probe(obj)->SetVisibility(i == position ? IView::VISIBLE : IView::GONE);
    }
    return NOERROR;
}

ECode CTrustedCredentialsSettings::AdapterViewOnItemSelectedListener::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    return NOERROR;
}

//===============================================================================
//                  CTrustedCredentialsSettings::DialogInterfaceOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CTrustedCredentialsSettings::DialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener)

CTrustedCredentialsSettings::DialogInterfaceOnClickListener::DialogInterfaceOnClickListener(
    /* [in] */ CTrustedCredentialsSettings* host,
    /* [in] */ Int32 id,
    /* [in] */ CertHolder* certHolder,
    /* [in] */ IDialog* dialog)
    : mHost(host)
    , mId(id)
    , mCertHolder(certHolder)
    , mDialog(dialog)
{}

ECode CTrustedCredentialsSettings::DialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 id)
{
    switch (mId) {
        case 0:
            return dialog->Dismiss();
        case 1: {
            AutoPtr<AliasOperation> ao = new AliasOperation(mCertHolder, mHost);
            AutoPtr< ArrayOf<IInterface*> > args;
            ao->Execute(args);
            dialog->Dismiss();
            IDialogInterface::Probe(mDialog)->Dismiss();
            return NOERROR;
        }
        case 2:
            return dialog->Cancel();
    }

    return NOERROR;
}

//===============================================================================
//                  CTrustedCredentialsSettings::ViewOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CTrustedCredentialsSettings::ViewOnClickListener, Object, IViewOnClickListener)

CTrustedCredentialsSettings::ViewOnClickListener::ViewOnClickListener(
    /* [in] */ CTrustedCredentialsSettings* host,
    /* [in] */ CertHolder* certHolder,
    /* [in] */ IDialog* dialog)
    : mHost(host)
    , mCertHolder(certHolder)
    , mDialog(dialog)
{}

ECode CTrustedCredentialsSettings::ViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
    builder->SetMessage(mCertHolder->mTab->GetButtonConfirmation(mCertHolder));
    AutoPtr<DialogInterfaceOnClickListener> posListener = new DialogInterfaceOnClickListener(mHost, 1, mCertHolder, mDialog);
    builder->SetPositiveButton(Elastos::Droid::R::string::yes, posListener);
    AutoPtr<DialogInterfaceOnClickListener> negListener = new DialogInterfaceOnClickListener(mHost, 2, NULL, NULL);
    builder->SetNegativeButton(Elastos::Droid::R::string::no, negListener);
    AutoPtr<IAlertDialog> alert;
    builder->Create((IAlertDialog**)&alert);
    return IDialog::Probe(alert)->Show();
}

//===============================================================================
//                  CTrustedCredentialsSettings
//===============================================================================

const String CTrustedCredentialsSettings::TAG("CTrustedCredentialsSettings");
const String CTrustedCredentialsSettings::USER_ACTION("com.android.settings.TRUSTED_CREDENTIALS_USER");

CAR_OBJECT_IMPL(CTrustedCredentialsSettings)

CTrustedCredentialsSettings::CTrustedCredentialsSettings()
{}

CTrustedCredentialsSettings::~CTrustedCredentialsSettings()
{}

ECode CTrustedCredentialsSettings::constructor()
{
    CSparseArray::New((ISparseArray**)&mKeyChainConnectionByProfileId);
    return Fragment::constructor();
}

ECode CTrustedCredentialsSettings::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Fragment::OnCreate(savedInstanceState);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUserManager = IUserManager::Probe(obj);
    return NOERROR;
}

ECode CTrustedCredentialsSettings::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* parent,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IView> tmp;
    inflater->Inflate(R::layout::trusted_credentials, parent, FALSE, (IView**)&tmp);
    mTabHost = ITabHost::Probe(tmp);
    mTabHost->Setup();
    AddTab(Tab::SYSTEM);
    // TODO add Install button on Tab.USER to go to CertInstaller like KeyChainActivity
    AddTab(Tab::USER);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    String action;
    if (intent != NULL &&
            USER_ACTION.Equals((intent->GetAction(&action), action))) {
        mTabHost->SetCurrentTabByTag(((Tab*)Tab::USER.Get())->mTag);
    }
    *result = IView::Probe(mTabHost);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTrustedCredentialsSettings::OnDestroy()
{
    CloseKeyChainConnections();
    return Fragment::OnDestroy();
}

void CTrustedCredentialsSettings::CloseKeyChainConnections()
{
    Int32 n;
    mKeyChainConnectionByProfileId->GetSize(&n);
    for (Int32 i = 0; i < n; ++i) {
        AutoPtr<IInterface> obj;
        mKeyChainConnectionByProfileId->ValueAt(i, (IInterface**)&obj);
        ICloseable::Probe(obj)->Close();
    }
    mKeyChainConnectionByProfileId->Clear();
}

void CTrustedCredentialsSettings::AddTab(
    /* [in] */ ITab* _tab)
{
    AutoPtr<Tab> tab = (Tab*)_tab;
    AutoPtr<ITabSpec> systemSpec;
    mTabHost->NewTabSpec(tab->mTag, (ITabSpec**)&systemSpec);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    String str;
    IContext::Probe(activity)->GetString(tab->mLabel, &str);
    systemSpec->SetIndicator(CoreUtils::Convert(str));
    systemSpec->SetContent(tab->mView);
    mTabHost->AddTab(systemSpec);

    AutoPtr<IList> list;
    mUserManager->GetUserProfiles((IList**)&list);
    Int32 size;
    list->GetSize(&size);
    if (size > 1) {
        AutoPtr<IView> tmp;
        IView::Probe(mTabHost)->FindViewById(tab->mExpandableList, (IView**)&tmp);
        IExpandableListView* lv = IExpandableListView::Probe(tmp);
        AutoPtr<TrustedCertificateExpandableAdapter> adapter = new TrustedCertificateExpandableAdapter(tab, this);
        lv->SetAdapter(adapter);
        AutoPtr<ExpandableListViewOnChildClickListener> listener = new ExpandableListViewOnChildClickListener(this, adapter);
        lv->SetOnChildClickListener(listener);
    }
    else {
        AutoPtr<IView> lvTmp;
        IView::Probe(mTabHost)->FindViewById(tab->mList, (IView**)&lvTmp);
        IAdapterView* lv = IAdapterView::Probe(lvTmp);
        AutoPtr<TrustedCertificateAdapter> adapter = new TrustedCertificateAdapter(tab, this);
        lv->SetAdapter(adapter);
        AutoPtr<AdapterViewOnItemClickListener> listener = new AdapterViewOnItemClickListener(this, adapter);
        lv->SetOnItemClickListener(listener);
    }
}

AutoPtr<IView> CTrustedCredentialsSettings::GetViewForCertificate(
    /* [in] */ CertHolder* certHolder,
    /* [in] */ Tab* mTab,
    /* [in] */ IView* _convertView,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> convertView = _convertView;
    AutoPtr<ViewHolder> holder;
    if (convertView == NULL) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(IContext::Probe(activity), (ILayoutInflater**)&inflater);
        inflater->Inflate(R::layout::trusted_credential, parent, FALSE, (IView**)&convertView);
        holder = new ViewHolder();
        AutoPtr<IView> tmp;
        convertView->FindViewById(R::id::trusted_credential_subject_primary, (IView**)&tmp);
        holder->mSubjectPrimaryView = ITextView::Probe(tmp);
        tmp = NULL;
        convertView->FindViewById(R::id::trusted_credential_subject_secondary, (IView**)&tmp);
        holder->mSubjectSecondaryView = ITextView::Probe(tmp);
        tmp = NULL;
        convertView->FindViewById(R::id::trusted_credential_status, (IView**)&tmp);
        holder->mCheckBox = ICheckBox::Probe(tmp);
        convertView->SetTag((IObject*)holder);
    }
    else {
        AutoPtr<IInterface> obj;
        convertView->GetTag((IInterface**)&obj);
        holder = (ViewHolder*)IObject::Probe(obj);
    }
    holder->mSubjectPrimaryView->SetText(CoreUtils::Convert(certHolder->mSubjectPrimary));
    holder->mSubjectSecondaryView->SetText(CoreUtils::Convert(certHolder->mSubjectSecondary));
    if (mTab->mCheckbox) {
        ICheckable::Probe(holder->mCheckBox)->SetChecked(!certHolder->mDeleted);
        IView::Probe(holder->mCheckBox)->SetVisibility(IView::VISIBLE);
    }
    return convertView;
}

void CTrustedCredentialsSettings::ShowCertDialog(
    /* [in] */ CertHolder* certHolder)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(Elastos::Droid::R::string::ssl_certificate);

    AutoPtr<IArrayList> views; //ArrayList<View>
    CArrayList::New((IArrayList**)&views);
    AutoPtr<IArrayList> titles; //ArrayList<String>
    CArrayList::New((IArrayList**)&titles);
    AddCertChain(certHolder, views, titles);

    AutoPtr<IArrayAdapter> arrayAdapter;
    CArrayAdapter::New(context, Elastos::Droid::R::layout::simple_spinner_item, IList::Probe(titles), (IArrayAdapter**)&arrayAdapter);
    arrayAdapter->SetDropDownViewResource(Elastos::Droid::R::layout::simple_spinner_dropdown_item);
    AutoPtr<ISpinner> spinner;
    CSpinner::New(context, (ISpinner**)&spinner);
    IAdapterView* _spinner = IAdapterView::Probe(spinner);
    _spinner->SetAdapter(IAdapter::Probe(arrayAdapter));
    AutoPtr<AdapterViewOnItemSelectedListener> listener = new AdapterViewOnItemSelectedListener(views);
    _spinner->SetOnItemSelectedListener(listener);

    AutoPtr<ILinearLayout> container;
    CLinearLayout::New(context, (ILinearLayout**)&container);
    container->SetOrientation(ILinearLayout::VERTICAL);
    IViewGroup* _container = IViewGroup::Probe(container);
    _container->AddView(IView::Probe(spinner));
    Int32 size;
    views->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        views->Get(i, (IInterface**)&obj);
        IView* certificateView = IView::Probe(obj);
        if (i != 0) {
            certificateView->SetVisibility(IView::GONE);
        }
        _container->AddView(certificateView);
    }
    builder->SetView(IView::Probe(container));
    AutoPtr<DialogInterfaceOnClickListener> positiveListener = new DialogInterfaceOnClickListener(this, 0, NULL, NULL);
    builder->SetPositiveButton(Elastos::Droid::R::string::ok, positiveListener);
    AutoPtr<IAlertDialog> alertDialog;
    builder->Create((IAlertDialog**)&alertDialog);
    AutoPtr<IDialog> certDialog = IDialog::Probe(alertDialog);

    AutoPtr<IView> tmp;
    IView::Probe(container)->FindViewById(Elastos::Droid::R::id::body, (IView**)&tmp);
    AutoPtr<IViewGroup> body = IViewGroup::Probe(tmp);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    AutoPtr<IView> removeButtonTmp;
    inflater->Inflate(R::layout::trusted_credential_details, body, FALSE, (IView**)&removeButtonTmp);
    AutoPtr<IButton> removeButton = IButton::Probe(removeButtonTmp);
    Boolean res;
    if (mUserManager->HasUserRestriction(IUserManager::DISALLOW_CONFIG_CREDENTIALS, &res), !res) {
        body->AddView(removeButtonTmp);
    }
    ITextView::Probe(removeButton)->SetText(certHolder->mTab->GetButtonLabel(certHolder));
    AutoPtr<ViewOnClickListener> viewListener = new ViewOnClickListener(this, certHolder, certDialog);
    removeButtonTmp->SetOnClickListener(viewListener);

    IDialog::Probe(certDialog)->Show();
}

void CTrustedCredentialsSettings::AddCertChain(
    /* [in] */ CertHolder* certHolder,
    /* [in] */ IArrayList* views,  // ArrayList<View>
    /* [in] */ IArrayList* titles) // ArrayList<String>
{
    AutoPtr<IList> certificates;//List<X509Certificate>
    // try {
        AutoPtr<IInterface> obj;
        mKeyChainConnectionByProfileId->Get(certHolder->mProfileId, (IInterface**)&obj);
        IKeyChainConnection* keyChainConnection = IKeyChainConnection::Probe(obj);
        AutoPtr<IIKeyChainService> service;
        keyChainConnection->GetService((IIKeyChainService**)&service);
        AutoPtr<IList> chain; //List<String>
        service->GetCaCertificateChainAliases(certHolder->mAlias, TRUE, (IList**)&chain);
        Int32 n;
        chain->GetSize(&n);
        CArrayList::New(n, (IList**)&certificates);
        for (Int32 i = 0; i < n; ++i) {
            AutoPtr<IInterface> tmp;
            chain->Get(i, (IInterface**)&tmp);
            AutoPtr< ArrayOf<Byte> > encodedCertificate;
            service->GetEncodedCaCertificate(Object::ToString(tmp), TRUE, (ArrayOf<Byte>**)&encodedCertificate);
            AutoPtr<IKeyChain> kc;
            CKeyChain::AcquireSingleton((IKeyChain**)&kc);
            AutoPtr<IX509Certificate> certificate;
            kc->ToCertificate(encodedCertificate, (IX509Certificate**)&certificate);
            certificates->Add(certificate);
        }
    // } catch (RemoteException ex) {
    //     Logger::E(TAG, "RemoteException while retrieving certificate chain for root "
    //             + certHolder.mAlias, ex);
    //     return;
    // }
    Int32 size;
    certificates->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        certificates->Get(i, (IInterface**)&obj);
        IX509Certificate* certificate = IX509Certificate::Probe(obj);
        AddCertDetails(certificate, views, titles);
    }
}

void CTrustedCredentialsSettings::AddCertDetails(
    /* [in] */ IX509Certificate* certificate,
    /* [in] */ IArrayList* views, //ArrayList<View>
    /* [in] */ IArrayList* titles) //ArrayList<String>
{
    AutoPtr<ISslCertificate> sslCert;
    CSslCertificate::New(certificate, (ISslCertificate**)&sslCert);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IView> view;
    sslCert->InflateCertificateView(IContext::Probe(activity), (IView**)&view);
    views->Add(view);
    AutoPtr<ISslCertificateDName> scdn;
    sslCert->GetIssuedTo((ISslCertificateDName**)&scdn);
    String name;
    scdn->GetCName(&name);
    titles->Add(CoreUtils::Convert(name));
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
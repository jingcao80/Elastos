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

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/CDeviceAdminSettings.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IDialog;
// using Elastos::Droid::App::Admin::CDeviceAdminInfo;
using Elastos::Droid::App::Admin::IDeviceAdminReceiver;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Logger;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CDeviceAdminSettings::PolicyListAdapter
//===============================================================================

CDeviceAdminSettings::PolicyListAdapter::PolicyListAdapter(
    /* [in] */ CDeviceAdminSettings* host)
    : mHost(host)
{
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
}

ECode CDeviceAdminSettings::PolicyListAdapter::HasStableIds(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has)
    *has = FALSE;
    return NOERROR;
}

ECode CDeviceAdminSettings::PolicyListAdapter::GetCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 adminCount = 0;
    Int32 profileCount;
    mHost->mAdminsByProfile->GetSize(&profileCount);
    Int32 size;
    for (Int32 i = 0; i < profileCount; ++i) {
        AutoPtr<IInterface> obj;
        mHost->mAdminsByProfile->ValueAt(i, (IInterface**)&obj);
        IArrayList::Probe(obj)->GetSize(&size);
        adminCount += size;
    }
    // Add 'profileCount' for title items.
    *result = adminCount + profileCount;
    return NOERROR;
}

ECode CDeviceAdminSettings::PolicyListAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (position < 0) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    // The position of the item in the list of admins.
    // We start from the given position and discount the length of the upper lists until we
    // get the one for the right profile
    Int32 adminPosition = position;
    Int32 n;
    mHost->mAdminsByProfile->GetSize(&n);
    Int32 i = 0;
    for (; i < n; ++i) {
        // The elements in that section including the title item (that's why adding one).
        AutoPtr<IInterface> obj;
        mHost->mAdminsByProfile->ValueAt(i, (IInterface**)&obj);
        Int32 size;
        IArrayList::Probe(obj)->GetSize(&size);
        const Int32 listSize = size + 1;

        if (adminPosition < listSize) {
            break;
        }
        adminPosition -= listSize;
    }
    if (i == n) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    // If countdown == 0 that means the title item
    if (adminPosition == 0) {
        AutoPtr<IActivity> activity;
        mHost->GetActivity((IActivity**)&activity);
        AutoPtr<IResources> res;
        IContext::Probe(activity)->GetResources((IResources**)&res);
        Int32 key;
        String str;
        AutoPtr<ICharSequence> cs;
        if ((mHost->mAdminsByProfile->KeyAt(i, &key), key) == UserHandle::GetMyUserId()) {
            res->GetString(R::string::personal_device_admin_title, &str);
            cs = CoreUtils::Convert(str);
            *result = cs;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        else {
            res->GetString(R::string::managed_device_admin_title, &str);
            cs =CoreUtils::Convert(str);
            *result = cs;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    else {
        // Subtracting one for the title.
        AutoPtr<IInterface> obj;
        mHost->mAdminsByProfile->ValueAt(i, (IInterface**)&obj);
        return IArrayList::Probe(obj)->Get(adminPosition - 1, result);
    }
    return NOERROR;
}

ECode CDeviceAdminSettings::PolicyListAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (Int64)position;
    return NOERROR;
}

ECode CDeviceAdminSettings::PolicyListAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode CDeviceAdminSettings::PolicyListAdapter::GetViewTypeCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 2;
    return NOERROR;
}

ECode CDeviceAdminSettings::PolicyListAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<IInterface> o;
    GetItem(position, (IInterface**)&o);
    if (ICharSequence::Probe(o) != NULL) {
        *result = 1;
    }
    return NOERROR;
}

ECode CDeviceAdminSettings::PolicyListAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IInterface> o;
    GetItem(position, (IInterface**)&o);

    *result = IsEnabled(o);
    return NOERROR;
}

Boolean CDeviceAdminSettings::PolicyListAdapter::IsEnabled(
    /* [in] */ IInterface* o)
{
    AutoPtr<IDeviceAdminInfo> info = IDeviceAdminInfo::Probe(o);
    if (info == NULL) {
        // Title item
        return FALSE;
    }
    if (mHost->IsActiveAdmin(info) && mHost->GetUserId(info) == UserHandle::GetMyUserId()
            && (mHost->IsDeviceOwner(info) || mHost->IsProfileOwner(info))) {
        return FALSE;
    }
    return TRUE;
}

ECode CDeviceAdminSettings::PolicyListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* _convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IInterface> o;
    GetItem(position, (IInterface**)&o);

    AutoPtr<IView> convertView = _convertView;
    AutoPtr<IDeviceAdminInfo> info = IDeviceAdminInfo::Probe(o);
    if (info != NULL) {
        if (convertView == NULL) {
            convertView = NewDeviceAdminView(parent);
        }
        BindView(convertView, info);
    }
    else {
        if (convertView == NULL) {
            convertView = NewTitleView(parent);
        }
        AutoPtr<IView> tmp;
        convertView->FindViewById(Elastos::Droid::R::id::title, (IView**)&tmp);
        ITextView* title = ITextView::Probe(tmp);
        title->SetText(CoreUtils::Convert(Object::ToString(o)));
    }
    *result = convertView;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IView> CDeviceAdminSettings::PolicyListAdapter::NewDeviceAdminView(
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> v;
    mInflater->Inflate(R::layout::device_admin_item, parent, FALSE, (IView**)&v);
    AutoPtr<ViewHolder> h = new ViewHolder();
    AutoPtr<IView> tmp;
    v->FindViewById(R::id::icon, (IView**)&tmp);
    h->mIcon = IImageView::Probe(tmp);
    tmp = NULL;
    v->FindViewById(R::id::name, (IView**)&tmp);
    h->mName = ITextView::Probe(tmp);
    tmp = NULL;
    v->FindViewById(R::id::checkbox, (IView**)&tmp);
    h->mCheckbox = ICheckBox::Probe(tmp);
    tmp = NULL;
    v->FindViewById(R::id::description, (IView**)&tmp);
    h->mDescription = ITextView::Probe(tmp);
    v->SetTag((IObject*)h);
    return v;
}

AutoPtr<IView> CDeviceAdminSettings::PolicyListAdapter::NewTitleView(
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IContext> context;
    mInflater->GetContext((IContext**)&context);
    AutoPtr< ArrayOf<Int32> > args = TO_ATTRS_ARRAYOF(Elastos::Droid::R::styleable::Preference);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(NULL,
            args, Elastos::Droid::R::attr::preferenceCategoryStyle, 0, (ITypedArray**)&a);
    Int32 resId;
    a->GetResourceId(Elastos::Droid::R::styleable::Preference_layout, 0, &resId);
    AutoPtr<IView> result;
    mInflater->Inflate(resId, parent, FALSE, (IView**)&result);
    return result;
}

void CDeviceAdminSettings::PolicyListAdapter::BindView(
    /* [in] */ IView* view,
    /* [in] */ IDeviceAdminInfo* item)
{
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    view->GetTag((IInterface**)&obj);
    AutoPtr<ViewHolder> vh = (ViewHolder*)IObject::Probe(obj);
    AutoPtr<IPackageManager> pm;
    IContext::Probe(activity)->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IDrawable> activityIcon;
    item->LoadIcon(pm, (IDrawable**)&activityIcon);
    AutoPtr<IUserHandle> uh;
    CUserHandle::New(mHost->GetUserId(item), (IUserHandle**)&uh);
    AutoPtr<IDrawable> badgedIcon;
    pm->GetUserBadgedIcon(activityIcon, uh, (IDrawable**)&badgedIcon);
    vh->mIcon->SetImageDrawable(badgedIcon);
    AutoPtr<ICharSequence> cs;
    item->LoadLabel(pm, (ICharSequence**)&cs);
    vh->mName->SetText(cs);
    ICheckable::Probe(vh->mCheckbox)->SetChecked(mHost->IsActiveAdmin(item));
    Boolean enabled = IsEnabled(item);
    // try {
    cs = NULL;
    item->LoadDescription(pm, (ICharSequence**)&cs);
    vh->mDescription->SetText(cs);
    // } catch (Resources.NotFoundException e) {
    // }
    IView::Probe(vh->mCheckbox)->SetEnabled(enabled);
    IView::Probe(vh->mName)->SetEnabled(enabled);
    IView::Probe(vh->mDescription)->SetEnabled(enabled);
    IView::Probe(vh->mIcon)->SetEnabled(enabled);
}

//===============================================================================
//                  CDeviceAdminSettings
//===============================================================================

const String CDeviceAdminSettings::TAG("DeviceAdminSettings");

const Int32 CDeviceAdminSettings::DIALOG_WARNING = 1;

CAR_OBJECT_IMPL(CDeviceAdminSettings)

CDeviceAdminSettings::CDeviceAdminSettings()
{}

CDeviceAdminSettings::~CDeviceAdminSettings()
{}

ECode CDeviceAdminSettings::constructor()
{
    CSparseArray::New((ISparseArray**)&mAdminsByProfile);
    CSparseArray::New((ISparseArray**)&mProfileOwnerComponents);
    return ListFragment::constructor();
}

ECode CDeviceAdminSettings::OnCreate(
    /* [in] */ IBundle* icicle)
{
    return ListFragment::OnCreate(icicle);
}

ECode CDeviceAdminSettings::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&obj);
    mDPM = IDevicePolicyManager::Probe(obj);
    obj = NULL;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUm = IUserManager::Probe(obj);
    return inflater->Inflate(R::layout::device_admin_settings, container, FALSE, result);
}

ECode CDeviceAdminSettings::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    ListFragment::OnActivityCreated(savedInstanceState);

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    Utils::ForceCustomPadding(IView::Probe(listView), TRUE /* additive padding */);
    return NOERROR;
}

ECode CDeviceAdminSettings::OnResume()
{
    ListFragment::OnResume();
    mDPM->GetDeviceOwner(&mDeviceOwnerPkg);
    Boolean res;
    if (!mDeviceOwnerPkg.IsNull() && (mDPM->IsDeviceOwner(mDeviceOwnerPkg, &res), !res)) {
        mDeviceOwnerPkg = String(NULL);
    }
    mProfileOwnerComponents->Clear();
    AutoPtr<IList> profiles;
    mUm->GetUserProfiles((IList**)&profiles);
    Int32 profilesSize;
    profiles->GetSize(&profilesSize);
    for (Int32 i = 0; i < profilesSize; ++i) {
        AutoPtr<IInterface> obj;
        profiles->Get(i, (IInterface**)&obj);
        Int32 profileId;
        IUserHandle::Probe(obj)->GetIdentifier(&profileId);
        AutoPtr<IComponentName> comp;
        mDPM->GetProfileOwnerAsUser(profileId, (IComponentName**)&comp);
        mProfileOwnerComponents->Put(profileId, comp);
    }
    UpdateList();
    return NOERROR;
}

void CDeviceAdminSettings::UpdateList()
{
    mAdminsByProfile->Clear();

    AutoPtr<IList> profiles;
    mUm->GetUserProfiles((IList**)&profiles);
    Int32 profilesSize;
    profiles->GetSize(&profilesSize);
    for (Int32 i = 0; i < profilesSize; ++i) {
        AutoPtr<IInterface> obj;
        profiles->Get(i, (IInterface**)&obj);
        Int32 profileId;
        IUserHandle::Probe(obj)->GetIdentifier(&profileId);

        UpdateAvailableAdminsForProfile(profileId);
    }

    AutoPtr<PolicyListAdapter> pla = new PolicyListAdapter(this);
    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    IAdapterView::Probe(listView)->SetAdapter(pla);
}

ECode CDeviceAdminSettings::OnListItemClick(
    /* [in] */ IListView* l,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IAdapter> adapter;
    IAdapterView::Probe(l)->GetAdapter((IAdapter**)&adapter);
    AutoPtr<IInterface> o;
    adapter->GetItem(position, (IInterface**)&o);
    AutoPtr<IDeviceAdminInfo> dpi = IDeviceAdminInfo::Probe(o);
    if (dpi == NULL) {
        // race conditions may cause this
        return NOERROR;
    }

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    Int32 userId = GetUserId(dpi);
    if (userId == UserHandle::GetMyUserId() || !IsProfileOwner(dpi)) {
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        Logger::D("CDeviceAdminSettings::OnListItemClick", "CDeviceAdminAdd is TODO");
        intent->SetClass(IContext::Probe(activity), ECLSID_CDeviceAdminAdd);
        AutoPtr<IComponentName> comp;
        dpi->GetComponent((IComponentName**)&comp);
        intent->PutExtra(IDevicePolicyManager::EXTRA_DEVICE_ADMIN, IParcelable::Probe(comp));
        AutoPtr<IUserHandle> uh;
        CUserHandle::New(userId, (IUserHandle**)&uh);
        activity->StartActivityAsUser(intent, uh);
    }
    else {
        IContext* context = IContext::Probe(activity);
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
        AutoPtr<IPackageManager> pm;
        context->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<ICharSequence> cs;
        dpi->LoadLabel(pm, (ICharSequence**)&cs);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, cs);
        String str;
        GetString(R::string::managed_profile_device_admin_info, args, &str);
        builder->SetMessage(CoreUtils::Convert(str));
        builder->SetPositiveButton(Elastos::Droid::R::string::ok, NULL);
        AutoPtr<IAlertDialog> dialog;
        builder->Create((IAlertDialog**)&dialog);
        IDialog::Probe(dialog)->Show();
    }
    return NOERROR;
}

Boolean CDeviceAdminSettings::IsDeviceOwner(
    /* [in] */ IDeviceAdminInfo* item)
{
    String str;
    return GetUserId(item) == UserHandle::GetMyUserId()
            && (item->GetPackageName(&str), str.Equals(mDeviceOwnerPkg));
}

Boolean CDeviceAdminSettings::IsProfileOwner(
    /* [in] */ IDeviceAdminInfo* item)
{
    AutoPtr<IInterface> obj;
    mProfileOwnerComponents->Get(GetUserId(item), (IInterface**)&obj);
    IComponentName* profileOwner = IComponentName::Probe(obj);
    AutoPtr<IComponentName> comp;
    item->GetComponent((IComponentName**)&comp);
    return Object::Equals(comp, profileOwner);
}

Boolean CDeviceAdminSettings::IsActiveAdmin(
    /* [in] */ IDeviceAdminInfo* item)
{
    AutoPtr<IComponentName> comp;
    item->GetComponent((IComponentName**)&comp);
    Boolean res;
    mDPM->IsAdminActiveAsUser(comp, GetUserId(item), &res);
    return res;
}

void CDeviceAdminSettings::UpdateAvailableAdminsForProfile(
    /* [in] */ Int32 profileId)
{
    // We are adding the union of two sets 'A' and 'B' of device admins to mAvailableAdmins.
    // Set 'A' is the set of active admins for the profile whereas set 'B' is the set of
    // listeners to DeviceAdminReceiver.ACTION_DEVICE_ADMIN_ENABLED for the profile.

    // Add all of set 'A' to mAvailableAdmins.
    AutoPtr<IList> activeAdminsListForProfile;
    mDPM->GetActiveAdminsAsUser(profileId, (IList**)&activeAdminsListForProfile);
    AddActiveAdminsForProfile(activeAdminsListForProfile, profileId);

    // Collect set 'B' and add B-A to mAvailableAdmins.
    AddDeviceAdminBroadcastReceiversForProfile(ICollection::Probe(activeAdminsListForProfile), profileId);
}

void CDeviceAdminSettings::AddDeviceAdminBroadcastReceiversForProfile(
    /* [in] */ ICollection* alreadyAddedComponents,
    /* [in] */ Int32 profileId)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IPackageManager> pm;
    IContext::Probe(activity)->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IIntent> intent;
    CIntent::New(IDeviceAdminReceiver::ACTION_DEVICE_ADMIN_ENABLED, (IIntent**)&intent);
    AutoPtr<IList> enabledForProfile;//List<ResolveInfo>
    pm->QueryBroadcastReceivers(intent,
            IPackageManager::GET_META_DATA | IPackageManager::GET_DISABLED_UNTIL_USED_COMPONENTS,
            profileId, (IList**)&enabledForProfile);
    if (enabledForProfile == NULL) {
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        coll->GetEmptyList((IList**)&enabledForProfile);
    }
    Int32 n;
    enabledForProfile->GetSize(&n);
    AutoPtr<IInterface> obj;
    mAdminsByProfile->Get(profileId, (IInterface**)&obj);
    AutoPtr<IArrayList> deviceAdmins = IArrayList::Probe(obj);// ArrayList<DeviceAdminInfo>
    if (deviceAdmins == NULL) {
        CArrayList::New(n, (IArrayList**)&deviceAdmins);
    }
    Boolean res;
    for (Int32 i = 0; i < n; ++i) {
        AutoPtr<IInterface> tmp;
        enabledForProfile->Get(i, (IInterface**)&tmp);
        IResolveInfo* resolveInfo = IResolveInfo::Probe(tmp);

        AutoPtr<IActivityInfo> activityInfo;
        resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        String packageName, name;
        IPackageItemInfo* pii = IPackageItemInfo::Probe(activityInfo);
        pii->GetPackageName(&packageName);
        pii->GetName(&name);
        AutoPtr<IComponentName> riComponentName;
        CComponentName::New(packageName, name, (IComponentName**)&riComponentName);
        if (alreadyAddedComponents == NULL
                || (alreadyAddedComponents->Contains(riComponentName, &res), !res)) {
            AutoPtr<IDeviceAdminInfo> deviceAdminInfo = CreateDeviceAdminInfo(resolveInfo);
            // add only visible ones (note: active admins are added regardless of visibility)
            if (deviceAdminInfo != NULL && (deviceAdminInfo->IsVisible(&res), res)) {
                deviceAdmins->Add(deviceAdminInfo);
            }
        }
    }
    if (deviceAdmins->IsEmpty(&res), !res) {
        mAdminsByProfile->Put(profileId, deviceAdmins);
    }
}

void CDeviceAdminSettings::AddActiveAdminsForProfile(
    /* [in] */ IList* activeAdmins,
    /* [in] */ Int32 profileId)
{
    if (activeAdmins != NULL) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IPackageManager> packageManager;
        IContext::Probe(activity)->GetPackageManager((IPackageManager**)&packageManager);
        Int32 n;
        activeAdmins->GetSize(&n);
        AutoPtr<IArrayList> deviceAdmins; // ArrayList<DeviceAdminInfo>
        CArrayList::New(n, (IArrayList**)&deviceAdmins);
        for (Int32 i = 0; i < n; ++i) {
            AutoPtr<IInterface> obj;
            activeAdmins->Get(i, (IInterface**)&obj);
            IComponentName* activeAdmin = IComponentName::Probe(obj);
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->SetComponent(activeAdmin);
            AutoPtr<IList> resolved; //List<ResolveInfo>
            packageManager->QueryBroadcastReceivers(
                    intent, IPackageManager::GET_META_DATA | IPackageManager::GET_DISABLED_UNTIL_USED_COMPONENTS,
                    profileId, (IList**)&resolved);
            if (resolved != NULL) {
                Int32 resolvedMax;
                resolved->GetSize(&resolvedMax);
                for (Int32 j = 0; j < resolvedMax; ++j) {
                    AutoPtr<IInterface> obj;
                    resolved->Get(j, (IInterface**)&obj);
                    AutoPtr<IDeviceAdminInfo> deviceAdminInfo = CreateDeviceAdminInfo(IResolveInfo::Probe(obj));
                    if (deviceAdminInfo != NULL) {
                        deviceAdmins->Add(deviceAdminInfo);
                    }
                }
            }
        }
        Boolean res;
        if (deviceAdmins->IsEmpty(&res), !res) {
            mAdminsByProfile->Put(profileId, deviceAdmins);
        }
    }
}

AutoPtr<IDeviceAdminInfo> CDeviceAdminSettings::CreateDeviceAdminInfo(
    /* [in] */ IResolveInfo* resolved)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    ECode ec;
    AutoPtr<IDeviceAdminInfo> dai;
    assert(0 && "TODO");
    // ec = CDeviceAdminInfo::New(IContext::Probe(activity), resolved, (IDeviceAdminInfo**)&dai);
    // if (SUCCEEDED(ec)) {
    //     return dai;
    // }

    AutoPtr<IActivityInfo> activityInfo;
    resolved->GetActivityInfo((IActivityInfo**)&activityInfo);

    if (ec == (ECode) E_XML_PULL_PARSER_EXCEPTION) {
        Logger::W(TAG, "Skipping %s, 0x%08x", TO_CSTR(activityInfo), ec);
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::W(TAG, "Skipping %s, 0x%08x", TO_CSTR(activityInfo), ec);
    }
    return NULL;
}

Int32 CDeviceAdminSettings::GetUserId(
    /* [in] */ IDeviceAdminInfo* adminInfo)
{
    AutoPtr<IActivityInfo> activityInfo;
    adminInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
    AutoPtr<IApplicationInfo> applicationInfo;
    IComponentInfo::Probe(activityInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    Int32 uid;
    applicationInfo->GetUid(&uid);
    return UserHandle::GetUserId(uid);
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
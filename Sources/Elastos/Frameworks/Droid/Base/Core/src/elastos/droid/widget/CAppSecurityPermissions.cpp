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

#include "elastos/droid/widget/CAppSecurityPermissions.h"
#include "elastos/droid/app/CAlertDialogBuilder.h"
#include "elastos/droid/os/CParcel.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/widget/CLinearLayoutLayoutParams.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Os::CParcel;
using Elastos::Droid::Text::CSpannableStringBuilder;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Text::CCollatorHelper;
using Elastos::Text::ICollatorHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::CHashSet;

namespace Elastos {
namespace Droid {
namespace Widget {

CAppSecurityPermissions::MyPermissionGroupInfo::MyPermissionGroupInfo(
    /* [in] */ PermissionInfo* perm)
{
    mName = perm->mPackageName;
    mPackageName = perm->mPackageName;
    CArrayList::New((IList**)&mNewPermissions);
    CArrayList::New((IList**)&mPersonalPermissions);
    CArrayList::New((IList**)&mDevicePermissions);
    CArrayList::New((IList**)&mAllPermissions);
}

CAppSecurityPermissions::MyPermissionGroupInfo::MyPermissionGroupInfo(
    /* [in] */ PermissionGroupInfo* info)
{
    PermissionGroupInfo::constructor(info);
    CArrayList::New((IList**)&mNewPermissions);
    CArrayList::New((IList**)&mPersonalPermissions);
    CArrayList::New((IList**)&mDevicePermissions);
    CArrayList::New((IList**)&mAllPermissions);
}

AutoPtr<IDrawable> CAppSecurityPermissions::MyPermissionGroupInfo::LoadGroupIcon(
    /* [in] */ IPackageManager* pm)
{
    if (mIcon != 0) {
        AutoPtr<IDrawable> icon;
        LoadIcon(pm, (IDrawable**)&icon);
        return icon;
    }
    else {
        AutoPtr<IApplicationInfo> appInfo;
        pm->GetApplicationInfo(mPackageName, 0, (IApplicationInfo**)&appInfo);
        if (appInfo != NULL) {
            AutoPtr<IDrawable> icon;
            IPackageItemInfo::Probe(appInfo)->LoadIcon(pm, (IDrawable**)&icon);
            return icon;
        }
    }
    return NULL;
}

CAppSecurityPermissions::MyPermissionInfo::MyPermissionInfo(
    /* [in] */ PermissionInfo* info)
    : mNewReqFlags(0)
    , mExistingReqFlags(0)
    , mNew(FALSE)
{
    PermissionInfo::constructor(info);
}

CAR_INTERFACE_IMPL(CAppSecurityPermissions::PermissionItemView::ViewOnClickListener,
    Object, IViewOnClickListener)

CAppSecurityPermissions::PermissionItemView::ViewOnClickListener::ViewOnClickListener(
    /* [in] */ PermissionItemView* host)
    : mHost(host)
{}

ECode CAppSecurityPermissions::PermissionItemView::ViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

CAR_INTERFACE_IMPL(CAppSecurityPermissions::PermissionItemView::DialogInterfaceOnClickListener,
    Object, IDialogInterfaceOnClickListener)

CAppSecurityPermissions::PermissionItemView::DialogInterfaceOnClickListener::DialogInterfaceOnClickListener(
    /* [in] */ PermissionItemView* host)
    : mHost(host)
{}

ECode CAppSecurityPermissions::PermissionItemView::DialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    pm->RevokePermission(mHost->mPackageName, mHost->mPerm->mName);
    mHost->SetVisibility(IView::GONE);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CAppSecurityPermissions::PermissionItemView, LinearLayout, IPermissionItemView)

CAppSecurityPermissions::PermissionItemView::PermissionItemView()
    : mShowRevokeUI(FALSE)
{}

ECode CAppSecurityPermissions::PermissionItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::constructor(context, attrs);
    SetClickable(TRUE);
    return NOERROR;
}

ECode CAppSecurityPermissions::PermissionItemView::SetPermission(
    /* [in] */ MyPermissionGroupInfo* grp,
    /* [in] */ MyPermissionInfo* perm,
    /* [in] */ Boolean first,
    /* [in] */ ICharSequence* newPermPrefix,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean showRevokeUI)
{
    mGroup = grp;
    mPerm = perm;
    mShowRevokeUI = showRevokeUI;
    mPackageName = packageName;

    AutoPtr<IView> view;
    FindViewById(R::id::perm_icon, (IView**)&view);
    AutoPtr<IImageView> permGrpIcon = IImageView::Probe(view);
    view = NULL;
    FindViewById(R::id::perm_name, (IView**)&view);
    AutoPtr<ITextView> permNameView = ITextView::Probe(view);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IDrawable> icon;
    if (first) {
        icon = grp->LoadGroupIcon(pm);
    }
    AutoPtr<ICharSequence> label = perm->mLabel;
    if (perm->mNew && newPermPrefix != NULL) {
        // If this is a new permission, format it appropriately.
        AutoPtr<CSpannableStringBuilder> builder;
        CSpannableStringBuilder::NewByFriend((CSpannableStringBuilder**)&builder);
        AutoPtr<IParcel> parcel;
        CParcel::New((IParcel**)&parcel);
        TextUtils::WriteToParcel(newPermPrefix, parcel);
        parcel->SetDataPosition(0);
        AutoPtr<ICharSequence> newStr;
        TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(parcel, (ICharSequence**)&newStr);
        builder->Append(newStr);
        builder->Append(label);
        label = builder;
    }

    permGrpIcon->SetImageDrawable(icon);
    permNameView->SetText(label);
    AutoPtr<IViewOnClickListener> listener = new ViewOnClickListener(this);
    SetOnClickListener(listener);
    if (localLOGV)
        Logger::I(TAG, "Made perm item %s: %s in group %s",
            perm->mName.string(), TO_CSTR(label), grp->mName.string());
    return NOERROR;
}

ECode CAppSecurityPermissions::PermissionItemView::OnClick(
    /* [in] */ IView* v)
{
    if (mGroup != NULL && mPerm != NULL) {
        if (mDialog != NULL) {
            IDialogInterface::Probe(mDialog)->Dismiss();
        }
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IPackageManager> pm;
        context->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
        builder->SetTitle(mGroup->mLabel);
        if (mPerm->mDescriptionRes != 0) {
            AutoPtr<ICharSequence> desc;
            mPerm->LoadDescription(pm, (ICharSequence**)&desc);
            builder->SetMessage(desc);
        }
        else {
            AutoPtr<ICharSequence> appName;
            AutoPtr<IApplicationInfo> app;
            pm->GetApplicationInfo(mPerm->mPackageName, 0, (IApplicationInfo**)&app);
            if (app != NULL) {
                IPackageItemInfo::Probe(app)->LoadLabel(pm, (ICharSequence**)&appName);
            }
            else {
                appName = CoreUtils::Convert(mPerm->mPackageName);
            }
            StringBuilder sbuilder(128);
            AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, appName);
            String str;
            context->GetString(R::string::perms_description_app, args, &str);
            sbuilder.Append(str);
            sbuilder.Append("\n\n");
            sbuilder.Append(mPerm->mName);
            builder->SetMessage(CoreUtils::Convert(sbuilder.ToString()));
        }
        builder->SetCancelable(TRUE);
        builder->SetIcon(mGroup->LoadGroupIcon(pm));
        AddRevokeUIIfNecessary(builder);
        builder->Show((IAlertDialog**)&mDialog);
        IDialog::Probe(mDialog)->SetCanceledOnTouchOutside(TRUE);
    }
    return NOERROR;
}

ECode CAppSecurityPermissions::PermissionItemView::OnDetachedFromWindow()
{
    LinearLayout::OnDetachedFromWindow();
    if (mDialog != NULL) {
        IDialogInterface::Probe(mDialog)->Dismiss();
    }
    return NOERROR;
}

void CAppSecurityPermissions::PermissionItemView::AddRevokeUIIfNecessary(
    /* [in] */ IAlertDialogBuilder* builder)
{
    if (!mShowRevokeUI) {
        return;
    }

    Boolean isRequired =
            ((mPerm->mExistingReqFlags & IPackageInfo::REQUESTED_PERMISSION_REQUIRED) != 0);

    if (isRequired) {
        return;
    }

    AutoPtr<IDialogInterfaceOnClickListener> ocl = new DialogInterfaceOnClickListener(this);
    builder->SetNegativeButton(R::string::revoke, ocl);
    builder->SetPositiveButton(R::string::ok, NULL);
}

CAR_INTERFACE_IMPL(CAppSecurityPermissions::PermissionGroupInfoComparator, Object, IComparator)

CAppSecurityPermissions::PermissionGroupInfoComparator::PermissionGroupInfoComparator()
{
    AutoPtr<ICollatorHelper> cHelper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&cHelper);
    cHelper->GetInstance((ICollator**)&sCollator);
}

ECode CAppSecurityPermissions::PermissionGroupInfoComparator::Compare(
    /* [in] */ IInterface* _a,
    /* [in] */ IInterface* _b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    MyPermissionGroupInfo* a = (MyPermissionGroupInfo*)IObject::Probe(_a);
    MyPermissionGroupInfo* b = (MyPermissionGroupInfo*)IObject::Probe(_b);
    if (((a->mFlags^b->mFlags) & IPermissionGroupInfo::FLAG_PERSONAL_INFO) != 0) {
        *result = ((a->mFlags & IPermissionGroupInfo::FLAG_PERSONAL_INFO) != 0) ? -1 : 1;
        return NOERROR;
    }
    if (a->mPriority != b->mPriority) {
        *result = a->mPriority > b->mPriority ? -1 : 1;
        return NOERROR;
    }
    return sCollator->Compare(a->mLabel, b->mLabel, result);
}

CAR_INTERFACE_IMPL(CAppSecurityPermissions::PermissionInfoComparator, Object, IComparator)

CAppSecurityPermissions::PermissionInfoComparator::PermissionInfoComparator()
{
    AutoPtr<ICollatorHelper> cHelper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&cHelper);
    cHelper->GetInstance((ICollator**)&sCollator);
}

ECode CAppSecurityPermissions::PermissionInfoComparator::Compare(
    /* [in] */ IInterface* _a,
    /* [in] */ IInterface* _b,
    /* [out] */ Int32* result)
{
    MyPermissionInfo* a = (MyPermissionInfo*)IObject::Probe(_a);
    MyPermissionInfo* b = (MyPermissionInfo*)IObject::Probe(_b);
    return sCollator->Compare(a->mLabel, b->mLabel, result);
}

const String CAppSecurityPermissions::TAG("AppSecurityPermissions");
const Boolean CAppSecurityPermissions::localLOGV = FALSE;

CAR_INTERFACE_IMPL(CAppSecurityPermissions, Object, IAppSecurityPermissions)

CAR_OBJECT_IMPL(CAppSecurityPermissions)

CAppSecurityPermissions::CAppSecurityPermissions()
{
    CArrayList::New((IList**)&mPermGroupsList);
    mPermGroupComparator = new PermissionGroupInfoComparator();
    mPermComparator = new PermissionInfoComparator();
    CArrayList::New((IList**)&mPermsList);
}

ECode CAppSecurityPermissions::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
    mInflater = ILayoutInflater::Probe(service);
    mContext->GetPackageManager((IPackageManager**)&mPm);
    // Pick up from framework resources instead.
    mContext->GetText(R::string::perms_new_perm_prefix, (ICharSequence**)&mNewPermPrefix);
    return NOERROR;
}

ECode CAppSecurityPermissions::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName)
{
    constructor(context);
    mPackageName = packageName;
    AutoPtr<ISet> permSet; //MyPermissionInfo
    CHashSet::New((ISet**)&permSet);
    AutoPtr<IPackageInfo> pkgInfo;
    if (FAILED(mPm->GetPackageInfo(packageName, IPackageManager::GET_PERMISSIONS, (IPackageInfo**)&pkgInfo))) {
        Logger::W(TAG, "Couldn't retrieve permissions for package:%s", packageName.string());
        return NOERROR;
    }
    // Extract all user permissions
    AutoPtr<IApplicationInfo> appInfo;
    pkgInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 uid;
    if((appInfo != NULL) && (appInfo->GetUid(&uid), uid != -1)) {
        GetAllUsedPermissions(uid, permSet);
    }
    mPermsList->AddAll(ICollection::Probe(permSet));
    SetPermissions(mPermsList);
    return NOERROR;
}

ECode CAppSecurityPermissions::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IPackageInfo* info)
{
    constructor(context);
    AutoPtr<ISet> permSet; //MyPermissionInfo
    CHashSet::New((ISet**)&permSet);
    if (info == NULL) {
        return NOERROR;
    }
    info->GetPackageName(&mPackageName);

    // Convert to a PackageInfo
    AutoPtr<IPackageInfo> installedPkgInfo;
    // Get requested permissions
    AutoPtr<ArrayOf<String> > requestedPermissions;
    info->GetRequestedPermissions((ArrayOf<String>**)&requestedPermissions);
    if (requestedPermissions != NULL) {
        mPm->GetPackageInfo(mPackageName, IPackageManager::GET_PERMISSIONS,
            (IPackageInfo**)&installedPkgInfo);
        ExtractPerms(info, permSet, installedPkgInfo);
    }
    // Get permissions related to  shared user if any
    String sharedUserId;
    info->GetSharedUserId(&sharedUserId);
    if (sharedUserId != NULL) {
        Int32 sharedUid;
        if (SUCCEEDED(mPm->GetUidForSharedUser(sharedUserId, &sharedUid))) {
            GetAllUsedPermissions(sharedUid, permSet);
        }
        else {
            Logger::W(TAG, "Couldn't retrieve shared user id for: %s", mPackageName.string());
        }
    }
    // Retrieve list of permissions
    mPermsList->AddAll(ICollection::Probe(permSet));
    SetPermissions(mPermsList);
    return NOERROR;
}

AutoPtr<IView> CAppSecurityPermissions::GetPermissionItemView(
    /* [in] */ IContext* context,
    /* [in] */ ICharSequence* grpName,
    /* [in] */ ICharSequence* description,
    /* [in] */ Boolean dangerous)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(service);
    AutoPtr<IDrawable> icon;
    context->GetDrawable(dangerous ? R::drawable::ic_bullet_key_permission
        : R::drawable::ic_text_dot, (IDrawable**)&icon);
    return GetPermissionItemViewOld(context, inflater, grpName,
            description, dangerous, icon);
}

void CAppSecurityPermissions::GetAllUsedPermissions(
    /* [in] */ Int32 sharedUid,
    /* [in] */ ISet* permSet)
{
    AutoPtr<ArrayOf<String> > sharedPkgList;
    mPm->GetPackagesForUid(sharedUid, (ArrayOf<String>**)&sharedPkgList);
    if (sharedPkgList == NULL || (sharedPkgList->GetLength() == 0)) {
        return;
    }
    for(Int32 i = 0; i < sharedPkgList->GetLength(); i++) {
        GetPermissionsForPackage((*sharedPkgList)[i], permSet);
    }
}

void CAppSecurityPermissions::GetPermissionsForPackage(
    /* [in] */ const String& packageName,
    /* [in] */ ISet* permSet)
{
    AutoPtr<IPackageInfo> pkgInfo;
    if (SUCCEEDED(mPm->GetPackageInfo(packageName, IPackageManager::GET_PERMISSIONS, (IPackageInfo**)&pkgInfo))) {
        ExtractPerms(pkgInfo, permSet, pkgInfo);
    }
    else {
        Logger::W(TAG, "Couldn't retrieve permissions for package: %s", packageName.string());
    }
}

void CAppSecurityPermissions::ExtractPerms(
    /* [in] */ IPackageInfo* info,
    /* [in] */ ISet* permSet,
    /* [in] */ IPackageInfo* installedPkgInfo)
{
    AutoPtr<ArrayOf<String> > strList;
    info->GetRequestedPermissions((ArrayOf<String>**)&strList);
    AutoPtr<ArrayOf<Int32> > flagsList;
    info->GetRequestedPermissionsFlags((ArrayOf<Int32>**)&flagsList);
    if ((strList == NULL) || (strList->GetLength() == 0)) {
        return;
    }
    for (Int32 i = 0; i < strList->GetLength(); i++) {
        String permName = (*strList)[i];
        // If we are only looking at an existing app, then we only
        // care about permissions that have actually been granted to it.
        if (installedPkgInfo != NULL && info == installedPkgInfo) {
            if (((*flagsList)[i]&IPackageInfo::REQUESTED_PERMISSION_GRANTED) == 0) {
                continue;
            }
        }

        AutoPtr<IPermissionInfo> _tmpPermInfo;
        if (FAILED(mPm->GetPermissionInfo(permName, 0, (IPermissionInfo**)&_tmpPermInfo))) {
            Logger::I(TAG, "Ignoring unknown permission:%s", permName.string());
            continue;
        }
        if (_tmpPermInfo == NULL) {
            continue;
        }
        PermissionInfo* tmpPermInfo = (PermissionInfo*)_tmpPermInfo.Get();
        Int32 existingIndex = -1;
        AutoPtr<ArrayOf<String> > requestedPermissions;
        if (installedPkgInfo != NULL)
            installedPkgInfo->GetRequestedPermissions((ArrayOf<String>**)&requestedPermissions);
        if (requestedPermissions != NULL) {
            for (Int32 j = 0; j < requestedPermissions->GetLength(); j++) {
                if (permName.Equals((*requestedPermissions)[j])) {
                    existingIndex = j;
                    break;
                }
            }
        }
        AutoPtr<ArrayOf<Int32> > requestedPermissionsFlags;
        if (existingIndex >= 0)
            installedPkgInfo->GetRequestedPermissionsFlags((ArrayOf<Int32>**)&requestedPermissionsFlags);
        Int32 existingFlags = existingIndex >= 0 ? (*requestedPermissionsFlags)[existingIndex] : 0;
        if (!IsDisplayablePermission(tmpPermInfo, (*flagsList)[i], existingFlags)) {
            // This is not a permission that is interesting for the user
            // to see, so skip it.
            continue;
        }
        String origGroupName = tmpPermInfo->mGroup;
        String groupName = origGroupName;
        if (groupName == NULL) {
            groupName = tmpPermInfo->mPackageName;
            tmpPermInfo->mGroup = groupName;
        }
        AutoPtr<MyPermissionGroupInfo> group = mPermGroups[groupName];
        if (group == NULL) {
            AutoPtr<IPermissionGroupInfo> grp;
            if (origGroupName != NULL) {
                mPm->GetPermissionGroupInfo(origGroupName, 0, (IPermissionGroupInfo**)&grp);
            }
            if (grp != NULL) {
                group = new MyPermissionGroupInfo((PermissionGroupInfo*)grp.Get());
            }
            else {
                // We could be here either because the permission
                // didn't originally specify a group or the group it
                // gave couldn't be found.  In either case, we consider
                // its group to be the permission's package name.
                tmpPermInfo->mGroup = tmpPermInfo->mPackageName;
                group = mPermGroups[tmpPermInfo->mGroup];
                if (group == NULL) {
                    group = new MyPermissionGroupInfo(tmpPermInfo);
                }
                // TODO: bug ???
                // group = new MyPermissionGroupInfo(tmpPermInfo);
            }
            mPermGroups[tmpPermInfo->mGroup] = group;
        }
        Boolean newPerm = installedPkgInfo != NULL
                && (existingFlags & IPackageInfo::REQUESTED_PERMISSION_GRANTED) == 0;
        AutoPtr<MyPermissionInfo> myPerm = new MyPermissionInfo(tmpPermInfo);
        myPerm->mNewReqFlags = (*flagsList)[i];
        myPerm->mExistingReqFlags = existingFlags;
        // This is a new permission if the app is already installed and
        // doesn't currently hold this permission.
        myPerm->mNew = newPerm;
        permSet->Add((IObject*)myPerm);
    }
}

ECode CAppSecurityPermissions::GetPermissionCount(
    /* [out] */ Int32* count)
{
    return GetPermissionCount(WHICH_ALL, count);
}

AutoPtr<IList> CAppSecurityPermissions::GetPermissionList(
    /* [in] */ MyPermissionGroupInfo* grp,
    /* [in] */ Int32 which)
{
    if (which == WHICH_NEW) {
        return grp->mNewPermissions;
    }
    else if (which == WHICH_PERSONAL) {
        return grp->mPersonalPermissions;
    }
    else if (which == WHICH_DEVICE) {
        return grp->mDevicePermissions;
    }
    else {
        return grp->mAllPermissions;
    }
}

ECode CAppSecurityPermissions::GetPermissionCount(
    /* [in] */ Int32 which,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    Int32 N = 0;
    Int32 size;
    mPermGroupsList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        mPermGroupsList->Get(i, (IInterface**)&item);
        Int32 s;
        GetPermissionList((MyPermissionGroupInfo*)IObject::Probe(item), which)->GetSize(&s);
        N += s;
    }
    *count = N;
    return NOERROR;
}

ECode CAppSecurityPermissions::GetPermissionsView(
    /* [out] */ IView** view)
{
    return GetPermissionsView(WHICH_ALL, FALSE, view);
}

ECode CAppSecurityPermissions::GetPermissionsViewWithRevokeButtons(
    /* [out] */ IView** view)
{
    return GetPermissionsView(WHICH_ALL, TRUE, view);
}

ECode CAppSecurityPermissions::GetPermissionsView(
    /* [in] */ Int32 which,
    /* [out] */ IView** view)
{
    return GetPermissionsView(which, FALSE, view);
}

ECode CAppSecurityPermissions::GetPermissionsView(
    /* [in] */ Int32 which,
    /* [in] */ Boolean showRevokeUI,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    AutoPtr<IView> permsView;
    mInflater->Inflate(R::layout::app_perms_summary, NULL, (IView**)&permsView);
    AutoPtr<IView> temp;
    permsView->FindViewById(R::id::perms_list, (IView**)&temp);
    AutoPtr<ILinearLayout> displayList = ILinearLayout::Probe(temp);
    AutoPtr<IView> noPermsView;
    permsView->FindViewById(R::id::no_permissions, (IView**)&noPermsView);

    DisplayPermissions(mPermGroupsList, displayList, which, showRevokeUI);
    Int32 childCount;
    IViewGroup::Probe(displayList)->GetChildCount(&childCount);
    if (childCount <= 0) {
        noPermsView->SetVisibility(IView::VISIBLE);
    }

    *view = permsView;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

void CAppSecurityPermissions::DisplayPermissions(
    /* [in] */ IList* groups,
    /* [in] */ ILinearLayout* _permListView,
    /* [in] */ Int32 which,
    /* [in] */ Boolean showRevokeUI)
{
    IViewGroup* permListView = IViewGroup::Probe(_permListView);
    permListView->RemoveAllViews();

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);
    Int32 spacing = (Int32)(8 * density);

    Int32 gs = 0;
    groups->GetSize(&gs);
    for (Int32 i = 0; i < gs; i++) {
        AutoPtr<IInterface> item;
        groups->Get(i, (IInterface**)&item);
        MyPermissionGroupInfo* grp = (MyPermissionGroupInfo*)IObject::Probe(item);
        Int32 s = 0;
        grp->mAllPermissions->GetSize(&s);
        AutoPtr<IList> perms = GetPermissionList(grp, which);
        Int32 ps = 0;
        perms->GetSize(&ps);
        for (Int32 j = 0; j < ps; j++) {
            AutoPtr<IInterface> obj;
            perms->Get(j, (IInterface**)&obj);
            MyPermissionInfo* perm = (MyPermissionInfo*)IObject::Probe(obj);
            AutoPtr<IView> view = GetPermissionItemView(grp, perm, j == 0,
                    which != WHICH_NEW ? mNewPermPrefix : NULL, showRevokeUI);
            AutoPtr<IViewGroupMarginLayoutParams> lp;
            CLinearLayoutLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
                IViewGroupLayoutParams::WRAP_CONTENT, (IViewGroupMarginLayoutParams**)&lp);
            if (j == 0) {
                lp->SetTopMargin(spacing);
            }
            if (j == s - 1) {
                lp->SetBottomMargin(spacing);
            }
            Int32 childCount;
            permListView->GetChildCount(&childCount);
            if (childCount == 0) {
                Int32 topMargin;
                lp->GetTopMargin(&topMargin);
                lp->SetTopMargin(topMargin * 2);
            }
            permListView->AddView(view, IViewGroupLayoutParams::Probe(lp));
        }
    }
}

AutoPtr<CAppSecurityPermissions::PermissionItemView> CAppSecurityPermissions::GetPermissionItemView(
    /* [in] */ MyPermissionGroupInfo* grp,
    /* [in] */ MyPermissionInfo* perm,
    /* [in] */ Boolean first,
    /* [in] */ ICharSequence* newPermPrefix,
    /* [in] */ Boolean showRevokeUI)
{
    return GetPermissionItemView(mContext, mInflater, grp, perm, first, newPermPrefix,
            mPackageName, showRevokeUI);
}

AutoPtr<CAppSecurityPermissions::PermissionItemView> CAppSecurityPermissions::GetPermissionItemView(
    /* [in] */ IContext* context,
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ MyPermissionGroupInfo* grp,
    /* [in] */ MyPermissionInfo* perm,
    /* [in] */ Boolean first,
    /* [in] */ ICharSequence* newPermPrefix,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean showRevokeUI)
{
    AutoPtr<IView> view;
    inflater->Inflate(
        (perm->mFlags & IPermissionInfo::FLAG_COSTS_MONEY) != 0
            ? R::layout::app_permission_item_money : R::layout::app_permission_item,
        NULL, (IView**)&view);
    AutoPtr<PermissionItemView> permView = (PermissionItemView*)view.Get();
    permView->SetPermission(grp, perm, first, newPermPrefix, packageName, showRevokeUI);
    return permView;
}

AutoPtr<IView> CAppSecurityPermissions::GetPermissionItemViewOld(
    /* [in] */ IContext* context,
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ ICharSequence* grpName,
    /* [in] */ ICharSequence* permList,
    /* [in] */ Boolean dangerous,
    /* [in] */ IDrawable* icon)
{
    AutoPtr<IView> permView;
    inflater->Inflate(R::layout::app_permission_item_old, NULL, (IView**)&permView);

    AutoPtr<IView> view;
    permView->FindViewById(R::id::permission_group, (IView**)&view);
    AutoPtr<ITextView> permGrpView = ITextView::Probe(view);
    view = NULL;
    permView->FindViewById(R::id::permission_list, (IView**)&view);
    AutoPtr<ITextView> permDescView = ITextView::Probe(view);

    view = NULL;
    permView->FindViewById(R::id::perm_icon, (IView**)&view);
    AutoPtr<IImageView> imgView = IImageView::Probe(permView);
    imgView->SetImageDrawable(icon);
    if(grpName != NULL) {
        permGrpView->SetText(grpName);
        permDescView->SetText(permList);
    }
    else {
        permGrpView->SetText(permList);
        IView::Probe(permDescView)->SetVisibility(IView::GONE);
    }
    return permView;
}

Boolean CAppSecurityPermissions::IsDisplayablePermission(
    /* [in] */ IPermissionInfo* pInfo,
    /* [in] */ Int32 newReqFlags,
    /* [in] */ Int32 existingReqFlags)
{
    Int32 protectionLevel;
    pInfo->GetProtectionLevel(&protectionLevel);
    Int32 base = protectionLevel & IPermissionInfo::PROTECTION_MASK_BASE;
    Boolean isNormal = (base == IPermissionInfo::PROTECTION_NORMAL);
    Boolean isDangerous = (base == IPermissionInfo::PROTECTION_DANGEROUS);
    Boolean isRequired = ((newReqFlags&IPackageInfo::REQUESTED_PERMISSION_REQUIRED) != 0);
    Boolean isDevelopment = ((protectionLevel&IPermissionInfo::PROTECTION_FLAG_DEVELOPMENT) != 0);
    Boolean wasGranted = ((existingReqFlags&IPackageInfo::REQUESTED_PERMISSION_GRANTED) != 0);
    Boolean isGranted = ((newReqFlags&IPackageInfo::REQUESTED_PERMISSION_GRANTED) != 0);

    // Dangerous and normal permissions are always shown to the user if the permission
    // is required, or it was previously granted
    if ((isNormal || isDangerous) && (isRequired || wasGranted || isGranted)) {
        return TRUE;
    }

    // Development permissions are only shown to the user if they are already
    // granted to the app -- if we are installing an app and they are not
    // already granted, they will not be granted as part of the install.
    if (isDevelopment && wasGranted) {
        if (localLOGV) {
            String name;
            IPackageItemInfo::Probe(pInfo)->GetName(&name);
            Logger::I(TAG, "Special perm %s: protlevel=0x%x", name.string(), protectionLevel);
        }
        return TRUE;
    }
    return FALSE;
}

void CAppSecurityPermissions::AddPermToList(
    /* [in] */ IList* permList,
    /* [in] */ MyPermissionInfo* pInfo)
{
    if (pInfo->mLabel == NULL) {
        pInfo->LoadLabel(mPm, (ICharSequence**)&pInfo->mLabel);
    }
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    Int32 idx;
    collections->BinarySearch(permList, (IObject*)pInfo, mPermComparator, &idx);
    if (localLOGV) {
        Int32 size;
        permList->GetSize(&size);
        Logger::I(TAG, "idx=%d, list.size=%d", idx, size);
    }
    if (idx < 0) {
        idx = -idx-1;
        permList->Add(idx, (IObject*)pInfo);
    }
}

void CAppSecurityPermissions::SetPermissions(
    /* [in] */ IList* permList)
{
    if (permList != NULL) {
        // First pass to group permissions
        Int32 size = 0;
        permList->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> item;
            permList->Get(i, (IInterface**)&item);
            MyPermissionInfo* pInfo = (MyPermissionInfo*)IObject::Probe(item);
            if(localLOGV)
                Logger::I(TAG, "Processing permission:%s", pInfo->mName.string());
            if(!IsDisplayablePermission(pInfo, pInfo->mNewReqFlags, pInfo->mExistingReqFlags)) {
                if(localLOGV) Logger::I(TAG, "Permission:%s is not displayable", pInfo->mName.string());
                continue;
            }
            HashMap<String, AutoPtr<MyPermissionGroupInfo> >::Iterator it = mPermGroups.Find(pInfo->mGroup);
            AutoPtr<MyPermissionGroupInfo> group;
            if (it != mPermGroups.End())
                group = it->mSecond;

            if (group != NULL) {
                pInfo->mLabel = NULL;
                pInfo->LoadLabel(mPm, (ICharSequence**)&pInfo->mLabel);
                AddPermToList(group->mAllPermissions, pInfo);
                if (pInfo->mNew) {
                    AddPermToList(group->mNewPermissions, pInfo);
                }
                if ((group->mFlags&IPermissionGroupInfo::FLAG_PERSONAL_INFO) != 0) {
                    AddPermToList(group->mPersonalPermissions, pInfo);
                }
                else {
                    AddPermToList(group->mDevicePermissions, pInfo);
                }
            }
        }
    }

    HashMap<String, AutoPtr<MyPermissionGroupInfo> >::Iterator it;
    for (it = mPermGroups.Begin(); it != mPermGroups.End(); ++it) {
        AutoPtr<MyPermissionGroupInfo> pgrp = it->mSecond;
        pgrp->mLabel = NULL;
        if (pgrp->mLabelRes != 0 || pgrp->mNonLocalizedLabel != NULL) {
            pgrp->LoadLabel(mPm, (ICharSequence**)&pgrp->mLabel);
        }
        else {
            AutoPtr<IApplicationInfo> app;
            mPm->GetApplicationInfo(pgrp->mPackageName, 0, (IApplicationInfo**)&app);
            if (app != NULL) {
                IPackageItemInfo::Probe(app)->LoadLabel(mPm, (ICharSequence**)&pgrp->mLabel);
            }
            else {
                pgrp->LoadLabel(mPm, (ICharSequence**)&pgrp->mLabel);
            }
        }
        mPermGroupsList->Add((IObject*)pgrp);
    }
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Sort(mPermGroupsList, mPermGroupComparator);
    if (localLOGV) {
        Int32 size = 0;
        mPermGroupsList->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> item;
            mPermGroupsList->Get(i, (IInterface**)&item);
            MyPermissionGroupInfo* grp = (MyPermissionGroupInfo*)IObject::Probe(item);
            Logger::I(TAG, "Group %s personal=%d priority=%d", grp->mName.string(),
                (grp->mFlags & IPermissionGroupInfo::FLAG_PERSONAL_INFO) != 0, grp->mPriority);
        }
    }
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#include "elastos/droid/packageinstaller/CGrantActivity.h"
#include "elastos/droid/packageinstaller/PackageUtil.h"
#include "elastos/droid/packageinstaller/TabsAdapter.h"
#include <Elastos.Droid.Graphics.h>
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Content::Pm::IPermissionInfo;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
//using Elastos::Droid::Widget::CAppSecurityPermissions;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Droid::Widget::ITabSpec;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

CAR_INTERFACE_IMPL(CGrantActivity::ViewOnClickListener, Object, IViewOnClickListener)

ECode CGrantActivity::ViewOnClickListener::OnClick(
  /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

CAR_INTERFACE_IMPL_2(CGrantActivity, Activity, IViewOnClickListener, IGrantActivity)

CAR_OBJECT_IMPL(CGrantActivity)

CGrantActivity::CGrantActivity()
{
}

ECode CGrantActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);
    GetPackageManager((IPackageManager**)&mPm);
    GetCallingPackage(&mRequestingPackage);

    mRequested_permissions = GetRequestedPermissions();
    if (mRequested_permissions->GetLength() == 0) {
        // The grant request was empty. Return success
        SetResult(RESULT_OK);
        Finish();
        return NOERROR;
    }

    AutoPtr<IPackageInfo> pkgInfo = GetUpdatedPackageInfo();
    AutoPtr<IAppSecurityPermissions> perms;
    Logger::D("CGrantActivity", "TODO: need CAppSecurityPermissions!");
    // CAppSecurityPermissions::New(this, pkgInfo);
    Int32 count = 0;
    // perms->GetPermissionCount(AppSecurityPermissions::WHICH_NEW, &count);
    if (count == 0) {
        // The updated permissions dialog said there are no new permissions.
        // This should never occur if mRequested_permissions.length > 0,
        // but we check for it anyway, just in case.
        SetResult(RESULT_OK);
        Finish();
        return NOERROR;
    }

    SetContentView(R::layout::install_start);
    AutoPtr<IView> viewTmp;
    FindViewById(R::id::install_confirm_question, (IView**)&viewTmp);
    ITextView::Probe(viewTmp)->SetText(R::string::grant_confirm_question);

    AutoPtr<IApplicationInfo> applicationInfo;
    pkgInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    AutoPtr<ICharSequence> label;
    mPm->GetApplicationLabel(applicationInfo, (ICharSequence**)&label);
    AutoPtr<IDrawable> drawableTmp;
    mPm->GetApplicationIcon(applicationInfo, (IDrawable**)&drawableTmp);
    AutoPtr<PackageUtil::AppSnippet> as = new PackageUtil::AppSnippet(label, drawableTmp);
    PackageUtil::InitSnippetForNewApp(IActivity::Probe(this), as, R::id::app_snippet);

    viewTmp = NULL;
    FindViewById(R::id::ok_button, (IView**)&viewTmp);
    mOk = IButton::Probe(viewTmp);

    ITextView::Probe(mOk)->SetText(R::string::ok);

    viewTmp = NULL;
    FindViewById(R::id::cancel_button, (IView**)&viewTmp);
    mCancel = IButton::Probe(viewTmp);
    AutoPtr<IViewOnClickListener> listener = new ViewOnClickListener(this);
    IView::Probe(mOk)->SetOnClickListener(listener);
    IView::Probe(mCancel)->SetOnClickListener(listener);

    viewTmp = NULL;
    FindViewById(Elastos::Droid::R::id::tabhost, (IView**)&viewTmp);
    ITabHost* tabHost = ITabHost::Probe(viewTmp);
    tabHost->Setup();

    viewTmp = NULL;
    FindViewById(R::id::pager, (IView**)&viewTmp);
    AutoPtr<IViewPager> viewPager = IViewPager::Probe(viewTmp);

    AutoPtr<TabsAdapter> adapter = new TabsAdapter(IActivity::Probe(this), tabHost, viewPager);

    AutoPtr<IView> newTab;
    perms->GetPermissionsView(IAppSecurityPermissions::WHICH_NEW, (IView**)&newTab);
    AutoPtr<IView> allTab = GetPermissionList(perms);

    AutoPtr<ITabSpec> newTabSpec;
    tabHost->NewTabSpec(String("new"), (ITabSpec**)&newTabSpec);
    AutoPtr<ICharSequence> charSequenceTmp;
    IContext::Probe(this)->GetText(R::string::newPerms, (ICharSequence**)&charSequenceTmp);
    newTabSpec->SetIndicator(charSequenceTmp);
    adapter->AddTab(newTabSpec, newTab);

    AutoPtr<ITabSpec> allTabSpec;
    tabHost->NewTabSpec(String("all"), (ITabSpec**)&allTabSpec);

    charSequenceTmp = NULL;
    IContext::Probe(this)->GetText(R::string::allPerms, (ICharSequence**)&charSequenceTmp);
    newTabSpec->SetIndicator(charSequenceTmp);
    adapter->AddTab(allTabSpec, allTab);
    return NOERROR;
}

ECode CGrantActivity::OnClick(
    /* [in] */ IView* v)
{
    if (v == IView::Probe(mOk)) {
        String permission;
        for (Int32 idx = 0; idx < mRequested_permissions->GetLength(); ++idx) {
            permission = (*mRequested_permissions)[idx];
            mPm->GrantPermission(mRequestingPackage, permission);
        }
        SetResult(RESULT_OK);
    }
    if (v == IView::Probe(mCancel)) {
        SetResult(RESULT_CANCELED);
    }
    Finish();
    return NOERROR;
}

AutoPtr<IPackageInfo> CGrantActivity::GetUpdatedPackageInfo()
{
    ///try {
        AutoPtr<IPackageInfo> pkgInfo;
        mPm->GetPackageInfo(mRequestingPackage, IPackageManager::GET_PERMISSIONS, (IPackageInfo**)&pkgInfo);
        AutoPtr< ArrayOf<String> > requestedPermissions;
        pkgInfo->GetRequestedPermissions((ArrayOf<String>**)&requestedPermissions);

        AutoPtr< ArrayOf<String> > pkgInfoRequestedPermissions;
        pkgInfo->GetRequestedPermissions((ArrayOf<String>**)&pkgInfoRequestedPermissions);

        AutoPtr< ArrayOf<Int32> > pkgInfoRequestedPermissionsFlags;
        pkgInfo->GetRequestedPermissionsFlags((ArrayOf<Int32>**)&pkgInfoRequestedPermissionsFlags);

        for (Int32 i = 0; i < requestedPermissions->GetLength(); ++i) {
            String requested_permission;
            for (Int32 idx=0; idx<mRequested_permissions->GetLength(); ++idx) {
                requested_permission = (*mRequested_permissions)[idx];
                if (requested_permission.Equals((*pkgInfoRequestedPermissions)[i])) {
                    (*pkgInfoRequestedPermissionsFlags)[i] |= IPackageInfo::REQUESTED_PERMISSION_GRANTED;
                }
            }
        }

        pkgInfo->SetRequestedPermissionsFlags(pkgInfoRequestedPermissionsFlags);
        return pkgInfo;
    //} catch (NameNotFoundException e) {
        //throw new RuntimeException(e); // will never occur
    //}
}

AutoPtr<IView> CGrantActivity::GetPermissionList(
    /* [in] */ IAppSecurityPermissions* perms)
{
    AutoPtr<IInterface> interfaceTmp;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&interfaceTmp);
    ILayoutInflater* inflater = ILayoutInflater::Probe(interfaceTmp);

    AutoPtr<IView> root;
    inflater->Inflate(R::layout::permissions_list, NULL, (IView**)&root);
    AutoPtr<IView> personalPermissions;
    perms->GetPermissionsView(IAppSecurityPermissions::WHICH_PERSONAL, (IView**)&personalPermissions);
    AutoPtr<IView> devicePermissions;
    perms->GetPermissionsView(IAppSecurityPermissions::WHICH_DEVICE, (IView**)&devicePermissions);

    AutoPtr<IView> viewTmp;
    root->FindViewById(R::id::privacylist, (IView**)&viewTmp);
    IViewGroup::Probe(viewTmp)->AddView(personalPermissions);

    viewTmp = NULL;
    root->FindViewById(R::id::devicelist, (IView**)&viewTmp);
    IViewGroup::Probe(viewTmp)->AddView(devicePermissions);
    return root;
}

AutoPtr< ArrayOf<String> > CGrantActivity::GetRequestedPermissions()
{
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr< ArrayOf<String> > permissions;
    intent->GetStringArrayExtra(IPackageManager::EXTRA_REQUEST_PERMISSION_PERMISSION_LIST, (ArrayOf<String>**)&permissions);
    if (permissions == NULL) {
        permissions = ArrayOf<String>::Alloc(0);
        return permissions;
    }

    AutoPtr< ArrayOf<String> > permissionsTmp = KeepNormalDangerousPermissions(permissions);
    AutoPtr< ArrayOf<String> > permissionsTmp1 = KeepRequestingPackagePermissions(permissionsTmp);
    return permissionsTmp1;
}

AutoPtr< ArrayOf<String> > CGrantActivity::KeepRequestingPackagePermissions(
    /* [in] */ ArrayOf<String>* permissions)
{
    List<String> result;
    //try {
        AutoPtr<IPackageInfo> pkgInfo;
        mPm->GetPackageInfo(mRequestingPackage, IPackageManager::GET_PERMISSIONS, (IPackageInfo**)&pkgInfo);
        AutoPtr< ArrayOf<String> > requestedPermissions;
        pkgInfo->GetRequestedPermissions((ArrayOf<String>**)&requestedPermissions);
        if (requestedPermissions == NULL) {
            requestedPermissions = ArrayOf<String>::Alloc(0);
            return requestedPermissions;
        }

        String permission;
        Boolean isRequired;
        Boolean isGranted;
        AutoPtr< ArrayOf<Int32> > requestedPermissionsFlags;
        pkgInfo->GetRequestedPermissionsFlags((ArrayOf<Int32>**)&requestedPermissionsFlags);
        for (Int32 i = 0; i < requestedPermissions->GetLength(); ++i) {
            permission = (*permissions)[i];
            isRequired = (((*requestedPermissionsFlags)[i] & IPackageInfo::REQUESTED_PERMISSION_REQUIRED) != 0);
            isGranted = (((*requestedPermissionsFlags)[i] & IPackageInfo::REQUESTED_PERMISSION_GRANTED) != 0);

            /*
             * We ignore required permissions, and permissions which have already
             * been granted, as it's useless to grant those permissions.
             */
            if (permission.Equals((*requestedPermissions)[i])
                    && !isRequired && !isGranted) {
                result.PushBack(permission);
                break;
            }
        }
    //} catch (NameNotFoundException e) {
        //throw new RuntimeException(e); // should never happen
    //}

    AutoPtr< ArrayOf<String> > ret = ArrayOf<String>::Alloc(result.GetSize());
    List<String>::Iterator it = result.Begin();
    for (Int32 idx = 0; it != result.End(); ++it, ++idx) {
        ret->Set(idx, *it);
    }
    return ret;
}

AutoPtr< ArrayOf<String> > CGrantActivity::KeepNormalDangerousPermissions(
    /* [in] */ ArrayOf<String>* permissions)
{
    List<String> result;
    String permission;
    for (Int32 i = 0; i < permissions->GetLength(); ++i) {
        permission = (*permissions)[i];
        //try {
            AutoPtr<IPermissionInfo> pInfo;
            mPm->GetPermissionInfo(permission, 0, (IPermissionInfo**)&pInfo);
            Int32 protectionLevel;
            pInfo->GetProtectionLevel(&protectionLevel);
            Int32 base = protectionLevel & IPermissionInfo::PROTECTION_MASK_BASE;
            if ((base != IPermissionInfo::PROTECTION_NORMAL)
                    && (base != IPermissionInfo::PROTECTION_DANGEROUS)) {
                continue;
            }

            result.PushBack(permission);
        //} catch (NameNotFoundException e) {
            // ignore
        //}
    }

    AutoPtr< ArrayOf<String> > ret = ArrayOf<String>::Alloc(result.GetSize());
    List<String>::Iterator it = result.Begin();
    for (Int32 idx = 0; it != result.End(); ++it, ++idx) {
        ret->Set(idx, *it);
    }
    return ret;
}

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos



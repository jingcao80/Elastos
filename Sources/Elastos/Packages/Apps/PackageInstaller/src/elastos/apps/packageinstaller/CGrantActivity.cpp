
#include "elastos/droid/R.h"
//#include "elastos/droid/widget/AppSecurityPermissions.h"
#include <Elastos.Droid.Graphics.h>
#include "elastos/apps/packageinstaller/CGrantActivity.h"
#include "elastos/apps/packageinstaller/PackageUtil.h"
#include "elastos/apps/packageinstaller/TabsAdapter.h"
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPermissionInfo;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
//using Elastos::Droid::Widget::AppSecurityPermissions;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Droid::Widget::ITabSpec;
using Elastos::Droid::Widget::ITextView;
using Elastos::Apps::PackageInstaller::EIID_IGrantActivity;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

//=====================================================================
//                            CGrantActivity
//=====================================================================
CAR_INTERFACE_IMPL_2(CGrantActivity, Object/*Activity*/, IViewOnClickListener, IGrantActivity)

CGrantActivity::CGrantActivity()
{
}

ECode CGrantActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    VALIDATE_NOT_NULL(icicle);
    // ==================before translated======================
    // super.onCreate(icicle);
    // mPm = getPackageManager();
    // mRequestingPackage = this.getCallingPackage();
    //
    // requested_permissions = getRequestedPermissions();
    // if (requested_permissions.length == 0) {
    //     // The grant request was empty. Return success
    //     setResult(RESULT_OK);
    //     finish();
    //     return;
    // }
    //
    // PackageInfo pkgInfo = getUpdatedPackageInfo();
    // AppSecurityPermissions perms = new AppSecurityPermissions(this, pkgInfo);
    // if (perms.getPermissionCount(AppSecurityPermissions.WHICH_NEW) == 0) {
    //     // The updated permissions dialog said there are no new permissions.
    //     // This should never occur if requested_permissions.length > 0,
    //     // but we check for it anyway, just in case.
    //     setResult(RESULT_OK);
    //     finish();
    //     return;
    // }
    //
    // setContentView(R.layout.install_start);
    // ((TextView)findViewById(R.id.install_confirm_question)).setText(R.string.grant_confirm_question);
    // PackageUtil.AppSnippet as = new PackageUtil.AppSnippet(mPm.getApplicationLabel(pkgInfo.applicationInfo),
    //         mPm.getApplicationIcon(pkgInfo.applicationInfo));
    // PackageUtil.initSnippetForNewApp(this, as, R.id.app_snippet);
    // mOk = (Button)findViewById(R.id.ok_button);
    // mOk.setText(R.string.ok);
    // mCancel = (Button)findViewById(R.id.cancel_button);
    // mOk.setOnClickListener(this);
    // mCancel.setOnClickListener(this);
    //
    // TabHost tabHost = (TabHost)findViewById(android.R.id.tabhost);
    // tabHost.setup();
    // ViewPager viewPager = (ViewPager) findViewById(R.id.pager);
    // TabsAdapter adapter = new TabsAdapter(this, tabHost, viewPager);
    //
    // View newTab = perms.getPermissionsView(AppSecurityPermissions.WHICH_NEW);
    // View allTab = getPermissionList(perms);
    //
    // adapter.addTab(tabHost.newTabSpec("new").setIndicator(
    //         getText(R.string.newPerms)), newTab);
    // adapter.addTab(tabHost.newTabSpec("all").setIndicator(
    //         getText(R.string.allPerms)), allTab);

    assert(0);
    //--: Activity::OnCreate(icicle);
    //--: GetPackageManager((IPackageManager**)&mPm);
    //--: GetCallingPackage(&mRequestingPackage);

    requested_permissions = GetRequestedPermissions();
    if (requested_permissions->GetLength() == 0) {
        // The grant request was empty. Return success
        //--: SetResult(RESULT_OK);
        //--: Finish();
        return NOERROR;
    }

    AutoPtr<IPackageInfo> pkgInfo = GetUpdatedPackageInfo();
    //--: AutoPtr<AppSecurityPermissions> perms = new AppSecurityPermissions(this, pkgInfo);
    /*if (perms->GetPermissionCount(AppSecurityPermissions::WHICH_NEW) == 0) {
        // The updated permissions dialog said there are no new permissions.
        // This should never occur if requested_permissions.length > 0,
        // but we check for it anyway, just in case.
        //--: SetResult(RESULT_OK);
        //--: Finish();
        return NOERROR;
    }*/

    //--: SetContentView(-1/*R::layout::install_start*/);
    AutoPtr<IView> viewTmp;
    //--: FindViewById(R::id::install_confirm_question, (IView**)&viewTmp);
    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(String("")/*R::string::grant_confirm_question*/, (ICharSequence**)&charSequenceTmp);
    ITextView::Probe(viewTmp)->SetText(charSequenceTmp);

    AutoPtr<IApplicationInfo> applicationInfo;
    pkgInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    charSequenceTmp = NULL;
    mPm->GetApplicationLabel(applicationInfo, (ICharSequence**)&charSequenceTmp);
    AutoPtr<IDrawable> drawableTmp;
    mPm->GetApplicationIcon(applicationInfo, (IDrawable**)&drawableTmp);
    AutoPtr<PackageUtil::AppSnippet> as = new PackageUtil::AppSnippet(charSequenceTmp, drawableTmp);
    PackageUtil::InitSnippetForNewApp(IActivity::Probe(this), as, -1/*R.id.app_snippet*/);

    viewTmp = NULL;
    //--: FindViewById(-1/*R::id::ok_button*/, (IView**)&viewTmp);
    mOk = IButton::Probe(viewTmp);

    charSequenceTmp = NULL;
    CString::New(String("")/*R::string::ok*/, (ICharSequence**)&charSequenceTmp);
    ITextView::Probe(mOk)->SetText(charSequenceTmp);

    viewTmp = NULL;
    //--: FindViewById(-1/*R::id::cancel_button*/, (IView**)&viewTmp);
    mCancel = IButton::Probe(viewTmp);
    IView::Probe(mOk)->SetOnClickListener(this);
    IView::Probe(mCancel)->SetOnClickListener(this);

    viewTmp = NULL;
    //--: FindViewById(-1/*R::id::tabhost*/, (IView**)&viewTmp);
    ITabHost* tabHost = ITabHost::Probe(viewTmp);
    tabHost->Setup();

    viewTmp = NULL;
    //--: FindViewById(-1/*R::id::pager*/, (IView**)&viewTmp);
    //--: ViewPager viewPager = (ViewPager)viewTmp;

    AutoPtr<TabsAdapter> adapter = new TabsAdapter(IActivity::Probe(this), tabHost, NULL/*viewPager*/);

    AutoPtr<IView> newTab;
    //--: perms->GetPermissionsView(AppSecurityPermissions::WHICH_NEW, (IView**)&newTab);
    AutoPtr<IView> allTab;//--: = GetPermissionList(perms);

    AutoPtr<ITabSpec> newTabSpec;
    tabHost->NewTabSpec(String("new"), (ITabSpec**)&newTabSpec);
    charSequenceTmp = NULL;
    IContext::Probe(this)->GetText(-1/*R::string::newPerms*/, (ICharSequence**)&charSequenceTmp);
    newTabSpec->SetIndicator(charSequenceTmp);
    //--: adapter->AddTab(newTabSpec, newTab);

    AutoPtr<ITabSpec> allTabSpec;
    tabHost->NewTabSpec(String("all"), (ITabSpec**)&allTabSpec);

    charSequenceTmp = NULL;
    IContext::Probe(this)->GetText(-1/*R::string::allPerms*/, (ICharSequence**)&charSequenceTmp);
    newTabSpec->SetIndicator(charSequenceTmp);
    //--: adapter->AddTab(allTabSpec, allTab);
    return NOERROR;
}

ECode CGrantActivity::OnClick(
    /* [in] */ IView* v)
{
    VALIDATE_NOT_NULL(v);
    // ==================before translated======================
    // if (v == mOk) {
    //     for (String permission : requested_permissions) {
    //         mPm.grantPermission(mRequestingPackage, permission);
    //     }
    //     setResult(RESULT_OK);
    // }
    // if (v == mCancel) {
    //     setResult(RESULT_CANCELED);
    // }
    // finish();

    assert(0);
    if (TO_IINTERFACE(v) == TO_IINTERFACE(mOk)) {
        String permission;
        for (Int32 idx=0; idx<requested_permissions->GetLength(); ++idx) {
            permission = (*requested_permissions)[idx];
            mPm->GrantPermission(mRequestingPackage, permission);
        }
        //--: SetResult(RESULT_OK);
    }
    if (TO_IINTERFACE(v) == TO_IINTERFACE(mCancel)) {
        //--: SetResult(RESULT_CANCELED);
    }
    //--: Finish();
    return NOERROR;
}

AutoPtr<IPackageInfo> CGrantActivity::GetUpdatedPackageInfo()
{
    // ==================before translated======================
    // try {
    //     PackageInfo pkgInfo = mPm.getPackageInfo(mRequestingPackage, PackageManager.GET_PERMISSIONS);
    //     for (int i = 0; i < pkgInfo.requestedPermissions.length; i++) {
    //         for (String requested_permission : requested_permissions) {
    //             if (requested_permission.equals(pkgInfo.requestedPermissions[i])) {
    //                 pkgInfo.requestedPermissionsFlags[i] |= PackageInfo.REQUESTED_PERMISSION_GRANTED;
    //             }
    //         }
    //     }
    //
    //     return pkgInfo;
    // } catch (NameNotFoundException e) {
    //     throw new RuntimeException(e); // will never occur
    // }

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
            for (Int32 idx=0; idx<requested_permissions->GetLength(); ++idx) {
                requested_permission = (*requested_permissions)[idx];
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
    // ==================before translated======================
    // LayoutInflater inflater = (LayoutInflater)getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    // View root = inflater.inflate(R.layout.permissions_list, null);
    // View personalPermissions = perms.getPermissionsView(AppSecurityPermissions.WHICH_PERSONAL);
    // View devicePermissions = perms.getPermissionsView(AppSecurityPermissions.WHICH_DEVICE);
    //
    // ((ViewGroup)root.findViewById(R.id.privacylist)).addView(personalPermissions);
    // ((ViewGroup)root.findViewById(R.id.devicelist)).addView(devicePermissions);
    //
    // return root;

    assert(0);
    AutoPtr<IInterface> interfaceTmp;
    //--: GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&interfaceTmp);
    ILayoutInflater* inflater = ILayoutInflater::Probe(interfaceTmp);

    AutoPtr<IView> root;
    inflater->Inflate(-1/*R::layout::permissions_list*/, NULL, (IView**)&root);
    AutoPtr<IView> personalPermissions;
    perms->GetPermissionsView(-1/*AppSecurityPermissions::WHICH_PERSONAL*/, (IView**)&personalPermissions);
    AutoPtr<IView> devicePermissions;
    perms->GetPermissionsView(-1/*AppSecurityPermissions::WHICH_DEVICE*/, (IView**)&devicePermissions);

    AutoPtr<IView> viewTmp;
    root->FindViewById(-1/*R::id::privacylist*/, (IView**)&viewTmp);
    IViewGroup::Probe(viewTmp)->AddView(personalPermissions);

    viewTmp = NULL;
    root->FindViewById(-1/*R::id::devicelist*/, (IView**)&viewTmp);
    IViewGroup::Probe(viewTmp)->AddView(devicePermissions);
    return root;
}

AutoPtr< ArrayOf<String> > CGrantActivity::GetRequestedPermissions()
{
    // ==================before translated======================
    // String[] permissions = getIntent()
    //         .getStringArrayExtra(PackageManager.EXTRA_REQUEST_PERMISSION_PERMISSION_LIST);
    // if (permissions == null) {
    //     return new String[0];
    // }
    // permissions = keepNormalDangerousPermissions(permissions);
    // permissions = keepRequestingPackagePermissions(permissions);
    // return permissions;

    assert(0);
    AutoPtr<IIntent> intent;
    //--: GetIntent((IIntent**)&intent);
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
    // ==================before translated======================
    // List<String> result = new ArrayList<String>();
    // try {
    //     PackageInfo pkgInfo = mPm.getPackageInfo(mRequestingPackage, PackageManager.GET_PERMISSIONS);
    //     if (pkgInfo.requestedPermissions == null) {
    //         return new String[0];
    //     }
    //     for (int i = 0; i < pkgInfo.requestedPermissions.length; i++) {
    //         for (String permission : permissions) {
    //             final boolean isRequired =
    //                     ((pkgInfo.requestedPermissionsFlags[i]
    //                             & PackageInfo.REQUESTED_PERMISSION_REQUIRED) != 0);
    //             final boolean isGranted =
    //                     ((pkgInfo.requestedPermissionsFlags[i]
    //                             & PackageInfo.REQUESTED_PERMISSION_GRANTED) != 0);
    //
    //             /*
    //              * We ignore required permissions, and permissions which have already
    //              * been granted, as it's useless to grant those permissions.
    //              */
    //             if (permission.equals(pkgInfo.requestedPermissions[i])
    //                     && !isRequired && !isGranted) {
    //                 result.add(permission);
    //                 break;
    //             }
    //         }
    //     }
    // } catch (NameNotFoundException e) {
    //     throw new RuntimeException(e); // should never happen
    // }
    // return result.toArray(new String[result.size()]);

    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);
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
                AutoPtr<ICharSequence> charSequenceTmp;
                CString::New(permission, (ICharSequence**)&charSequenceTmp);
                result->Add(TO_IINTERFACE(charSequenceTmp));
                break;
            }
        }
    //} catch (NameNotFoundException e) {
        //throw new RuntimeException(e); // should never happen
    //}

    Int32 size = 0;
    result->GetSize(&size);
    AutoPtr< ArrayOf<String> > ret = ArrayOf<String>::Alloc(size);
    String itemTmp;
    for (Int32 idx=0; idx<size; ++idx) {
        AutoPtr<IInterface> interfaceTmp;
        result->Get(idx, (IInterface**)&interfaceTmp);
        ICharSequence* charSequenceTmp = ICharSequence::Probe(interfaceTmp);
        charSequenceTmp->ToString(&itemTmp);
        ret->Set(idx, itemTmp);
    }
    return ret;
}

AutoPtr< ArrayOf<String> > CGrantActivity::KeepNormalDangerousPermissions(
    /* [in] */ ArrayOf<String>* permissions)
{
    // ==================before translated======================
    // List<String> result = new ArrayList<String>();
    // for (String permission : permissions) {
    //     try {
    //         PermissionInfo pInfo = mPm.getPermissionInfo(permission, 0);
    //         final int base = pInfo.protectionLevel & PermissionInfo.PROTECTION_MASK_BASE;
    //         if ((base != PermissionInfo.PROTECTION_NORMAL)
    //                 && (base != PermissionInfo.PROTECTION_DANGEROUS)) {
    //             continue;
    //         }
    //         result.add(permission);
    //     } catch (NameNotFoundException e) {
    //         // ignore
    //     }
    // }
    // return result.toArray(new String[result.size()]);

    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);
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

            AutoPtr<ICharSequence> charSequenceTmp;
            CString::New(permission, (ICharSequence**)&charSequenceTmp);
            result->Add(TO_IINTERFACE(charSequenceTmp));
        //} catch (NameNotFoundException e) {
            // ignore
        //}
    }

    Int32 size = 0;
    result->GetSize(&size);
    AutoPtr< ArrayOf<String> > ret = ArrayOf<String>::Alloc(size);
    String itemTmp;
    for (Int32 idx=0; idx<size; ++idx) {
        AutoPtr<IInterface> interfaceTmp;
        result->Get(idx, (IInterface**)&interfaceTmp);
        ICharSequence* charSequenceTmp = ICharSequence::Probe(interfaceTmp);
        charSequenceTmp->ToString(&itemTmp);
        ret->Set(idx, itemTmp);
    }
    return ret;
}

} // namespace PackageInstaller
} // namespace Apps
} // namespace Elastos




#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.Widget.h>
#include "elastos/apps/packageinstaller/PackageUtil.h"
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::CAssetManager;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CString;
using Elastos::Utility::IList;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

//=====================================================================
//                       PackageUtil::AppSnippet
//=====================================================================
PackageUtil::AppSnippet::AppSnippet(
    /* [in] */ ICharSequence* label,
    /* [in] */ IDrawable* icon)
    : mLabel(label)
    , mIcon(icon)
{
    // ==================before translated======================
    // this.label = label;
    // this.icon = icon;
}

//=====================================================================
//                             PackageUtil
//=====================================================================
const String PackageUtil::PREFIX("com.android.packageinstaller.");
const String PackageUtil::INTENT_ATTR_INSTALL_STATUS(PREFIX+"installStatus");
const String PackageUtil::INTENT_ATTR_APPLICATION_INFO(PREFIX+"applicationInfo");
const String PackageUtil::INTENT_ATTR_PERMISSIONS_LIST(PREFIX+"PermissionsList");
const String PackageUtil::INTENT_ATTR_PACKAGE_NAME(PREFIX+"PackageName");

AutoPtr<IApplicationInfo> PackageUtil::GetApplicationInfo(
    /* [in] */ IFile* sourcePath)
{
    // ==================before translated======================
    // final PackageParser parser = new PackageParser();
    // try {
    //     PackageParser.Package pkg = parser.parseMonolithicPackage(sourcePath, 0);
    //     return pkg.applicationInfo;
    // } catch (PackageParserException e) {
    //     return null;
    // }

    //AutoPtr<PackageParser> parser = new PackageParser();
    //try {
        //AutoPtr<PackageParser::Package> pkg;
        //parser->ParseMonolithicPackage(sourcePath, 0, (PackageParser::Package**)&pkg);
        return NULL;//pkg->mApplicationInfo;
    //} catch (PackageParserException e) {
        //return null;
    //}
}

AutoPtr<IInterface/*PackageParser::Package*/> PackageUtil::GetPackageInfo(
    /* [in] */ IFile* sourceFile)
{
    // ==================before translated======================
    // final PackageParser parser = new PackageParser();
    // try {
    //     PackageParser.Package pkg = parser.parseMonolithicPackage(sourceFile, 0);
    //     parser.collectManifestDigest(pkg);
    //     return pkg;
    // } catch (PackageParserException e) {
    //     return null;
    // }

    //AutoPtr<PackageParser> parser = new PackageParser();
    //try {
        AutoPtr<IInterface/*PackageParser::Package*/> pkg;
        //parser->ParseMonolithicPackage(sourceFile, 0, (PackageParser::Package**)&pkg);
        //parser->CollectManifestDigest(pkg);
        return pkg;
    //} catch (PackageParserException e) {
        //return null;
    //}
}

AutoPtr<IView> PackageUtil::InitSnippet(
    /* [in] */ IView* snippetView,
    /* [in] */ ICharSequence* label,
    /* [in] */ IDrawable* icon)
{
    // ==================before translated======================
    // ((ImageView)snippetView.findViewById(R.id.app_icon)).setImageDrawable(icon);
    // ((TextView)snippetView.findViewById(R.id.app_name)).setText(label);
    // return snippetView;

    AutoPtr<IView> viewTmp;
    snippetView->FindViewById(-1/*R::id::app_icon*/, (IView**)&viewTmp);
    IImageView::Probe(viewTmp)->SetImageDrawable(icon);

    viewTmp = NULL;
    snippetView->FindViewById(-1/*R::id::app_name*/, (IView**)&viewTmp);
    ITextView::Probe(viewTmp)->SetText(label);
    return snippetView;
}

AutoPtr<IView> PackageUtil::InitSnippetForInstalledApp(
    /* [in] */ IActivity* pContext,
    /* [in] */ IApplicationInfo* appInfo,
    /* [in] */ IView* snippetView)
{
    // ==================before translated======================
    // return initSnippetForInstalledApp(pContext, appInfo, snippetView, null);

    return InitSnippetForInstalledApp(pContext, appInfo, snippetView, NULL);
}

AutoPtr<IView> PackageUtil::InitSnippetForInstalledApp(
    /* [in] */ IActivity* pContext,
    /* [in] */ IApplicationInfo* appInfo,
    /* [in] */ IView* snippetView,
    /* [in] */ IUserHandle* user)
{
    // ==================before translated======================
    // final PackageManager pm = pContext.getPackageManager();
    // Drawable icon = appInfo.loadIcon(pm);
    // if (user != null) {
    //     icon = pContext.getPackageManager().getUserBadgedIcon(icon, user);
    // }
    // return initSnippet(
    //         snippetView,
    //         appInfo.loadLabel(pm),
    //         icon);

    AutoPtr<IPackageManager> pm;
    IContext::Probe(pContext)->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IDrawable> icon;
    IPackageItemInfo::Probe(appInfo)->LoadIcon(pm, (IDrawable**)&icon);
    if (TO_IINTERFACE(user) != TO_IINTERFACE(icon)) {
        AutoPtr<IPackageManager> packageManager;
        IContext::Probe(pContext)->GetPackageManager((IPackageManager**)&packageManager);
        packageManager->GetUserBadgedIcon(icon, user, (IDrawable**)&icon);
    }
    AutoPtr<ICharSequence> charSequenceTmp;
    IPackageItemInfo::Probe(appInfo)->LoadLabel(pm, (ICharSequence**)&charSequenceTmp);
    return InitSnippet(snippetView, charSequenceTmp, icon);
}

AutoPtr<IView> PackageUtil::InitSnippetForNewApp(
    /* [in] */ IActivity* pContext,
    /* [in] */ AppSnippet* as,
    /* [in] */ Int32 snippetId)
{
    // ==================before translated======================
    // View appSnippet = pContext.findViewById(snippetId);
    // ((ImageView)appSnippet.findViewById(R.id.app_icon)).setImageDrawable(as.icon);
    // ((TextView)appSnippet.findViewById(R.id.app_name)).setText(as.label);
    // return appSnippet;

    AutoPtr<IView> appSnippet;
    pContext->FindViewById(snippetId, (IView**)&appSnippet);

    AutoPtr<IView> viewTmp;
    appSnippet->FindViewById(-1/*R::id::app_icon*/, (IView**)&viewTmp);
    IImageView::Probe(viewTmp)->SetImageDrawable(as->mIcon);

    viewTmp = NULL;
    appSnippet->FindViewById(-1/*R::id::app_name*/, (IView**)&viewTmp);
    ITextView::Probe(viewTmp)->SetText(as->mLabel);
    return appSnippet;
}

Boolean PackageUtil::IsPackageAlreadyInstalled(
    /* [in] */ IActivity* context,
    /* [in] */ const String& pkgName)
{
    // ==================before translated======================
    // List<PackageInfo> installedList = context.getPackageManager().getInstalledPackages(
    //         PackageManager.GET_UNINSTALLED_PACKAGES);
    // int installedListSize = installedList.size();
    // for(int i = 0; i < installedListSize; i++) {
    //     PackageInfo tmp = installedList.get(i);
    //     if(pkgName.equalsIgnoreCase(tmp.packageName)) {
    //         return true;
    //     }
    // }
    // return false;

    AutoPtr<IPackageManager> packageManager;
    IContext::Probe(context)->GetPackageManager((IPackageManager**)&packageManager);

    AutoPtr<IList> installedList;
    packageManager->GetInstalledPackages(IPackageManager::GET_UNINSTALLED_PACKAGES, (IList**)&installedList);
    Int32 installedListSize = 0;
    installedList->GetSize(&installedListSize);
    for (Int32 i = 0; i < installedListSize; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        installedList->Get(i, (IInterface**)&interfaceTmp);

        IPackageInfo* tmp = IPackageInfo::Probe(interfaceTmp);
        String packageName;
        tmp->GetPackageName(&packageName);
        if(pkgName.EqualsIgnoreCase(packageName)) {
            return TRUE;
        }
    }
    return FALSE;
}

AutoPtr<PackageUtil::AppSnippet> PackageUtil::GetAppSnippet(
    /* [in] */ IActivity* pContext,
    /* [in] */ IApplicationInfo* appInfo,
    /* [in] */ IFile* sourceFile)
{
    // ==================before translated======================
    // final String archiveFilePath = sourceFile.getAbsolutePath();
    // Resources pRes = pContext.getResources();
    // AssetManager assmgr = new AssetManager();
    // assmgr.addAssetPath(archiveFilePath);
    // Resources res = new Resources(assmgr, pRes.getDisplayMetrics(), pRes.getConfiguration());
    // CharSequence label = null;
    // // Try to load the label from the package's resources. If an app has not explicitly
    // // specified any label, just use the package name.
    // if (appInfo.labelRes != 0) {
    //     try {
    //         label = res.getText(appInfo.labelRes);
    //     } catch (Resources.NotFoundException e) {
    //     }
    // }
    // if (label == null) {
    //     label = (appInfo.nonLocalizedLabel != null) ?
    //             appInfo.nonLocalizedLabel : appInfo.packageName;
    // }
    // Drawable icon = null;
    // // Try to load the icon from the package's resources. If an app has not explicitly
    // // specified any resource, just use the default icon for now.
    // if (appInfo.icon != 0) {
    //     try {
    //         icon = res.getDrawable(appInfo.icon);
    //     } catch (Resources.NotFoundException e) {
    //     }
    // }
    // if (icon == null) {
    //     icon = pContext.getPackageManager().getDefaultActivityIcon();
    // }
    // return new PackageUtil.AppSnippet(label, icon);

    String archiveFilePath;
    sourceFile->GetAbsolutePath(&archiveFilePath);

    AutoPtr<IResources> pRes;
    IContext::Probe(pContext)->GetResources((IResources**)&pRes);

    AutoPtr<IAssetManager> assmgr;
    CAssetManager::New((IAssetManager**)&assmgr);
    Int32 cookieTmp;
    assmgr->AddAssetPath(archiveFilePath, &cookieTmp);

    AutoPtr<IDisplayMetrics> displayMetrics;
    pRes->GetDisplayMetrics((IDisplayMetrics**)&displayMetrics);
    AutoPtr<IConfiguration> configuration;
    pRes->GetConfiguration((IConfiguration**)&configuration);
    AutoPtr<IResources> res;
    CResources::New(assmgr, displayMetrics, configuration, (IResources**)&res);

    AutoPtr<ICharSequence> label;
    // Try to load the label from the package's resources. If an app has not explicitly
    // specified any label, just use the package name.
    Int32 labelRes = 0;
    IPackageItemInfo::Probe(appInfo)->GetLabelRes(&labelRes);
    if (labelRes != 0) {
        //try {
            res->GetText(labelRes, (ICharSequence**)&label);
        //} catch (Resources.NotFoundException e) {
        //}
    }
    if (label == NULL) {
        AutoPtr<ICharSequence> charSequenceTmp;
        IPackageItemInfo::Probe(appInfo)->GetNonLocalizedLabel((ICharSequence**)&charSequenceTmp);

        String packageName;
        IPackageItemInfo::Probe(appInfo)->GetPackageName(&packageName);

        AutoPtr<ICharSequence> charSequenceTmp1;
        CString::New(packageName, (ICharSequence**)&charSequenceTmp1);
        label = (charSequenceTmp != NULL) ? charSequenceTmp : charSequenceTmp1;
    }
    AutoPtr<IDrawable> icon;
    // Try to load the icon from the package's resources. If an app has not explicitly
    // specified any resource, just use the default icon for now.
    Int32 iconTmp = 0;
    IPackageItemInfo::Probe(appInfo)->GetIcon(&iconTmp);
    if (iconTmp != 0) {
        //try {
            res->GetDrawable(iconTmp, (IDrawable**)&icon);
        //} catch (Resources.NotFoundException e) {
        //}
    }
    if (icon == NULL) {
        AutoPtr<IPackageManager> packageManager;
        IContext::Probe(pContext)->GetPackageManager((IPackageManager**)&packageManager);
        packageManager->GetDefaultActivityIcon((IDrawable**)&icon);
    }

    AutoPtr<AppSnippet> result = new PackageUtil::AppSnippet(label, icon);
    return result;
}

} // namespace PackageInstaller
} // namespace Apps
} // namespace Elastos



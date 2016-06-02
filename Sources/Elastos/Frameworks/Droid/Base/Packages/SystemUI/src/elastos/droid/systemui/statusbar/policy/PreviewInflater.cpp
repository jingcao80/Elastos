
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/systemui/statusbar/policy/PreviewInflater.h"
#include "elastos/droid/systemui/statusbar/phone/KeyguardPreviewContainer.h"
#include <elastos/droid/view/View.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::SystemUI::StatusBar::Phone::KeyguardPreviewContainer;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

PreviewInflater::WidgetInfo::WidgetInfo()
    : mLayoutId(0)
{}

const String PreviewInflater::TAG("PreviewInflater");
const String PreviewInflater::META_DATA_KEYGUARD_LAYOUT("com.android.keyguard.layout");

CAR_INTERFACE_IMPL(PreviewInflater, Object, IPreviewInflater)

PreviewInflater::PreviewInflater(
    /* [in] */ IContext* context,
    /* [in] */ ILockPatternUtils* lockPatternUtils)
{
    mContext = context;
    mLockPatternUtils = lockPatternUtils;
}

ECode PreviewInflater::InflatePreview(
    /* [in] */ IIntent* intent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<WidgetInfo> info = GetWidgetInfo(intent);
    if (info == NULL) {
        *view = NULL;
        return NOERROR;
    }
    AutoPtr<IView> v = InflateWidgetView(info);
    if (v == NULL) {
        *view = NULL;
        return NOERROR;
    }
    AutoPtr<KeyguardPreviewContainer> container = new KeyguardPreviewContainer();
    container->constructor(mContext, NULL);
    container->AddView(v);
    *view = (IView*)container;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

AutoPtr<IView> PreviewInflater::InflateWidgetView(
    /* [in] */ WidgetInfo* widgetInfo)
{
    AutoPtr<IView> widgetView;
    ECode ec = NOERROR;
    do {
        AutoPtr<IContext> appContext;
        ec = mContext->CreatePackageContext(
                widgetInfo->mContextPackage, IContext::CONTEXT_RESTRICTED, (IContext**)&appContext);
        if (FAILED(ec)) break;

        AutoPtr<IInterface> obj;
        ec = appContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
        if (FAILED(ec)) break;
        AutoPtr<ILayoutInflater> appInflater = ILayoutInflater::Probe(obj);

        ec = appInflater->CloneInContext(appContext, (ILayoutInflater**)&appInflater);
        if (FAILED(ec)) break;
        ec = appInflater->Inflate(widgetInfo->mLayoutId, NULL, FALSE, (IView**)&widgetView);
        // widgetView should hold the refcount of appContext
        ((Elastos::Droid::View::View*)widgetView.Get())->HoldContext();
    } while (/*PackageManager.NameNotFoundException|RuntimeException e*/0);

    if (FAILED(ec)) {
        Logger::W(TAG, "Error creating widget view");
    }
    return widgetView;
}

AutoPtr<PreviewInflater::WidgetInfo> PreviewInflater::GetWidgetInfo(
    /* [in] */ IIntent* intent)
{
    AutoPtr<WidgetInfo> info = new WidgetInfo();
    AutoPtr<IPackageManager> packageManager;
    mContext->GetPackageManager((IPackageManager**)&packageManager);

    Int32 user = 0;
    mLockPatternUtils->GetCurrentUser(&user);
    AutoPtr<IList> appList;  /*<ResolveInfo*/
    packageManager->QueryIntentActivitiesAsUser(intent,
            IPackageManager::MATCH_DEFAULT_ONLY, user, (IList**)&appList);

    Int32 size = 0;
    if ((appList->GetSize(&size), size) == 0) {
        return NULL;
    }
    AutoPtr<IResolveInfo> resolved;
    packageManager->ResolveActivityAsUser(intent,
            IPackageManager::MATCH_DEFAULT_ONLY | IPackageManager::GET_META_DATA,
            user, (IResolveInfo**)&resolved);
    if (WouldLaunchResolverActivity(resolved, appList)) {
        return NULL;
    }

    AutoPtr<IActivityInfo> activityInfo;
    resolved->GetActivityInfo((IActivityInfo**)&activityInfo);
    if (resolved == NULL || activityInfo == NULL) {
        return NULL;
    }

    AutoPtr<IBundle> metaData;
    IPackageItemInfo::Probe(activityInfo)->GetMetaData((IBundle**)&metaData);
    Boolean tmp = FALSE;
    if (metaData == NULL || (metaData->IsEmpty(&tmp), tmp)) {
        return NULL;
    }
    Int32 layoutId = 0;
    metaData->GetInt32(META_DATA_KEYGUARD_LAYOUT, &layoutId);
    if (layoutId == 0) {
        return NULL;
    }
    IPackageItemInfo::Probe(activityInfo)->GetPackageName(&info->mContextPackage);
    info->mLayoutId = layoutId;
    return info;
}

Boolean PreviewInflater::WouldLaunchResolverActivity(
    /* [in] */ IContext* ctx,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 currentUserId)
{
    AutoPtr<IPackageManager> packageManager;
    ctx->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IList> appList;  /*<ResolveInfo*/
    packageManager->QueryIntentActivitiesAsUser(
            intent, IPackageManager::MATCH_DEFAULT_ONLY, currentUserId, (IList**)&appList);

    Int32 size = 0;
    if ((appList->GetSize(&size), size) == 0) {
        return FALSE;
    }
    AutoPtr<IResolveInfo> resolved;
    packageManager->ResolveActivityAsUser(intent, IPackageManager::MATCH_DEFAULT_ONLY | IPackageManager::GET_META_DATA,
            currentUserId, (IResolveInfo**)&resolved);
    return WouldLaunchResolverActivity(resolved, appList);
}

Boolean PreviewInflater::WouldLaunchResolverActivity(
    /* [in] */ IResolveInfo* resolved,
    /* [in] */ IList/*<ResolveInfo>*/* appList)
{
    // If the list contains the above resolved activity, then it can't be
    // ResolverActivity itself.
    Int32 size = 0;
    appList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        appList->Get(i, (IInterface**)&obj);
        AutoPtr<IResolveInfo> tmp = IResolveInfo::Probe(obj);
        AutoPtr<IActivityInfo> activityInfo;
        tmp->GetActivityInfo((IActivityInfo**)&activityInfo);
        String name, pname;
        IPackageItemInfo::Probe(activityInfo)->GetName(&name);
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&pname);

        AutoPtr<IActivityInfo> activityInfo2;
        resolved->GetActivityInfo((IActivityInfo**)&activityInfo2);
        String name2, pname2;
        IPackageItemInfo::Probe(activityInfo2)->GetName(&name2);
        IPackageItemInfo::Probe(activityInfo2)->GetPackageName(&pname2);
        if (name.Equals(name2) && pname.Equals(pname2)) {
            return FALSE;
        }
    }
    return TRUE;
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

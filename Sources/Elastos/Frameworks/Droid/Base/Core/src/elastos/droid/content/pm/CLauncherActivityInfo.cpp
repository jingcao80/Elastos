
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/pm/CLauncherActivityInfo.h"
//#include "elastos/droid/content/pm/CLauncherApps.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

const String CLauncherActivityInfo::TAG("LauncherActivityInfo");

CAR_INTERFACE_IMPL(CLauncherActivityInfo, Object, ILauncherActivityInfo)

CAR_OBJECT_IMPL(CLauncherActivityInfo)

CLauncherActivityInfo::CLauncherActivityInfo()
    : mFirstInstallTime(0)
{
}

CLauncherActivityInfo::~CLauncherActivityInfo()
{
}

ECode CLauncherActivityInfo::constructor(
    /* [in] */ IContext* context)
{
    VALIDATE_NOT_NULL(context)
    context->GetPackageManager((IPackageManager**)&mPm);
    return NOERROR;
}

ECode CLauncherActivityInfo::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* info,
    /* [in] */ IUserHandle* user,
    /* [in] */ Int64 firstInstallTime)
{
    VALIDATE_NOT_NULL(context)
    context->GetPackageManager((IPackageManager**)&mPm);
    VALIDATE_NOT_NULL(info)
    info->GetActivityInfo((IActivityInfo**)&mActivityInfo);
    // CLauncherApps::GetComponentName(info, (IComponentName**)&mComponentName);
    mUser = user;
    mFirstInstallTime =firstInstallTime;

    return NOERROR;
}

ECode CLauncherActivityInfo::GetComponentName(
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name)
    *name = mComponentName;
    REFCOUNT_ADD(*name)
    return NOERROR;
}

ECode CLauncherActivityInfo::GetUser(
    /* [out] */ IUserHandle** user)
{
    VALIDATE_NOT_NULL(user)
    *user = mUser;
    REFCOUNT_ADD(*user)
    return NOERROR;
}

ECode CLauncherActivityInfo::GetLabel(
    /* [out] */ ICharSequence** seq)
{
    return IPackageItemInfo::Probe(mActivityInfo)->LoadLabel(mPm, seq);
}

ECode CLauncherActivityInfo::GetIcon(
    /* [in] */ Int32 density,
    /* [out] */ IDrawable** drawable)
{
    // TODO: Use density
    return IPackageItemInfo::Probe(mActivityInfo)->LoadIcon(mPm, drawable);
}

ECode CLauncherActivityInfo::GetApplicationFlags(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IApplicationInfo> ai;
    IComponentInfo::Probe(mActivityInfo)->GetApplicationInfo((IApplicationInfo**)&ai);
    return ai->GetFlags(result);
}

ECode CLauncherActivityInfo::GetApplicationInfo(
    /* [out] */ IApplicationInfo** info)
{
    return IComponentInfo::Probe(mActivityInfo)->GetApplicationInfo(info);
}

ECode CLauncherActivityInfo::GetFirstInstallTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = mFirstInstallTime;
    return NOERROR;
}

ECode CLauncherActivityInfo::GetName(
    /* [out] */ String* name)
{
    return IPackageItemInfo::Probe(mActivityInfo)->GetName(name);
}

ECode CLauncherActivityInfo::GetBadgedIcon(
    /* [in] */ Int32 density,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    *drawable = NULL;

    Int32 iconRes;
    IComponentInfo::Probe(mActivityInfo)->GetIconResource(&iconRes);
    AutoPtr<IResources> resources;
    AutoPtr<IDrawable> originalIcon;
    // try {
    AutoPtr<IApplicationInfo> ai;
    IComponentInfo::Probe(mActivityInfo)->GetApplicationInfo((IApplicationInfo**)&ai);
    mPm->GetResourcesForApplication(ai, (IResources**)&resources);
        // try {
    if (density != 0) {
        resources->GetDrawableForDensity(iconRes, density, (IDrawable**)&originalIcon);
    }
        // } catch (Resources.NotFoundException e) {
        // }
    // } catch (NameNotFoundException nnfe) {
    // }

    if (originalIcon == NULL) {
        IPackageItemInfo::Probe(mActivityInfo)->LoadIcon(mPm, (IDrawable**)&originalIcon);
    }

    if (IBitmapDrawable::Probe(originalIcon) != NULL) {
        return mPm->GetUserBadgedIcon(originalIcon, mUser, drawable);
    }
    else {
        Logger::E(TAG, "Unable to create badged icon for %s", Object::ToString(mActivityInfo).string());
    }

    *drawable = originalIcon;
    REFCOUNT_ADD(*drawable)
    return NOERROR;
}


}
}
}
}
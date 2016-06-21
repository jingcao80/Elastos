
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Utility.h>
#include "elastos/droid/appwidget/AppWidgetProviderInfo.h"
#include "elastos/droid/appwidget/CAppWidgetProviderInfo.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/core/StringBuilder.h"

using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace AppWidget {

CAR_INTERFACE_IMPL_2(AppWidgetProviderInfo, Object, IAppWidgetProviderInfo, IParcelable);

AppWidgetProviderInfo::AppWidgetProviderInfo()
    : mMinWidth(0)
    , mMinHeight(0)
    , mMinResizeWidth(0)
    , mMinResizeHeight(0)
    , mUpdatePeriodMillis(0)
    , mInitialLayout(0)
    , mInitialKeyguardLayout(0)
    , mIcon(0)
    , mAutoAdvanceViewId(0)
    , mPreviewImage(0)
    , mResizeMode(0)
    , mWidgetCategory(0)
{}

AppWidgetProviderInfo::~AppWidgetProviderInfo()
{}

ECode AppWidgetProviderInfo::constructor()
{
    return NOERROR;
}

ECode AppWidgetProviderInfo::LoadLabel(
    /* [in] */ IPackageManager* packageManager,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICharSequence> label;
    IPackageItemInfo::Probe(mProviderInfo)->LoadLabel(packageManager, (ICharSequence**)&label);
    if (label != NULL) {
        String str;
        label->ToString(&str);
        *result = str.Trim();
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

ECode AppWidgetProviderInfo::LoadIcon(
    /* [in] */ IContext* context,
    /* [in] */ Int32 density,
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 icon;
    IComponentInfo::Probe(mProviderInfo)->GetIconResource(&icon);
    AutoPtr<IDrawable> drawable = LoadDrawable(context, density, icon, TRUE);
    *result = drawable;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AppWidgetProviderInfo::LoadPreviewImage(
    /* [in] */ IContext* context,
    /* [in] */ Int32 density,
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IDrawable> drawable = LoadDrawable(context, density, mPreviewImage, FALSE);
    *result = drawable;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetProfile(
    /* [out] */ IUserHandle** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IApplicationInfo> applicationInfo;
    IComponentInfo::Probe(mProviderInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    Int32 uid;
    applicationInfo->GetUid(&uid);
    return CUserHandle::New(UserHandle::GetUserId(uid), result);
}

ECode AppWidgetProviderInfo::SetProvider(
    /* [in] */ IComponentName* name)
{
    mProvider = name;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetProvider(
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);
    *name = mProvider;
    REFCOUNT_ADD(*name);
    return NOERROR;
}

ECode AppWidgetProviderInfo::SetMinWidth(
    /* [in] */ Int32 width)
{
    mMinWidth = width;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetMinWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mMinWidth;
    return NOERROR;
}

ECode AppWidgetProviderInfo::SetMinHeight(
    /* [in] */ Int32 height)
{
    mMinHeight = height;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetMinHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mMinHeight;
    return NOERROR;
}

ECode AppWidgetProviderInfo::SetMinResizeWidth(
    /* [in] */ Int32 width)
{
    mMinResizeWidth = width;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetMinResizeWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mMinResizeWidth;
    return NOERROR;
}

ECode AppWidgetProviderInfo::SetMinResizeHeight(
    /* [in] */ Int32 height)
{
    mMinResizeHeight = height;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetMinResizeHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mMinResizeHeight;
    return NOERROR;
}

ECode AppWidgetProviderInfo::SetUpdatePeriodMillis(
    /* [in] */ Int32 period)
{
    mUpdatePeriodMillis = period;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetUpdatePeriodMillis(
    /* [out] */ Int32* period)
{
    VALIDATE_NOT_NULL(period);
    *period = mUpdatePeriodMillis;
    return NOERROR;
}

ECode AppWidgetProviderInfo::SetInitialLayout(
    /* [in] */ Int32 layout)
{
    mInitialLayout = layout;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetInitialLayout(
    /* [out] */ Int32* layout)
{
    VALIDATE_NOT_NULL(layout);
    *layout = mInitialLayout;
    return NOERROR;
}

ECode AppWidgetProviderInfo::SetInitialKeyguardLayout(
    /* [in] */ Int32 layout)
{
    mInitialKeyguardLayout = layout;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetInitialKeyguardLayout(
    /* [out] */ Int32* layout)
{
    VALIDATE_NOT_NULL(layout);
    *layout = mInitialKeyguardLayout;
    return NOERROR;
}

ECode AppWidgetProviderInfo::SetConfigure(
    /* [in] */ IComponentName* name)
{
    mConfigure = name;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetConfigure(
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);
    *name = mConfigure;
    REFCOUNT_ADD(*name);
    return NOERROR;
}

ECode AppWidgetProviderInfo::SetLabel(
    /* [in] */ const String& label)
{
    mLabel = label;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetLabel(
    /* [out] */ String* label)
{
    VALIDATE_NOT_NULL(label);
    *label = mLabel;
    return NOERROR;
}

ECode AppWidgetProviderInfo::SetIcon(
    /* [in] */ Int32 icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetIcon(
    /* [out] */ Int32* icon)
{
    VALIDATE_NOT_NULL(icon);
    *icon = mIcon;
    return NOERROR;
}

ECode AppWidgetProviderInfo::SetAutoAdvanceViewId(
    /* [in] */ Int32 id)
{
    mAutoAdvanceViewId = id;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetAutoAdvanceViewId(
    /* [out]*/ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mAutoAdvanceViewId;
    return NOERROR;
}

ECode AppWidgetProviderInfo::SetPreviewImage(
    /* [in] */ Int32 image)
{
    mPreviewImage = image;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetPreviewImage(
    /* [out]*/ Int32* image)
{
    VALIDATE_NOT_NULL(image);
    *image = mPreviewImage;
    return NOERROR;
}

ECode AppWidgetProviderInfo::SetResizeMode(
    /* [in] */ Int32 mode)
{
    mResizeMode = mode;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetResizeMode(
    /* [out]*/ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mResizeMode;
    return NOERROR;
}

ECode AppWidgetProviderInfo::SetWidgetCategory(
    /* [in] */ Int32 category)
{
    mWidgetCategory = category;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetWidgetCategory(
    /* [out]*/ Int32* category)
{
    VALIDATE_NOT_NULL(category);
    *category = mWidgetCategory;
    return NOERROR;
}

ECode AppWidgetProviderInfo::SetProviderInfo(
    /* [in] */ IActivityInfo* providerInfo)
{
    mProviderInfo = providerInfo;
    return NOERROR;
}

ECode AppWidgetProviderInfo::GetProviderInfo(
    /* [out] */ IActivityInfo** providerInfo)
{
    VALIDATE_NOT_NULL(providerInfo);
    *providerInfo = mProviderInfo;
    REFCOUNT_ADD(*providerInfo);
    return NOERROR;
}

ECode AppWidgetProviderInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInterfacePtr((Handle32*)&mProvider);
    source->ReadInt32(&mMinWidth);
    source->ReadInt32(&mMinHeight);
    source->ReadInt32(&mMinResizeWidth);
    source->ReadInt32(&mMinResizeHeight);
    source->ReadInt32(&mUpdatePeriodMillis);
    source->ReadInt32(&mInitialLayout);
    source->ReadInt32(&mInitialKeyguardLayout);
    source->ReadInterfacePtr((Handle32*)&mConfigure);
    source->ReadString(&mLabel);
    source->ReadInt32(&mIcon);
    source->ReadInt32(&mPreviewImage);
    source->ReadInt32(&mAutoAdvanceViewId);
    source->ReadInt32(&mResizeMode);
    source->ReadInt32(&mWidgetCategory);
    source->ReadInterfacePtr((Handle32*)&mProviderInfo);
    return NOERROR;
}

ECode AppWidgetProviderInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mProvider);
    dest->WriteInt32(mMinWidth);
    dest->WriteInt32(mMinHeight);
    dest->WriteInt32(mMinResizeWidth);
    dest->WriteInt32(mMinResizeHeight);
    dest->WriteInt32(mUpdatePeriodMillis);
    dest->WriteInt32(mInitialLayout);
    dest->WriteInt32(mInitialKeyguardLayout);
    dest->WriteInterfacePtr(mConfigure);
    dest->WriteString(mLabel);
    dest->WriteInt32(mIcon);
    dest->WriteInt32(mPreviewImage);
    dest->WriteInt32(mAutoAdvanceViewId);
    dest->WriteInt32(mResizeMode);
    dest->WriteInt32(mWidgetCategory);
    dest->WriteInterfacePtr(mProviderInfo);
    return NOERROR;
}

ECode AppWidgetProviderInfo::Clone(
    /* [out] */ IAppWidgetProviderInfo** info)
{
    VALIDATE_NOT_NULL(info);

    AutoPtr<IAppWidgetProviderInfo> that;
    CAppWidgetProviderInfo::New((IAppWidgetProviderInfo**)&that);
    if (mProvider != NULL) {
        AutoPtr<IInterface> obj;
        ICloneable::Probe(mProvider)->Clone((IInterface**)&obj);
        AutoPtr<IComponentName> provider = IComponentName::Probe(obj);
        that->SetProvider(provider);
    }
    that->SetMinWidth(mMinWidth);
    that->SetMinHeight(mMinHeight);
    that->SetMinResizeWidth(mMinResizeWidth);
    that->SetMinResizeHeight(mMinResizeHeight);
    that->SetUpdatePeriodMillis(mUpdatePeriodMillis);
    that->SetInitialLayout(mInitialLayout);
    that->SetInitialKeyguardLayout(mInitialKeyguardLayout);
    if (mConfigure != NULL) {
        AutoPtr<IInterface> obj;
        ICloneable::Probe(mConfigure)->Clone((IInterface**)&obj);
        AutoPtr<IComponentName> configure = IComponentName::Probe(obj);
        that->SetConfigure(configure);
    }
    if (mLabel.IsNull()) {
        that->SetLabel(String(NULL));
    }
    else {
        that->SetLabel(mLabel.Substring(0));
    }
    that->SetIcon(mIcon);
    that->SetPreviewImage(mPreviewImage);
    that->SetAutoAdvanceViewId(mAutoAdvanceViewId);
    that->SetResizeMode(mResizeMode);
    that->SetWidgetCategory(mWidgetCategory);
    that->SetProviderInfo(mProviderInfo);
    *info = that;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

AutoPtr<IDrawable> AppWidgetProviderInfo::LoadDrawable(
    /* [in] */ IContext* context,
    /* [in] */ Int32 density,
    /* [in] */ Int32 resourceId,
    /* [in] */ Boolean loadDefaultIcon)
{
    // try {
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IApplicationInfo> applicationInfo;
    IComponentInfo::Probe(mProviderInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    AutoPtr<IResources> resources;
    packageManager->GetResourcesForApplication(applicationInfo, (IResources**)&resources);

    AutoPtr<IDrawable> drawable;
    if (resources != NULL && resourceId > 0) {
        if (density <= 0) {
            AutoPtr<IResources> res;
            context->GetResources((IResources**)&res);
            AutoPtr<IDisplayMetrics> metrics;
            res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
            Int32 value;
            metrics->GetDensityDpi(&value);
            density = value;
        }
        resources->GetDrawableForDensity(resourceId, density, (IDrawable**)&drawable);
        return drawable;
    }
    // } catch (PackageManager.NameNotFoundException | Resources.NotFoundException e) {
    //     /* ignore */
    // }
    if (loadDefaultIcon) {
        IPackageItemInfo::Probe(mProviderInfo)->LoadIcon(packageManager, (IDrawable**)&drawable);
        return drawable;
    }
    else {
        return NULL;
    }
}

ECode AppWidgetProviderInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<IUserHandle> handle;
    GetProfile((IUserHandle**)&handle);

    StringBuilder builder;
    builder += "AppWidgetProviderInfo(";
    builder += handle;
    builder += '/';
    builder += mProvider ;
    builder += ')';
    *str = builder.ToString();
    return NOERROR;
}

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos

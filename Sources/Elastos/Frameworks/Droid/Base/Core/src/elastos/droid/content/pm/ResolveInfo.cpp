
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/ResolveInfo.h"
#include "elastos/droid/content/pm/CActivityInfo.h"
#include "elastos/droid/content/pm/CProviderInfo.h"
#include "elastos/droid/content/pm/CServiceInfo.h"
#include "elastos/droid/os/UserHandle.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::UserHandle;

using Elastos::Core::CString;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

const String ResolveInfo::TAG("ResolveInfo");

CAR_INTERFACE_IMPL_2(ResolveInfo, Object, IResolveInfo, IParcelable)

ResolveInfo::ResolveInfo()
    : mPriority(0)
    , mPreferredOrder(0)
    , mMatch(0)
    , mSpecificIndex(-1)
    , mIsDefault(FALSE)
    , mLabelRes(0)
    , mIcon(0)
    , mNoResourceId(0)
    , mSystem(FALSE)
{
    mTargetUserId = UserHandle::USER_CURRENT;
}

ResolveInfo::~ResolveInfo()
{
}

ECode ResolveInfo::constructor()
{
    return NOERROR;
}

ECode ResolveInfo::constructor(
    /* [in] */ IResolveInfo* other)
{
    VALIDATE_NOT_NULL(other);
    ResolveInfo* orig = (ResolveInfo*)other;

    mActivityInfo = orig->mActivityInfo;
    mServiceInfo = orig->mServiceInfo;
    mProviderInfo = orig->mProviderInfo;
    mFilter = orig->mFilter;
    mPriority = orig->mPriority;
    mPreferredOrder = orig->mPreferredOrder;
    mMatch = orig->mMatch;
    mSpecificIndex = orig->mSpecificIndex;
    mLabelRes = orig->mLabelRes;
    mNonLocalizedLabel = orig->mNonLocalizedLabel;
    mIcon = orig->mIcon;
    mResolvePackageName = orig->mResolvePackageName;
    mSystem = orig->mSystem;
    mTargetUserId = orig->mTargetUserId;

    return NOERROR;
}

ECode ResolveInfo::GetComponentInfo(
    /* [out] */ IComponentInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = NULL;

    if (mActivityInfo != NULL) *info = IComponentInfo::Probe(mActivityInfo);
    else if (mServiceInfo != NULL) *info = IComponentInfo::Probe(mServiceInfo);
    else if (mProviderInfo != NULL) *info = IComponentInfo::Probe(mProviderInfo);
    REFCOUNT_ADD(*info)

    if (*info == NULL) {
        // throw new IllegalStateException("Missing ComponentInfo!");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

ECode ResolveInfo::LoadLabel(
    /* [in] */ IPackageManager* pm,
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);
    *label = NULL;

    if (mNonLocalizedLabel != NULL) {
        *label = mNonLocalizedLabel;
        REFCOUNT_ADD(*label);
        return NOERROR;
    }

    if (!mResolvePackageName.IsNull() && mLabelRes != 0) {
        pm->GetText(mResolvePackageName, mLabelRes, NULL, label);
        if (*label != NULL) {
            String str;
            (*label)->ToString(&str);
            *label = NULL;
            return CString::New(str.Trim(), label);
        }
    }

    AutoPtr<IComponentInfo> ci;
    GetComponentInfo((IComponentInfo**)&ci);
    AutoPtr<IApplicationInfo> ai;
    ci->GetApplicationInfo((IApplicationInfo**)&ai);
    if (mLabelRes != 0) {
        String packageName;
        IPackageItemInfo::Probe(ci)->GetPackageName(&packageName);
        pm->GetText(packageName, mLabelRes, ai, label);
        if (*label != NULL) {
            String str = Object::ToString(*label);
            *label = NULL;
            return CString::New(str.Trim(), label);
        }
    }

    AutoPtr<ICharSequence> data;
    IPackageItemInfo::Probe(ci)->LoadLabel(pm, (ICharSequence**)&data);
    // Make the data safe
    if (data != NULL) {
        String str;
        data->ToString(&str);
        CString::New(str.Trim(), label);
    }

    return NOERROR;
}

ECode ResolveInfo::LoadIcon(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon);
    *icon = NULL;

    if (!mResolvePackageName.IsNull() && mIcon != 0) {
        pm->GetDrawable(mResolvePackageName, mIcon, NULL, icon);
        if (*icon != NULL) {
            return NOERROR;
        }
    }

    AutoPtr<IComponentInfo> ci;
    GetComponentInfo((IComponentInfo**)&ci);

    AutoPtr<IApplicationInfo> ai;
    ci->GetApplicationInfo((IApplicationInfo**)&ai);
    if (mIcon != 0) {
        String packageName;
        IPackageItemInfo::Probe(ci)->GetPackageName(&packageName);
        pm->GetDrawable(packageName, mIcon, ai, icon);
        if (*icon != NULL) {
            return NOERROR;
        }
    }
    return IPackageItemInfo::Probe(ci)->LoadIcon(pm, icon);
}

ECode ResolveInfo::GetIconResource(
    /* [out] */ Int32* iconRes)
{
    VALIDATE_NOT_NULL(iconRes);
    *iconRes = 0;

    if (mNoResourceId) return NOERROR;

    if (mIcon != 0) {
        *iconRes = mIcon;
        return NOERROR;
    }

    AutoPtr<IComponentInfo> ci;
    GetComponentInfo((IComponentInfo**)&ci);
    if (ci != NULL) {
        return ci->GetIconResource(iconRes);
    }
    return NOERROR;
}

ECode ResolveInfo::Dump(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    // if (filter != NULL) {
    //     pw.println(prefix + "Filter:");
    //     filter.dump(pw, prefix + "  ");
    // }
    // pw.println(prefix + "priority=" + priority
    //         + " preferredOrder=" + preferredOrder
    //         + " match=0x" + Integer.toHexString(match)
    //         + " specificIndex=" + specificIndex
    //         + " isDefault=" + isDefault);
    // if (resolvePackageName != NULL) {
    //     pw.println(prefix + "resolvePackageName=" + resolvePackageName);
    // }
    // if (labelRes != 0 || nonLocalizedLabel != NULL || icon != 0) {
    //     pw.println(prefix + "labelRes=0x" + Integer.toHexString(labelRes)
    //             + " nonLocalizedLabel=" + nonLocalizedLabel
    //             + " icon=0x" + Integer.toHexString(icon));
    // }
    // if (activityInfo != NULL) {
    //     pw.println(prefix + "ActivityInfo:");
    //     activityInfo.dump(pw, prefix + "  ");
    // } else if (serviceInfo != NULL) {
    //     pw.println(prefix + "ServiceInfo:");
    //     serviceInfo.dump(pw, prefix + "  ");
    // }
    // assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode ResolveInfo::ToString(
    /* [out] */ String* str)
{
    return NOERROR;

    // final ComponentInfo ci = getComponentInfo();
    // StringBuilder sb = new StringBuilder(128);
    // sb.append("ResolveInfo{");
    // sb.append(Integer.toHexString(System.identityHashCode(this)));
    // sb.append(' ');
    // ComponentName.appendShortString(sb, ci.packageName, ci.name);
    // if (priority != 0) {
    //     sb.append(" p=");
    //     sb.append(priority);
    // }
    // if (preferredOrder != 0) {
    //     sb.append(" o=");
    //     sb.append(preferredOrder);
    // }
    // sb.append(" m=0x");
    // sb.append(Integer.toHexString(match));
    // if (targetUserId != UserHandle.USER_CURRENT) {
    //     sb.append(" targetUserId=");
    //     sb.append(targetUserId);
    // }
    // sb.append('}');
    // return sb.toString();
}

ECode ResolveInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 ival;
    source->ReadInt32(&ival);
    if (ival == 1) {
        CActivityInfo::New((IActivityInfo**)&mActivityInfo);
        IParcelable::Probe(mActivityInfo)->ReadFromParcel(source);
    }
    else if (ival == 2) {
        CServiceInfo::New((IServiceInfo**)&mServiceInfo);
        IParcelable::Probe(mServiceInfo)->ReadFromParcel(source);
    }
    else if (ival == 3) {
        CProviderInfo::New((IProviderInfo**)&mProviderInfo);
        IParcelable::Probe(mProviderInfo)->ReadFromParcel(source);
    }

    source->ReadInterfacePtr((Handle32*)&mFilter);
    source->ReadInt32(&mPriority);
    source->ReadInt32(&mPreferredOrder);
    source->ReadInt32(&mMatch);
    source->ReadInt32(&mSpecificIndex);
    source->ReadBoolean(&mIsDefault);
    source->ReadInt32(&mLabelRes);
    source->ReadInterfacePtr((Handle32*)&mNonLocalizedLabel);
    source->ReadInt32(&mIcon);
    source->ReadString(&mResolvePackageName);
    source->ReadInt32(&mTargetUserId);
    source->ReadBoolean(&mSystem);
    source->ReadBoolean(&mNoResourceId);
    return NOERROR;
}

ECode ResolveInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (mActivityInfo != NULL) {
        dest->WriteInt32(1);
        IParcelable::Probe(mActivityInfo)->WriteToParcel(dest);
    }
    else if (mServiceInfo != NULL) {
        dest->WriteInt32(2);
        IParcelable::Probe(mServiceInfo)->WriteToParcel(dest);
    }
    else if (mProviderInfo != NULL) {
        dest->WriteInt32(3);
        IParcelable::Probe(mProviderInfo)->WriteToParcel(dest);
    }
    else {
        dest->WriteInt32(0);
    }

    dest->WriteInterfacePtr(mFilter);
    dest->WriteInt32(mPriority);
    dest->WriteInt32(mPreferredOrder);
    dest->WriteInt32(mMatch);
    dest->WriteInt32(mSpecificIndex);
    dest->WriteBoolean(mIsDefault);
    dest->WriteInt32(mLabelRes);
    dest->WriteInterfacePtr(mNonLocalizedLabel);
    dest->WriteInt32(mIcon);
    dest->WriteString(mResolvePackageName);
    dest->WriteInt32(mTargetUserId);
    dest->WriteBoolean(mSystem);
    dest->WriteBoolean(mNoResourceId);
    return NOERROR;
}

ECode ResolveInfo::GetActivityInfo(
    /* [out] */ IActivityInfo** activityInfo)
{
    VALIDATE_NOT_NULL(activityInfo);
    *activityInfo = mActivityInfo;
    REFCOUNT_ADD(*activityInfo);
    return NOERROR;
}

ECode ResolveInfo::SetActivityInfo(
    /* [in] */ IActivityInfo* activityInfo)
{
    mActivityInfo = activityInfo;
    return NOERROR;
}

ECode ResolveInfo::GetServiceInfo(
    /* [out] */ IServiceInfo** serviceInfo)
{
    VALIDATE_NOT_NULL(serviceInfo);
    *serviceInfo = mServiceInfo;
    REFCOUNT_ADD(*serviceInfo);
    return NOERROR;
}

ECode ResolveInfo::SetServiceInfo(
    /* [in] */ IServiceInfo* serviceInfo)
{
    mServiceInfo = serviceInfo;
    return NOERROR;
}

ECode ResolveInfo::GetProviderInfo(
    /* [out] */ IProviderInfo** providerInfo)
{
    VALIDATE_NOT_NULL(providerInfo)
    *providerInfo = mProviderInfo;
    REFCOUNT_ADD(*providerInfo)
    return NOERROR;
}

ECode ResolveInfo::SetProviderInfo(
    /* [in] */ IProviderInfo* providerInfo)
{
    mProviderInfo = providerInfo;
    return NOERROR;
}

ECode ResolveInfo::GetFilter(
    /* [out] */ IIntentFilter** filter)
{
    VALIDATE_NOT_NULL(filter);
    *filter = mFilter;
    REFCOUNT_ADD(*filter);
    return NOERROR;
}

ECode ResolveInfo::SetFilter(
    /* [in] */ IIntentFilter* filter)
{
    mFilter = filter;
    return NOERROR;
}

ECode ResolveInfo::GetPriority(
    /* [out] */ Int32* priority)
{
    VALIDATE_NOT_NULL(priority);
    *priority = mPriority;
    return NOERROR;
}

ECode ResolveInfo::SetPriority(
    /* [in] */ Int32 priority)
{
    mPriority = priority;
    return NOERROR;
}

ECode ResolveInfo::GetPreferredOrder(
    /* [out] */ Int32* preferredOrder)
{
    VALIDATE_NOT_NULL(preferredOrder);
    *preferredOrder = mPreferredOrder;
    return NOERROR;
}

ECode ResolveInfo::SetPreferredOrder(
    /* [in] */ Int32 preferredOrder)
{
    mPreferredOrder = preferredOrder;
    return NOERROR;
}

ECode ResolveInfo::GetMatch(
    /* [out] */ Int32* match)
{
    VALIDATE_NOT_NULL(match);
    *match = mMatch;
    return NOERROR;
}

ECode ResolveInfo::SetMatch(
    /* [in] */ Int32 match)
{
    mMatch = match;
    return NOERROR;
}

ECode ResolveInfo::GetSpecificIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mSpecificIndex;
    return NOERROR;
}

ECode ResolveInfo::SetSpecificIndex(
    /* [in] */ Int32 index)
{
    mSpecificIndex = index;
    return NOERROR;
}

ECode ResolveInfo::GetIsDefault(
    /* [out] */ Boolean* isDefault)
{
    VALIDATE_NOT_NULL(isDefault);
    *isDefault = mIsDefault;
    return NOERROR;
}

ECode ResolveInfo::SetIsDefault(
    /* [in] */ Boolean isDefault)
{
    mIsDefault = isDefault;
    return NOERROR;
}

ECode ResolveInfo::GetLabelRes(
    /* [out] */ Int32* labelRes)
{
    VALIDATE_NOT_NULL(labelRes);
    *labelRes = mLabelRes;
    return NOERROR;
}

ECode ResolveInfo::SetLabelRes(
    /* [in] */ Int32 labelRes)
{
    mLabelRes = labelRes;
    return NOERROR;
}

ECode ResolveInfo::GetNonLocalizedLabel(
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);
    *label = mNonLocalizedLabel;
    REFCOUNT_ADD(*label);
    return NOERROR;
}

ECode ResolveInfo::SetNonLocalizedLabel(
    /* [in] */ ICharSequence* label)
{
    mNonLocalizedLabel = label;
    return NOERROR;
}

ECode ResolveInfo::GetIcon(
    /* [out] */ Int32* icon)
{
    VALIDATE_NOT_NULL(icon);
    *icon = mIcon;
    return NOERROR;
}

ECode ResolveInfo::SetIcon(
    /* [in] */ Int32 icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode ResolveInfo::GetResolvePackageName(
    /* [out] */ String* resolvePackageName)
{
    VALIDATE_NOT_NULL(resolvePackageName);
    *resolvePackageName = mResolvePackageName;
    return NOERROR;
}

ECode ResolveInfo::SetResolvePackageName(
    /* [in] */ const String& resolvePackageName)
{
    mResolvePackageName = resolvePackageName;
    return NOERROR;
}

ECode ResolveInfo::GetTargetComponentName(
    /* [out] */ IComponentName** targetComponentName)
{
    VALIDATE_NOT_NULL(targetComponentName)
    *targetComponentName = mTargetComponentName;
    REFCOUNT_ADD(*targetComponentName)
    return NOERROR;
}

ECode ResolveInfo::SetTargetComponentName(
    /* [in] */ IComponentName* targetComponentName)
{
    mTargetComponentName = targetComponentName;
    return NOERROR;
}

ECode ResolveInfo::GetTargetUserId(
    /* [out] */ Int32* userId)
{
    VALIDATE_NOT_NULL(userId)
    *userId = mTargetUserId;
    return NOERROR;
}

ECode ResolveInfo::SetTargetUserId(
    /* [in] */ Int32 userId)
{
    mTargetUserId = userId;
    return NOERROR;
}

ECode ResolveInfo::GetNoResourceId(
    /* [out] */ Boolean* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mNoResourceId;
    return NOERROR;
}

ECode ResolveInfo::SetNoResourceId(
    /* [in] */ Boolean id)
{
    mNoResourceId = id;
    return NOERROR;
}

ECode ResolveInfo::GetSystem(
    /* [out] */ Boolean* sys)
{
    VALIDATE_NOT_NULL(sys)
    *sys = mSystem;
    return NOERROR;
}

ECode ResolveInfo::SetSystem(
    /* [in] */ Boolean sys)
{
    mSystem = sys;
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos


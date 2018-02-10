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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/ComponentInfo.h"

using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(ComponentInfo, PackageItemInfo, IComponentInfo)

ComponentInfo::ComponentInfo()
    : mDescriptionRes(0)
    , mEnabled(TRUE)
    , mExported(FALSE)
{}

ComponentInfo::~ComponentInfo()
{}

ECode ComponentInfo::constructor()
{
    return PackageItemInfo::constructor();
}

ECode ComponentInfo::constructor(
    /* [in] */ IComponentInfo* orig)
{
    VALIDATE_NOT_NULL(orig)

    PackageItemInfo::constructor(IPackageItemInfo::Probe(orig));
    orig->GetApplicationInfo((IApplicationInfo**)&mApplicationInfo);
    orig->GetProcessName(&mProcessName);
    orig->GetDescriptionRes(&mDescriptionRes);
    orig->GetEnabled(&mEnabled);
    orig->GetExported(&mExported);
    return NOERROR;
}

ECode ComponentInfo::LoadLabel(
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

    AutoPtr<IApplicationInfo> ai = mApplicationInfo;
    if (mLabelRes != 0) {
        pm->GetText(mPackageName, mLabelRes, ai, label);
        if (*label != NULL) {
             return NOERROR;
        }
    }
    AutoPtr<ICharSequence> nonLocalizedLabel;
    IPackageItemInfo::Probe(ai)->GetNonLocalizedLabel((ICharSequence**)&nonLocalizedLabel);
    if (nonLocalizedLabel != NULL) {
        *label = nonLocalizedLabel;
        REFCOUNT_ADD(*label);
        return NOERROR;
    }
    Int32 labelRes;
    IPackageItemInfo::Probe(ai)->GetLabelRes(&labelRes);
    if (labelRes != 0) {
        pm->GetText(mPackageName, labelRes, ai, label);
        if (*label != NULL) {
            return NOERROR;
       }
    }
    return CString::New(mName, label);
}

ECode ComponentInfo::IsEnabled(
    /* [out] */ Boolean* isEnable)
{
    VALIDATE_NOT_NULL(isEnable)

    Boolean enable = FALSE;
    mApplicationInfo->GetEnabled(&enable);
    *isEnable = mEnabled && enable;
    return NOERROR;
}

ECode ComponentInfo::GetIconResource(
    /* [out] */ Int32* icon)
{
    VALIDATE_NOT_NULL(icon)

    if(mIcon !=0) {
        *icon = mIcon;
    }
    else {
        IPackageItemInfo::Probe(mApplicationInfo)->GetIcon(icon);
    }
    return NOERROR;
}

ECode ComponentInfo::GetLogoResource(
    /* [out] */ Int32* icon)
{
    VALIDATE_NOT_NULL(icon)

    if(mLogo !=0) {
        *icon = mLogo;
    }
    else {
        IPackageItemInfo::Probe(mApplicationInfo)->GetLogo(icon);
    }
    return NOERROR;
}

ECode ComponentInfo::GetBannerResource(
    /* [out] */ Int32* icon)
{
    VALIDATE_NOT_NULL(icon)

    if(mBanner != 0) {
        *icon = mBanner;
    }
    else {
        IPackageItemInfo::Probe(mApplicationInfo)->GetBanner(icon);
    }
    return NOERROR;
}

ECode ComponentInfo::DumpFront(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    // super.dumpFront(pw, prefix);
    // pw.println(prefix + "enabled=" + enabled + " exported=" + exported
    //         + " processName=" + processName);
    // if (descriptionRes != 0) {
    //     pw.println(prefix + "description=" + descriptionRes);
    // }
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode ComponentInfo::DumpBack(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    // if (applicationInfo != null) {
    //     pw.println(prefix + "ApplicationInfo:");
    //     applicationInfo.dump(pw, prefix + "  ");
    // } else {
    //     pw.println(prefix + "ApplicationInfo: null");
    // }
    // super.dumpBack(pw, prefix);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode ComponentInfo::WriteToParcel(
        /* [in] */ IParcel* dest)
{
    assert(dest != NULL);

    PackageItemInfo::WriteToParcel(dest);
    dest->WriteInterfacePtr(mApplicationInfo);
    dest->WriteString(mProcessName);
    dest->WriteInt32(mDescriptionRes);
    dest->WriteBoolean(mEnabled);
    dest->WriteBoolean(mExported);
    return NOERROR;
}

ECode ComponentInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(source != NULL);

    PackageItemInfo::ReadFromParcel(source);
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mApplicationInfo = obj != NULL ? IApplicationInfo::Probe(obj) : NULL;
    source->ReadString(&mProcessName);
    source->ReadInt32(&mDescriptionRes);
    source->ReadBoolean(&mEnabled);
    source->ReadBoolean(&mExported);
    return NOERROR;
}

ECode ComponentInfo::LoadDefaultIcon(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** icon)
{
    return IPackageItemInfo::Probe(mApplicationInfo)->LoadIcon(pm, icon);
}

ECode ComponentInfo::LoadDefaultBanner(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** icon)
{
    return IPackageItemInfo::Probe(mApplicationInfo)->LoadBanner(pm, icon);
}

ECode ComponentInfo::LoadDefaultLogo(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** logo)
{
    return IPackageItemInfo::Probe(mApplicationInfo)->LoadLogo(pm, logo);
}

AutoPtr<IApplicationInfo> ComponentInfo::GetApplicationInfo()
{
    return mApplicationInfo;
}

ECode ComponentInfo::GetApplicationInfo(
    /* [out] */ IApplicationInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = mApplicationInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode ComponentInfo::SetApplicationInfo(
    /* [in] */ IApplicationInfo* info)
{
    mApplicationInfo = info;
    return NOERROR;
}

ECode ComponentInfo::GetProcessName(
    /* [out] */ String* processName)
{
    VALIDATE_NOT_NULL(processName)
    *processName = mProcessName;
    return NOERROR;
}

ECode ComponentInfo::SetProcessName(
    /* [in] */ const String& processName)
{
    mProcessName = processName;
    return NOERROR;
}

ECode ComponentInfo::GetDescriptionRes(
    /* [out] */ Int32* desRes)
{
    VALIDATE_NOT_NULL(desRes)
    *desRes = mDescriptionRes;
    return NOERROR;
}

ECode ComponentInfo::SetDescriptionRes(
    /* [in] */ Int32 desRes)
{
    mDescriptionRes = desRes;
    return NOERROR;
}

ECode ComponentInfo::GetEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    *enabled = mEnabled;
    return NOERROR;
}

ECode ComponentInfo::SetEnabled(
    /* [in] */ Boolean enabled)
{
    mEnabled = enabled;
    return NOERROR;
}

ECode ComponentInfo::GetExported(
    /* [out] */ Boolean* exported)
{
    VALIDATE_NOT_NULL(exported)
    *exported = mExported;
    return NOERROR;
}

ECode ComponentInfo::SetExported(
    /* [in] */ Boolean exported)
{
    mExported = exported;
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

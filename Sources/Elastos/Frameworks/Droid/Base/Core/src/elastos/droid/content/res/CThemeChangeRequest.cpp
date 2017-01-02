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
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/content/res/CThemeChangeRequest.h"

using Elastos::Droid::Provider::IThemesContractThemesColumns;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CThemeChangeRequest::CThemeChangeRequest()
{
    CHashMap::New((IMap**)&mThemeComponents);
    CHashMap::New((IMap**)&mPerAppOverlays);
}

CAR_INTERFACE_IMPL_2(CThemeChangeRequest, Object, IThemeChangeRequest, IParcelable)

CAR_OBJECT_IMPL(CThemeChangeRequest)

ECode CThemeChangeRequest::constructor()
{
    return NOERROR;
}

ECode CThemeChangeRequest::GetOverlayThemePackageName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = GetThemePackageNameForComponent(IThemesContractThemesColumns::MODIFIES_OVERLAYS);
    return NOERROR;
}

ECode CThemeChangeRequest::GetStatusBarThemePackageName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = GetThemePackageNameForComponent(IThemesContractThemesColumns::MODIFIES_STATUS_BAR);
    return NOERROR;
}

ECode CThemeChangeRequest::GetNavBarThemePackageName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = GetThemePackageNameForComponent(IThemesContractThemesColumns::MODIFIES_NAVIGATION_BAR);
    return NOERROR;
}

ECode CThemeChangeRequest::GetFontThemePackageName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = GetThemePackageNameForComponent(IThemesContractThemesColumns::MODIFIES_FONTS);
    return NOERROR;
}

ECode CThemeChangeRequest::GetIconsThemePackageName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = GetThemePackageNameForComponent(IThemesContractThemesColumns::MODIFIES_ICONS);
    return NOERROR;
}

ECode CThemeChangeRequest::GetBootanimationThemePackageName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = GetThemePackageNameForComponent(IThemesContractThemesColumns::MODIFIES_BOOT_ANIM);
    return NOERROR;
}

ECode CThemeChangeRequest::GetWallpaperThemePackageName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = GetThemePackageNameForComponent(IThemesContractThemesColumns::MODIFIES_LAUNCHER);
    return NOERROR;
}

ECode CThemeChangeRequest::GetLockWallpaperThemePackageName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = GetThemePackageNameForComponent(IThemesContractThemesColumns::MODIFIES_LOCKSCREEN);
    return NOERROR;
}

ECode CThemeChangeRequest::GetAlarmThemePackageName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = GetThemePackageNameForComponent(IThemesContractThemesColumns::MODIFIES_ALARMS);
    return NOERROR;
}

ECode CThemeChangeRequest::GetNotificationThemePackageName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = GetThemePackageNameForComponent(IThemesContractThemesColumns::MODIFIES_NOTIFICATIONS);
    return NOERROR;
}

ECode CThemeChangeRequest::GetRingtoneThemePackageName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = GetThemePackageNameForComponent(IThemesContractThemesColumns::MODIFIES_RINGTONES);
    return NOERROR;
}

ECode CThemeChangeRequest::GetThemeComponentsMap(
    /* [out] */ IMap** components)
{
    VALIDATE_NOT_NULL(components)
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    return collections->UnmodifiableMap(mThemeComponents, components);
}

ECode CThemeChangeRequest::SetThemeComponentsMap(
    /* [in] */ IMap* components)
{
    mThemeComponents = components;
    return NOERROR;
}

ECode CThemeChangeRequest::GetPerAppOverlays(
    /* [out] */ IMap** components)
{
    VALIDATE_NOT_NULL(components)
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    return collections->UnmodifiableMap(mPerAppOverlays, components);
}

ECode CThemeChangeRequest::SetPerAppOverlays(
    /* [in] */ IMap* components)
{
    mPerAppOverlays = components;
    return NOERROR;
}

ECode CThemeChangeRequest::GetNumChangesRequested(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    Int32 themeSize, overlaysSize;
    mThemeComponents->GetSize(&themeSize);
    mPerAppOverlays->GetSize(&overlaysSize);
    *num = themeSize + overlaysSize;
    return NOERROR;
}

ECode CThemeChangeRequest::GetReqeustType(
    /* [out] */ RequestType* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mRequestType;
    return NOERROR;
}

ECode CThemeChangeRequest::SetReqeustType(
    /* [in] */ RequestType type)
{
    mRequestType = type;
    return NOERROR;
}

String CThemeChangeRequest::GetThemePackageNameForComponent(
    /* [in] */ const String& componentName)
{
    AutoPtr<ICharSequence> cs;
    CString::New(componentName, (ICharSequence**)&cs);
    AutoPtr<IInterface> value;
    mThemeComponents->Get(cs, (IInterface**)&value);
    String str;
    ICharSequence::Probe(value)->ToString(&str);
    return str;
}

ECode CThemeChangeRequest::constructor(
    /* [in] */ IMap* components,
    /* [in] */ IMap* perAppThemes,
    /* [in] */ RequestType requestType)
{
    if (components != NULL) {
        mThemeComponents->PutAll(components);
    }
    if (perAppThemes != NULL) {
        mPerAppOverlays->PutAll(perAppThemes);
    }
    mRequestType = requestType;
    return NOERROR;
}

ECode CThemeChangeRequest::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Int32 themeSize;
    mThemeComponents->GetSize(&themeSize);
    dest->WriteInt32(themeSize);
    AutoPtr<ISet> keySet;
    mThemeComponents->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        dest->WriteInterfacePtr(next);
        AutoPtr<IInterface> value;
        mThemeComponents->Get(next, (IInterface**)&value);
        dest->WriteInterfacePtr(value);
    }
    Int32 overlaysSize;
    mPerAppOverlays->GetSize(&overlaysSize);
    dest->WriteInt32(overlaysSize);
    AutoPtr<ISet> keySet1;
    mPerAppOverlays->GetKeySet((ISet**)&keySet1);
    AutoPtr<IIterator> it1;
    keySet1->GetIterator((IIterator**)&it1);
    while (it1->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it1->GetNext((IInterface**)&next);
        dest->WriteInterfacePtr(next);
        AutoPtr<IInterface> value;
        mPerAppOverlays->Get(next, (IInterface**)&value);
        dest->WriteInterfacePtr(value);
    }
    dest->WriteInt32(mRequestType);
    return NOERROR;
}

ECode CThemeChangeRequest::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 numComponents;
    source->ReadInt32(&numComponents);
    for (Int32 i = 0; i < numComponents; i++) {
        AutoPtr<IInterface> ikey, ivalue;
        source->ReadInterfacePtr((Handle32*)(IInterface**)&ikey);
        source->ReadInterfacePtr((Handle32*)(IInterface**)&ivalue);
        mThemeComponents->Put(ikey, ivalue);
    }

    source->ReadInt32(&numComponents);
    for (Int32 i = 0 ; i < numComponents; i++) {
        AutoPtr<IInterface> ikey, ivalue;
        source->ReadInterfacePtr((Handle32*)(IInterface**)&ikey);
        source->ReadInterfacePtr((Handle32*)(IInterface**)&ivalue);
        mPerAppOverlays->Put(ikey, ivalue);
    }
    source->ReadInt32(&mRequestType);
    return NOERROR;
}

}
}
}
}

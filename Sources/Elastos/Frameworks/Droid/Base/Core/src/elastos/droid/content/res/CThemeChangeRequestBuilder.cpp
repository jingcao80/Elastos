
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/content/res/CThemeChangeRequestBuilder.h"
#include "elastos/droid/content/res/CThemeChangeRequest.h"
#include "elastos/droid/content/pm/ThemeUtils.h"
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Content::Pm::ThemeUtils;
using Elastos::Droid::Provider::IThemesContractThemesColumns;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CThemeChangeRequestBuilder::CThemeChangeRequestBuilder()
{
    CHashMap::New((IMap**)&mThemeComponents);
    CHashMap::New((IMap**)&mPerAppOverlays);
    mRequestType = RequestType_USER_REQUEST;
}

CAR_INTERFACE_IMPL(CThemeChangeRequestBuilder, Object, IThemeChangeRequestBuilder)

CAR_OBJECT_IMPL(CThemeChangeRequestBuilder)

ECode CThemeChangeRequestBuilder::constructor()
{
    return NOERROR;
}

ECode CThemeChangeRequestBuilder::constructor(
    /* [in] */ IThemeConfig* themeConfig)
{
    if (themeConfig != NULL) {
        BuildChangeRequestFromThemeConfig(themeConfig);
    }
    return NOERROR;
}

ECode CThemeChangeRequestBuilder::SetOverlay(
    /* [in] */ const String& pkgName,
    /* [out] */ IThemeChangeRequestBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    return SetComponent(IThemesContractThemesColumns::MODIFIES_OVERLAYS, pkgName, builder);
}

ECode CThemeChangeRequestBuilder::SetStatusBar(
    /* [in] */ const String& pkgName,
    /* [out] */ IThemeChangeRequestBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    return SetComponent(IThemesContractThemesColumns::MODIFIES_STATUS_BAR, pkgName, builder);
}

ECode CThemeChangeRequestBuilder::SetNavBar(
    /* [in] */ const String& pkgName,
    /* [out] */ IThemeChangeRequestBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    return SetComponent(IThemesContractThemesColumns::MODIFIES_NAVIGATION_BAR, pkgName, builder);
}

ECode CThemeChangeRequestBuilder::SetFont(
    /* [in] */ const String& pkgName,
    /* [out] */ IThemeChangeRequestBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    return SetComponent(IThemesContractThemesColumns::MODIFIES_FONTS, pkgName, builder);
}

ECode CThemeChangeRequestBuilder::SetIcons(
    /* [in] */ const String& pkgName,
    /* [out] */ IThemeChangeRequestBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    return SetComponent(IThemesContractThemesColumns::MODIFIES_ICONS, pkgName, builder);
}

ECode CThemeChangeRequestBuilder::SetBootanimation(
    /* [in] */ const String& pkgName,
    /* [out] */ IThemeChangeRequestBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    return SetComponent(IThemesContractThemesColumns::MODIFIES_BOOT_ANIM, pkgName, builder);
}

ECode CThemeChangeRequestBuilder::SetWallpaper(
    /* [in] */ const String& pkgName,
    /* [out] */ IThemeChangeRequestBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    return SetComponent(IThemesContractThemesColumns::MODIFIES_LAUNCHER, pkgName, builder);
}

ECode CThemeChangeRequestBuilder::SetLockWallpaper(
    /* [in] */ const String& pkgName,
    /* [out] */ IThemeChangeRequestBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    return SetComponent(IThemesContractThemesColumns::MODIFIES_LOCKSCREEN, pkgName, builder);
}

ECode CThemeChangeRequestBuilder::SetAlarm(
    /* [in] */ const String& pkgName,
    /* [out] */ IThemeChangeRequestBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    return SetComponent(IThemesContractThemesColumns::MODIFIES_ALARMS, pkgName, builder);
}

ECode CThemeChangeRequestBuilder::SetNotification(
    /* [in] */ const String& pkgName,
    /* [out] */ IThemeChangeRequestBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    return SetComponent(IThemesContractThemesColumns::MODIFIES_NOTIFICATIONS, pkgName, builder);
}

ECode CThemeChangeRequestBuilder::SetRingtone(
    /* [in] */ const String& pkgName,
    /* [out] */ IThemeChangeRequestBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    return SetComponent(IThemesContractThemesColumns::MODIFIES_RINGTONES, pkgName, builder);
}

ECode CThemeChangeRequestBuilder::SetComponent(
    /* [in] */ const String& component,
    /* [in] */ const String& pkgName,
    /* [out] */ IThemeChangeRequestBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    if (!pkgName.IsNull()) {
        AutoPtr<ICharSequence> key, value;
        CString::New(component, (ICharSequence**)&key);
        CString::New(pkgName, (ICharSequence**)&value);
        mThemeComponents->Put(key, value);
    }
    else {
        AutoPtr<ICharSequence> key;
        CString::New(component, (ICharSequence**)&key);
        mThemeComponents->Remove(key);
    }
    *builder = (IThemeChangeRequestBuilder*)this;
    REFCOUNT_ADD(*builder)
    return NOERROR;
}

ECode CThemeChangeRequestBuilder::SetAppOverlay(
    /* [in] */ const String& appPkgName,
    /* [in] */ const String& themePkgName,
    /* [out] */ IThemeChangeRequestBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    if (!appPkgName.IsNull()) {
        if (!themePkgName.IsNull()) {
            AutoPtr<ICharSequence> key, value;
            CString::New(appPkgName, (ICharSequence**)&key);
            CString::New(themePkgName, (ICharSequence**)&value);
            mPerAppOverlays->Put(key, value);
        }
        else {
            AutoPtr<ICharSequence> key;
            CString::New(appPkgName, (ICharSequence**)&key);
            mPerAppOverlays->Remove(key);
        }
    }
    *builder = (IThemeChangeRequestBuilder*)this;
    REFCOUNT_ADD(*builder)
    return NOERROR;
}

ECode CThemeChangeRequestBuilder::SetRequestType(
    /* [in] */ RequestType requestType,
    /* [out] */ IThemeChangeRequestBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    // TODO
    mRequestType = requestType;// requestType != null ? requestType : RequestType.USER_REQUEST;
    *builder = (IThemeChangeRequestBuilder*)this;
    REFCOUNT_ADD(*builder)
    return NOERROR;
}

ECode CThemeChangeRequestBuilder::Build(
    /* [out] */ IThemeChangeRequest** request)
{
    VALIDATE_NOT_NULL(request)
    AutoPtr<CThemeChangeRequest> req;
    CThemeChangeRequest::NewByFriend((CThemeChangeRequest**)&req);
    req->constructor(mThemeComponents, mPerAppOverlays, mRequestType);
    *request = (IThemeChangeRequest*)req.Get();
    REFCOUNT_ADD(*request)
    return NOERROR;
}

void CThemeChangeRequestBuilder::BuildChangeRequestFromThemeConfig(
    /* [in] */ IThemeConfig* themeConfig)
{
    String pkgName;
    if (themeConfig->GetFontPkgName(&pkgName), !pkgName.IsNull()) {
        AutoPtr<IThemeChangeRequestBuilder> b;
        SetFont(pkgName, (IThemeChangeRequestBuilder**)&b);
    }
    if (themeConfig->GetIconPackPkgName(&pkgName), !pkgName.IsNull()) {
        AutoPtr<IThemeChangeRequestBuilder> b;
        SetIcons(pkgName, (IThemeChangeRequestBuilder**)&b);
    }
    if (themeConfig->GetOverlayPkgName(&pkgName), !pkgName.IsNull()) {
        AutoPtr<IThemeChangeRequestBuilder> b;
        SetOverlay(pkgName, (IThemeChangeRequestBuilder**)&b);
    }
    if (themeConfig->GetOverlayForStatusBar(&pkgName), !pkgName.IsNull()) {
        AutoPtr<IThemeChangeRequestBuilder> b;
        SetStatusBar(pkgName, (IThemeChangeRequestBuilder**)&b);
    }
    if (themeConfig->GetOverlayForNavBar(&pkgName), !pkgName.IsNull()) {
        AutoPtr<IThemeChangeRequestBuilder> b;
        SetNavBar(pkgName, (IThemeChangeRequestBuilder**)&b);
    }

    // Check if there are any per-app overlays using this theme
    AutoPtr<IMap> themes;
    themeConfig->GetAppThemes((IMap**)&themes);
    AutoPtr<ISet> keySet;
    themes->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        String appPkgName;
        ICharSequence::Probe(next)->ToString(&appPkgName);
        if (ThemeUtils::IsPerAppThemeComponent(appPkgName)) {
            AutoPtr<IInterface> value;
            themes->Get(next, (IInterface**)&value);
            String overlayPkgName;
            IAppTheme::Probe(value)->GetOverlayPkgName(&overlayPkgName);
            AutoPtr<IThemeChangeRequestBuilder> b;
            SetAppOverlay(appPkgName, overlayPkgName, (IThemeChangeRequestBuilder**)&b);
        }
    }
}

}
}
}
}


#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/res/CThemeConfigBuilder.h"
#include "elastos/droid/content/res/CThemeConfig.h"
#include "elastos/droid/content/res/CAppTheme.h"
#include "elastos/droid/content/res/ThemeConfig.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::IMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CThemeConfigBuilder, Object, IThemeConfigBuilder)

CAR_OBJECT_IMPL(CThemeConfigBuilder)

ECode CThemeConfigBuilder::constructor()
{
    return NOERROR;
}

ECode CThemeConfigBuilder::constructor(
    /* [in] */ IThemeConfig* theme)
{
    AutoPtr<IMap> themes;
    theme->GetAppThemes((IMap**)&themes);
    AutoPtr<ISet> entrySet;
    themes->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(next);
        AutoPtr<IInterface> keyint;
        entry->GetKey((IInterface**)&keyint);
        String key;
        ICharSequence::Probe(keyint)->ToString(&key);
        AutoPtr<IInterface> value;
        entry->GetValue((IInterface**)&value);
        AutoPtr<IAppTheme> appTheme = IAppTheme::Probe(value);
        String fontPackPkgName;
        appTheme->GetFontPackPkgName(&fontPackPkgName);
        mFonts[key] = fontPackPkgName;
        String iconPackPkgName;
        appTheme->GetIconPackPkgName(&iconPackPkgName);
        mIcons[key] = iconPackPkgName;
        String overlayPkgName;
        appTheme->GetOverlayPkgName(&overlayPkgName);
        mOverlays[key] = overlayPkgName;
    }
    theme->GetLastThemeChangeRequestType(&mLastThemeChangeRequestType);
    return NOERROR;
}

ECode CThemeConfigBuilder::DefaultOverlay(
    /* [in] */ const String& themePkgName)
{
    if (!themePkgName.IsNull()) {
        mOverlays[ThemeConfig::KEY_DEFAULT_PKG] = themePkgName;
    }
    else {
        mOverlays.Erase(ThemeConfig::KEY_DEFAULT_PKG);
    }
    return NOERROR;
}

ECode CThemeConfigBuilder::DefaultFont(
    /* [in] */ const String& themePkgName)
{
    if (!themePkgName.IsNull()) {
        mFonts[ThemeConfig::KEY_DEFAULT_PKG] = themePkgName;
    }
    else {
        mFonts.Erase(ThemeConfig::KEY_DEFAULT_PKG);
    }
    return NOERROR;
}

ECode CThemeConfigBuilder::DefaultIcon(
    /* [in] */ const String& themePkgName)
{
    if (!themePkgName.IsNull()) {
        mIcons[ThemeConfig::KEY_DEFAULT_PKG] = themePkgName;
    }
    else {
        mIcons.Erase(ThemeConfig::KEY_DEFAULT_PKG);
    }
    return NOERROR;
}

ECode CThemeConfigBuilder::Icon(
    /* [in] */ const String&appPkgName,
    /* [in] */ const String&themePkgName)
{
    if (!themePkgName.IsNull()) {
        mIcons[appPkgName] = themePkgName;
    }
    else {
        mIcons.Erase(appPkgName);
    }
    return NOERROR;
}

ECode CThemeConfigBuilder::Overlay(
    /* [in] */ const String&appPkgName,
    /* [in] */ const String&themePkgName)
{
    if (!themePkgName.IsNull()) {
        mOverlays[appPkgName] = themePkgName;
    }
    else {
        mOverlays.Erase(appPkgName);
    }
    return NOERROR;
}

ECode CThemeConfigBuilder::Font(
    /* [in] */ const String&appPkgName,
    /* [in] */ const String&themePkgName)
{
    if (!themePkgName.IsNull()) {
        mFonts[appPkgName] = themePkgName;
    }
    else {
        mFonts.Erase(appPkgName);
    }
    return NOERROR;
}

ECode CThemeConfigBuilder::SetLastThemeChangeRequestType(
    /* [in] */ RequestType requestType)
{
    mLastThemeChangeRequestType = requestType;
    return NOERROR;
}

ECode CThemeConfigBuilder::Build(
    /* [out] */ IThemeConfig** _themeConfig)
{
    VALIDATE_NOT_NULL(_themeConfig)
    HashSet<String> appPkgSet;
    HashMap<String, String>::Iterator it = mOverlays.Begin();
    for (; it != mOverlays.End(); ++it) {
        appPkgSet.Insert(it->mFirst);
    }
    it = mIcons.Begin();
    for (; it != mIcons.End(); ++it) {
        appPkgSet.Insert(it->mFirst);
    }
    it = mFonts.Begin();
    for (; it != mFonts.End(); ++it) {
        appPkgSet.Insert(it->mFirst);
    }

    AutoPtr<IMap> appThemes;
    CHashMap::New((IMap**)&appThemes);
    HashSet<String>::Iterator appPkgNameIt = appPkgSet.Begin();
    for (; appPkgNameIt != appPkgSet.End(); ++appPkgNameIt) {
        String appPkgName = *appPkgNameIt;
        String icon(NULL);
        it = mIcons.Find(appPkgName);
        if (it != mIcons.End()) {
            icon = it->mSecond;
        }
        String overlay(NULL);
        it = mOverlays.Find(appPkgName);
        if (it != mOverlays.End()) {
            overlay = it->mSecond;
        }
        String font(NULL);
        it = mFonts.Find(appPkgName);
        if (it != mFonts.End()) {
            font = it->mSecond;
        }

        // Remove app theme if all items are null   224
        if (overlay.IsNull() && icon.IsNull() && font.IsNull()) {
            AutoPtr<ICharSequence> cs;
            CString::New(appPkgName, (ICharSequence**)&cs);
            Boolean containsKey;
            if (appThemes->ContainsKey(cs, &containsKey), containsKey) {
                appThemes->Remove(cs);
            }
        }
        else {
            AutoPtr<IAppTheme> appTheme;
            CAppTheme::New(overlay, icon, font, (IAppTheme**)&appTheme);
            AutoPtr<ICharSequence> cs;
            CString::New(appPkgName, (ICharSequence**)&cs);
            appThemes->Put(cs, appTheme);
        }
    }
    AutoPtr<CThemeConfig> themeConfig;
    CThemeConfig::NewByFriend(appThemes, (CThemeConfig**)&themeConfig);
    themeConfig->mLastThemeChangeRequestType = mLastThemeChangeRequestType;
    *_themeConfig = IThemeConfig::Probe(themeConfig);
    REFCOUNT_ADD(*_themeConfig)
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos


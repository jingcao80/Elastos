
#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/res/ThemeConfig.h"
#include "elastos/droid/content/res/CAppTheme.h"
#include "elastos/droid/content/res/CSystemThemeConfig.h"
#include "elastos/droid/content/res/CSystemAppTheme.h"
#include "elastos/droid/content/res/CThemeConfigBuilder.h"
#include "elastos/droid/provider/CSettingsSecure.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Utility::JsonToken;
using Elastos::Droid::Utility::JsonToken_NULL;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::EIID_IComparable;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::IStringReader;
using Elastos::IO::CStringReader;
using Elastos::IO::CStringWriter;
using Elastos::IO::ICloseable;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

//==============================================================================
//                  ThemeConfig::AppTheme
//==============================================================================

CAR_INTERFACE_IMPL_3(ThemeConfig::AppTheme, Object, IAppTheme, ICloneable, IComparable)

ECode ThemeConfig::AppTheme::constructor(
    /* [in] */ const String& overlayPkgName,
    /* [in] */ const String& iconPkgName,
    /* [in] */ const String& fontPkgName)
{
    mOverlayPkgName = overlayPkgName;
    mIconPkgName = iconPkgName;
    mFontPkgName = fontPkgName;
    return NOERROR;
}

ECode ThemeConfig::AppTheme::GetIconPackPkgName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = mIconPkgName;
    return NOERROR;
}

ECode ThemeConfig::AppTheme::GetOverlayPkgName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = mOverlayPkgName;
    return NOERROR;
}

ECode ThemeConfig::AppTheme::GetFontPackPkgName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = mFontPkgName;
    return NOERROR;
}

ECode ThemeConfig::AppTheme::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 hash = 17;
    hash = 31 * hash + (mOverlayPkgName.IsNull() ? 0 : mOverlayPkgName.GetHashCode());
    hash = 31 * hash + (mIconPkgName.IsNull() ? 0 : mIconPkgName.GetHashCode());
    hash = 31 * hash + (mFontPkgName.IsNull() ? 0 : mFontPkgName.GetHashCode());
    *hashCode = hash;
    return NOERROR;
}

ECode ThemeConfig::AppTheme::CompareTo(
    /* [in] */ IInterface* o,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;
    if (o == NULL) return NOERROR;
    AutoPtr<IAppTheme> otherTheme = IAppTheme::Probe(o);
    Int32 n = 0;
    String iconPkgName;
    otherTheme->GetIconPackPkgName(&iconPkgName);
    n = mIconPkgName.Compare(iconPkgName);
    if (n != 0) {
        *result = n;
        return NOERROR;
    }
    String fontPkgName;
    otherTheme->GetFontPackPkgName(&fontPkgName);
    n = mFontPkgName.Compare(fontPkgName);
    if (n != 0) {
        *result = n;
        return NOERROR;
    }
    String overlayPkgName;
    otherTheme->GetOverlayPkgName(&overlayPkgName);
    *result = mOverlayPkgName.Equals(overlayPkgName) ? 0 : 1;
    return NOERROR;
}

ECode ThemeConfig::AppTheme::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    IAppTheme* o = IAppTheme::Probe(object);
    if (o == (IAppTheme*)this) {
        *result = TRUE;
        return NOERROR;
    }
    if (o != NULL) {
        String currentOverlayPkgName = mOverlayPkgName.IsNull()? String("") : mOverlayPkgName;
        String overlayPkgName;
        o->GetOverlayPkgName(&overlayPkgName);
        String newOverlayPkgName = overlayPkgName.IsNull()? String("") : overlayPkgName;
        String currentIconPkgName = mIconPkgName.IsNull()? String("") : mIconPkgName;
        String iconPkgName;
        o->GetIconPackPkgName(&iconPkgName);
        String newIconPkgName = iconPkgName.IsNull()? String("") : iconPkgName;
        String currentFontPkgName = mFontPkgName.IsNull()? String("") : mFontPkgName;
        String fontPkgName;
        o->GetFontPackPkgName(&fontPkgName);
        String newFontPkgName = fontPkgName.IsNull()? String("") : fontPkgName;

        *result = (currentIconPkgName.Equals(newIconPkgName) &&
                currentFontPkgName.Equals(newFontPkgName) &&
                currentOverlayPkgName.Equals(newOverlayPkgName));
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode ThemeConfig::AppTheme::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder result;;
    if (!mOverlayPkgName.IsNull()) {
        result.Append("overlay:");
        result.Append(mOverlayPkgName);
    }

    if (!TextUtils::IsEmpty(mIconPkgName)) {
        result.Append(", iconPack:");
        result.Append(mIconPkgName);
    }

    if (!TextUtils::IsEmpty(mFontPkgName)) {
        result.Append(", fontPkg:");
        result.Append(mFontPkgName);
    }
    *str = result.ToString();
    return NOERROR;
}

ECode ThemeConfig::AppTheme::Clone(
    /* [out] */ IInterface** object)
{
    return NOERROR;
}


//==============================================================================
//                  ThemeConfig::JsonSerializer
//==============================================================================

const String ThemeConfig::JsonSerializer::NAME_OVERLAY_PKG("mOverlayPkgName");
const String ThemeConfig::JsonSerializer::NAME_ICON_PKG("mIconPkgName");
const String ThemeConfig::JsonSerializer::NAME_FONT_PKG("mFontPkgName");

String ThemeConfig::JsonSerializer::ToJson(
    /* [in] */ IThemeConfig* theme)
{
    String json(NULL);
    AutoPtr<IWriter> writer;
    AutoPtr<IJsonWriter> jsonWriter;
    // try {
    CStringWriter::New((IWriter**)&writer);
    assert(0);
    // jsonWriter = new JsonWriter(writer);
    WriteTheme(jsonWriter, theme);
    IObject::Probe(writer)->ToString(&json);
    // } catch(IOException e) {
    //     Log.e(TAG, "Could not write theme mapping", e);
    // } finally {
    //     closeQuietly(writer);
    //     closeQuietly(jsonWriter);
    // }
    CloseQuietly(writer);
    CloseQuietly(jsonWriter);
    return json;
}

ECode ThemeConfig::JsonSerializer::WriteTheme(
    /* [in] */ IJsonWriter* writer,
    /* [in] */ IThemeConfig* theme)
{
    writer->BeginObject();
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
        AutoPtr<IInterface> key;
        entry->GetKey((IInterface**)&key);
        String appPkgName;
        ICharSequence::Probe(key)->ToString(&appPkgName);
        AutoPtr<IInterface> value;
        entry->GetValue((IInterface**)&value);
        AutoPtr<IAppTheme> appTheme = IAppTheme::Probe(value);
        AutoPtr<IJsonWriter> jw;
        writer->Name(appPkgName, (IJsonWriter**)&jw);
        WriteAppTheme(writer, appTheme);
    }
    writer->EndObject();
    return NOERROR;
}

ECode ThemeConfig::JsonSerializer::WriteAppTheme(
    /* [in] */ IJsonWriter* writer,
    /* [in] */ IAppTheme* appTheme)
{
    writer->BeginObject();
    AutoPtr<IJsonWriter> jsonW;
    writer->Name(NAME_OVERLAY_PKG, (IJsonWriter**)&jsonW);
    String overlayPkgName;
    appTheme->GetOverlayPkgName(&overlayPkgName);
    jsonW->Value(overlayPkgName);
    jsonW = NULL;
    writer->Name(NAME_ICON_PKG, (IJsonWriter**)&jsonW);
    String iconPkgName;
    appTheme->GetIconPackPkgName(&iconPkgName);
    jsonW->Value(iconPkgName);
    jsonW = NULL;
    writer->Name(NAME_FONT_PKG, (IJsonWriter**)&jsonW);
    String fontPkgName;
    appTheme->GetFontPackPkgName(&fontPkgName);
    jsonW->Value(fontPkgName);
    writer->EndObject();
    return NOERROR;
}

AutoPtr<IThemeConfig> ThemeConfig::JsonSerializer::FromJson(
    /* [in] */ const String& json)
{
    if (json.IsNull()) return NULL;
    AutoPtr<IMap> map;
    CHashMap::New((IMap**)&map);
    AutoPtr<IStringReader> reader;
    AutoPtr<IJsonReader> jsonReader;
    // try {
    CStringReader::New(json, (IStringReader**)&reader);
    assert(0);
    // jsonReader = new JsonReader(reader);
    jsonReader->BeginObject();
    Boolean hasNext;
    while (jsonReader->HasNext(&hasNext), hasNext) {
        String appPkgName;
        jsonReader->NextName(&appPkgName);
        AutoPtr<IAppTheme> appTheme;
        ReadAppTheme(jsonReader, (IAppTheme**)&appTheme);
        AutoPtr<ICharSequence> cs;
        CString::New(appPkgName, (ICharSequence**)&cs);
        map->Put(cs, appTheme);
    }
    jsonReader->EndObject();
    // } catch(Exception e) {
    //     Log.e(TAG, "Could not parse ThemeConfig from: " + json, e);
    // } finally {
    //     closeQuietly(reader);
    //     closeQuietly(jsonReader);
    // }
    CloseQuietly(IReader::Probe(reader));
    CloseQuietly(jsonReader);
    AutoPtr<IThemeConfig> config;
    CThemeConfig::New(map, (IThemeConfig**)&config);
    return config;
}

ECode ThemeConfig::JsonSerializer::ReadAppTheme(
    /* [in] */ IJsonReader* reader,
    /* [out] */ IAppTheme** appTheme)
{
    String overlay("NULL");
    String icon("NULL");
    String font("NULL");

    reader->BeginObject();
    Boolean hasNext;
    while (reader->HasNext(&hasNext), hasNext) {
        String name;
        reader->NextName(&name);
        JsonToken token;
        if (NAME_OVERLAY_PKG.Equals(name) && (reader->Peek(&token), token != JsonToken_NULL)) {
            reader->NextString(&overlay);
        }
        else if (NAME_ICON_PKG.Equals(name) && (reader->Peek(&token), token != JsonToken_NULL)) {
            reader->NextString(&icon);
        }
        else if (NAME_FONT_PKG.Equals(name) && (reader->Peek(&token) != JsonToken_NULL)) {
            reader->NextString(&font);
        }
        else {
            reader->SkipValue();
        }
    }
    reader->EndObject();

    return CAppTheme::New(overlay, icon, font, appTheme);
}

void ThemeConfig::JsonSerializer::CloseQuietly(
    /* [in] */ IReader* reader)
{
    if (reader != NULL) ICloseable::Probe(reader)->Close();
}

void ThemeConfig::JsonSerializer::CloseQuietly(
    /* [in] */ IJsonReader* reader)
{
    if (reader != NULL) ICloseable::Probe(reader)->Close();
}

void ThemeConfig::JsonSerializer::CloseQuietly(
    /* [in] */ IWriter* writer)
{
    if (writer != NULL) ICloseable::Probe(writer)->Close();
}

void ThemeConfig::JsonSerializer::CloseQuietly(
    /* [in] */ IJsonWriter* writer)
{
    if (writer != NULL) ICloseable::Probe(writer)->Close();
}


//==============================================================================
//                  ThemeConfig
//==============================================================================

const String ThemeConfig::TAG("ThemeConfig");
const String ThemeConfig::KEY_DEFAULT_PKG("default");

AutoPtr<IThemeConfig> ThemeConfig::sSystemConfig;
AutoPtr<IAppTheme> ThemeConfig::sSystemAppTheme;

CAR_INTERFACE_IMPL_4(ThemeConfig, Object, IThemeConfig, ICloneable, IParcelable, IComparable)

ThemeConfig::ThemeConfig()
    : mLastThemeChangeRequestType(RequestType_USER_REQUEST)
{
    CHashMap::New((IMap**)&mThemes);
}

ECode ThemeConfig::constructor()
{
    return NOERROR;
}

ECode ThemeConfig::constructor(
    /* [in] */ IMap* appThemes)
{
    mThemes->PutAll(appThemes);
    return NOERROR;
}

ECode ThemeConfig::GetOverlayPkgName(
    /* [out] */ String* str)
{
    return GetDefaultTheme()->GetOverlayPkgName(str);
}

ECode ThemeConfig::GetOverlayForStatusBar(
    /* [out] */ String* str)
{
    return GetOverlayPkgNameForApp(SYSTEMUI_STATUS_BAR_PKG, str);
}

ECode ThemeConfig::GetOverlayForNavBar(
    /* [out] */ String* str)
{
    return GetOverlayPkgNameForApp(SYSTEMUI_NAVBAR_PKG, str);
}

ECode ThemeConfig::GetOverlayPkgNameForApp(
    /* [in] */ const String& appPkgName,
    /* [out] */ String* str)
{
    return GetThemeFor(appPkgName)->GetOverlayPkgName(str);
}

ECode ThemeConfig::GetIconPackPkgName(
    /* [out] */ String* str)
{
    return GetDefaultTheme()->GetIconPackPkgName(str);
}

ECode ThemeConfig::GetIconPackPkgNameForApp(
    /* [in] */ const String& appPkgName,
    /* [out] */ String* str)
{
    return GetThemeFor(appPkgName)->GetIconPackPkgName(str);
}

ECode ThemeConfig::GetFontPkgName(
    /* [out] */ String* pkgName)
{
    return GetDefaultTheme()->GetFontPackPkgName(pkgName);
}

ECode ThemeConfig::GetFontPkgNameForApp(
    /* [in] */ const String& appPkgName,
    /* [out] */ String* str)
{
    return GetThemeFor(appPkgName)->GetFontPackPkgName(str);
}

ECode ThemeConfig::GetAppThemes(
    /* [out] */ IMap** themes)
{
    VALIDATE_NOT_NULL(themes)
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    return collections->UnmodifiableMap(mThemes, themes);
}

ECode ThemeConfig::GetLastThemeChangeRequestType(
    /* [out] */ RequestType* requestType)
{
    VALIDATE_NOT_NULL(requestType)
    *requestType = mLastThemeChangeRequestType;
    return NOERROR;
}

AutoPtr<IAppTheme> ThemeConfig::GetThemeFor(
    /* [in] */ const String& pkgName)
{
    AutoPtr<ICharSequence> cs;
    CString::New(pkgName, (ICharSequence**)&cs);
    AutoPtr<IInterface> value;
    mThemes->Get(cs, (IInterface**)&value);
    AutoPtr<IAppTheme> theme = IAppTheme::Probe(value);
    if (theme == NULL) theme = GetDefaultTheme();
    return theme;
}

AutoPtr<IAppTheme> ThemeConfig::GetDefaultTheme()
{
    AutoPtr<ICharSequence> cs;
    CString::New(KEY_DEFAULT_PKG, (ICharSequence**)&cs);
    AutoPtr<IInterface> value;
    mThemes->Get(cs, (IInterface**)&value);
    AutoPtr<IAppTheme> theme = IAppTheme::Probe(value);
    if (theme == NULL) theme = GetSystemAppTheme();
    return theme;
}

ECode ThemeConfig::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    IThemeConfig* o = IThemeConfig::Probe(object);
    if (o == this) {
        *result = TRUE;
        return NOERROR;
    }
    if (o != NULL) {
        AutoPtr<IMap> currThemes;
        if (mThemes != NULL) {
            currThemes = mThemes;
        }
        else {
            CHashMap::New((IMap**)&currThemes);
        }
        AutoPtr<IMap> newThemes;
        AutoPtr<IMap> oThemes;
        o->GetAppThemes((IMap**)&oThemes);
        if (oThemes != NULL) {
            newThemes = oThemes;
        }
        else {
            CHashMap::New((IMap**)&newThemes);
        }

        Int32 oRequestType;
        o->GetLastThemeChangeRequestType(&oRequestType);
        Boolean equals = Object::Equals(currThemes, newThemes);
        if (equals) {
            Int32 oRequestType;
            o->GetLastThemeChangeRequestType(&oRequestType);
            *result = oRequestType;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode ThemeConfig::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("ThemeConfig{0x");
    sb += StringUtils::ToHexString((Int32)this);
    if (mThemes != NULL) {
        sb += ", themes:";
        String themesStr;
        sb += Object::ToString(mThemes);
    }
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

ECode ThemeConfig::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = 17;
    Int32 themesHash;
    IObject::Probe(mThemes)->GetHashCode(&themesHash);
    *hashCode = 31 * *hashCode + themesHash;
    *hashCode = 31 * *hashCode + mLastThemeChangeRequestType;
    return NOERROR;
}

ECode ThemeConfig::ToJson(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = JsonSerializer::ToJson(this);
    return NOERROR;
}

AutoPtr<IThemeConfig> ThemeConfig::FromJson(
    /* [in] */ const String& json)
{
    return JsonSerializer::FromJson(json);
}

ECode ThemeConfig::GetBootTheme(
    /* [in] */ IContentResolver* resolver,
    /* [out] */ IThemeConfig** theme)
{
    return GetBootThemeForUser(resolver, IUserHandle::USER_OWNER, theme);
}

ECode ThemeConfig::GetBootThemeForUser(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int32 userHandle,
    /* [out] */ IThemeConfig** theme)
{
    VALIDATE_NOT_NULL(theme);
    *theme = NULL;

    AutoPtr<IThemeConfig> bootTheme = GetSystemTheme();
    // try {
    AutoPtr<ISettingsSecure> settingSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingSecure);
    String json;
    ECode ec = settingSecure->GetStringForUser(resolver,
            IConfiguration::THEME_PKG_CONFIGURATION_PERSISTENCE_PROPERTY,
            userHandle, &json);
    if (SUCCEEDED(ec)) {
        bootTheme = ThemeConfig::FromJson(json);

        // Handle upgrade Case: Previously the theme configuration was in separate fields
        if (bootTheme == NULL) {
            AutoPtr<IThemeConfigBuilder> builder;
            CThemeConfigBuilder::New((IThemeConfigBuilder**)&builder);
            String overlayPkgName, iconPackPkgName, fontPkgName;
            ec = settingSecure->GetStringForUser(resolver,
                    IConfiguration::THEME_PACKAGE_NAME_PERSISTENCE_PROPERTY,
                    userHandle, &overlayPkgName);
            FAIL_GOTO(ec, _EXIT_)
            ec = settingSecure->GetStringForUser(resolver,
                    IConfiguration::THEME_ICONPACK_PACKAGE_NAME_PERSISTENCE_PROPERTY,
                    userHandle, &iconPackPkgName);
            FAIL_GOTO(ec, _EXIT_)
            ec = settingSecure->GetStringForUser(resolver,
                    IConfiguration::THEME_FONT_PACKAGE_NAME_PERSISTENCE_PROPERTY,
                    userHandle, &fontPkgName);
            FAIL_GOTO(ec, _EXIT_)

            builder->DefaultOverlay(overlayPkgName);
            builder->DefaultIcon(iconPackPkgName);
            builder->DefaultFont(fontPkgName);
            builder->Build((IThemeConfig**)&bootTheme);
            Slogger::I(TAG, "GetBootThemeForUser:%d, overlayPkgName:%s, iconPkgName:%s, fontPkgName:%s",
                userHandle, overlayPkgName.string(), iconPackPkgName.string(), fontPkgName.string());
        }
    }
    else {
        Slogger::E(TAG, "Could not get boot theme, ec=%08x", ec);
    }

_EXIT_:
    *theme = bootTheme;
    REFCOUNT_ADD(*theme);
    return NOERROR;
}


AutoPtr<IThemeConfig> ThemeConfig::GetSystemTheme()
{
    if (sSystemConfig == NULL) {
        CSystemThemeConfig::New((IThemeConfig**)&sSystemConfig);
    }
    return sSystemConfig;
}

AutoPtr<IAppTheme> ThemeConfig::GetSystemAppTheme()
{
    if (sSystemAppTheme == NULL) {
        CSystemAppTheme::New((IAppTheme**)&sSystemAppTheme);
    }
   return sSystemAppTheme;
}

ECode ThemeConfig::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    String json = JsonSerializer::ToJson(this);
    dest->WriteString(json);
    dest->WriteInt32(mLastThemeChangeRequestType);
    return NOERROR;
}

ECode ThemeConfig::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    String json;
    source->ReadString(&json);
    AutoPtr<IMap> map;
    CHashMap::New((IMap**)&map);
    AutoPtr<IStringReader> reader;
    AutoPtr<IJsonReader> jsonReader;
    // try {
    CStringReader::New(json, (IStringReader**)&reader);
    assert(0);
    // jsonReader = new JsonReader(reader);
    jsonReader->BeginObject();
    Boolean hasNext;
    while (jsonReader->HasNext(&hasNext), hasNext) {
        String appPkgName;
        jsonReader->NextName(&appPkgName);
        AutoPtr<IAppTheme> appTheme;
        JsonSerializer::ReadAppTheme(jsonReader, (IAppTheme**)&appTheme);
        AutoPtr<ICharSequence> cs;
        CString::New(appPkgName, (ICharSequence**)&cs);
        map->Put(cs, appTheme);
    }
    jsonReader->EndObject();
    // } catch(Exception e) {
    //     Log.e(TAG, "Could not parse ThemeConfig from: " + json, e);
    // } finally {
    //     closeQuietly(reader);
    //     closeQuietly(jsonReader);
    // }
    JsonSerializer::CloseQuietly(IReader::Probe(reader));
    JsonSerializer::CloseQuietly(jsonReader);
    mThemes = map;
    source->ReadInt32(&mLastThemeChangeRequestType);
    return NOERROR;
}

ECode ThemeConfig::CompareTo(
    /* [in] */ IInterface* o,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;
    if (o == NULL) return NOERROR;
    AutoPtr<IMap> themes;
    IThemeConfig::Probe(o)->GetAppThemes((IMap**)&themes);
    Boolean equals;
    IObject::Probe(mThemes)->Equals(themes, &equals);
    *result = equals? 0 : 1;
    return NOERROR;
}

ECode ThemeConfig::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    *object = NULL;
    // try {
    //     return super.clone();
    // } catch (CloneNotSupportedException e) {
    //     Log.d(TAG, "clone not supported", e);
    //     return null;
    // }
    return E_NOT_IMPLEMENTED;
}

}
}
}
}

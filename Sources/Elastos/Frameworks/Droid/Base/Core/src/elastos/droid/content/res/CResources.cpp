
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/IconPackHelper.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/content/res/CColorStateList.h"
#include "elastos/droid/content/res/XmlBlock.h"
#include "elastos/droid/content/pm/CActivityInfo.h"
#include "elastos/droid/graphics/Movie.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/graphics/drawable/CColorDrawable.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/utility/CInt64SparseArray.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::App::IconPackHelper;
using Elastos::Droid::Content::Pm::IActivityInfoHelper;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::CActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Graphics::Movie;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableConstantState;
using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Utility::CInt64SparseArray;
using Libcore::ICU::INativePluralRulesHelper;
using Libcore::ICU::CNativePluralRulesHelper;
using Libcore::ICU::INativePluralRules;
using Elastos::Core::StringUtils;
using Elastos::Core::IAppendable;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IFormatter;
using Elastos::Utility::CFormatter;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CResources::Theme, Object, IResourcesTheme)

CResources::Theme::Theme(
    /* [in] */ CResources* host)
    : mHost(host)
    , mAssets(host->mAssets)
    , mTheme(0)
    , mThemeResId(0)
{
    mAssets->CreateTheme(&mTheme);
}

CResources::Theme::~Theme()
{
    mAssets->ReleaseTheme(mTheme);
}

ECode CResources::Theme::ApplyStyle(
    /* [in] */ Int32 resId,
    /* [in] */ Boolean force)
{
    CAssetManager::ApplyThemeStyle(mTheme, resId, force);

    mThemeResId = resId;

    StringBuilder sb(mKey);
    sb += StringUtils::ToHexString(resId);
    sb += (force ? "! " : " ");
    mKey = sb.ToString();
    return NOERROR;
}

ECode CResources::Theme::SetTo(
    /* [in] */ IResourcesTheme* other)
{
    Theme* o = (Theme*)other;
    CAssetManager::CopyTheme(mTheme, o->mTheme);

    mThemeResId = o->mThemeResId;
    mKey = o->mKey;
    return NOERROR;
}

ECode CResources::Theme::ObtainStyledAttributes(
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** styles)
{
    VALIDATE_NOT_NULL(styles);
    *styles = NULL;

    Int32 len = attrs->GetLength();
    AutoPtr<ITypedArray> ta = CTypedArray::Obtain(IResources::Probe(mHost), len);;
    CTypedArray* array = (CTypedArray*)ta.Get();
    assert(array);
    array->mTheme = this;
    Boolean result;
    FAIL_RETURN(CAssetManager::ApplyStyle(mTheme, 0, 0, 0, *attrs,
            array->mData, array->mIndices, &result));
    *styles = ta;
    REFCOUNT_ADD(*styles);
    return NOERROR;
}

ECode CResources::Theme::ObtainStyledAttributes(
    /* [in] */ Int32 resid,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** styles)
{
    VALIDATE_NOT_NULL(styles);

    Int32 len = attrs->GetLength();
    AutoPtr<ITypedArray> ta = CTypedArray::Obtain(IResources::Probe(mHost), len);;
    CTypedArray* array = (CTypedArray*)ta.Get();
    assert(array);
    array->mTheme = this;

    Boolean result;
    FAIL_RETURN(CAssetManager::ApplyStyle(mTheme, 0, resid, 0, *attrs,
            array->mData, array->mIndices, &result));
    // if (false) {
    //     Int32[] data = array.mData;

    //     System.out.println("**********************************************************");
    //     System.out.println("**********************************************************");
    //     System.out.println("**********************************************************");
    //     System.out.println("Attributes:");
    //     String s = "  Attrs:";
    //     Int32 i;
    //     for (i=0; i<attrs.length; i++) {
    //         s = s + " 0x" + Integer.toHexString(attrs[i]);
    //     }
    //     System.out.println(s);
    //     s = "  Found:";
    //     TypedValue value = new TypedValue();
    //     for (i=0; i<attrs.length; i++) {
    //         Int32 d = i*AssetManager.STYLE_NUM_ENTRIES;
    //         value.type = data[d+AssetManager.STYLE_TYPE];
    //         value.data = data[d+AssetManager.STYLE_DATA];
    //         value.assetCookie = data[d+AssetManager.STYLE_ASSET_COOKIE];
    //         value.resourceId = data[d+AssetManager.STYLE_RESOURCE_ID];
    //         s = s + " 0x" + Integer.toHexString(attrs[i])
    //             + "=" + value;
    //     }
    //     System.out.println(s);
    // }
    *styles = (ITypedArray*)array;
    REFCOUNT_ADD(*styles);
    return NOERROR;
}

ECode CResources::Theme::ObtainStyledAttributes(
    /* [in] */ IAttributeSet* set,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes,
    /* [out] */ ITypedArray** styles)
{
    VALIDATE_NOT_NULL(styles);
    *styles = NULL;
    VALIDATE_NOT_NULL(attrs);

    Int32 len = attrs->GetLength();
    AutoPtr<ITypedArray> ta = CTypedArray::Obtain(IResources::Probe(mHost), len);;
    CTypedArray* array = (CTypedArray*)ta.Get();
    assert(array);

    // XXX note that for now we only work with compiled XML files.
    // To support generic XML files we will need to manually parse
    // out the attributes from the XML file (applying type information
    // contained in the resources and such).
    //begin from this
    XmlBlock::Parser* parser = (XmlBlock::Parser*)set;
    Boolean result;
    FAIL_RETURN(CAssetManager::ApplyStyle(
        mTheme, defStyleAttr, defStyleRes,
        parser != NULL ? parser->mParseState : 0, *attrs,
                array->mData, array->mIndices, &result));

    array->mTheme = this;
    array->mXml = parser;

    // if (false) {
    //     Int32[] data = array.mData;

    //     System.out.println("Attributes:");
    //     String s = "  Attrs:";
    //     Int32 i;
    //     for (i=0; i<set.getAttributeCount(); i++) {
    //         s = s + " " + set.getAttributeName(i);
    //         Int32 id = set.getAttributeNameResource(i);
    //         if (id != 0) {
    //             s = s + "(0x" + Integer.toHexString(id) + ")";
    //         }
    //         s = s + "=" + set.getAttributeValue(i);
    //     }
    //     System.out.println(s);
    //     s = "  Found:";
    //     TypedValue value = new TypedValue();
    //     for (i=0; i<attrs.length; i++) {
    //         Int32 d = i*AssetManager.STYLE_NUM_ENTRIES;
    //         value.type = data[d+AssetManager.STYLE_TYPE];
    //         value.data = data[d+AssetManager.STYLE_DATA];
    //         value.assetCookie = data[d+AssetManager.STYLE_ASSET_COOKIE];
    //         value.resourceId = data[d+AssetManager.STYLE_RESOURCE_ID];
    //         s = s + " 0x" + Integer.toHexString(attrs[i])
    //             + "=" + value;
    //     }
    //     System.out.println(s);
    // }
    *styles = (ITypedArray*)array;
    REFCOUNT_ADD(*styles);
    return NOERROR;
}

ECode CResources::Theme::ResolveAttribute(
    /* [in] */ ArrayOf<Int32>* values,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 len = attrs->GetLength();
    if (values != NULL && len != values->GetLength()) {
        // throw new IllegalArgumentException(
        //         "Base attribute values must be null or the same length as attrs");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }


    AutoPtr<ITypedArray> ta = CTypedArray::Obtain(IResources::Probe(mHost), len);
    CTypedArray* array = (CTypedArray*)ta.Get();
    Boolean bval;
    CAssetManager::ResolveAttrs(mTheme, 0, 0, *values, *attrs, array->mData, array->mIndices, &bval);

    array->mTheme = this;
    array->mXml = NULL;

    *result = array;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CResources::Theme::ResolveAttribute(
    /* [in] */ Int32 resid,
    /* [in, out] */ ITypedValue* outValue,
    /* [in] */ Boolean resolveRefs,
    /* [out] */ Boolean* isFounded)
{
    VALIDATE_NOT_NULL(isFounded);

    *isFounded = mAssets->GetThemeValue(mTheme, resid, outValue, resolveRefs);
    // if (false) {
    //     System.out.println(
    //         "resolveAttribute #" + Integer.toHexString(resid)
    //         + " got=" + got + ", type=0x" + Integer.toHexString(outValue.type)
    //         + ", data=0x" + Integer.toHexString(outValue.data));
    // }
    return NOERROR;
}

ECode CResources::Theme::GetAllAttributes(
    /* [out, callee] */ ArrayOf<Int32>** attrs)
{
    return mAssets->GetStyleAttributes(GetAppliedStyleResId(), attrs);
}

ECode CResources::Theme::GetResources(
    /* [out] */ IResources** res)
{
    VALIDATE_NOT_NULL(res)
    *res = IResources::Probe(mHost);
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CResources::Theme::GetDrawable(
    /* [in] */ Int32 id,
    /* [out] */ IDrawable** drawable)
{
    return IResources::Probe(mHost)->GetDrawable(id, this, drawable);
}

ECode CResources::Theme::Dump(
    /* [in] */ Int32 priority,
    /* [in] */ const String& tag,
    /* [in] */ const String& prefix)
{
    CAssetManager::DumpTheme(mTheme, priority, tag, prefix);
    return NOERROR;
}

// Needed by layoutlib.
/*package*/ Int64 CResources::Theme::GetNativeTheme()
{
    return mTheme;
}

/*package*/ Int32 CResources::Theme::GetAppliedStyleResId()
{
    return mThemeResId;
}

/*package*/ String CResources::Theme::GetKey()
{
    return mKey;
}

String CResources::Theme::GetResourceNameFromHexString(
    /* [in] */ const String& hexString)
{
    String str;
    Int32 value;
    StringUtils::Parse(hexString, 16, &value);
    mHost->GetResourceName(value, &str);
    return str;
}

ECode CResources::Theme::GetTheme(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > themeData;
    StringUtils::Split(mKey, " ", (ArrayOf<String>**)&themeData);

    AutoPtr<ArrayOf<String> > themes = ArrayOf<String>::Alloc(themeData->GetLength() * 2);
    String theme;
    Boolean forced;

    for (Int32 i = 0, j = themeData->GetLength() - 1; i < themes->GetLength(); i += 2, --j) {
        theme = (*themeData)[j];
        forced = theme.EndWith("!");
        (*themes)[i] = forced ?
            GetResourceNameFromHexString(theme.Substring(0, theme.GetLength() - 1)) :
            GetResourceNameFromHexString(theme);
        (*themes)[i + 1] = forced ? String("forced") : String("not forced");
    }

    *result = themes;
    REFCOUNT_ADD(*result)
    return NOERROR;
}


CResources::StaticInitializer::StaticInitializer()
{
    CResources::LAYOUT_DIR_CONFIG = CActivityInfo::ActivityInfoConfigToNative(IActivityInfo::CONFIG_LAYOUT_DIRECTION);
    sPreloadedDrawables = ArrayOf<IInt64SparseArray*>::Alloc(2);
    AutoPtr<IInt64SparseArray> array;
    CInt64SparseArray::New((IInt64SparseArray**)&array);
    sPreloadedDrawables->Set(0, array);
    array = NULL;
    CInt64SparseArray::New((IInt64SparseArray**)&array);
    sPreloadedDrawables->Set(1, array);

    CInt64SparseArray::New((IInt64SparseArray**)&sPreloadedColorDrawables);
}


const String CResources::TAG("CResources");
const Boolean CResources::DEBUG_LOAD = FALSE;
const Boolean CResources::DEBUG_CONFIG = FALSE;
const Boolean CResources::TRACE_FOR_PRELOAD = FALSE;
const Boolean CResources::TRACE_FOR_MISS_PRELOAD = FALSE;
const Int32 CResources::ID_OTHER = 0x01000004;
Int32 CResources::LAYOUT_DIR_CONFIG = 0;
const String CResources::WIDGET_SUFFIX("widget_preview");

INIT_PROI_4 AutoPtr<IResources> CResources::sSystem;
INIT_PROI_4 AutoPtr< ArrayOf<IInt64SparseArray*> > CResources::sPreloadedDrawables;
INIT_PROI_4 AutoPtr<IInt64SparseArray> CResources::sPreloadedColorDrawables;
INIT_PROI_4 CResources::ColorStateMap CResources::sPreloadedColorStateLists;
Boolean CResources::sPreloaded = FALSE;
Int32 CResources::sPreloadedDensity = 0;
INIT_PROI_4 Object CResources::sSync;
INIT_PROI_4 const CResources::StaticInitializer CResources::sInitializer;

CAR_INTERFACE_IMPL(CResources, Object, IResources)

CAR_OBJECT_IMPL(CResources)

CResources::CResources()
    : mPreloading(FALSE)
    , mLastCachedXmlBlockIndex(-1)
{
    ASSERT_SUCCEEDED(CTypedValue::NewByFriend((CTypedValue**)&mTmpValue));
    ASSERT_SUCCEEDED(CConfiguration::NewByFriend((CConfiguration**)&mTmpConfig));

    mTypedArrayPool = new Pools::SynchronizedPool<ITypedArray>(5);

    mCachedXmlBlockIds = ArrayOf<Int32>::Alloc(4);
    mCachedXmlBlocks = ArrayOf<XmlBlock*>::Alloc(4);

    ASSERT_SUCCEEDED(CConfiguration::NewByFriend((CConfiguration**)&mConfiguration));
    ASSERT_SUCCEEDED(CDisplayMetrics::NewByFriend((CDisplayMetrics**)&mMetrics));

    mCompatibilityInfo = CCompatibilityInfo::DEFAULT_COMPATIBILITY_INFO;
}

CResources::~CResources()
{
    mAssets = NULL;
    mCachedXmlBlocks = NULL;
}

ECode CResources::constructor()
{
    mAssets = CAssetManager::GetSystem();
    // NOTE: Intentionally leaving this uninitialized (all values set
    // to zero), so that anyone who tries to do something that requires
    // metrics will get a very wrong value.
    mConfiguration->SetToDefaults();
    mMetrics->SetToDefaults();
    UpdateConfiguration(NULL, NULL);
    mAssets->RecreateStringBlocks();
    return NOERROR;
}

ECode CResources::constructor(
    /* [in] */ IAssetManager* assets,
    /* [in] */ IDisplayMetrics* metrics,
    /* [in] */ IConfiguration* config)
{
    return constructor(assets, metrics, config, CCompatibilityInfo::DEFAULT_COMPATIBILITY_INFO, NULL);
}

ECode CResources::constructor(
    /* [in] */ IAssetManager* assets,
    /* [in] */ IDisplayMetrics* metrics,
    /* [in] */ IConfiguration* config,
    /* [in] */ ICompatibilityInfo* compInfo,
    /* [in] */ IBinder* token)
{
    if (!assets) {
        return E_INVALID_ARGUMENT;
    }
    mAssets = (CAssetManager*)assets;
    FAIL_RETURN(mMetrics->SetToDefaults())
    if (compInfo != NULL) {
        mCompatibilityInfo = compInfo;
    }
    FAIL_RETURN(UpdateConfiguration(config, metrics))
    mAssets->EnsureStringBlocks();
    // if (token) {
    //     assert(0 && "error: get GetWeakReference from IPC!!");
    //     IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(token);
    //     wrs->GetWeakReference((IWeakReference**)&mToken);
    // }
    return NOERROR;
}

Int32 CResources::SelectDefaultTheme(
    /* [in] */ Int32 curTheme,
    /* [in] */ Int32 targetSdkVersion)
{
    return SelectSystemTheme(curTheme, targetSdkVersion,
            R::style::Theme,
            R::style::Theme_Holo,
            R::style::Theme_DeviceDefault,
            R::style::Theme_DeviceDefault_Light_DarkActionBar);
}

Int32 CResources::SelectSystemTheme(
    /* [in] */ Int32 curTheme,
    /* [in] */ Int32 targetSdkVersion,
    /* [in] */ Int32 orig,
    /* [in] */ Int32 holo,
    /* [in] */ Int32 dark,
    /* [in] */ Int32 deviceDefault)
{
    if (curTheme != 0) {
        return curTheme;
    }
    if (targetSdkVersion < Build::VERSION_CODES::HONEYCOMB) {
        return orig;
    }
    if (targetSdkVersion < Build::VERSION_CODES::ICE_CREAM_SANDWICH) {
        return holo;
    }
    if (targetSdkVersion < Build::VERSION_CODES::CUR_DEVELOPMENT) {
        return dark;
    }
    return deviceDefault;
}

AutoPtr<IResources> CResources::GetSystem()
{
    AutoLock lock(sSync);

    AutoPtr<IResources> ret = sSystem;
    if (ret == NULL) {
        CResources::New((IResources**)&ret);
        sSystem = ret;
    }

    return ret;
}

ECode CResources::GetText(
    /* [in] */ Int32 id,
    /* [out] */ ICharSequence** seq)
{
    VALIDATE_NOT_NULL(seq);
    *seq = NULL;

    AutoPtr<ICharSequence> temp = mAssets->GetResourceText(id);
    if (temp != NULL) {
        *seq = temp;
        REFCOUNT_ADD(*seq);
        return NOERROR;
    }

    Logger::E(TAG, "String resource ID #0x%08x", id);
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetQuantityText(
    /* [in] */ Int32 id,
    /* [in] */ Int32 quantity,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq);
    *csq = NULL;

    AutoPtr<INativePluralRules> rules = GetPluralRule();
    Int32 value;
    rules->QuantityForInt(quantity, &value);
    AutoPtr<ICharSequence> temp = mAssets->GetResourceBagText(id, AttrForQuantityCode(value));
    if (temp != NULL) {
        *csq = temp;
        REFCOUNT_ADD(*csq);
        return NOERROR;
    }

    temp = mAssets->GetResourceBagText(id, ID_OTHER);
    if (temp != NULL) {
        *csq = temp;
        REFCOUNT_ADD(*csq);
        return NOERROR;
    }

    Logger::E(TAG, "Plural resource ID #0x%08x quantity=%d item=%s", id, quantity,
             StringForQuantityCode(value).string());
    return E_NOT_FOUND_EXCEPTION;
}

AutoPtr<INativePluralRules> CResources::GetPluralRule()
{
    AutoLock lock(this);

    if (mPluralRule == NULL) {
        AutoPtr<INativePluralRulesHelper> helper;
        CNativePluralRulesHelper::AcquireSingleton((INativePluralRulesHelper**)&helper);
        AutoPtr<ILocale> locale;
        mConfiguration->GetLocale((ILocale**)&locale);
        helper->ForLocale(locale, (INativePluralRules**)&mPluralRule);
    }
    return mPluralRule;
}

Int32 CResources::AttrForQuantityCode(
    /* [in] */ Int32 quantityCode)
{
    Int32 ret;
    switch (quantityCode) {
        case INativePluralRules::ZERO:
            ret = 0x01000005;
            break;
        case INativePluralRules::ONE:
            ret = 0x01000006;
            break;
        case INativePluralRules::TWO:
            ret = 0x01000007;
            break;
        case INativePluralRules::FEW:
            ret = 0x01000008;
            break;
        case INativePluralRules::MANY:
            ret = 0x01000009;
            break;
        default:
            ret = ID_OTHER;
    }
    return ret;
}

String CResources::StringForQuantityCode(
    /* [in] */ Int32 quantityCode)
{
    String ret;
    switch (quantityCode) {
        case INativePluralRules::ZERO:
            ret = "zero";
            break;
        case INativePluralRules::ONE:
            ret = "one";
            break;
        case INativePluralRules::TWO:
            ret = "two";
            break;
        case INativePluralRules::FEW:
            ret = "few";
            break;
        case INativePluralRules::MANY:
            ret = "many";
            break;
        default:
            ret = "other";
    }
    return ret;
}

ECode CResources::GetString(
    /* [in] */ Int32 id,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<ICharSequence> res;
    if (SUCCEEDED(GetText(id, (ICharSequence**)&res)) && res != NULL) {
        res->ToString(str);
        return NOERROR;
    }

    Logger::E(TAG, "String resource ID #0x%08x", id);
    *str = NULL;
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetString(
    /* [in] */ Int32 id,
    /* [in] */ ArrayOf<IInterface*>* formatArgs,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    String raw;
    FAIL_RETURN(GetString(id, &raw));
    if (raw.IsNull()) {
        // throw new NullPointerException("format == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 bufferSize = raw.GetLength() + (formatArgs == NULL ? 0 : formatArgs->GetLength() * 10);
    AutoPtr<IAppendable> a = new StringBuilder(bufferSize);
    AutoPtr<Elastos::Utility::IFormatter> f;
    Elastos::Utility::CFormatter::New(a, mConfiguration->mLocale, (IFormatter**)&f);
    FAIL_RETURN(f->Format(raw, formatArgs));
    return f->ToString(str);
}

ECode CResources::GetQuantityString(
    /* [in] */ Int32 id,
    /* [in] */ Int32 quantity,
    /* [in] */ ArrayOf<IInterface*>* formatArgs,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<ICharSequence> res;
    ECode ec = GetQuantityText(id, quantity, (ICharSequence**)&res);
    if (SUCCEEDED(ec) && res != NULL) {
        String raw;
        res->ToString(&raw);
        assert(0);
        // return String.format(mConfiguration.locale, raw, formatArgs);
        return NOERROR;
    }

    *str = NULL;
    return ec;
}

ECode CResources::GetQuantityString(
    /* [in] */ Int32 id,
    /* [in] */ Int32 quantity,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<ICharSequence> res;
    ECode ec = GetQuantityText(id, quantity, (ICharSequence**)&res);
    if (SUCCEEDED(ec) && res != NULL) {
        res->ToString(str);
        return NOERROR;
    }

    *str = NULL;
    return ec;
}

ECode CResources::GetText(
    /* [in] */ Int32 id,
    /* [in] */ ICharSequence* def,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq);

    AutoPtr<ICharSequence> temp;
    if (id != 0) {
        temp = mAssets->GetResourceText(id);
    }
    if (temp != NULL) {
        *csq = temp;
    }
    else *csq = def;

    REFCOUNT_ADD(*csq);
    return NOERROR;
}

ECode CResources::GetTextArray(
    /* [in] */ Int32 id,
    /* [out, callee] */ ArrayOf<ICharSequence*>** seqs)
{
    VALIDATE_NOT_NULL(seqs);
    *seqs = NULL;

    AutoPtr< ArrayOf<ICharSequence*> > temp = mAssets->GetResourceTextArray(id);
    if (temp) {
        *seqs = temp;
        REFCOUNT_ADD(*seqs);
        return NOERROR;
    }

    Logger::E(TAG, "Text array resource ID #0x%08x", id);
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetStringArray(
    /* [in] */ Int32 id,
    /* [out, callee] */ ArrayOf<String>** strs)
{
    VALIDATE_NOT_NULL(strs);
    *strs = NULL;

    AutoPtr< ArrayOf<String> > temp = mAssets->GetResourceStringArray(id);
    if (temp) {
        *strs = temp;
        REFCOUNT_ADD(*strs);
        return NOERROR;
    }

    Logger::E(TAG, "String array resource ID #0x%08x", id);
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetInt32Array(
    /* [in] */ Int32 id,
    /* [out, callee] */ ArrayOf<Int32>** ints)
{
    VALIDATE_NOT_NULL(ints);
    *ints = NULL;

    AutoPtr< ArrayOf<Int32> > temp;
    FAIL_RETURN(mAssets->GetArrayIntResource(id, (ArrayOf<Int32>**)&temp));
    if (temp) {
        *ints = temp;
        REFCOUNT_ADD(*ints);
        return NOERROR;
    }

    Logger::E(TAG, "Int array resource ID #0x%08x", id);
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::ObtainTypedArray(
    /* [in] */ Int32 id,
    /* [out] */ ITypedArray** array)
{
    VALIDATE_NOT_NULL(array);
    *array = NULL;

    Int32 len = mAssets->GetArraySize(id);
    if (len < 0) {
        Logger::E(TAG, "Array resource ID #0x%08x", id);
        return E_NOT_FOUND_EXCEPTION;
    }

    AutoPtr<ITypedArray> ta = CTypedArray::Obtain(this, len);
    assert(ta != NULL);
    CTypedArray* temp = (CTypedArray*)ta.Get();
    FAIL_RETURN(mAssets->RetrieveArray(id, temp->mData, &temp->mLength))
    (*temp->mIndices)[0] = 0;

    *array = ta;
    REFCOUNT_ADD(*array);
    return NOERROR;
}

ECode CResources::GetDimension(
    /* [in] */ Int32 id,
    /* [out] */ Float* dim)
{
    VALIDATE_NOT_NULL(dim);

    AutoLock lock(mAccessLock);

    ITypedValue* value = (ITypedValue*)mTmpValue.Get();
    if (value == NULL) {
        CTypedValue::NewByFriend((CTypedValue**)&mTmpValue);
        value = (ITypedValue*)mTmpValue.Get();
    }

    FAIL_RETURN(GetValue(id, value, TRUE))
    if (mTmpValue->mType == ITypedValue::TYPE_DIMENSION) {
        *dim = CTypedValue::ComplexToDimension(mTmpValue->mData,
                (IDisplayMetrics*)mMetrics.Get());
        return NOERROR;
    }

    Logger::E(TAG, "Resource ID #0x%08x type #0x%08x is not valid", id, mTmpValue->mType);
    *dim = 0.0f;
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetDimensionPixelOffset(
    /* [in] */ Int32 id,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);

    AutoLock lock(mAccessLock);

    ITypedValue* value = (ITypedValue*)mTmpValue.Get();
    if (value == NULL) {
        CTypedValue::NewByFriend((CTypedValue**)&mTmpValue);
        value = (ITypedValue*)mTmpValue.Get();
    }

    FAIL_RETURN(GetValue(id, value, TRUE))
    if (mTmpValue->mType == ITypedValue::TYPE_DIMENSION) {
        *offset = CTypedValue::ComplexToDimensionPixelOffset(
                mTmpValue->mData,
                (IDisplayMetrics*)mMetrics.Get());
        return NOERROR;
    }

    Logger::E(TAG, "Resource ID #0x%08x type #0x%08x is not valid", id, mTmpValue->mType);
    *offset = 0;
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetDimensionPixelSize(
    /* [in] */ Int32 id,
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);

    AutoLock lock(mAccessLock);

    ITypedValue* value = (ITypedValue*)mTmpValue.Get();
    if (value == NULL) {
        CTypedValue::NewByFriend((CTypedValue**)&mTmpValue);
        value = (ITypedValue*)mTmpValue.Get();
    }

    FAIL_RETURN(GetValue(id, value, TRUE))
    if (mTmpValue->mType == ITypedValue::TYPE_DIMENSION) {
        *size = CTypedValue::ComplexToDimensionPixelSize(
                mTmpValue->mData,
                (IDisplayMetrics*)mMetrics.Get());
        return NOERROR;
    }

    Logger::E(TAG, "Resource ID #0x%08x type #0x%08x is not valid", id, mTmpValue->mType);
    *size = 0;
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetFraction(
    /* [in] */ Int32 id,
    /* [in] */ Int32 base,
    /* [in] */ Int32 pbase,
    /* [out] */ Float* fraction)
{
    VALIDATE_NOT_NULL(fraction);

    AutoLock lock(mAccessLock);

    ITypedValue* value = (ITypedValue*)mTmpValue.Get();
    if (value == NULL) {
        CTypedValue::NewByFriend((CTypedValue**)&mTmpValue);
        value = (ITypedValue*)mTmpValue.Get();
    }

    FAIL_RETURN(GetValue(id, value, TRUE))
    if (mTmpValue->mType == ITypedValue::TYPE_FRACTION) {
        *fraction = CTypedValue::ComplexToFraction(mTmpValue->mData, base, pbase);
        return NOERROR;
    }

    Logger::E(TAG, "Resource ID #0x%08x type #0x%08x is not valid", id, mTmpValue->mType);
    *fraction = 0.0f;
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetDrawable(
    /* [in] */ Int32 id,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)

    AutoPtr<IDrawable> d;
    GetDrawable(id, NULL, (IDrawable**)&d);
    Boolean can;
    d->CanApplyTheme(&can);
    if (can) {
        // Log.w(TAG, "Drawable " + getResourceName(id) + " has unresolved theme "
        //         + "attributes! Consider using Resources.getDrawable(Int32, Theme) or "
        //         + "Context.getDrawable(Int32).", new RuntimeException());
    }

    *drawable = d;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode CResources::GetDrawable(
    /* [in] */ Int32 id,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    return GetDrawable(id, theme, TRUE, drawable);
}

ECode CResources::GetDrawable(
    /* [in] */ Int32 id,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ Boolean supportComposedIcons,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    *drawable = NULL;

    //Check if an icon is themed
    AutoPtr<IPackageItemInfo> info;
    if (mIcons != NULL) {
        AutoPtr<IInterface> value;
        mIcons->Get(id, (IInterface**)&value);
        info = IPackageItemInfo::Probe(value);
    }
    Int32 themedIcon;
    if (info != NULL && (info->GetThemedIcon(&themedIcon), themedIcon != 0)) {
        id = themedIcon;
    }

    AutoPtr<ITypedValue> value;
    synchronized(mAccessLock) {
        value = (ITypedValue*)mTmpValue.Get();
        if (value == NULL) {
            CTypedValue::New((ITypedValue**)&value);
        }
        else {
            mTmpValue = NULL;
        }
        GetValue(id, value, TRUE, supportComposedIcons);
    }

    AutoPtr<IDrawable> res;
    // try {
    ECode ec = LoadDrawable(value, id, theme, (IDrawable**)&res);
    if (ec == (ECode)E_NOT_FOUND_EXCEPTION) {
        // The below statement will be true if we were trying to load a composed icon.
        // Since we received a NotFoundException, try to load the original if this
        // condition is true, otherwise throw the original exception.
        if (supportComposedIcons && mComposedIconInfo != NULL && info != NULL &&
                (info->GetThemedIcon(&themedIcon), themedIcon == 0)) {
            Logger::E(TAG, "Failed to retrieve composed icon. 0x%08x", ec);
            GetValue(id, value, TRUE, FALSE);
            LoadDrawable(value, id, theme, (IDrawable**)&res);
        }
        else {
            return ec;
        }
    }
    // } catch (NotFoundException e) {
    //     // The below statement will be true if we were trying to load a composed icon.
    //     // Since we received a NotFoundException, try to load the original if this
    //     // condition is true, otherwise throw the original exception.
    //     if (supportComposedIcons && mComposedIconInfo != null && info != null &&
    //             info.themedIcon == 0) {
    //         Log.e(TAG, "Failed to retrieve composed icon.", e);
    //         getValue(id, value, true, false);
    //         res = loadDrawable(value, id, theme);
    //     } else {
    //         throw e;
    //     }
    // }

    synchronized(mAccessLock) {
        if (mTmpValue == NULL) {
            mTmpValue = (CTypedValue*)value.Get();
        }
    }

    *drawable = res;
    REFCOUNT_ADD(*drawable)
    return NOERROR;
}

ECode CResources::GetDrawableForDensity(
    /* [in] */ Int32 id,
    /* [in] */ Int32 density,
    /* [out] */ IDrawable** drawable)
{
    return GetDrawableForDensity(id, density, NULL, drawable);
}

ECode CResources::GetDrawableForDensity(
    /* [in] */ Int32 id,
    /* [in] */ Int32 density,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    return GetDrawableForDensity(id, density, theme, TRUE, drawable);
}

ECode CResources::GetDrawableForDensity(
    /* [in] */ Int32 id,
    /* [in] */ Int32 density,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ Boolean supportComposedIcons,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    *drawable = NULL;

    //Check if an icon was themed
    AutoPtr<IPackageItemInfo> info;
    if (mIcons != NULL) {
        AutoPtr<IInterface> value;
        mIcons->Get(id, (IInterface**)&value);
        info = IPackageItemInfo::Probe(value);
    }
    Int32 themedIcon;
    if (info != NULL && (info->GetThemedIcon(&themedIcon), themedIcon != 0)) {
        id = themedIcon;
    }

    AutoLock lock(mAccessLock);
    AutoPtr<ITypedValue> value = (ITypedValue*)mTmpValue.Get();
    if (value == NULL) {
        CTypedValue::New((ITypedValue**)&value);
    }
    else {
        mTmpValue = NULL;
    }

    FAIL_RETURN(GetValueForDensity(id, density, value, TRUE, supportComposedIcons))

    /*
     * Pretend the requested density is actually the display density. If
     * the drawable returned is not the requested density, then force it
     * to be scaled later by dividing its density by the ratio of
     * requested density to actual device density. Drawables that have
     * undefined density or no density don't need to be handled here.
     */
    if (mTmpValue->mDensity > 0 && mTmpValue->mDensity != ITypedValue::DENSITY_NONE) {
        if (mTmpValue->mDensity == density) {
            mTmpValue->mDensity = mMetrics->mDensityDpi;
        }
        else {
            mTmpValue->mDensity = (mTmpValue->mDensity * mMetrics->mDensityDpi) / density;
        }
    }

    AutoPtr<IDrawable> res;
    LoadDrawable(value, id, theme, (IDrawable**)&res);

    synchronized(mAccessLock) {
        if (mTmpValue == NULL) {
            mTmpValue = (CTypedValue*)value.Get();
        }
    }

    *drawable = res;
    REFCOUNT_ADD(*drawable)
    return NOERROR;
}

ECode CResources::GetMovie(
    /* [in] */ Int32 id,
    /* [out] */ IMovie** movie)
{
    VALIDATE_NOT_NULL(movie);

    AutoPtr<IInputStream> is;
    OpenRawResource(id, (IInputStream**)&is);

    ECode ec = Movie::DecodeStream(is, movie);
    is->Close(); // don't care, since the return value is valid

    return ec;
}

ECode CResources::GetColor(
    /* [in] */ Int32 id,
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);


    AutoPtr<ITypedValue> value;

    synchronized(mAccessLock) {
        value = (ITypedValue*)mTmpValue.Get();
        if (value == NULL) {
            CTypedValue::New((ITypedValue**)&value);
        }

        AutoPtr<ITypedValue> value = (ITypedValue*)mTmpValue.Get();
        FAIL_RETURN(GetValue(id, value, TRUE))
        if (mTmpValue->mType >= ITypedValue::TYPE_FIRST_INT
                && mTmpValue->mType <= ITypedValue::TYPE_LAST_INT) {
            *color = mTmpValue->mData;
            mTmpValue = (CTypedValue*)value.Get();
            return NOERROR;
        }
        else if (mTmpValue->mType != ITypedValue::TYPE_STRING) {
            Logger::E(TAG, "Resource ID #0x%08x type #0x%08x is not valid", id, mTmpValue->mType);
            *color = 0;
            mTmpValue = NULL;
            return E_NOT_FOUND_EXCEPTION;
        }
    }

    AutoPtr<IColorStateList> csl;
    ASSERT_SUCCEEDED(LoadColorStateList(value, id, (IColorStateList**)&csl));
    synchronized(mAccessLock) {
        if (mTmpValue == NULL) {
            mTmpValue = (CTypedValue*)value.Get();
        }
    }
    return csl->GetDefaultColor(color);
}

ECode CResources::GetColorStateList(
    /* [in] */ Int32 id,
    /* [out] */ IColorStateList** list)
{
    VALIDATE_NOT_NULL(list);

    AutoPtr<ITypedValue> value;

    synchronized(mAccessLock) {
        value = (ITypedValue*)mTmpValue.Get();
        if (value == NULL) {
            CTypedValue::New((ITypedValue**)&value);
        }
        else {
            mTmpValue = NULL;
        }

        FAIL_RETURN(GetValue(id, value, TRUE))
    }

    AutoPtr<IColorStateList> csl;
    ASSERT_SUCCEEDED(LoadColorStateList(value, id, (IColorStateList**)&csl));
    synchronized(mAccessLock) {
        if (mTmpValue == NULL) {
            mTmpValue = (CTypedValue*)value.Get();
        }
    }

    *list = csl;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

ECode CResources::GetBoolean(
    /* [in] */ Int32 id,
    /* [out] */ Boolean* b)
{
    VALIDATE_NOT_NULL(b);

    AutoLock lock(mAccessLock);

    AutoPtr<ITypedValue> value = (ITypedValue*)mTmpValue.Get();
    if (value == NULL) {
        CTypedValue::NewByFriend((CTypedValue**)&mTmpValue);
        value = (ITypedValue*)mTmpValue.Get();
    }

    FAIL_RETURN(GetValue(id, value, TRUE))
    if (mTmpValue->mType >= ITypedValue::TYPE_FIRST_INT
            && mTmpValue->mType <= ITypedValue::TYPE_LAST_INT) {
        *b = mTmpValue->mData != 0;
        return NOERROR;
    }

    Logger::E(TAG, "Resource ID #0x%08x type #0x%08x is not valid", id, mTmpValue->mType);
    *b = FALSE;
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetInteger(
    /* [in] */ Int32 id,
    /* [out] */ Int32* i)
{
    VALIDATE_NOT_NULL(i);

    AutoLock lock(mAccessLock);

    AutoPtr<ITypedValue> value = (ITypedValue*)mTmpValue.Get();
    if (value == NULL) {
        CTypedValue::NewByFriend((CTypedValue**)&mTmpValue);
        value = (ITypedValue*)mTmpValue.Get();
    }

    FAIL_RETURN(GetValue(id, value, TRUE))
    if (mTmpValue->mType >= ITypedValue::TYPE_FIRST_INT
            && mTmpValue->mType <= ITypedValue::TYPE_LAST_INT) {
        *i = mTmpValue->mData;
        return NOERROR;
    }

    Logger::E(TAG, "Resource ID #0x%08x type #0x%08x is not valid", id, mTmpValue->mType);
    *i = 0;
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetFloat(
    /* [in] */ Int32 id,
    /* [out] */ Float* f)
{
    VALIDATE_NOT_NULL(f)
    *f = 0.0f;

    AutoLock lock(mAccessLock);

    AutoPtr<ITypedValue> value = (ITypedValue*)mTmpValue.Get();
    if (value == NULL) {
        CTypedValue::NewByFriend((CTypedValue**)&mTmpValue);
        value = (ITypedValue*)mTmpValue.Get();
    }

    FAIL_RETURN(GetValue(id, value, TRUE))
    if (mTmpValue->mType == ITypedValue::TYPE_FLOAT) {
        return mTmpValue->GetFloat(f);
    }

    Logger::E(TAG, "Resource ID #0x%08x type #0x%08x is not valid", id, mTmpValue->mType);
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetLayout(
    /* [in] */ Int32 id,
    /* [out] */ IXmlResourceParser** parser)
{
    VALIDATE_NOT_NULL(parser);
    return LoadXmlResourceParser(id, String("layout"), parser);
}

ECode CResources::GetAnimation(
    /* [in] */ Int32 id,
    /* [out] */ IXmlResourceParser** animation)
{
    VALIDATE_NOT_NULL(animation);
    return LoadXmlResourceParser(id, String("anim"), animation);
}

ECode CResources::GetXml(
    /* [in] */ Int32 id,
    /* [out] */ IXmlResourceParser** xml)
{
    VALIDATE_NOT_NULL(xml);
    return LoadXmlResourceParser(id, String("xml"), xml);
}

ECode CResources::OpenRawResource(
    /* [in] */ Int32 id,
    /* [out] */ IInputStream** res)
{
    VALIDATE_NOT_NULL(res);

    AutoPtr<ITypedValue> value;
    synchronized(mAccessLock) {
        value = (ITypedValue*)mTmpValue.Get();
        if (value == NULL) {
            CTypedValue::New((ITypedValue**)&value);
        }
        else {
            mTmpValue = NULL;
        }
    }

    AutoPtr<IInputStream> is;
    OpenRawResource(id, value, (IInputStream**)&is);
    synchronized(mAccessLock) {
        if (mTmpValue == NULL) {
            mTmpValue = (CTypedValue*)value.Get();
        }
    }
    *res = is;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CResources::OpenRawResource(
    /* [in] */ Int32 id,
    /* [in] */ ITypedValue* value,
    /* [out] */ IInputStream** res)
{
    VALIDATE_NOT_NULL(res);

    FAIL_RETURN(GetValue(id, value, TRUE))

//    try {
    String str;
    ((CTypedValue*)value)->mString->ToString(&str);
    return mAssets->OpenNonAsset(
            ((CTypedValue*)value)->mAssetCookie,
            str, IAssetManager::ACCESS_STREAMING, res);
//    } catch (Exception e) {
//        NotFoundException rnf = new NotFoundException("File " + value.string.toString() +
//                " from drawable resource ID #0x" + Integer.toHexString(id));
//        rnf.initCause(e);
//        throw rnf;
//    }
}

ECode CResources::OpenRawResourceFd(
    /* [in] */ Int32 id,
    /* [out] */ IAssetFileDescriptor** des)
{
    VALIDATE_NOT_NULL(des);

    AutoPtr<ITypedValue> value;
    synchronized(mAccessLock) {
        value = mTmpValue;
        if (value == NULL) {
            CTypedValue::New((ITypedValue**)&value);
        }
        else {
            mTmpValue = NULL;
        }

        FAIL_RETURN(GetValue(id, value, TRUE))
    }

//    try {
    String str;
    ((CTypedValue*)value.Get())->mString->ToString(&str);
    ECode ec = mAssets->OpenNonAssetFd(
            ((CTypedValue*)value.Get())->mAssetCookie, str, des);
//    } catch (Exception e) {
//        NotFoundException rnf = new NotFoundException(
//            "File " + value.string.toString()
//            + " from drawable resource ID #0x"
//            + Integer.toHexString(id));
//        rnf.initCause(e);
//        throw rnf;
//    }
    synchronized(mAccessLock) {
        if (mTmpValue == NULL) {
            mTmpValue = (CTypedValue*)value.Get();
        }
    }
    return ec;
}

ECode CResources::GetValue(
    /* [in] */ Int32 id,
    /* [in, out] */ ITypedValue* outValue,
    /* [in] */ Boolean resolveRefs)
{
    return GetValue(id, outValue, resolveRefs, TRUE);
}

ECode CResources::GetValue(
    /* [in] */ Int32 id,
    /* [in, out] */ ITypedValue* outValue,
    /* [in] */ Boolean resolveRefs,
    /* [in] */ Boolean supportComposedIcons)
{
    VALIDATE_NOT_NULL(outValue)

    //Check if an icon was themed
    AutoPtr<IPackageItemInfo> info;
    if (mIcons != NULL) {
        AutoPtr<IInterface> value;
        mIcons->Get(id, (IInterface**)&value);
        info = IPackageItemInfo::Probe(value);
    }
    Int32 themedIcon;
    if (info != NULL && (info->GetThemedIcon(&themedIcon), themedIcon != 0)) {
        id = themedIcon;
    }

    Boolean result;
    if (mAssets->GetResourceValue(id, 0, outValue, resolveRefs)) {
        if (supportComposedIcons && (IconPackHelper::ShouldComposeIcon(mComposedIconInfo, &result), result)
                && info != NULL && (info->GetThemedIcon(&themedIcon), themedIcon == 0)) {
            AutoPtr<IDrawable> dr;
            LoadDrawable(outValue, id, NULL, (IDrawable**)&dr);
            IconPackHelper::IconCustomizer::GetValue(this, id, outValue, dr);
        }
        return NOERROR;
    }
    Logger::E(TAG, "Resource ID #0x%08x", id);
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetValueForDensity(
    /* [in] */ Int32 id,
    /* [in] */ Int32 density,
    /* [in] */ ITypedValue* outValue,
    /* [in] */ Boolean resolveRefs)
{
    return GetValueForDensity(id, density, outValue, resolveRefs, TRUE);
}

ECode CResources::GetValueForDensity(
    /* [in] */ Int32 id,
    /* [in] */ Int32 density,
    /* [in] */ ITypedValue* outValue,
    /* [in] */ Boolean supportComposedIcons,
    /* [in] */ Boolean resolveRefs)
{
    //Check if an icon was themed
    AutoPtr<IPackageItemInfo> info;
    if (mIcons != NULL) {
        AutoPtr<IInterface> value;
        mIcons->Get(id, (IInterface**)&value);
        info = IPackageItemInfo::Probe(value);
    }
    Int32 themedIcon;
    if (info != NULL && (info->GetThemedIcon(&themedIcon), themedIcon != 0)) {
        id = themedIcon;
    }

    if (mAssets->GetResourceValue(id, density, outValue, resolveRefs)) {
        Boolean result;
        if (supportComposedIcons && (IconPackHelper::ShouldComposeIcon(mComposedIconInfo, &result), result) &&
                info != NULL && (info->GetThemedIcon(&themedIcon), themedIcon == 0)) {
            Int32 outDensity;
            outValue->GetDensity(&outDensity);
            Int32 tmpDensity = outDensity;
            /*
             * Pretend the requested density is actually the display density. If
             * the drawable returned is not the requested density, then force it
             * to be scaled later by dividing its density by the ratio of
             * requested density to actual device density. Drawables that have
             * undefined density or no density don't need to be handled here.
             */
            if ((outValue->GetDensity(&outDensity), outDensity > 0) && outDensity != ITypedValue::DENSITY_NONE) {
                if (outDensity == density) {
                    outValue->SetDensity(mMetrics->mDensityDpi);
                }
                else {
                    outValue->SetDensity((outDensity * mMetrics->mDensityDpi) / density);
                }
            }
            AutoPtr<IDrawable> dr;
            LoadDrawable(outValue, id, NULL, (IDrawable**)&dr);

            // Return to original density. If we do not do this then
            // the caller will get the wrong density for the given id and perform
            // more of its own scaling in loadDrawable
            outValue->SetDensity(tmpDensity);
            IconPackHelper::IconCustomizer::GetValue(this, id, outValue, dr);
        }
        return NOERROR;
    }
    Logger::E(TAG, "Resource ID #0x%08x", id);
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetValue(
    /* [in] */ const String& name,
    /* [in, out] */ ITypedValue* outValue,
    /* [in] */ Boolean resolveRefs)
{
    VALIDATE_NOT_NULL(outValue);

    Int32 id;
    GetIdentifier(name, String("string"), String(NULL), &id);
    if (id != 0) {
        return GetValue(id, outValue, resolveRefs);
    }
    Logger::E(TAG, "String resource name %s", (const char*)name);
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::NewTheme(
    /* [out] */ IResourcesTheme** theme)
{
    VALIDATE_NOT_NULL(theme);

    *theme = new CResources::Theme(this);
    REFCOUNT_ADD(*theme);
    return NOERROR;
}

ECode CResources::ObtainAttributes(
    /* [in] */ IAttributeSet* set,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** array)
{
    VALIDATE_NOT_NULL(array);
    *array = NULL;

    Int32 len = attrs->GetLength();
    AutoPtr<ITypedArray> ta = CTypedArray::Obtain(this, len);
    CTypedArray* temp = (CTypedArray*)ta.Get();

    // XXX note that for now we only work with compiled XML files.
    // To support generic XML files we will need to manually parse
    // out the attributes from the XML file (applying type information
    // contained in the resources and such).
    AutoPtr<XmlBlock::Parser> parser = (XmlBlock::Parser*)set;
    Boolean result;
    ECode ec = mAssets->RetrieveAttributes(
        parser->mParseState, *attrs, temp->mData, temp->mIndices, &result);
    if (FAILED(ec)) {
        Logger::E(TAG, "failed to RetrieveAttributes!");
        return ec;
    }

    temp->mXml = parser;

    *array = ta.Get();
    REFCOUNT_ADD(*array);

    return NOERROR;
}

ECode CResources::UpdateConfiguration(
    /* [in] */ IConfiguration* config,
    /* [in] */ IDisplayMetrics* metrics)
{
    return UpdateConfiguration(config, metrics, NULL);
}

ECode CResources::UpdateConfiguration(
    /* [in] */ IConfiguration* config,
    /* [in] */ IDisplayMetrics* metrics,
    /* [in] */ ICompatibilityInfo* compat)
{
    {
        AutoLock lock(mAccessLock);

        // if (false) {
        //     Slog.i(TAG, "**** Updating config of " + this + ": old config is "
        //             + mConfiguration + " old compat is " + mCompatibilityInfo);
        //     Slog.i(TAG, "**** Updating config of " + this + ": new config is "
        //             + config + " new compat is " + compat);
        // }

        if (compat != NULL) {
            mCompatibilityInfo = (CCompatibilityInfo*)compat;
        }
        if (metrics != NULL) {
            mMetrics->SetTo(metrics);
        }

        // NOTE: We should re-arrange this code to create a Display
        // with the CompatibilityInfo that is used everywhere we deal
        // with the display in relation to this app, rather than
        // doing the conversion here.  This impl should be okay because
        // we make sure to return a compatible display in the places
        // where there are public APIs to retrieve the display...  but
        // it would be cleaner and more maintainble to just be
        // consistently dealing with a compatible display everywhere in
        // the framework.
        mCompatibilityInfo->ApplyToDisplayMetrics(mMetrics);

        Int32 configChanges = 0xfffffff;
        if (config != NULL) {
            mTmpConfig->SetTo(config);
            Int32 density;
            config->GetDensityDpi(&density);
            if (density == IConfiguration::DENSITY_DPI_UNDEFINED) {
                mMetrics->GetNoncompatDensityDpi(&density);
            }

            mCompatibilityInfo->ApplyToConfiguration(density, mTmpConfig);

            if (mTmpConfig->mLocale == NULL) {
                AutoPtr<ILocaleHelper> helper;
                CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
                helper->GetDefault((ILocale**)&mTmpConfig->mLocale);
                mTmpConfig->SetLayoutDirection(mTmpConfig->mLocale);
            }
            mConfiguration->UpdateFrom(mTmpConfig, &configChanges);

            /* This is ugly, but modifying the activityInfoConfigToNative
             * adapter would be messier */
            if ((configChanges & IActivityInfo::CONFIG_THEME_RESOURCE) != 0) {
                configChanges = CActivityInfo::ActivityInfoConfigToNative(configChanges);
                configChanges |= IActivityInfo::CONFIG_THEME_RESOURCE;
            }
            else {
                configChanges = CActivityInfo::ActivityInfoConfigToNative(configChanges);
            }
        }
        if (mConfiguration->mLocale == NULL) {
            AutoPtr<ILocaleHelper> helper;
            CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
            helper->GetDefault((ILocale**)&mConfiguration->mLocale);
            mConfiguration->SetLayoutDirection(mConfiguration->mLocale);
        }
        if (mConfiguration->mDensityDpi != IConfiguration::DENSITY_DPI_UNDEFINED) {
            mMetrics->mDensityDpi = mConfiguration->mDensityDpi;
            mMetrics->mDensity = mConfiguration->mDensityDpi * IDisplayMetrics::DENSITY_DEFAULT_SCALE;
        }
        mMetrics->mScaledDensity = mMetrics->mDensity * mConfiguration->mFontScale;

        String locale;
        if (mConfiguration->mLocale != NULL) {
            String tag;
            mConfiguration->mLocale->ToLanguageTag(&tag);
            locale = AdjustLanguageTag(tag);
        }
        Int32 width, height;
        if (mMetrics->mWidthPixels >= mMetrics->mHeightPixels) {
            width = mMetrics->mWidthPixels;
            height = mMetrics->mHeightPixels;
        }
        else {
            //noinspection SuspiciousNameCombination
            width = mMetrics->mHeightPixels;
            //noinspection SuspiciousNameCombination
            height = mMetrics->mWidthPixels;
        }
        Int32 keyboardHidden = mConfiguration->mKeyboardHidden;
        if (keyboardHidden == IConfiguration::KEYBOARDHIDDEN_NO
                && mConfiguration->mHardKeyboardHidden
                        == IConfiguration::HARDKEYBOARDHIDDEN_YES) {
            keyboardHidden = IConfiguration::KEYBOARDHIDDEN_SOFT;
        }

        mAssets->SetConfiguration(mConfiguration->mMcc, mConfiguration->mMnc,
                locale, mConfiguration->mOrientation,
                mConfiguration->mTouchscreen, mConfiguration->mDensityDpi,
                mConfiguration->mKeyboard, keyboardHidden,
                mConfiguration->mNavigation, width, height,
                mConfiguration->mSmallestScreenWidthDp,
                mConfiguration->mScreenWidthDp, mConfiguration->mScreenHeightDp,
                mConfiguration->mScreenLayout, mConfiguration->mUiMode,
                Build::VERSION::RESOURCES_SDK_INT);

        if (DEBUG_CONFIG) {
            Logger::I(TAG, "**** Updating config of %p: config is %p compat is %p",
                    this, mConfiguration.Get(), mCompatibilityInfo.Get());
        }

        ClearDrawableCachesLocked(mDrawableCache, configChanges);
        ClearDrawableCachesLocked(mColorDrawableCache, configChanges);
        mColorStateListCache.Clear();
        FlushLayoutCache();
    }

    {
        AutoLock lock(this);
        if (mPluralRule != NULL) {
            AutoPtr<INativePluralRulesHelper> helper;
            CNativePluralRulesHelper::AcquireSingleton((INativePluralRulesHelper**)&helper);
            AutoPtr<ILocale> locale;
            assert(config != NULL);
            config->GetLocale((ILocale**)&locale);
            helper->ForLocale(locale, (INativePluralRules**)&mPluralRule);
        }
    }

    return NOERROR;
}

void CResources::ClearDrawableCachesLocked(
    /* [in] */ DrawableMap& cache,
    /* [in] */ Int32 configChanges)
{
    DrawableMapIterator it = cache.Begin();
    for (; it != cache.End(); ++it) {
        ClearDrawableCacheLocked(it->mSecond, configChanges);
    }
}

void CResources::ClearDrawableCacheLocked(
    /* [in] */ IInt64SparseArray* cache,
    /* [in] */ Int32 configChanges)
{
    /*
     * Quick test to find out if the config change that occurred should
     * trigger a full cache wipe.
     */
    if (CConfiguration::NeedNewResources(configChanges, 0)) {
        if (DEBUG_CONFIG) {
            Logger::D(TAG, "Clear drawable cache from config changes: 0x%s",
                    StringUtils::ToHexString(configChanges).string());
        }
        cache->Clear();
        return;
    }

    if (DEBUG_CONFIG) {
        Logger::D(TAG, "Cleaning up drawables config changes: 0x%08x", configChanges);
    }

    Int32 N;
    cache->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> value;
        cache->ValueAt(i, (IInterface**)&value);
        IWeakReference* wr = IWeakReference::Probe(value);
        if (wr != NULL) {
            AutoPtr<IDrawableConstantState> cs;
            wr->Resolve(EIID_IDrawableConstantState, (IInterface**)&cs);
            if (cs != NULL) {
                Int32 con;
                cs->GetChangingConfigurations(&con);
                if (CConfiguration::NeedNewResources(configChanges, con)) {
                    if (DEBUG_CONFIG) {
                        Int64 key;
                        cache->KeyAt(i, &key);
                        Logger::D(TAG, "FLUSHING #0x%08x /%p with changes: 0x%08x", key, cs.Get(), con);
                    }
                    cache->SetValueAt(i, NULL);
                }
                else if (DEBUG_CONFIG) {
                    Int64 key;
                    cache->KeyAt(i, &key);
                    Logger::D(TAG, "(Keeping #0x%08x /%p with changes: 0x%08x", key, cs.Get(), con);
                }
            }
        }
    }
}

String CResources::AdjustLanguageTag(
    /* [in] */ const String& languageTag)
{
    Int32 separator = languageTag.IndexOf('-');
    String language;
    String remainder;

    if (separator == -1) {
        language = languageTag;
        remainder = "";
    }
    else {
        language = languageTag.Substring(0, separator);
        remainder = languageTag.Substring(separator);
    }

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    String l;
    helper->AdjustLanguageCode(language, &l);

    StringBuilder sb(l);
    sb += remainder;
    return sb.ToString();
}

void CResources::UpdateSystemConfiguration(
    /* [in] */ IConfiguration* config,
    /* [in] */ IDisplayMetrics* metrics,
    /* [in] */ ICompatibilityInfo* compat)
{
    if (sSystem != NULL) {
        sSystem->UpdateConfiguration(config, metrics, compat);
        if (DEBUG_CONFIG)
            Logger::V(TAG, "Updated system resources config.");
    }
}

ECode CResources::GetDisplayMetrics(
    /* [out] */ IDisplayMetrics** metrics)
{
    VALIDATE_NOT_NULL(metrics);

    if (DEBUG_CONFIG) Logger::V(TAG, "Returning DisplayMetrics: %d x %d %f",
            mMetrics->mWidthPixels, mMetrics->mHeightPixels, mMetrics->mDensity);
    *metrics = (IDisplayMetrics*)mMetrics.Get();
    REFCOUNT_ADD(*metrics);
    return NOERROR;
}

ECode CResources::GetConfiguration(
    /* [out] */ IConfiguration** config)
{
    VALIDATE_NOT_NULL(config);

    *config = (IConfiguration*)mConfiguration.Get();
    REFCOUNT_ADD(*config);
    return NOERROR;
}

ECode CResources::GetCompatibilityInfo(
    /* [out] */ ICompatibilityInfo** ci)
{
    VALIDATE_NOT_NULL(ci);

    *ci = mCompatibilityInfo;
    REFCOUNT_ADD(*ci);
    return NOERROR;
}

ECode CResources::SetCompatibilityInfo(
    /* [in] */ ICompatibilityInfo* ci)
{
    if (ci != NULL) {
        mCompatibilityInfo = ci;
        return UpdateConfiguration((IConfiguration*)mConfiguration.Get(),
                (IDisplayMetrics*)mMetrics.Get());
    }
    return NOERROR;
}

ECode CResources::GetIdentifier(
    /* [in] */ const String& name,
    /* [in] */ const String& defType,
    /* [in] */ const String& defPackage,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = 0;

    if (name.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    ECode ec = StringUtils::Parse(name, id);
    if (SUCCEEDED(ec)) {
        return NOERROR;
    }

    *id = mAssets->GetResourceIdentifier(name, defType, defPackage);
    return NOERROR;
}

Boolean CResources::ResourceHasPackage(
    /* [in] */ Int32 resid)
{
    return (resid >> 24) != 0;
}

ECode CResources::GetResourceName(
    /* [in] */ Int32 resid,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mAssets->GetResourceName(resid);
    if (!name->IsNull()) {
        return NOERROR;
    }

    Logger::E(TAG, "Unable to find resource ID #0x%08x", resid);
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetResourcePackageName(
    /* [in] */ Int32 resid,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mAssets->GetResourcePackageName(resid);
    if (!name->IsNull()) {
        return NOERROR;
    }

    Logger::E(TAG, "Unable to find resource ID #0x%08x", resid);
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetResourceTypeName(
    /* [in] */ Int32 resid,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mAssets->GetResourceTypeName(resid);
    if (!name->IsNull()) {
        return NOERROR;
    }

    Logger::E(TAG, "Unable to find resource ID #0x%08x", resid);
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::GetResourceEntryName(
    /* [in] */ Int32 resid,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mAssets->GetResourceEntryName(resid);
    if (!name->IsNull()) {
        return NOERROR;
    }

    Logger::E(TAG, "Unable to find resource ID #0x%08x", resid);
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::ParseBundleExtras(
    /* [in] */ IXmlResourceParser* parser,
    /* [in, out] */ IBundle* outBundle)
{
    VALIDATE_NOT_NULL(outBundle);

    IXmlPullParser* xpp = IXmlPullParser::Probe(parser);

    Int32 outerDepth;
    xpp->GetDepth(&outerDepth);
    Int32 type;
    Int32 depth;
    while ((xpp->Next(&type), type != IXmlPullParser::END_DOCUMENT)
            && (type != IXmlPullParser::END_TAG || (xpp->GetDepth(&depth), depth > outerDepth))) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        String nodeName;
        xpp->GetName(&nodeName);
        if (nodeName.Equals("extra")) {
            ParseBundleExtra(nodeName, IAttributeSet::Probe(parser), outBundle);
            XmlUtils::SkipCurrentTag(xpp);
        }
        else {
            XmlUtils::SkipCurrentTag(xpp);
        }
    }

    return NOERROR;
}

ECode CResources::ParseBundleExtra(
    /* [in] */ const String& tagName,
    /* [in] */ IAttributeSet* attrs,
    /* [in, out] */ IBundle* outBundle)
{
    VALIDATE_NOT_NULL(outBundle);

    assert(attrs != NULL);

    Int32 size = ArraySize(R::styleable::Extra);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::Extra, size);

    AutoPtr<ITypedArray> sa;
    ObtainAttributes(attrs, layout, (ITypedArray**)&sa);

    String name;
    sa->GetString(R::styleable::Extra_name, &name);
    if (name.IsNull()) {
        sa->Recycle();
        String s;
        attrs->GetPositionDescription(&s);
        Logger::E(TAG, "<%s> requires an android:name attribute at %s",
                (const char*)tagName, (const char*)s);
        // throw new XmlPullParserException("<" + tagName
        //         + "> requires an android:name attribute at "
        //         + attrs.getPositionDescription());
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    AutoPtr<ITypedValue> v;
    sa->PeekValue(R::styleable::Extra_value, (ITypedValue**)&v);
    if (v != NULL) {
        Int32 type;
        v->GetType(&type);
        if (type == ITypedValue::TYPE_STRING) {
            AutoPtr<ICharSequence> cs;
            v->CoerceToString((ICharSequence**)&cs);
            // TODO: ALEX need PutCharSequence
            outBundle->PutCharSequence(name, cs);
        }
        else if (type == ITypedValue::TYPE_INT_BOOLEAN) {
            Int32 data;
            v->GetData(&data);
            outBundle->PutBoolean(name, (data != 0));
        }
        else if (type >= ITypedValue::TYPE_FIRST_INT && type <= ITypedValue::TYPE_LAST_INT) {
            Int32 data;
            v->GetData(&data);
            outBundle->PutInt32(name, data);
        }
        else if (type == ITypedValue::TYPE_FLOAT) {
            Float data;
            v->GetFloat(&data);
            outBundle->PutFloat(name, data);
        }
        else {
            sa->Recycle();
//          throw new XmlPullParserException("<" + tagName
//                  + "> only supports string, integer, float, color, and Boolean at "
//                  + attrs.getPositionDescription());
            return E_XML_PULL_PARSER_EXCEPTION;
        }
    }
    else {
        sa->Recycle();
//        throw new XmlPullParserException("<" + tagName
//                + "> requires an android:value or android:resource attribute at "
//                + attrs.getPositionDescription());
        String s;
        attrs->GetPositionDescription(&s);
        Logger::E(TAG, "<%s> requires an android:name attribute at %s",
                (const char*)tagName, (const char*)s);
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    sa->Recycle();

    return NOERROR;
}

ECode CResources::GetAssets(
    /* [out] */ IAssetManager** manager)
{
    VALIDATE_NOT_NULL(manager);

    *manager = (IAssetManager*)mAssets.Get();
    REFCOUNT_ADD(*manager);
    return NOERROR;
}

ECode CResources::FlushLayoutCache()
{
    AutoLock lock(mCachedXmlBlockIdsLock);

    // First see if this block is in our cache.
    for (Int32 i = 0; i < mCachedXmlBlockIds->GetLength(); i++) {
        (*mCachedXmlBlockIds)[i] = -0;
        AutoPtr<XmlBlock> oldBlock = (*mCachedXmlBlocks)[i];
        if (oldBlock != NULL) {
            oldBlock->Close();
            oldBlock = NULL;
        }
        mCachedXmlBlocks->Set(i, NULL);
    }

    return NOERROR;
}

ECode CResources::StartPreloading()
{
    AutoLock lock(this);

    if (sPreloaded) {
        Logger::E(TAG, "Resources already preloaded");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    sPreloaded = TRUE;
    mPreloading = TRUE;
    sPreloadedDensity = CDisplayMetrics::DENSITY_DEVICE;
    mConfiguration->mDensityDpi = sPreloadedDensity;
    UpdateConfiguration(NULL, NULL);
    return NOERROR;
}

ECode CResources::FinishPreloading()
{
    if (mPreloading) {
        mPreloading = FALSE;
        FlushLayoutCache();
    }
    return NOERROR;
}

ECode CResources::GetPreloadedDrawables(
    /* [out] */ IInt64SparseArray** drawables)
{
    VALIDATE_NOT_NULL(drawables);
    *drawables = (*sPreloadedDrawables)[0];
    REFCOUNT_ADD(*drawables);
    return NOERROR;
}

Boolean CResources::VerifyPreloadConfig(
    /* [in] */ Int32 changingConfigurations,
    /* [in] */ Int32 allowVarying,
    /* [in] */ Int32 resourceId,
    /* [in] */ const String& name)
{
    // We allow preloading of resources even if they vary by font scale (which
    // doesn't impact resource selection) or density (which we handle specially by
    // simply turning off all preloading), as well as any other configs specified
    // by the caller.
    if (((changingConfigurations & ~(IActivityInfo::CONFIG_FONT_SCALE |
            IActivityInfo::CONFIG_DENSITY)) & ~allowVarying) != 0) {
        String resName;
        ECode ec = GetResourceName(resourceId, &resName);
        if (ec == (ECode)E_NOT_FOUND_EXCEPTION) {
            resName = "?";
        }
        // This should never happen in production, so we should log a
        // warning even if we're not debugging.
        Logger::W(TAG, "Preloaded %s resource #0x%08x (%s) that varies with configuration!!",
            name.string(), resourceId, resName.string());
        return FALSE;
    }

    if (TRACE_FOR_PRELOAD) {
        String resName;
        ECode ec = GetResourceName(resourceId, &resName);
        if (ec == (ECode)E_NOT_FOUND_EXCEPTION) {
            resName = "?";
        }

        Logger::W(TAG, "Preloaded %s resource #0x%08x (%s) that varies with configuration!!",
            name.string(), resourceId, resName.string());
    }
    return TRUE;
}

ECode CResources::UpdateStringCache()
{
    synchronized (mAccessLock) {
        mAssets->RecreateStringBlocks();
    }
    return NOERROR;
}

ECode CResources::LoadDrawable(
    /* [in] */ ITypedValue* typedValue,
    /* [in] */ Int32 id,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = NULL;

    CTypedValue* value = (CTypedValue*)typedValue;

    if (TRACE_FOR_PRELOAD) {
        // Log only framework resources
        if (((unsigned Int32)id >> 24) == 0x1) {
            String name;
            GetResourceName(id, &name);
            Logger::D(TAG, "PreloadDrawable %s", name.string());
        }
    }

    Boolean isColorDrawable;
    DrawableMap* caches = NULL;

    Int64 key;
    if (value->mType >= ITypedValue::TYPE_FIRST_COLOR_INT
            && value->mType <= ITypedValue::TYPE_LAST_COLOR_INT) {
        isColorDrawable = TRUE;
        caches = &mColorDrawableCache;
        key = value->mData;
    }
    else {
        isColorDrawable = FALSE;
        caches = &mDrawableCache;
        key = (((Int64) value->mAssetCookie) << 32) | value->mData;
    }

    // First, check whether we have a cached version of this drawable
    // that was inflated against the specified theme.
    if (!mPreloading) {
        AutoPtr<IDrawable> cachedDrawable = GetCachedDrawable(*caches, key, theme);
        if (cachedDrawable != NULL) {
            *drawable = cachedDrawable;
            REFCOUNT_ADD(*drawable)
            return NOERROR;
        }
    }

    // Next, check preloaded drawables. These are unthemed but may have
    // themeable attributes.
    AutoPtr<IWeakReference> wr;
    if (isColorDrawable) {
        Boolean hasThemedAssets;
        if (mAssets->HasThemedAssets(&hasThemedAssets), !hasThemedAssets) {
            AutoPtr<IInterface> value;
            sPreloadedColorDrawables->Get(key, (IInterface**)&value);
            wr = IWeakReference::Probe(value);
        }
    }
    else {
        Boolean hasThemedAssets;
        if (mAssets->HasThemedAssets(&hasThemedAssets), !hasThemedAssets) {
            Int32 direction;
            mConfiguration->GetLayoutDirection(&direction);
            AutoPtr<IInterface> value;
            (*sPreloadedDrawables)[direction]->Get(key, (IInterface**)&value);
            wr = IWeakReference::Probe(value);
        }
    }

    AutoPtr<IDrawableConstantState> cs;
    if (wr != NULL) {
        wr->Resolve(EIID_IDrawableConstantState, (IInterface**)&cs);
    }

    AutoPtr<IDrawable> dr;
    if (cs != NULL) {
        cs->NewDrawable(this, theme, (IDrawable**)&dr);
    }
    else if (isColorDrawable) {
        CColorDrawable::New(value->mData, (IDrawable**)&dr);
    }
    else {
        LoadDrawableForCookie(typedValue, id, theme, (IDrawable**)&dr);
    }

    // If we were able to obtain a drawable, store it in the appropriate
    // cache (either preload or themed).
    if (dr != NULL) {
        dr->SetChangingConfigurations(value->mChangingConfigurations);
        CacheDrawable(typedValue, theme, isColorDrawable, *caches, key, dr);
    }

    *drawable = dr;
    REFCOUNT_ADD(*drawable)
    return NOERROR;
}

ECode CResources::CacheDrawable(
    /* [in] */ ITypedValue* typedValue,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ Boolean isColorDrawable,
    /* [in] */ DrawableMap& caches,
    /* [in] */ Int64 key,
    /* [in] */ IDrawable* dr)
{
    AutoPtr<IDrawableConstantState> cs;
    dr->GetConstantState((IDrawableConstantState**)&cs);
    if (cs == NULL) {
        return NOERROR;
    }

    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(cs);
    AutoPtr<IWeakReference> wr;
    wrs->GetWeakReference((IWeakReference**)&wr);

    CTypedValue* value = (CTypedValue*)typedValue;
    if (mPreloading) {
        // Preloaded drawables never have a theme, but may be themeable.
        Int32 changingConfigs;
        cs->GetChangingConfigurations(&changingConfigs);

        if (isColorDrawable) {
            if (VerifyPreloadConfig(changingConfigs, 0, value->mResourceId, String("drawable"))) {
                sPreloadedColorDrawables->Put(key, wr);
            }
        }
        else {
            if (VerifyPreloadConfig(
                    changingConfigs, LAYOUT_DIR_CONFIG, value->mResourceId, String("drawable"))) {
                if ((changingConfigs & LAYOUT_DIR_CONFIG) == 0) {
                    // If this resource does not vary based on layout direction,
                    // we can put it in all of the preload maps.
                    (*sPreloadedDrawables)[0]->Put(key, wr);
                    (*sPreloadedDrawables)[1]->Put(key, wr);
                }
                else {
                    // Otherwise, only in the layout dir we loaded it for.
                    Int32 direction;
                    mConfiguration->GetLayoutDirection(&direction);
                    (*sPreloadedDrawables)[direction]->Put(key, wr);
                }
            }
        }
    }
    else {
        synchronized(mAccessLock) {
            String themeKey("");
            if (theme != NULL) {
                themeKey = ((CResources::Theme*)theme)->mKey;
            }

            DrawableMapIterator it = caches.Find(themeKey);
            AutoPtr<IInt64SparseArray> themedCache;
            if (it == caches.End()) {
                CInt64SparseArray::New(1, (IInt64SparseArray**)&themedCache);
                caches[themeKey] = themedCache;
            }
            else {
                themedCache = it->mSecond;
            }
            themedCache->Put(key, wr);
        }
    }

    return NOERROR;
}

ECode CResources::LoadDrawableForCookie(
    /* [in] */ ITypedValue* typedValue,
    /* [in] */ Int32 id,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    *drawable = NULL;

    CTypedValue* value = (CTypedValue*)typedValue;
    if (value->mString == NULL) {
        // throw new NotFoundException("Resource \"" + getResourceName(id) + "\" ("
        //         + Integer.toHexString(id) + ")  is not a Drawable (color or path): " + value);
        return E_NOT_FOUND_EXCEPTION;
    }

    String file = Object::ToString(value->mString);

    if (TRACE_FOR_MISS_PRELOAD) {
        // Log only framework resources
        if ((id >> 24) == 0x1) {
            String name;
            GetResourceName(id, &name);
            if (!name.IsNull()) {
                Logger::D(TAG, "Loading framework drawable #%08x : %s at %s",
                    id, name.string(), file.string());
            }
        }
    }

    if (DEBUG_LOAD) {
        Logger::V(TAG, "Loading drawable for cookie %d : %s", value->mAssetCookie, file.string());
    }

    AutoPtr<IDrawable> dr;
    ECode ec = NOERROR;
    // Trace.traceBegin(Trace.TRACE_TAG_RESOURCES, file);
    // try {
    if (file.EndWith(".xml")) {
        AutoPtr<IXmlResourceParser> rp;
        ec = LoadXmlResourceParser(file, id, value->mAssetCookie, String("drawable"), (IXmlResourceParser**)&rp);
        FAIL_GOTO(ec, _EXIT_)
        Drawable::CreateFromXml(this, IXmlPullParser::Probe(rp), theme, (IDrawable**)&dr);
        ICloseable::Probe(rp)->Close();
    }
    else {
        AutoPtr<IInputStream> is;
        ec = mAssets->OpenNonAsset(value->mAssetCookie, file, IAssetManager::ACCESS_STREAMING, (IInputStream**)&is);
        FAIL_GOTO(ec, _EXIT_)
        Drawable::CreateFromResourceStream(this, value, is, file, NULL, (IDrawable**)&dr);
        ICloseable::Probe(is)->Close();
    }

_EXIT_:
    if (FAILED(ec)) {
        Logger::E(TAG, "File %s from drawable resource ID #0x08x not found.", file.string(), id);
        return E_NOT_FOUND_EXCEPTION;
    }
    // } catch (Exception e) {
    //     Trace.traceEnd(Trace.TRACE_TAG_RESOURCES);
    //     NotFoundException rnf = new NotFoundException(
    //             "File " + file + " from drawable resource ID #0x" + Integer.toHexString(id));
    //     rnf.initCause(e);
    //     throw rnf;
    // }
    // Trace.traceEnd(Trace.TRACE_TAG_RESOURCES);

    *drawable = dr;
    REFCOUNT_ADD(*drawable)
    return NOERROR;
}

AutoPtr<IDrawable> CResources::GetCachedDrawable(
    /* [in] */ DrawableMap& caches,
    /* [in] */ Int64 key,
    /* [in] */ IResourcesTheme* theme)
{
    synchronized(mAccessLock) {
        String themeKey("");
        if (theme != NULL) {
            themeKey = ((CResources::Theme*)theme)->mKey;
        }

        DrawableMapIterator it = caches.Find(themeKey);
        if (it != caches.End()) {
            AutoPtr<IInt64SparseArray> themedCache = it->mSecond;
            AutoPtr<IDrawable> themedDrawable = GetCachedDrawableLocked(themedCache, key);
            if (themedDrawable != NULL) {
                return themedDrawable;
            }
        }
    }

    // No cached drawable, we'll need to create a new one.
    return NULL;
}

AutoPtr<IDrawableConstantState> CResources::GetConstantStateLocked(
    /* [in] */ IInt64SparseArray* drawableCache,
    /* [in] */ Int64 key)
{
    AutoPtr<IInterface> value;
    drawableCache->Get(key, (IInterface**)&value);
    IWeakReference* wr = IWeakReference::Probe(value);
    if (wr != NULL) { // we have the key
        AutoPtr<IDrawableConstantState> entry;
        wr->Resolve(EIID_IDrawableConstantState, (IInterface**)&entry);
        if (entry != NULL) {
            //Log.i(TAG, "Returning cached drawable @ #" +
            //        Integer.toHexString(((Integer)key).intValue())
            //        + " in " + this + ": " + entry);
            return entry;
        }
        else {  // our entry has been purged
            drawableCache->Delete(key);
        }
    }
    return NULL;
}

AutoPtr<IDrawable> CResources::GetCachedDrawableLocked(
    /* [in] */ IInt64SparseArray* drawableCache,
    /* [in] */ Int64 key)
{
    AutoPtr<IDrawableConstantState> entry = GetConstantStateLocked(drawableCache, key);
    if (entry != NULL) {
        AutoPtr<IDrawable> dr;
        entry->NewDrawable(this, (IDrawable**)&dr);
        return NOERROR;
    }
    return NULL;
}

ECode CResources::LoadColorStateList(
    /* [in] */ ITypedValue* value,
    /* [in] */ Int32 id,
    /* [out] */ IColorStateList** stateList)
{
    VALIDATE_NOT_NULL(stateList);

    CTypedValue* typedValue = (CTypedValue*)value;

    if (TRACE_FOR_PRELOAD) {
        // Log only framework resources
       if (((unsigned Int32)id >> 24) == 0x1) {
            String name;
            GetResourceName(id, &name);
            if (!name.IsNull())
                Logger::D(NULL, "PreloadColorStateList %s", (const char*)name);
        }
    }

    Int64 key = (((Int64)typedValue->mAssetCookie) << 32) | typedValue->mData;

    AutoPtr<IColorStateList> csl;
    Int32 type;
    value->GetType(&type);
    if (type >= ITypedValue::TYPE_FIRST_COLOR_INT && type <= ITypedValue::TYPE_LAST_COLOR_INT) {
        Boolean hasThemedAssets;
        if (mAssets->HasThemedAssets(&hasThemedAssets), !hasThemedAssets) {
            ColorStateIterator it = sPreloadedColorStateLists.Find(key);
            if (it != sPreloadedColorStateLists.End()) {
                AutoPtr<IWeakReference> wr = it->mSecond;
                if (wr) {
                    wr->Resolve(EIID_IColorStateList, (IInterface**)&csl);
                }
            }
        }

        if (csl != NULL) {
            *stateList = csl;
            REFCOUNT_ADD(*stateList)
            return NOERROR;
        }

        ASSERT_SUCCEEDED(CColorStateList::ValueOf(typedValue->mData,
                (IColorStateList**)&csl));
        if (mPreloading) {
            if (VerifyPreloadConfig(typedValue->mChangingConfigurations, 0,
                typedValue->mResourceId, String("color"))) {
                IWeakReferenceSource* wrs = (IWeakReferenceSource*)csl->Probe(EIID_IWeakReferenceSource);
                assert(wrs != NULL);
                AutoPtr<IWeakReference> wr;
                wrs->GetWeakReference((IWeakReference**)&wr);
                sPreloadedColorStateLists[key] = wr;
            }
        }

        *stateList = csl;
        REFCOUNT_ADD(*stateList)
        return NOERROR;
    }

    csl = GetCachedColorStateList(key);
    if (csl != NULL) {
        *stateList = csl;
        REFCOUNT_ADD(*stateList)
        return NOERROR;
    }

    Boolean hasThemedAssets;
    if (mAssets->HasThemedAssets(&hasThemedAssets), !hasThemedAssets) {
        ColorStateIterator it = sPreloadedColorStateLists.Find(key);
        if (it != sPreloadedColorStateLists.End()) {
            AutoPtr<IWeakReference> wr = it->mSecond;
            if (wr) {
                wr->Resolve(EIID_IColorStateList, (IInterface**)&csl);
            }
        }
    }
    if (csl != NULL) {
        *stateList = csl;
        REFCOUNT_ADD(*stateList)
        return NOERROR;
    }

    if (typedValue->mString == NULL) {
        Logger::E(TAG, "Resource is not a ColorStateList (color or path): %p", value);
        return E_NOT_FOUND_EXCEPTION;
        // throw new NotFoundException(
        //         "Resource is not a ColorStateList (color or path): " + value);
    }

    String file;
    typedValue->mString->ToString(&file);

    if (file.EndWith(".xml")) {
        // try {
        AutoPtr<IXmlResourceParser> rp;
        ECode ec = LoadXmlResourceParser(file, id, typedValue->mAssetCookie,
                String("colorstatelist"), (IXmlResourceParser**)&rp);
        if (FAILED(ec)) {
            Logger::E(TAG, "File %s from color state list resource ID #0x%08x",
                file.string(), id);
            return E_NOT_FOUND_EXCEPTION;
        }

        ec = CColorStateList::CreateFromXml(this, IXmlPullParser::Probe(rp), (IColorStateList**)&csl);
        rp->Close();

        if (FAILED(ec)) {
            Logger::E(TAG, "File %s from color state list resource ID #0x%08x",
                file.string(), id);
            return E_NOT_FOUND_EXCEPTION;
        }
        // } catch (Exception e) {
        //     NotFoundException rnf = new NotFoundException(
        //         "File " + file + " from color state list resource ID #0x"
        //         + Integer.toHexString(id));
        //     rnf.initCause(e);
        //     throw rnf;
        // }
    }
    else {
        Logger::E(TAG, "File %s from color state list resource ID #0x%08x: .xml extension required",
            file.string(), id);
        return E_NOT_FOUND_EXCEPTION;
    }

    if (csl != NULL) {
        IWeakReferenceSource* wrs = (IWeakReferenceSource*)csl->Probe(EIID_IWeakReferenceSource);
        assert(wrs != NULL);
        AutoPtr<IWeakReference> wr;
        wrs->GetWeakReference((IWeakReference**)&wr);
        if (mPreloading) {
            if (VerifyPreloadConfig(typedValue->mChangingConfigurations, 0,
                typedValue->mResourceId, String("color"))) {
                sPreloadedColorStateLists[key] = wr;
            }
        }
        else {
            AutoLock lock(mAccessLock);

            // Log.i(TAG, "Saving cached color state list @ #" +
            //         Integer.toHexString(key.intValue())
            //         + " in " + this + ": " + csl);
            mColorStateListCache[key] = wr;
        }
    }

    *stateList = csl;
    REFCOUNT_ADD(*stateList);
    return NOERROR;
}

AutoPtr<IColorStateList> CResources::GetCachedColorStateList(
    /* [in] */ Int64 key)
{
    AutoLock lock(mAccessLock);

    AutoPtr<IColorStateList> entry;
    ColorStateIterator it = mColorStateListCache.Find(key);
    if (it != mColorStateListCache.End()) {
        AutoPtr<IWeakReference> wr = it->mSecond;
        if (wr != NULL) {
            wr->Resolve(EIID_IColorStateList, (IInterface**)&entry);
        }
    }

    if (entry != NULL) {   // we have the key
        //Log.i(TAG, "Returning cached color state list @ #" +
        //        Integer.toHexString(((Integer)key).intValue())
        //        + " in " + this + ": " + entry);
    }
    else { // our entry has been purged
        mColorStateListCache.Erase(it);
    }

    return entry;
}

ECode CResources::LoadXmlResourceParser(
    /* [in] */ Int32 id,
    /* [in] */ const String& type,
    /* [out] */ IXmlResourceParser** parser)
{
    VALIDATE_NOT_NULL(parser);
    *parser = NULL;

    AutoLock lock(mAccessLock);

    AutoPtr<ITypedValue> value = (ITypedValue*)mTmpValue.Get();
    if (value == NULL) {
        CTypedValue::NewByFriend((CTypedValue**)&mTmpValue);
        value = (ITypedValue*)mTmpValue.Get();
    }

    FAIL_RETURN(GetValue(id, value, TRUE))
    if (mTmpValue->mType == ITypedValue::TYPE_STRING) {
        String str;
        mTmpValue->mString->ToString(&str);
        return LoadXmlResourceParser(str, id, mTmpValue->mAssetCookie, type, parser);
    }
//    throw new NotFoundException(
//            "Resource ID #0x" + Integer.toHexString(id) + " type #0x"
//            + Integer.toHexString(value.type) + " is not valid");
    Logger::E(TAG, "Resource ID #0x%08x type #0x%08x is not valid", id, mTmpValue->mType);
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::LoadXmlResourceParser(
    /* [in] */ const String& file,
    /* [in] */ Int32 id,
    /* [in] */ Int32 assetCookie,
    /* [in] */ const String& type,
    /* [out] */ IXmlResourceParser** parser)
{
    VALIDATE_NOT_NULL(parser);
    *parser = NULL;

    if (id != 0) {
        // try {
        // These may be compiled...
        AutoLock lock(mCachedXmlBlockIdsLock);
        String name;
        // First see if this block is in our cache.
        Int32 num = mCachedXmlBlockIds->GetLength();
        for (Int32 i = 0; i < num; i++) {
            if ((*mCachedXmlBlockIds)[i] == id) {
                mAssets->GetCookieName(assetCookie, &name);
                String filename = file;
                if (!name.IsNull()) {
                    StringBuilder sb(name);
                    sb += IResources::path_separate;
                    sb += file;
                    filename = sb.ToString();
                }

                AutoPtr<IXmlResourceParser> p = (*mCachedXmlBlocks)[i]->NewParser();
                *parser = p;
                REFCOUNT_ADD(*parser);
                return NOERROR;
            }
        }

        // Not in the cache, create a new block and put it at
        // the next slot in the cache.
        AutoPtr<XmlBlock> block;
        if (FAILED(mAssets->OpenXmlBlockAsset(assetCookie, file, (XmlBlock**)&block))) {
            Logger::E(TAG, "File %s from xml type %s resource ID #0x%08x",
                file.string(), type.string(), id);
            return E_NOT_FOUND_EXCEPTION;
        }
        if (block != NULL) {
            Int32 pos = (mLastCachedXmlBlockIndex + 1) % num;
            mLastCachedXmlBlockIndex = pos;
            AutoPtr<XmlBlock> oldBlock = (*mCachedXmlBlocks)[pos];

            mCachedXmlBlocks->Set(pos, NULL);
            if (oldBlock != NULL) {
                oldBlock->Close();
                oldBlock = NULL;
            }

            (*mCachedXmlBlockIds)[pos] = id;
            mCachedXmlBlocks->Set(pos, block);

            AutoPtr<IXmlResourceParser> p = block->NewParser();
            *parser = p;
            REFCOUNT_ADD(*parser);
            return NOERROR;
        }
        // } catch (Exception e) {
        //     NotFoundException rnf = new NotFoundException(
        //             "File " + file + " from xml type " + type + " resource ID #0x"
        //             + Integer.toHexString(id));
        //     rnf.initCause(e);
        //     throw rnf;
        // }
    }

    // throw new NotFoundException(
    //     "File " + file + " from xml type " + type + " resource ID #0x"
    //     + Integer.toHexString(id));
    Logger::E(TAG, "File %s from xml type %s resource ID #0x%08x",
        file.string(), type.string(), id);
    return E_NOT_FOUND_EXCEPTION;
}

ECode CResources::RecycleCachedStyledAttributes(
    /* [in] */ CTypedArray* attrs)
{
    synchronized(mAccessLock) {
        AutoPtr<CTypedArray> cached = mCachedStyledAttributes;
        if (cached == NULL || cached->mData->GetLength() < attrs->mData->GetLength()) {
            mCachedStyledAttributes = attrs;
        }
    }

    return NOERROR;
}

ECode CResources::SetIconResources(
    /* [in] */ ISparseArray* icons)
{
    mIcons = icons;
    return NOERROR;
}

ECode CResources::SetComposedIconInfo(
    /* [in] */ IComposedIconInfo* iconInfo)
{
    mComposedIconInfo = iconInfo;
    return NOERROR;
}

ECode CResources::GetComposedIconInfo(
    /* [out] */ IComposedIconInfo** iconInfo)
{
    VALIDATE_NOT_NULL(iconInfo)
    *iconInfo = mComposedIconInfo;
    REFCOUNT_ADD(*iconInfo)
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

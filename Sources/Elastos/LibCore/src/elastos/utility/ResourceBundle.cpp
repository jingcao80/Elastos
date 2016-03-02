
#include "Elastos.CoreLibrary.Net.h"
#include "ResourceBundle.h"
#include "CArrayList.h"
#include "CString.h"
#include "Collections.h"
#include "CWeakHashMap.h"
#include "CLocale.h"
#include "CFile.h"
#include "CHashSet.h"
#include "CHashTable.h"
#include "StringBuilder.h"
#include "StringUtils.h"
#include "CSystem.h"
#include "AutoLock.h"

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::IInputStream;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Net::IURL;
using Elastos::Net::IURLConnection;
using Elastos::Utility::CLocale;
using Elastos::Utility::Collections;

namespace Elastos {
namespace Utility {


//==========================================================
//       ResourceBundle::Control
//==========================================================

AutoPtr<IList> ResourceBundle::Control::sListDefault;

AutoPtr<IList> ResourceBundle::Control::sListClass;

AutoPtr<IList> ResourceBundle::Control::sListProperties;

String ResourceBundle::Control::JAVACLASS = String("java.class");

String ResourceBundle::Control::JAVAPROPERTIES = String("java.properties");

const AutoPtr<IList> ResourceBundle::Control::FORMAT_DEFAULT;

const AutoPtr<IList> ResourceBundle::Control::FORMAT_CLASS;

const AutoPtr<IList> ResourceBundle::Control::FORMAT_PROPERTIES;

const Int64 ResourceBundle::Control::TTL_DONT_CACHE = -1L;

const Int64 ResourceBundle::Control::TTL_NO_EXPIRATION_CONTROL = -2L;

const AutoPtr<ResourceBundle::Control> ResourceBundle::Control::FORMAT_PROPERTIES_CONTROL = new SimpleControl(JAVAPROPERTIES);

const AutoPtr<ResourceBundle::Control> ResourceBundle::Control::FORMAT_CLASS_CONTROL = new SimpleControl(JAVACLASS);

const AutoPtr<ResourceBundle::Control> ResourceBundle::Control::FORMAT_DEFAULT_CONTROL = new Control();

Boolean ResourceBundle::Control::isInit()
{
    AutoPtr<CArrayList> outlist;
    CArrayList::NewByFriend((CArrayList**)&outlist);
    sListDefault = IList::Probe(outlist);

    outlist = NULL;
    CArrayList::NewByFriend((CArrayList**)&outlist);
    sListClass = IList::Probe(outlist);

    outlist = NULL;
    CArrayList::NewByFriend((CArrayList**)&outlist);
    sListProperties = IList::Probe(outlist);

    AutoPtr<CString> sqjavaclass;
    CString::NewByFriend(JAVACLASS, (CString**)&sqjavaclass);
    Boolean isflag = FALSE;
    sListDefault->Add(sqjavaclass->Probe(EIID_IInterface), &isflag);

    AutoPtr<CString> sqjavaproperties;
    CString::NewByFriend(JAVAPROPERTIES, (CString**)&sqjavaproperties);
    sListDefault->Add(sqjavaproperties->Probe(EIID_IInterface), &isflag);
    sListClass->Add(sqjavaclass->Probe(EIID_IInterface), &isflag);
    sListProperties->Add(sqjavaproperties->Probe(EIID_IInterface), &isflag);

//    CCollections::_NewUnmodifiableList(sListDefault, (IList**)&FORMAT_DEFAULT);
//    CCollections::_NewUnmodifiableList(sListClass, (IList**)&FORMAT_CLASS);
//    CCollections::_NewUnmodifiableList(sListProperties, (IList**)&FORMAT_PROPERTIES);

    return TRUE;
}

Boolean ResourceBundle::Control::isinitflag = isInit();

ResourceBundle::Control::Control()
{
    AutoPtr<CArrayList> outlist;
    CArrayList::NewByFriend((CArrayList**)&outlist);
    sListClass = IList::Probe(outlist);
    AutoPtr<CString> sqjavaclass;
    CString::NewByFriend(JAVACLASS, (CString**)&sqjavaclass);
    Boolean isflag = FALSE;
    sListClass->Add(sqjavaclass->Probe(EIID_IInterface), &isflag);

    AutoPtr<CString> sqjavaproperties;
    CString::NewByFriend(JAVAPROPERTIES, (CString**)&sqjavaproperties);
    sListClass->Add(sqjavaproperties->Probe(EIID_IInterface), &isflag);
//    CCollections::_NewUnmodifiableList(sListClass, (IList**)&mFormat);
}

CAR_INTERFACE_IMPL(ResourceBundle::Control, Object, IResourceBundleControl)

ECode ResourceBundle::Control::GetCandidateLocales(
    /* [in] */ const String& baseName,
    /* [in] */ ILocale* locale,
    /* [out] */ IList** obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = NULL;

    if (baseName.IsNull()) {
        // throw new NullPointerException("baseName == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    else if (locale == NULL) {
        // throw new NullPointerException("locale == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IArrayList> retList;
    CArrayList::New((IArrayList**)&retList);
    String language;
    locale->GetLanguage(&language);
    String country;
    locale->GetCountry(&country);
    String variant;
    locale->GetVariant(&variant);
    Boolean isflag = FALSE;
    if (!EMPTY_STRING.Equals(variant)) {
        AutoPtr<ILocale> newloc;
        CLocale::New(language, country, variant, (ILocale**)&newloc);
        (IList::Probe(retList))->Add(newloc, &isflag);
    }
    if (!EMPTY_STRING.Equals(country)) {
        AutoPtr<ILocale> newloc;
        CLocale::New(language, country, (ILocale**)&newloc);
        (IList::Probe(retList))->Add(newloc, &isflag);
    }
    if (!EMPTY_STRING.Equals(language)) {
        AutoPtr<ILocale> newloc;
        CLocale::New(language, (ILocale**)&newloc);
        (IList::Probe(retList))->Add(newloc, &isflag);
    }
    (IList::Probe(retList))->Add(CLocale::ROOT, &isflag);
    *obj = IList::Probe(retList);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode ResourceBundle::Control::GetFormats(
    /* [in] */ const String& baseName,
    /* [out] */ IList** obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = NULL;

    if (baseName.IsNull()) {
        // throw new NullPointerException("baseName == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    *obj = mFormat;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode ResourceBundle::Control::GetFallbackLocale(
    /* [in] */ const String& baseName,
    /* [in] */ ILocale* locale,
    /* [out] */ ILocale** loc)
{
    VALIDATE_NOT_NULL(loc)
    *loc = NULL;

    if (baseName.IsNull()) {
        // throw new NullPointerException("baseName == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    else if (locale == NULL) {
        // throw new NullPointerException("locale == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ILocale> inloc = CLocale::GetDefault();
    Boolean isflag = FALSE;
    if (inloc->Equals(locale, &isflag), isflag) {
        *loc = inloc;
        REFCOUNT_ADD(*loc);
        return NOERROR;
    }
    *loc = NULL;
    return NOERROR;
}

ECode ResourceBundle::Control::NewBundle(
    /* [in] */ const String& baseName,
    /* [in] */ ILocale* locale,
    /* [in] */ const String& format,
    /* [in] */ IClassLoader* loader,
    /* [in] */ Boolean reload,
    /* [out] */ IResourceBundle** resbun)
{
    VALIDATE_NOT_NULL(resbun)
    *resbun = NULL;

    if (format.IsNull()) {
        // throw new NullPointerException("format == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    else if (loader == NULL) {
        // throw new NullPointerException("loader == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    String bundleName;
    ToBundleName(baseName, locale, &bundleName);
    AutoPtr<IClassLoader> clsloader = loader;
    AutoPtr<IResourceBundle> ret;
    if (format.Equals(JAVACLASS)) {
        AutoPtr<IClassInfo> cls;
        // try {
        clsloader->LoadClass(bundleName, (IClassInfo**)&cls);
        // } catch (Exception e) {
        // } catch (NoClassDefFoundError e) {
        // }
        if (cls == NULL) {
            *resbun = NULL;
            return NOERROR;
        }
        // try {
        AutoPtr<IInterface> outface;
        ECode ec = cls->CreateObject((IInterface**)&outface);
        if (ec == NOERROR) {
            AutoPtr<IResourceBundle> bundle = IResourceBundle::Probe(outface);
            if (bundle != NULL) {
                assert(0 && "TODO");
                // bundle->SetLocale(locale);
                *resbun = bundle;
                REFCOUNT_ADD(*resbun)
                return NOERROR;
            }
        }
        // } catch (NullPointerException e) {
        if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
            *resbun = NULL;
            return NOERROR;
        }
        // }
    }
    if (format.Equals(JAVAPROPERTIES)) {
        AutoPtr<IInputStream> streams;
        String resourceName;
        ToResourceName(bundleName, String("properties"), &resourceName);
        if (reload) {
            AutoPtr<IURL> url;
            // try {
            assert(0 && "TODO");
            // url = loader.getResource(resourceName);
            // } catch (NullPointerException e) {
            //     // do nothing
            // }
            if (url != NULL) {
                AutoPtr<IURLConnection> con;
                url->OpenConnection((IURLConnection**)&con);
                con->SetUseCaches(FALSE);
                con->GetInputStream((IInputStream**)&streams);
            }
        }
        else {
            // try {
            assert(0 && "TODO");
            // streams = clsloader.getResourceAsStream(resourceName);
            // } catch (NullPointerException e) {
            //     // do nothing
            // }
        }
        if (streams != NULL) {
            // try {
            assert(0 && "TODO");
            // ret = new PropertyResourceBundle(new InputStreamReader(streams));
            // ret.setLocale(locale);
            // streams.close();
            // } catch (IOException e) {
                // return NULL;
            // }
            // return ret;
        }
        *resbun = NULL;
        return NOERROR;
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode ResourceBundle::Control::GetTimeToLive(
    /* [in] */ const String& baseName,
    /* [in] */ ILocale* locale,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    if (baseName.IsNull()) {
        // throw new NullPointerException("baseName == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    else if (locale == NULL) {
        // throw new NullPointerException("locale == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    *value = TTL_NO_EXPIRATION_CONTROL;
    return NOERROR;
}

ECode ResourceBundle::Control::NeedsReload(
    /* [in] */ const String& baseName,
    /* [in] */ ILocale* locale,
    /* [in] */ const String& format,
    /* [in] */ IClassLoader* loader,
    /* [in] */ IResourceBundle* bundle,
    /* [in] */ Int64 loadTime,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    if (bundle == NULL) {
        // FIXME what's the use of bundle?
        // throw new NullPointerException("bundle == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    String bundleName;
    ToBundleName(baseName, locale, &bundleName);
    String suffix = format;
    if (format.Equals(JAVACLASS)) {
        suffix = "class";
    }
    if (format.Equals(JAVAPROPERTIES)) {
        suffix = "properties";
    }
    String urlname;
    ToResourceName(bundleName, suffix, &urlname);
    assert(0 && "TODO");
    AutoPtr<IURL> url;
    // loader->GetResource(urlname, (IURL**)&url);
    if (url != NULL) {
        String fileName;
        url->GetFile(&fileName);
        Int64 lastModified = 0;
        AutoPtr<IFile> infile;
        CFile::New(fileName, (IFile**)&infile);
        infile->GetLastModified(&lastModified);
        if (lastModified > loadTime) {
            *value = TRUE;
            return NOERROR;
        }
    }
    *value = FALSE;
    return NOERROR;
}

ECode ResourceBundle::Control::ToBundleName(
    /* [in] */ const String& baseName,
    /* [in] */ ILocale* locale,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);

    String emptyString = EMPTY_STRING;
    String preString = UNDER_SCORE;
    String underline = UNDER_SCORE;
    if (baseName == NULL) {
        // throw new NullPointerException("baseName == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    StringBuilder ret;
    StringBuilder prefix;
    ret += baseName;
    String langstr;
    locale->GetLanguage(&langstr);
    if (!langstr.Equals(emptyString)) {
        ret += underline;
        ret += langstr;
    }
    else {
        prefix += preString;
    }
    String counstr;
    locale->GetCountry(&counstr);
    if (!counstr.Equals(emptyString)) {
        ret += prefix;
        ret += underline;
        ret += counstr;
        prefix.Reset();
    }
    else {
        prefix += preString;
    }
    String varstr;
    locale->GetVariant(&varstr);
    if (!varstr.Equals(emptyString)) {
        ret += prefix;
        ret += underline;
        ret += varstr;
    }
    return ret.ToString(str);
}

ECode ResourceBundle::Control::ToResourceName(
    /* [in] */ const String& bundleName,
    /* [in] */ const String& suffix,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);

    if (suffix.IsNull()) {
        // throw new NullPointerException("suffix == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    String inbn = bundleName;
    StringBuilder ret(inbn.Replace('.', '/'));
    ret += '.';
    ret += suffix;
    return ret.ToString(str);
}

AutoPtr<ResourceBundle::Control> ResourceBundle::Control::GetControl(
    /* [in] */ IList* formats)
{
    Int32 sizelen = 0;
    (ICollection::Probe(formats))->GetSize(&sizelen);
    Boolean isflag = FALSE;
    AutoPtr<ICharSequence> sqjavaclass;
    CString::New(JAVACLASS, (ICharSequence**)&sqjavaclass);
    AutoPtr<ICharSequence> sqjavaproperties;
    CString::New(JAVAPROPERTIES, (ICharSequence**)&sqjavaproperties);
    switch (sizelen) {
        case 1:
            if ((ICollection::Probe(formats))->Contains(sqjavaclass, &isflag), isflag) {
                return FORMAT_CLASS_CONTROL;
            }
            if ((ICollection::Probe(formats))->Contains(sqjavaproperties, &isflag), isflag) {
                return FORMAT_PROPERTIES_CONTROL;
            }
            break;
        case 2:
            if ((ICollection::Probe(formats))->Equals(FORMAT_DEFAULT, &isflag), isflag) {
                return FORMAT_DEFAULT_CONTROL;
            }
            break;
    }
    // throw new IllegalArgumentException();
    return NULL;
}

AutoPtr<ResourceBundle::Control> ResourceBundle::Control::GetNoFallbackControl(
    /* [in] */ IList* formats)
{
    Int32 sizelen = 0;
    (ICollection::Probe(formats))->GetSize(&sizelen);
    Boolean isflag = FALSE;
    AutoPtr<ICharSequence> sqjavaclass;
    CString::New(JAVACLASS, (ICharSequence**)&sqjavaclass);
    AutoPtr<ICharSequence> sqjavaproperties;
    CString::New(JAVAPROPERTIES, (ICharSequence**)&sqjavaproperties);
    switch (sizelen) {
        case 1:
            if ((ICollection::Probe(formats))->Contains(sqjavaclass, &isflag), isflag) {
                return NoFallbackControl::NOFALLBACK_FORMAT_CLASS_CONTROL;
            }
            if ((ICollection::Probe(formats))->Contains(sqjavaproperties, &isflag), isflag) {
                return NoFallbackControl::NOFALLBACK_FORMAT_PROPERTIES_CONTROL;
            }
            break;
        case 2:
            if ((ICollection::Probe(formats))->Equals(FORMAT_DEFAULT, &isflag), isflag) {
                return NoFallbackControl::NOFALLBACK_FORMAT_DEFAULT_CONTROL;
            }
            break;
    }
    // throw new IllegalArgumentException();
    return NULL;
}

//==========================================================
//       ResourceBundle::NoFallbackControl
//==========================================================

const AutoPtr<ResourceBundle::Control> ResourceBundle::NoFallbackControl::NOFALLBACK_FORMAT_PROPERTIES_CONTROL = new NoFallbackControl(JAVAPROPERTIES);

const AutoPtr<ResourceBundle::Control> ResourceBundle::NoFallbackControl::NOFALLBACK_FORMAT_CLASS_CONTROL = new NoFallbackControl(JAVACLASS);

const AutoPtr<ResourceBundle::Control> ResourceBundle::NoFallbackControl::NOFALLBACK_FORMAT_DEFAULT_CONTROL = new NoFallbackControl(sListDefault);

ResourceBundle::NoFallbackControl::NoFallbackControl(
    /* [in] */ const String& format)
{
    AutoPtr<CArrayList> outlist;
    CArrayList::NewByFriend((CArrayList**)&outlist);
    sListClass = IList::Probe(outlist);
    AutoPtr<CString> sq;
    CString::NewByFriend(format, (CString**)&sq);
    Boolean isflag = FALSE;
    sListClass->Add(sq->Probe(EIID_IInterface), &isflag);
//    CCollections::_NewUnmodifiableList(sListClass, (IList**)&mFormat);
}

ResourceBundle::NoFallbackControl::NoFallbackControl(
    /* [in] */ IList* list)
{
    mFormat = list;
}

ECode ResourceBundle::NoFallbackControl::GetFallbackLocale(
    /* [in] */ const String& baseName,
    /* [in] */ ILocale* locale,
    /* [out] */ ILocale** outlocale)
{
    if (baseName.IsNull()) {
        // throw new NullPointerException("baseName == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    else if (locale == NULL) {
        // throw new NullPointerException("locale == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    *outlocale = NULL;
    return NOERROR;
}

//==========================================================
//       ResourceBundle::SimpleControl
//==========================================================

ResourceBundle::SimpleControl::SimpleControl(
    /* [in] */ const String& format)
{
    AutoPtr<CArrayList> outlist;
    CArrayList::NewByFriend((CArrayList**)&outlist);
    sListClass = IList::Probe(outlist);
    AutoPtr<CString> sq;
    CString::NewByFriend(format, (CString**)&sq);
    Boolean isflag = FALSE;
    sListClass->Add(sq->Probe(EIID_IInterface), &isflag);
//    CCollections::_NewUnmodifiableList(sListClass, (IList**)&mFormat);
}

//==========================================================
//       ResourceBundle
//==========================================================

const String ResourceBundle::UNDER_SCORE = String("_");

const String ResourceBundle::EMPTY_STRING = String("");

const ResourceBundle* ResourceBundle::MISSING = new MissingBundle();

const ResourceBundle* ResourceBundle::MISSINGBASE = new MissingBundle();

const AutoPtr<IMap> ResourceBundle::sCache;

Object ResourceBundle::mLock;

AutoPtr<ILocale> ResourceBundle::sCacheLocale; // = CLocale::GetDefault();

CAR_INTERFACE_IMPL(ResourceBundle, Object, IResourceBundle)

Boolean ResourceBundle::ishostinit()
{
    // AutoPtr<CWeakHashMap> whm;
    // CWeakHashMap::NewByFriend((CWeakHashMap**)&whm);
    // sCache = (IMap*)whm->Probe(EIID_IMap);
    // AutoPtr<CLocale> clhel;
    // CLocale::NewByFriend((CLocale**)&clhel);
    // sCacheLocale = clhel->GetDefault();
    return TRUE;
}

Boolean ResourceBundle::ishostflag = ishostinit();

ResourceBundle::ResourceBundle()
    : mLastLoadTime(0)
{
}

ECode ResourceBundle::GetBundle(
    /* [in] */ const String& bundleName,
    /* [out] */ IResourceBundle** outrb)
{
    VALIDATE_NOT_NULL(outrb)
    *outrb = NULL;

    assert(0 && "TODO");
    // ClassLoader classLoader = VMStack.getCallingClassLoader();
    // if (classLoader == NULL) {
    //     classLoader = getLoader();
    // }
    // return getBundle(bundleName, Locale.getDefault(), classLoader);
    return NOERROR;
}

ECode ResourceBundle::GetBundle(
    /* [in] */ const String& bundleName,
    /* [in] */ ILocale* locale,
    /* [out] */ IResourceBundle** outrb)
{
    VALIDATE_NOT_NULL(outrb)
    *outrb = NULL;

    assert(0 && "TODO");
    // ClassLoader classLoader = VMStack.getCallingClassLoader();
    // if (classLoader == NULL) {
    //     classLoader = getLoader();
    // }
    // return getBundle(bundleName, locale, classLoader);
    return NOERROR;
}

ECode ResourceBundle::GetBundle(
    /* [in] */ const String& bundleName,
    /* [in] */ ILocale* locale,
    /* [in] */ IClassLoader* loader,
    /* [out] */ IResourceBundle** outrb)
{
    VALIDATE_NOT_NULL(outrb)
    *outrb = NULL;

    if (loader == NULL) {
        // throw new NullPointerException("loader == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    else if (bundleName == NULL) {
        // throw new NullPointerException("bundleName == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ILocale> defaultLocale = CLocale::GetDefault();
    Boolean isflag = FALSE;
    if (!(sCacheLocale->Equals(defaultLocale, &isflag), isflag)) {
        sCache->Clear();
        sCacheLocale = defaultLocale;
    }
    AutoPtr<IResourceBundle> bundle;
    if (!(locale->Equals(defaultLocale, &isflag), isflag)) {
        bundle = HandleGetBundle(FALSE, bundleName, locale, loader);
    }
    if (bundle == NULL) {
        bundle = HandleGetBundle(true, bundleName, defaultLocale, loader);
        if (bundle == NULL) {
            // throw missingResourceException(bundleName + '_' + locale, "");
            return E_MISSING_RESOURCE_EXCEPTION;
        }
    }
    *outrb = bundle;
    REFCOUNT_ADD(*outrb)
    return NOERROR;
}

ECode ResourceBundle::MissingResourceException(
    /* [in] */ const String& className,
    /* [in] */ const String& key)
{
    // String detail = "Can't find resource for bundle '" + className + "', key '" + key + "'";
    // throw new MissingResourceException(detail, className, key);
    return E_MISSING_RESOURCE_EXCEPTION;
}

ECode ResourceBundle::GetBundle(
    /* [in] */ const String& baseName,
    /* [in] */ IResourceBundleControl* control,
    /* [out] */ IResourceBundle** outrb)
{
    return GetBundle(baseName, CLocale::GetDefault(), GetLoader(), control, outrb);
}

ECode ResourceBundle::GetBundle(
    /* [in] */ const String& baseName,
    /* [in] */ ILocale* targetLocale,
    /* [in] */ IResourceBundleControl* control,
    /* [out] */ IResourceBundle** outrb)
{
    return GetBundle(baseName, targetLocale, GetLoader(), control, outrb);
}

ECode ResourceBundle::GetBundle(
    /* [in] */ const String& baseName,
    /* [in] */ ILocale* targetLocale,
    /* [in] */ IClassLoader* loader,
    /* [in] */ IResourceBundleControl* control,
    /* [out] */ IResourceBundle** outrb)
{
    VALIDATE_NOT_NULL(outrb)
    *outrb = NULL;

    Boolean expired = FALSE;
    String bundleName;
    control->ToBundleName(baseName, targetLocale, &bundleName);
    AutoPtr<IInterface> cacheKey;
    if (loader != NULL) {
        cacheKey = loader;
    }
    else {
        AutoPtr<ICharSequence> sq;
        CString::New(String("NULL"), (ICharSequence**)&sq);
        cacheKey = sq;
    }
    AutoPtr<IDictionary> loaderCache = GetLoaderCache(cacheKey);
    AutoPtr<ICharSequence> sqbundleName;
    CString::New(bundleName, (ICharSequence**)&sqbundleName);
    AutoPtr<IInterface> outface;
    loaderCache->Get(sqbundleName, (IInterface**)&outface);
    AutoPtr<IResourceBundle> result = IResourceBundle::Probe(outface);
    if (result != NULL) {
        Int64 time = 0;
        control->GetTimeToLive(baseName, targetLocale, &time);
        assert(0 && "TODO");
        // if (time == 0 || time == Control::TTL_NO_EXPIRATION_CONTROL
        //         || time + result.lastLoadTime < System.currentTimeMillis()) {
        //     if (MISSING == result) {
        //         throw new MissingResourceException(NULL, bundleName + '_'
        //                 + targetLocale, EMPTY_STRING);
        //     }
        //     return result;
        // }
        expired = TRUE;
    }
    // try to load
    AutoPtr<IResourceBundle> ret = ProcessGetBundle(baseName, targetLocale, loader,
            control, expired, result);

    if (ret != NULL) {
        AutoPtr<IInterface> outface;
        loaderCache->Put(sqbundleName, ret, (IInterface**)&outface);
        assert(0 && "TODO");
        // ret.lastLoadTime = System.currentTimeMillis();
        *outrb = ret;
        REFCOUNT_ADD(*outrb)
        return NOERROR;
    }
    assert(0 && "TODO");
    // loaderCache->Put(sqbundleName, MISSING);
    // throw new MissingResourceException(NULL, bundleName + '_' + targetLocale, EMPTY_STRING);
    return E_MISSING_RESOURCE_EXCEPTION;
}

ECode ResourceBundle::GetLocale(
    /* [out] */ ILocale** outlocale)
{
    VALIDATE_NOT_NULL(outlocale)
    *outlocale = NULL;

    *outlocale = mLocale;
    REFCOUNT_ADD(*outlocale)
    return NOERROR;
}

ECode ResourceBundle::GetObject(
    /* [in] */ const String& key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)
    *outface = NULL;

    assert(0 && "TODO");
    // ResourceBundle last, theParent = this;
    // do {
    //     Object result = theParent.handleGetObject(key);
    //     if (result != NULL) {
    //         return result;
    //     }
    //     last = theParent;
    //     theParent = theParent.parent;
    // } while (theParent != NULL);
    // throw missingResourceException(last.getClass().getName(), key);
    return NOERROR;
}

ECode ResourceBundle::GetString(
    /* [in] */ const String& key,
    /* [out] */ String* outstr)
{
    VALIDATE_NOT_NULL(outstr)
    *outstr = String(NULL);

    AutoPtr<IInterface> outface;
    GetObject(key, (IInterface**)&outface);
    AutoPtr<ICharSequence> sq = ICharSequence::Probe(outface);
    if (sq != NULL) {
        return sq->ToString(outstr);
    }

    *outstr = String(NULL);
    return NOERROR;
}

ECode ResourceBundle::GetStringArray(
    /* [in] */ const String& key,
    /* [out, callee] */ ArrayOf<String>** arrstr)
{
    VALIDATE_NOT_NULL(arrstr)
    *arrstr = NULL;

    AutoPtr<IInterface> outface;
    GetObject(key, (IInterface**)&outface);
    AutoPtr<ICharSequence> sq = ICharSequence::Probe(outface);
    String outstr;
    if (sq != NULL) {
        sq->ToString(&outstr);
    }
    else {
        outstr = String(NULL);
    }

    AutoPtr< ArrayOf<String> > arrout = ArrayOf<String>::Alloc(1);
    (*arrout)[0] = outstr;
    *arrstr = arrout;
    REFCOUNT_ADD(*arrstr)
    return NOERROR;
}

ECode ResourceBundle::ContainsKey(
    /* [in] */ const String& key,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    if (key.IsNull()) {
        // throw new NullPointerException("key == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ISet> outset;
    GetKeySet((ISet**)&outset);
    AutoPtr<ICharSequence> sq;
    CString::New(key, (ICharSequence**)&sq);
    return (ICollection::Probe(outset))->Contains(sq, value);
}

ECode ResourceBundle::GetKeySet(
    /* [out] */ ISet** outset)
{
    VALIDATE_NOT_NULL(outset)
    *outset = NULL;

    AutoPtr<ISet> ret;
    CHashSet::New((ISet**)&ret);
    AutoPtr<IEnumeration> keys;
    GetKeys((IEnumeration**)&keys);
    Boolean isflag = FALSE;
    while (keys->HasMoreElements(&isflag), isflag) {
        AutoPtr<IInterface> outface;
        keys->GetNextElement((IInterface**)&outface);
        (ICollection::Probe(ret))->Add(outface, &isflag);
    }
    *outset = ret;
    REFCOUNT_ADD(*outset)
    return NOERROR;
}

ECode ResourceBundle::SetParent(
    /* [in] */ IResourceBundle* bundle)
{
    mParent = bundle;
    return NOERROR;
}

ECode ResourceBundle::ClearCache()
{
    assert(0 && "TODO");
    // cache.remove(ClassLoader.getSystemClassLoader());
    return NOERROR;
}

ECode ResourceBundle::ClearCache(
    /* [in] */ IClassLoader* loader)
{
    if (loader == NULL) {
        // throw new NullPointerException("loader == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IInterface> outface;
    return sCache->Remove(loader, (IInterface**)&outface);
}

ECode ResourceBundle::HandleKeySet(
    /* [out] */ ISet** outset)
{
    VALIDATE_NOT_NULL(outset)
    *outset = NULL;

    AutoPtr<ISet> set;
    GetKeySet((ISet**)&set);
    AutoPtr<ISet> ret;
    CHashSet::New((ISet**)&ret);
    AutoPtr< ArrayOf<IInterface*> > outarr;
    (ICollection::Probe(set))->ToArray((ArrayOf<IInterface*>**)&outarr);
    for (Int32 i = 0; i < outarr->GetLength(); i++) {
        AutoPtr<ICharSequence> sq = ICharSequence::Probe((*outarr)[i]);
        if (sq != NULL) {
            String key;
            sq->ToString(&key);
            AutoPtr<IInterface> outface;
            if ((HandleGetObject(key, (IInterface**)&outface), outface != NULL)) {
                Boolean isflag = FALSE;
                (ICollection::Probe(ret))->Add(sq, &isflag);
            }
        }
    }
    *outset = ret;
    REFCOUNT_ADD(*outset)
    return NOERROR;
}

AutoPtr<IClassLoader> ResourceBundle::GetLoader()
{
    assert(0 && "TODO");
    AutoPtr<IClassLoader> cl; // = ResourceBundle.class.getClassLoader();
    // if (cl == NULL) {
    //     cl = ClassLoader.getSystemClassLoader();
    // }
    return cl;
}

AutoPtr<IResourceBundle> ResourceBundle::ProcessGetBundle(
    /* [in] */ const String& baseName,
    /* [in] */ ILocale* targetLocale,
    /* [in] */ IClassLoader* loader,
    /* [in] */ IResourceBundleControl* control,
    /* [in] */ Boolean expired,
    /* [in] */ IResourceBundle* result)
{
    AutoPtr<IList> locales;
    control->GetCandidateLocales(baseName, targetLocale, (IList**)&locales);
    if (locales == NULL) {
        // throw new IllegalArgumentException();
        return NULL;
    }
    AutoPtr<IList> formats;
    control->GetFormats(baseName, (IList**)&formats);
    if (Object::Equals(Control::FORMAT_CLASS, formats)
        || Object::Equals(Control::FORMAT_PROPERTIES, formats)
        || Object::Equals(Control::FORMAT_DEFAULT, formats)) {
        // throw new IllegalArgumentException();
        return NULL;
    }
    AutoPtr<IResourceBundle> ret;
    AutoPtr<IResourceBundle> currentBundle;
    AutoPtr<IResourceBundle> bundle;
    AutoPtr< ArrayOf<IInterface*> > outlocarr;
    (ICollection::Probe(locales))->ToArray((ArrayOf<IInterface*>**)&outlocarr);
    for (Int32 i = 0; i < outlocarr->GetLength(); i++) {
        AutoPtr<ILocale> locale = ILocale::Probe((*outlocarr)[i]);
        AutoPtr< ArrayOf<IInterface*> > outformatarr;
        (ICollection::Probe(formats))->ToArray((ArrayOf<IInterface*>**)&outformatarr);
        for (Int32 j = 0; j < outformatarr->GetLength(); j++) {
            AutoPtr<ICharSequence> sq = ICharSequence::Probe((*outformatarr)[j]);
            String format;
            if (sq != NULL) {
                sq->ToString(&format);
            }
            // try {
            Boolean isflag = FALSE;
            if (expired) {
                AutoPtr<ISystem> system;
                Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
                Int64 ctm = 0;
                system->GetCurrentTimeMillis(&ctm);
                control->NeedsReload(baseName, locale,
                                format, loader, result, ctm, &isflag);
                control->NewBundle(baseName, locale, format,
                        loader, isflag, (IResourceBundle**)&bundle);

            }
            else {
                // try {
                control->NewBundle(baseName, locale,
                            format, loader, FALSE, (IResourceBundle**)&bundle);
                // } catch (IllegalArgumentException e) {
                //     // do nothing
                // }
            }
            // } catch (IllegalAccessException e) {
            //     // do nothing
            // } catch (InstantiationException e) {
            //     // do nothing
            // } catch (IOException e) {
            //     // do nothing
            // }
            if (bundle != NULL) {
                if (currentBundle != NULL) {
                    assert(0 && "TODO");
                    // currentBundle->SetParent(bundle);
                    currentBundle = bundle;
                }
                else {
                    if (ret == NULL) {
                        ret = bundle;
                        currentBundle = ret;
                    }
                }
            }
            if (bundle != NULL) {
                break;
            }
        }
    }

    AutoPtr<ILocale> retloc;
    ret->GetLocale((ILocale**)&retloc);
    Int32 sizelen = 0;
    Boolean isflag = FALSE;
    if ((ret == NULL)
            || (Object::Equals(CLocale::ROOT, retloc)
            && (!((ICollection::Probe(locales))->GetSize(&sizelen), sizelen == 1)
            && ((ICollection::Probe(locales))->Contains(CLocale::ROOT, &isflag), isflag)))) {
        AutoPtr<ILocale> nextLocale;
        control->GetFallbackLocale(baseName, targetLocale, (ILocale**)&nextLocale);
        if (nextLocale != NULL) {
            ret = ProcessGetBundle(baseName, nextLocale, loader, control, expired, result);
        }
    }

    return ret;
}

AutoPtr<IResourceBundle> ResourceBundle::HandleGetBundle(
    /* [in] */ Boolean loadBase,
    /* [in] */ const String& base,
    /* [in] */ ILocale* locale,
    /* [in] */ IClassLoader* loader)
{
    String localeName;
    locale->ToString(&localeName);
    String bundleName = localeName.IsEmpty() ? base : (base + String("_") + localeName);
    AutoPtr<ICharSequence> sq;
    CString::New(String("NULL"), (ICharSequence**)&sq);
    AutoPtr<IInterface> cacheKey;
    if (loader != NULL) {
        cacheKey = loader;
    }
    else {
        cacheKey = sq;
    }
    AutoPtr<IDictionary> loaderCache = GetLoaderCache(cacheKey);
    AutoPtr<IInterface> outface;
    sq = NULL;
    CString::New(bundleName, (ICharSequence**)&sq);
    loaderCache->Get(sq, (IInterface**)&outface);
    AutoPtr<IResourceBundle> cached = IResourceBundle::Probe(outface);
    // if (cached != NULL) {
    //     if (cached == MISSINGBASE) {
    //         return NULL;
    //     }
    //     else if (cached == MISSING) {
    //         if (!loadBase) {
    //             return NULL;
    //         }
    //         AutoPtr<ILocale> newLocale = Strip(locale);
    //         if (newLocale == NULL) {
    //             return NULL;
    //         }
    //         return HandleGetBundle(loadBase, base, newLocale, loader);
    //     }
    //     return cached;
    // }

    AutoPtr<IResourceBundle> bundle;
    // try {
    assert(0 && "TODO");
    // Class<?> bundleClass = Class.forName(bundleName, true, loader);
    // if (ResourceBundle.class.isAssignableFrom(bundleClass)) {
    //     bundle = (ResourceBundle) bundleClass.newInstance();
    // }
    // } catch (LinkageError ignored) {
    // } catch (Exception ignored) {
    // }

    if (bundle != NULL) {
        assert(0 && "TODO");
        // bundle->SetLocale(locale);
    }
    else {
        String fileName = bundleName.Replace('.', '/') + String(".properties");
        assert(0 && "TODO");
        // AutoPtr<IInputStream> stream = loader != NULL
        //         ? loader.getResourceAsStream(fileName)
        //         : ClassLoader.getSystemResourceAsStream(fileName);
        // if (stream != NULL) {
        //     try {
        //         bundle = new PropertyResourceBundle(new InputStreamReader(stream, UTF_8));
        //         bundle.setLocale(locale);
        //     } catch (IOException ignored) {
        //     } finally {
        //         IoUtils.closeQuietly(stream);
        //     }
        // }
    }

    sq = NULL;
    CString::New(bundleName, (ICharSequence**)&sq);
    AutoPtr<ILocale> strippedLocale = Strip(locale);
    if (bundle != NULL) {
        if (strippedLocale != NULL) {
            AutoPtr<IResourceBundle> parent = HandleGetBundle(loadBase, base, strippedLocale, loader);
            if (parent != NULL) {
                assert(0 && "TODO");
                // bundle->SetParent(parent);
            }
        }
        loaderCache->Put(sq, bundle, (IInterface**)&outface);
        return bundle;
    }

    String outstr;
    if (strippedLocale != NULL && (loadBase || !(strippedLocale->ToString(&outstr), outstr.IsEmpty()))) {
        bundle = HandleGetBundle(loadBase, base, strippedLocale, loader);
        if (bundle != NULL) {
            loaderCache->Put(sq, bundle, (IInterface**)&outface);
            return bundle;
        }
    }
    assert(0 && "TODO");
    // loaderCache->Put(sq, loadBase ? MISSINGBASE : MISSING, (IInterface**)&outface);
    return NULL;
}

AutoPtr<IDictionary> ResourceBundle::GetLoaderCache(
    /* [in] */ IInterface* cacheKey)
{
    AutoLock lock(mLock);
    AutoPtr<IInterface> outface;
    sCache->Get(cacheKey, (IInterface**)&outface);
    AutoPtr<IDictionary> loaderCache = IDictionary::Probe(outface);
    if (loaderCache == NULL) {
        CHashTable::New((IDictionary**)&loaderCache);
        sCache->Put(cacheKey, loaderCache, (IInterface**)&outface);
    }
    return loaderCache;
}

AutoPtr<ILocale> ResourceBundle::Strip(
    /* [in] */ ILocale* locale)
{
    String language;
    locale->GetLanguage(&language);
    String country;
    locale->GetCountry(&country);
    String variant;
    locale->GetVariant(&variant);
    if (!variant.IsEmpty()) {
        variant = "";
    }
    else if (!country.IsEmpty()) {
        country = "";
    }
    else if (!language.IsEmpty()) {
        language = "";
    }
    else {
        return NULL;
    }
    AutoPtr<ILocale> outloc;
    CLocale::New(language, country, variant, (ILocale**)&outloc);
    return outloc;
}

ECode ResourceBundle::SetLocale(
    /* [in] */ ILocale* locale)
{
    mLocale = locale;
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos

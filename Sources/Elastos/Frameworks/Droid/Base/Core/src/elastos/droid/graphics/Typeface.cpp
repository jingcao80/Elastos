
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/Typeface.h"
#include "elastos/droid/graphics/CTypeface.h"
#include "elastos/droid/graphics/FontFamily.h"
#include "elastos/droid/graphics/TypefaceImpl.h"
#include <elastos/utility/logging/Logger.h>
#include <skia/core/SkStream.h>
#include <skia/core/SkTypeface.h>
#include <androidfw/AssetManager.h>

using Elastos::IO::CFile;
using Elastos::IO::IInputStream;
using Elastos::Utility::Logging::Logger;

extern void skia_set_text_gamma(float, float);

#define GET_TYPEFACE_NATIVE_INSTANCE(obj) ((Typeface*)obj)->mNativeInstance
#define SET_TYPEFACE_NATIVE_INSTANCE(obj, ni) ((Typeface*)obj)->mNativeInstance = ni
#define SET_TYPEFACE_SHOULD_UNREF(obj, value) ((Typeface*)obj)->mShouldUnref = value

namespace Elastos {
namespace Droid {
namespace Graphics {

Typeface::StaticInitializer::StaticInitializer()
{
    Init();
    // Set up defaults and typefaces exposed in public API
    Typeface::Create(String(NULL), 0, (ITypeface**)&DEFAULT_INTERNAL);
    Typeface::Create(String(NULL), ITypeface::BOLD, (ITypeface**)&DEFAULT_BOLD_INTERNAL);
    Typeface::Create(String("sans-serif"), 0, (ITypeface**)&SANS_SERIF_INTERNAL);
    Typeface::Create(String("serif"), 0, (ITypeface**)&SERIF_INTERNAL);
    Typeface::Create(String("monospace"), 0, (ITypeface**)&MONOSPACE_INTERNAL);

    CTypeface::New(GET_TYPEFACE_NATIVE_INSTANCE(DEFAULT_INTERNAL.Get()), (ITypeface**)&DEFAULT);
    SET_TYPEFACE_SHOULD_UNREF(DEFAULT.Get(), FALSE);
    CTypeface::New(GET_TYPEFACE_NATIVE_INSTANCE(DEFAULT_BOLD_INTERNAL.Get()), (ITypeface**)&DEFAULT_BOLD);
    SET_TYPEFACE_SHOULD_UNREF(DEFAULT_BOLD.Get(), FALSE);
    CTypeface::New(GET_TYPEFACE_NATIVE_INSTANCE(SANS_SERIF_INTERNAL.Get()), (ITypeface**)&SANS_SERIF);
    SET_TYPEFACE_SHOULD_UNREF(SANS_SERIF.Get(), FALSE);
    CTypeface::New(GET_TYPEFACE_NATIVE_INSTANCE(SERIF_INTERNAL.Get()), (ITypeface**)&SERIF);
    SET_TYPEFACE_SHOULD_UNREF(SERIF.Get(), FALSE);
    CTypeface::New(GET_TYPEFACE_NATIVE_INSTANCE(MONOSPACE_INTERNAL.Get()), (ITypeface**)&MONOSPACE);
    SET_TYPEFACE_SHOULD_UNREF(MONOSPACE.Get(), FALSE);

    AutoPtr<ITypeface> italic, boldItalic;
    Typeface::Create(String(NULL), ITypeface::ITALIC, (ITypeface**)&italic);
    Typeface::Create(String(NULL), ITypeface::BOLD_ITALIC, (ITypeface**)&boldItalic);

    sDefaults = ArrayOf<ITypeface*>::Alloc(4);
    sDefaults->Set(0, Typeface::DEFAULT);
    sDefaults->Set(1, Typeface::DEFAULT_BOLD);
    sDefaults->Set(2, italic);
    sDefaults->Set(3, boldItalic);
}


const String Typeface::TAG("Typeface");
const String Typeface::FONTS_CONFIG("fonts.xml");
const String Typeface::SANS_SERIF_FAMILY_NAME("sans-serif");
AutoPtr<ITypeface> Typeface::DEFAULT;
AutoPtr<ITypeface> Typeface::DEFAULT_BOLD;
AutoPtr<ITypeface> Typeface::SANS_SERIF;
AutoPtr<ITypeface> Typeface::SERIF;
AutoPtr<ITypeface> Typeface::MONOSPACE;
AutoPtr< ArrayOf<ITypeface*> > Typeface::sDefaults;
HashMap<Int64, AutoPtr<Typeface::TypefaceMap> > Typeface::sTypefaceCache(3);
AutoPtr<ITypeface> Typeface::sDefaultTypeface;
HashMap<String, AutoPtr<ITypeface> > Typeface::sSystemFontMap;
AutoPtr<ArrayOf<IFontFamily*> > Typeface::sFallbackFonts;
AutoPtr<ITypeface> Typeface::DEFAULT_INTERNAL;
AutoPtr<ITypeface> Typeface::DEFAULT_BOLD_INTERNAL;
AutoPtr<ITypeface> Typeface::SANS_SERIF_INTERNAL;
AutoPtr<ITypeface> Typeface::SERIF_INTERNAL;
AutoPtr<ITypeface> Typeface::MONOSPACE_INTERNAL;
const Typeface::StaticInitializer Typeface::sInitializer;

CAR_INTERFACE_IMPL(Typeface, Object, ITypeface);
Typeface::Typeface()
    : mNativeInstance(0)
    , mStyle(0)
    , mShouldUnref(TRUE)
{}

Typeface::~Typeface()
{
    if (mShouldUnref) NativeUnref(mNativeInstance);
}

ECode Typeface::GetStyle(
    /* [out] */ Int32* style)
{
    VALIDATE_NOT_NULL(style);
    *style = mStyle;
    return NOERROR;
}

ECode Typeface::IsBold(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mStyle & ITypeface::BOLD) != 0;
    return NOERROR;
}

ECode Typeface::IsItalic(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mStyle & ITypeface::ITALIC) != 0;
    return NOERROR;
}

ECode Typeface::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* e)
{
    VALIDATE_NOT_NULL(e);
    if (TO_IINTERFACE(this) == IInterface::Probe(o)) {
        *e = TRUE;
        return NOERROR;
    }

    ClassID id1, id2;
    GetClassID(&id1);
    if (o == NULL || id1 != (IObject::Probe(o)->GetClassID(&id2), id2)) {
        *e = FALSE;
        return NOERROR;
    }

    Typeface* typeface = (Typeface*) ITypeface::Probe(o);

    *e = mStyle == typeface->mStyle && mNativeInstance == typeface->mNativeInstance;
    return NOERROR;
}

ECode Typeface::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);
    /*
     * Modified method for hashCode with long native_instance derived from
     * http://developer.android.com/reference/java/lang/Object.html
     */
    Int32 result = 17;
    result = 31 * result + (Int32) (mNativeInstance ^ (mNativeInstance >> 32));
    result = 31 * result + mStyle;
    *code = result;
    return NOERROR;
}

ECode Typeface::Create(
    /* [in] */ const String& familyName,
    /* [in] */ Int32 style,
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface);

    if (sSystemFontMap.GetSize() > 0) {
        HashMap<String, AutoPtr<ITypeface> >::Iterator ator = sSystemFontMap.Find(familyName);
        AutoPtr<ITypeface> t;
        if (ator != sSystemFontMap.End()) {
            t = ator->mSecond;
        }
        return Create(t, style, typeface);
    }
    *typeface = NULL;
    return NOERROR;
}

ECode Typeface::Create(
    /* [in] */ ITypeface* family,
    /* [in] */ Int32 style,
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface);

    if (style < 0 || style > 3) {
        style = 0;
    }
    Int64 ni = 0;
    if (family != NULL) {
        // Return early if we're asked for the same face/style
        Int32 familyStyle;
        family->GetStyle(&familyStyle);
        if (familyStyle == style) {
            *typeface = family;
            REFCOUNT_ADD(*typeface);
            return NOERROR;
        }

        ni =  GET_TYPEFACE_NATIVE_INSTANCE(family);
    }

    AutoPtr<ITypeface> tmpTypeface;
    AutoPtr<TypefaceMap> styles;
    typename TypefaceCacheMap::Iterator it = sTypefaceCache.Find(ni);
    if (it != sTypefaceCache.End()) {
        styles = it->mSecond;
    }

    if (styles != NULL) {
        typename TypefaceMap::Iterator sit = styles->Find(style);
        if (sit != styles->End()) {
            tmpTypeface = sit->mSecond;
            if (tmpTypeface != NULL) {
                *typeface = tmpTypeface;
                REFCOUNT_ADD(*typeface);
                return NOERROR;
            }
        }
    }

    CTypeface::New(NativeCreateFromTypeface(ni, style), (ITypeface**)&tmpTypeface);
    *typeface = tmpTypeface;
    REFCOUNT_ADD(*typeface);

    if (styles == NULL) {
        styles = new TypefaceMap(4);
        sTypefaceCache[ni] = styles;
    }
    (*styles)[style] = tmpTypeface;

    return NOERROR;
}

ECode Typeface::DefaultFromStyle(
    /* [in] */ Int32 style,
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface);

    *typeface = (*sDefaults)[style];
    REFCOUNT_ADD(*typeface);
    return NOERROR;
}

ECode Typeface::CreateFromAsset(
    /* [in] */ IAssetManager* mgr,
    /* [in] */ const String& path,
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface);
    if (sFallbackFonts != NULL) {
        AutoPtr<IFontFamily> fontFamily = new FontFamily();
        Boolean result = FALSE;
        if (fontFamily->AddFontFromAsset(mgr, path, &result), result) {
            AutoPtr<ArrayOf<IFontFamily*> > families = ArrayOf<IFontFamily*>::Alloc(1);
            families->Set(0, fontFamily);
            return CreateFromFamiliesWithDefault(families, typeface);
        }
    }
    *typeface = NULL;
    // throw new RuntimeException("Font asset not found " + path);
    return E_RUNTIME_EXCEPTION;
}

ECode Typeface::CreateFromFile(
    /* [in] */ IFile* path,
    /* [out] */ ITypeface** typeface)
{
    String absPath;
    path->GetAbsolutePath(&absPath);
    return CreateFromFile(absPath, typeface);
}

ECode Typeface::CreateFromFile(
    /* [in]*/ const String& path,
    /* [out] */ ITypeface** typeface)
{
    VALIDATE_NOT_NULL(typeface);
    if (sFallbackFonts != NULL) {
        AutoPtr<IFontFamily> fontFamily = new FontFamily();
        Boolean result = FALSE;
        if (fontFamily->AddFont(path, &result), result) {
            AutoPtr<ArrayOf<IFontFamily*> > families = ArrayOf<IFontFamily*>::Alloc(1);
            families->Set(0, fontFamily);
            return CreateFromFamiliesWithDefault(families, typeface);
        }
    }
    *typeface = NULL;
    // throw new RuntimeException("Font not found " + path);
    return E_RUNTIME_EXCEPTION;
}

ECode Typeface::CreateFromFamilies(
    /* [in]*/ ArrayOf<IFontFamily*>* families,
    /* [out]*/ ITypeface** typeface)
{
    AutoPtr<ArrayOf<Int64> > ptrArray = ArrayOf<Int64>::Alloc(families->GetLength());
    for (Int32 i = 0; i < families->GetLength(); i++) {
        (*ptrArray)[i] = ((FontFamily*)(*families)[i])->mNativePtr;
    }
    return CTypeface::New(NativeCreateFromArray(ptrArray), typeface);
}

ECode Typeface::CreateFromFamiliesWithDefault(
    /* [in]*/ ArrayOf<IFontFamily*>* families,
    /* [out]*/ ITypeface** typeface)
{
    Int32 len = families->GetLength();
    AutoPtr<ArrayOf<Int64> > ptrArray = ArrayOf<Int64>::Alloc(len + sFallbackFonts->GetLength());
    for (Int32 i = 0; i < len; i++) {
        (*ptrArray)[i] = ((FontFamily*)(*families)[i])->mNativePtr;
    }
    for (Int32 i = 0; i < sFallbackFonts->GetLength(); i++) {
        (*ptrArray)[i + len] = ((FontFamily*)(*sFallbackFonts)[i])->mNativePtr;
    }
    return CTypeface::New(NativeCreateFromArray(ptrArray), typeface);
}

ECode Typeface::constructor(
    /* [in] */ Int64 ni)
{
    if (ni == 0) {
//        throw new RuntimeException("native typeface cannot be made");
        return E_RUNTIME_EXCEPTION;
    }
    mNativeInstance = ni;
    mStyle = NativeGetStyle(ni);
    return NOERROR;
}

void Typeface::SetDefault(
    /* [in] */ ITypeface* t)
{
    sDefaultTypeface = t;
    NativeSetDefault(GET_TYPEFACE_NATIVE_INSTANCE(t));
}

Int64 Typeface::NativeCreateFromTypeface(
    /* [in] */ Int64 familyHandle,
    /* [in] */ Int32 style)
{
    TypefaceImpl* family = reinterpret_cast<TypefaceImpl*>(familyHandle);
    TypefaceImpl* face = TypefaceImpl_createFromTypeface(family, (SkTypeface::Style)style);
    // TODO: the following logic shouldn't be necessary, the above should always succeed.
    // Try to find the closest matching font, using the standard heuristic
    if (NULL == face) {
        face = TypefaceImpl_createFromTypeface(family, (SkTypeface::Style)(style ^ SkTypeface::kItalic));
    }
    for (int i = 0; NULL == face && i < 4; i++) {
        face = TypefaceImpl_createFromTypeface(family, (SkTypeface::Style)i);
    }
    return reinterpret_cast<Int64>(face);
}

Int64 Typeface::NativeCreateWeightAlias(
    /* [in] */ Int64 familyHandle,
    /* [in] */ Int32 weight)
{
    TypefaceImpl* family = reinterpret_cast<TypefaceImpl*>(familyHandle);
    TypefaceImpl* face = TypefaceImpl_createWeightAlias(family, weight);
    return reinterpret_cast<Int64>(face);
}

void Typeface::NativeUnref(
    /* [in] */ Int64 faceHandle)
{
    if (faceHandle) {
        TypefaceImpl* face = reinterpret_cast<TypefaceImpl*>(faceHandle);
        TypefaceImpl_unref(face);
    }
}

Int32 Typeface::NativeGetStyle(
    /* [in] */ Int64 faceHandle)
{
    TypefaceImpl* face = reinterpret_cast<TypefaceImpl*>(faceHandle);
    return TypefaceImpl_getStyle(face);
}

Int64 Typeface::NativeCreateFromArray(
    /* [in] */ ArrayOf<Int64>* familyArray)
{
    return reinterpret_cast<Int64>(TypefaceImpl_createFromFamilies(familyArray->GetPayload(), familyArray->GetLength()));
}

void Typeface::NativeSetDefault(
    /* [in] */ Int64 faceHandle)
{
    TypefaceImpl* face = reinterpret_cast<TypefaceImpl*>(faceHandle);
    return TypefaceImpl_setDefault(face);
}

class AssetStream : public SkStream
{
public:
    AssetStream(android::Asset* asset, bool hasMemoryBase) : fAsset(asset)
    {
        fMemoryBase = hasMemoryBase ? fAsset->getBuffer(false) : NULL;
    }

    virtual ~AssetStream()
    {
        delete fAsset;
    }

    virtual const void* getMemoryBase()
    {
        return fMemoryBase;
    }

    virtual bool rewind()
    {
        off_t pos = fAsset->seek(0, SEEK_SET);
        return pos != (off_t)-1;
    }

    virtual size_t read(void* buffer, size_t size)
    {
        ssize_t amount;

        if (NULL == buffer)
        {
            if (0 == size)  // caller is asking us for our total length
                return fAsset->getLength();

            // asset->seek returns new total offset
            // we want to return amount that was skipped

            off_t oldOffset = fAsset->seek(0, SEEK_CUR);
            if (-1 == oldOffset)
                return 0;
            off_t newOffset = fAsset->seek(size, SEEK_CUR);
            if (-1 == newOffset)
                return 0;

            amount = newOffset - oldOffset;
        }
        else
        {
            amount = fAsset->read(buffer, size);
        }

        if (amount < 0)
            amount = 0;
        return amount;
    }

private:
    android::Asset* fAsset;
    const void* fMemoryBase;
};

AutoPtr<IFontFamily> Typeface::MakeFamilyFromParsed(
    /* [in] */ FontListParser::Family* family)
{
    AutoPtr<FontFamily> fontFamily = new FontFamily(family->mLang, family->mVariant);
    Int32 N;
    family->mFonts->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        family->mFonts->Get(i, (IInterface**)&obj);
        FontListParser::Font* font = (FontListParser::Font*)IObject::Probe(obj);
        Boolean result = FALSE;
        fontFamily->AddFontWeightStyle(font->mFontName, font->mWeight, font->mIsItalic, &result);
    }
    return fontFamily;
}

void Typeface::AddFallbackFontsForFamilyName(
    /* [in] */ FontListParser::Config* src,
    /* [in] */ FontListParser::Config* dst,
    /* [in] */ const String& familyName)
{
    Int32 N;
    src->mFamilies->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        src->mFamilies->Get(i, (IInterface**)&obj);
        FontListParser::Family* srcFamily = (FontListParser::Family*)IObject::Probe(obj);
        if (familyName.Equals(srcFamily->mName)) {
            // set the name to null so that it will be added as a fallback
            srcFamily->mName = NULL;
            dst->mFamilies->Add((IObject*)srcFamily);
            return;
        }
    }
}

void Typeface::AddMissingFontFamilies(
    /* [in] */ FontListParser::Config* src,
    /* [in] */ FontListParser::Config* dst)
{
    Int32 N;
    dst->mFamilies->GetSize(&N);
    // add missing families
    Int32 NN;
    src->mFamilies->GetSize(&NN);
    for (Int32 j = 0; j < NN; j++) {
        AutoPtr<IInterface> obj;
        src->mFamilies->Get(j, (IInterface**)&obj);
        FontListParser::Family* srcFamily = (FontListParser::Family*)IObject::Probe(obj);
        Boolean addFamily = TRUE;
        for (Int32 i = 0; i < N && addFamily; i++) {
            AutoPtr<IInterface> dstObj;
            dst->mFamilies->Get(i, (IInterface**)&dstObj);
            FontListParser::Family* dstFamily = (FontListParser::Family*)IObject::Probe(dstObj);
            String dstFamilyName = dstFamily->mName;
            if (!dstFamilyName.IsNull() && dstFamilyName.Equals(srcFamily->mName)) {
                addFamily = FALSE;
                break;
            }
        }
        if (addFamily) {
            dst->mFamilies->Add((IObject*)srcFamily);
        }
    }
}

void Typeface::AddMissingFontAliases(
    /* [in] */ FontListParser::Config* src,
    /* [in] */ FontListParser::Config* dst)
{
    Int32 N;
    dst->mAliases->GetSize(&N);
    // add missing aliases
    Int32 NN;
    src->mAliases->GetSize(&NN);
    for (Int32 j = 0; j < NN; j++) {
        AutoPtr<IInterface> obj;
        src->mAliases->Get(j, (IInterface**)&obj);
        FontListParser::Alias* srcAlias = (FontListParser::Alias*)IObject::Probe(obj);
        Boolean addAlias = TRUE;
        for (Int32 i = 0; i < N && addAlias; i++) {
            AutoPtr<IInterface> dstObj;
            dst->mAliases->Get(i, (IInterface**)&dstObj);
            FontListParser::Alias* dstAlias = (FontListParser::Alias*)IObject::Probe(dstObj);
            String dstAliasName = dstAlias->mName;
            if (!dstAliasName.IsNull() && dstAliasName.Equals(srcAlias->mName)) {
                addAlias = FALSE;
                break;
            }
        }
        if (addAlias) {
            dst->mAliases->Add((IObject*)srcAlias);
        }
    }
}

void Typeface::Init()
{
    // Load font config and initialize Minikin state
    AutoPtr<IFile> systemFontConfigLocation = GetSystemFontConfigLocation();
    AutoPtr<IFile> themeFontConfigLocation = GetThemeFontConfigLocation();

    AutoPtr<IFile> systemConfigFile;
    CFile::New(systemFontConfigLocation, FONTS_CONFIG, (IFile**)&systemConfigFile);
    AutoPtr<IFile> themeConfigFile;
    CFile::New(themeFontConfigLocation, FONTS_CONFIG, (IFile**)&themeConfigFile);
    AutoPtr<IFile> configFile;
    AutoPtr<IFile> fontDir;

    Boolean isExisted;
    if (themeConfigFile->Exists(&isExisted), isExisted) {
        configFile = themeConfigFile;
        fontDir = GetThemeFontDirLocation();
    }
    else {
        configFile = systemConfigFile;
        fontDir = GetSystemFontDirLocation();
    }

    // try {
    ECode ec = NOERROR;
    Int32 N = 0, i = 0;
    AutoPtr<IList> familyList;
    AutoPtr<ITypeface> tmp;
    AutoPtr<FontListParser::Config> fontConfig;
    AutoPtr<FontListParser::Config> systemFontConfig;
    ec = FontListParser::Parse(configFile, fontDir, (FontListParser::Config**)&fontConfig);
    FAIL_GOTO(ec, Error);

    // If the fonts are coming from a theme, we will need to make sure that we include
    // any font families from the system fonts that the theme did not include.
    // NOTE: All the system font families without names ALWAYS get added.
    if (configFile == themeConfigFile) {
        ec = FontListParser::Parse(systemConfigFile, GetSystemFontDirLocation(), (FontListParser::Config**)&systemFontConfig);
        FAIL_GOTO(ec, Error);
        AddMissingFontFamilies(systemFontConfig, fontConfig);
        AddMissingFontAliases(systemFontConfig, fontConfig);
        AddFallbackFontsForFamilyName(systemFontConfig, fontConfig, SANS_SERIF_FAMILY_NAME);
    }

    CArrayList::New((IList**)&familyList);

    // Note that the default typeface is always present in the fallback list;
    // this is an enhancement from pre-Minikin behavior.
    fontConfig->mFamilies->GetSize(&N);
    for (i = 0; i < N; ++i) {
        AutoPtr<IInterface> obj;
        fontConfig->mFamilies->Get(i, (IInterface**)&obj);
        FontListParser::Family* f = (FontListParser::Family*)IObject::Probe(obj);
        if (i == 0 || f->mName.IsNull()) {
            familyList->Add(MakeFamilyFromParsed(f));
        }
    }

    familyList->GetSize(&N);
    sFallbackFonts = ArrayOf<IFontFamily*>::Alloc(N);
    for (i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        familyList->Get(i, (IInterface**)&obj);
        sFallbackFonts->Set(i, IFontFamily::Probe(obj));
    }

    ec = Typeface::CreateFromFamilies(sFallbackFonts, (ITypeface**)&tmp);
    FAIL_GOTO(ec, Error);
    SetDefault(tmp);

    fontConfig->mFamilies->GetSize(&N);
    for (i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        fontConfig->mFamilies->Get(i, (IInterface**)&obj);
        FontListParser::Family* f = (FontListParser::Family*)IObject::Probe(obj);
        AutoPtr<ITypeface> typeface;
        if (!f->mName.IsNull()) {
            if (i == 0) {
                // The first entry is the default typeface; no sense in
                // duplicating the corresponding FontFamily.
                typeface = sDefaultTypeface;
            }
            else {
                AutoPtr<IFontFamily> fontFamily = MakeFamilyFromParsed(f);
                AutoPtr<ArrayOf<IFontFamily*> > families = ArrayOf<IFontFamily*>::Alloc(1);
                families->Set(0, fontFamily);
                Typeface::CreateFromFamiliesWithDefault(families, (ITypeface**)&typeface);
            }
            sSystemFontMap[f->mName] = typeface;
        }
    }
    fontConfig->mAliases->GetSize(&N);
    for (i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        fontConfig->mAliases->Get(i, (IInterface**)&obj);
        FontListParser::Alias* alias = (FontListParser::Alias*)IObject::Probe(obj);
        HashMap<String, AutoPtr<ITypeface> >::Iterator sfIter = sSystemFontMap.Find(alias->mToName);
        AutoPtr<ITypeface> base;
        if (sfIter != sSystemFontMap.End()) {
            base = sfIter->mSecond;
        }
        AutoPtr<ITypeface> newFace = base;
        Int32 weight = alias->mWeight;
        if (weight != 400) {
            newFace = NULL;
            CTypeface::New(NativeCreateWeightAlias(GET_TYPEFACE_NATIVE_INSTANCE(base.Get()), weight), (ITypeface**)&newFace);
        }
        sSystemFontMap[alias->mName] = newFace;
    }
    return;

Error:
    String filename;
    configFile->GetName(&filename);
    if ((ECode)E_RUNTIME_EXCEPTION == ec) {
        Logger::W(TAG, "Didn't create default family (most likely, non-Minikin build)");
        // TODO: normal in non-Minikin case, remove or make error when Minikin-only
    }
    else if ((ECode)E_FILE_NOT_FOUND_EXCEPTION == ec) {
        Logger::E(TAG, "Error opening: %s", filename.string());
    }
    else if ((ECode)E_IO_EXCEPTION == ec) {
        Logger::E(TAG, "Error reading: %s", filename.string());
    }
    else if ((ECode)E_XML_PULL_PARSER_EXCEPTION == ec) {
        Logger::E(TAG, "XML parse exception for: %s", filename.string());
    }
}

void Typeface::RecreateDefaults()
{
    sTypefaceCache.Clear();
    sSystemFontMap.Clear();
    Init();

    DEFAULT_INTERNAL = NULL;
    DEFAULT_BOLD_INTERNAL = NULL;
    SANS_SERIF_INTERNAL = NULL;
    SERIF_INTERNAL = NULL;
    MONOSPACE_INTERNAL = NULL;
    Typeface::Create(String(NULL), 0, (ITypeface**)&DEFAULT_INTERNAL);
    Typeface::Create(String(NULL), ITypeface::BOLD, (ITypeface**)&DEFAULT_BOLD_INTERNAL);
    Typeface::Create(String("sans-serif"), 0, (ITypeface**)&SANS_SERIF_INTERNAL);
    Typeface::Create(String("serif"), 0, (ITypeface**)&SERIF_INTERNAL);
    Typeface::Create(String("monospace"), 0, (ITypeface**)&MONOSPACE_INTERNAL);

    SET_TYPEFACE_NATIVE_INSTANCE(DEFAULT.Get(), GET_TYPEFACE_NATIVE_INSTANCE(DEFAULT_INTERNAL.Get()));
    SET_TYPEFACE_NATIVE_INSTANCE(DEFAULT_BOLD.Get(), GET_TYPEFACE_NATIVE_INSTANCE(DEFAULT_BOLD_INTERNAL.Get()));
    SET_TYPEFACE_NATIVE_INSTANCE(SANS_SERIF.Get(), GET_TYPEFACE_NATIVE_INSTANCE(SANS_SERIF_INTERNAL.Get()));
    SET_TYPEFACE_NATIVE_INSTANCE(SERIF.Get(), GET_TYPEFACE_NATIVE_INSTANCE(SERIF_INTERNAL.Get()));
    SET_TYPEFACE_NATIVE_INSTANCE(MONOSPACE.Get(), GET_TYPEFACE_NATIVE_INSTANCE(MONOSPACE_INTERNAL.Get()));

    AutoPtr<ITypeface> italic, boldItalic;
    Typeface::Create(String(NULL), ITypeface::ITALIC, (ITypeface**)&italic);
    Typeface::Create(String(NULL), ITypeface::BOLD_ITALIC, (ITypeface**)&boldItalic);
    sDefaults->Set(2, italic);
    sDefaults->Set(3, boldItalic);
}

AutoPtr<IFile> Typeface::GetSystemFontConfigLocation()
{
    AutoPtr<IFile> file;
    CFile::New(String("/system/etc/"), (IFile**)&file);
    return file;
}

AutoPtr<IFile> Typeface::GetSystemFontDirLocation()
{
    AutoPtr<IFile> file;
    CFile::New(String("/system/fonts/"), (IFile**)&file);
    return file;
}

AutoPtr<IFile> Typeface::GetThemeFontConfigLocation()
{
    AutoPtr<IFile> file;
    CFile::New(String("/data/system/theme/fonts/"), (IFile**)&file);
    return file;
}

AutoPtr<IFile> Typeface::GetThemeFontDirLocation()
{
    AutoPtr<IFile> file;
    CFile::New(String("/data/system/theme/fonts/"), (IFile**)&file);
    return file;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

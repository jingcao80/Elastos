
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
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::Logging::Logger;

extern void skia_set_text_gamma(float, float);

namespace Elastos {
namespace Droid {
namespace Graphics {

Typeface::StaticInitializer::StaticInitializer()
{
    Init();
    // Set up defaults and typefaces exposed in public API
    Typeface::Create(String(NULL), 0, (ITypeface**)&Typeface::DEFAULT);
    Typeface::Create(String(NULL), ITypeface::BOLD, (ITypeface**)&Typeface::DEFAULT_BOLD);
    Typeface::Create(String("sans-serif"), 0, (ITypeface**)&Typeface::SANS_SERIF);
    Typeface::Create(String("serif"), 0, (ITypeface**)&Typeface::SERIF);
    Typeface::Create(String("monospace"), 0, (ITypeface**)&Typeface::MONOSPACE);

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
AutoPtr<ITypeface> Typeface::DEFAULT;
AutoPtr<ITypeface> Typeface::DEFAULT_BOLD;
AutoPtr<ITypeface> Typeface::SANS_SERIF;
AutoPtr<ITypeface> Typeface::SERIF;
AutoPtr<ITypeface> Typeface::MONOSPACE;
AutoPtr< ArrayOf<ITypeface*> > Typeface::sDefaults;
HashMap<Int32, AutoPtr<Typeface::TypefaceMap> > Typeface::sTypefaceCache(3);
AutoPtr<ITypeface> Typeface::sDefaultTypeface;
HashMap<String, AutoPtr<ITypeface> > Typeface::sSystemFontMap;
AutoPtr<ArrayOf<IFontFamily*> > Typeface::sFallbackFonts;
const Typeface::StaticInitializer Typeface::sInitializer;

CAR_INTERFACE_IMPL(Typeface, Object, ITypeface);
Typeface::Typeface()
    : mNativeInstance(0)
    , mStyle(0)
{}

Typeface::~Typeface()
{
    NativeUnref(mNativeInstance);
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
    if (THIS_PROBE(IInterface) == IInterface::Probe(o)) {
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

        ni = ((Typeface*)family)->mNativeInstance;
    }

    AutoPtr<ITypeface> tmpTypeface;
    AutoPtr<TypefaceMap> styles;
    HashMap<Int32, AutoPtr<TypefaceMap> >::Iterator it = sTypefaceCache.Find(ni);
    if (it != sTypefaceCache.End()) {
        styles = it->mSecond;
    }

    if (styles != NULL) {
        typename TypefaceMap::Iterator sit = styles->Find(style);
        if (sit != styles->End()) {
            tmpTypeface = sit->mSecond;
        }
        if (tmpTypeface != NULL) {
            *typeface = tmpTypeface;
            REFCOUNT_ADD(*typeface);
            return NOERROR;
        }
    }

    AutoPtr<CTypeface> tmpType;
    CTypeface::NewByFriend(NativeCreateFromTypeface(ni, style), (CTypeface**)&tmpType);
    *typeface = (ITypeface*)tmpType.Get();
    REFCOUNT_ADD(*typeface);
    if (styles == NULL) {
        styles = new TypefaceMap(4);
        sTypefaceCache[ni] = styles;
    }
    (*styles)[style] = *typeface;

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
    NativeSetDefault(((Typeface*)t)->mNativeInstance);
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
    TypefaceImpl* face = reinterpret_cast<TypefaceImpl*>(faceHandle);
    TypefaceImpl_unref(face);
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

void Typeface::Init()
{
    // Load font config and initialize Minikin state
    AutoPtr<IFile> systemFontConfigLocation = GetSystemFontConfigLocation();
    AutoPtr<IFile> configFilename;
    CFile::New(systemFontConfigLocation, FONTS_CONFIG, (IFile**)&configFilename);
    // try {
    ECode ec = NOERROR;
    Int32 N = 0, i = 0;
    AutoPtr<IList> familyList;
    CArrayList::New((IList**)&familyList);
    AutoPtr<ITypeface> tmp;
    HashMap<String, AutoPtr<ITypeface> >::Iterator sfIter;
    AutoPtr<FontListParser::Config> fontConfig;
    AutoPtr<IFileInputStream> fontsIn;
    ec = CFileInputStream::New(configFilename, (IFileInputStream**)&fontsIn);
    FAIL_GOTO(ec, Error);
    ec = FontListParser::Parse(IInputStream::Probe(fontsIn), (FontListParser::Config**)&fontConfig);
    FAIL_GOTO(ec, Error);

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
        sfIter = sSystemFontMap.Find(alias->mToName);
        AutoPtr<ITypeface> base;
        if (sfIter != sSystemFontMap.End()) {
            base = sfIter->mSecond;
        }
        AutoPtr<ITypeface> newFace = base;
        Int32 weight = alias->mWeight;
        if (weight != 400) {
            CTypeface::New(NativeCreateWeightAlias(((Typeface*)base.Get())->mNativeInstance, weight), (ITypeface**)&newFace);
        }
        sSystemFontMap[alias->mName] = newFace;
    }
    return;

Error:
    String filename;
    configFilename->GetName(&filename);
    if ((ECode)E_RUNTIME_EXCEPTION == ec) {
        Logger::W(TAG, String("Didn't create default family (most likely, non-Minikin build)"));
        // TODO: normal in non-Minikin case, remove or make error when Minikin-only
    }
    else if ((ECode)E_FILE_NOT_FOUND_EXCEPTION == ec) {
        Logger::E(TAG, String("Error opening: %s"), filename.string());
    }
    else if ((ECode)E_IO_EXCEPTION == ec) {
        Logger::E(TAG, String("Error reading: %s"), filename.string());
    }
    else if ((ECode)E_XML_PULL_PARSER_EXCEPTION == ec) {
        Logger::E(TAG, String("XML parse exception for: %s"), filename.string());
    }
}

AutoPtr<IFile> Typeface::GetSystemFontConfigLocation()
{
    AutoPtr<IFile> file;
    CFile::New(String("/system/etc/"), (IFile**)&file);
    return file;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

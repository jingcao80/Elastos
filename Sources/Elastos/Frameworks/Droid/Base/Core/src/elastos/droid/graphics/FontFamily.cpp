
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/FontFamily.h"
#include "elastos/droid/graphics/MinikinSkia.h"
#include "elastos/droid/graphics/Utils.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include <elastos/utility/logging/Logger.h>
#include <skia/core/SkTypeface.h>
#include <androidfw/AssetManager.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::Res::CAssetManager;

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_INTERFACE_IMPL(FontFamily, Object, IFontFamily);
FontFamily::FontFamily()
{
    mNativePtr = NativeCreateFamily(String(NULL), 0);
    if (mNativePtr == 0) {
        // throw new IllegalStateException("error creating native FontFamily");
        assert(0 && "error creating native FontFamily");
    }
}

FontFamily::FontFamily(
    /* [in] */ const String& lang,
    /* [in] */ const String& variant)
{
    Int32 varEnum = 0;
    if (String("compact").Equals(variant)) {
        varEnum = 1;
    } else if (String("elegant").Equals(variant)) {
        varEnum = 2;
    }
    mNativePtr = NativeCreateFamily(lang, varEnum);
    if (mNativePtr == 0) {
        // throw new IllegalStateException("error creating native FontFamily");
        assert(0 && "error creating native FontFamily");
    }
}

FontFamily::~FontFamily()
{
    NativeUnrefFamily(mNativePtr);
}

ECode FontFamily::AddFont(
    /* [in] */ const String& path,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NativeAddFont(mNativePtr, path);
    return NOERROR;
}

ECode FontFamily::AddFontWeightStyle(
    /* [in] */ const String& path,
    /* [in] */ Int32 weight,
    /* [in] */ Boolean style,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NativeAddFontWeightStyle(mNativePtr, path, weight, style);
    return NOERROR;
}

ECode FontFamily::AddFontFromAsset(
    /* [in] */ IAssetManager* mgr,
    /* [in] */ const String& path,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NativeAddFontFromAsset(mNativePtr, mgr, path);
    return NOERROR;
}

Int64 FontFamily::NativeCreateFamily(
    /* [in] */ const String& lang,
    /* [in] */ Int32 variant)
{
    android::FontLanguage fontLanguage;
    if (!lang.IsNull()) {
        fontLanguage = android::FontLanguage(lang.string(), lang.GetLength());
    }
    return (Int64)new android::FontFamily(fontLanguage, variant);
}

void FontFamily::NativeUnrefFamily(
    /* [in] */ Int64 familyPtr)
{
    android::FontFamily* fontFamily = reinterpret_cast<android::FontFamily*>(familyPtr);
    fontFamily->Unref();
}

static Boolean addSkTypeface(
    /* [in] */ android::FontFamily* family,
    /* [in] */ SkTypeface* face)
{
    MinikinFont* minikinFont = new MinikinFontSkia(face);
    bool result = family->addFont(minikinFont);
    minikinFont->Unref();
    return result ? TRUE : FALSE;
}

Boolean FontFamily::NativeAddFont(
    /* [in] */ Int64 familyPtr,
    /* [in] */ const String& path)
{
    if (path.IsNull()) {
        return FALSE;
    }
    SkTypeface* face = SkTypeface::CreateFromFile(path.string());
    if (face == NULL) {
        Logger::E(String("FontFamily"), String("addFont failed to create font %s"), path.string());
        return FALSE;
    }
    android::FontFamily* fontFamily = reinterpret_cast<android::FontFamily*>(familyPtr);
    return addSkTypeface(fontFamily, face);
}

Boolean FontFamily::NativeAddFontWeightStyle(
    /* [in] */ Int64 familyPtr,
    /* [in] */ const String& path,
    /* [in] */ Int32 weight,
    /* [in] */ Boolean isItalic)
{
    if (path.IsNull()) {
        return FALSE;
    }
    SkTypeface* face = SkTypeface::CreateFromFile(path.string());
    if (face == NULL) {
        Logger::E(String("FontFamily"), String("addFont failed to create font %s"), path.string());
        return FALSE;
    }
    android::FontFamily* fontFamily = reinterpret_cast<android::FontFamily*>(familyPtr);
    MinikinFont* minikinFont = new MinikinFontSkia(face);
    fontFamily->addFont(minikinFont, android::FontStyle(weight / 100, isItalic));
    minikinFont->Unref();
    return TRUE;
}

Boolean FontFamily::NativeAddFontFromAsset(
    /* [in] */ Int64 familyPtr,
    /* [in] */ IAssetManager* assetMgr,
    /* [in] */ const String& path)
{
    if (assetMgr == NULL || path.IsNull()) {
        return FALSE;
    }

    android::AssetManager* mgr = (android::AssetManager*)((CAssetManager*)assetMgr)->Ni();
    if (NULL == mgr) {
        return FALSE;
    }

    android::Asset* asset = mgr->open(path.string(), android::Asset::ACCESS_BUFFER);
    if (NULL == asset) {
        return FALSE;
    }

    SkStream* stream = new AssetStreamAdaptor(asset,
                                              AssetStreamAdaptor::kYes_OwnAsset,
                                              AssetStreamAdaptor::kYes_HasMemoryBase);
    SkTypeface* face = SkTypeface::CreateFromStream(stream);
    // Note: SkTypeface::CreateFromStream holds its own reference to the stream
    stream->unref();
    if (face == NULL) {
        Logger::E(String("FontFamily"), String("addFontFromAsset failed to create font %s"), path.string());
        return FALSE;
    }
    android::FontFamily* fontFamily = reinterpret_cast<android::FontFamily*>(familyPtr);
    return addSkTypeface(fontFamily, face);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

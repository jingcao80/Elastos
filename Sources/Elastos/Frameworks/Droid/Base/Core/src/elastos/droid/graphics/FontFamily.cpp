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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/FontFamily.h"
#include "elastos/droid/graphics/FontUtils.h"
#include "elastos/droid/graphics/Utils.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include <elastos/utility/logging/Logger.h>
#include <skia/core/SkTypeface.h>
#include <skia/ports/SkFontMgr.h>
#include <androidfw/AssetManager.h>
#include <minikin/FontFamily.h>
#include <utils/FatVector.h>
#include <hwui/MinikinSkia.h>

using Elastos::Droid::Content::Res::CAssetManager;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IBuffer;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IMappedByteBuffer;
using Elastos::IO::Channels::FileChannelMapMode_READ_ONLY;
using Elastos::IO::Channels::IFileChannel;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_INTERFACE_IMPL(FontFamily, Object, IFontFamily);

FontFamily::FontFamily()
{
    mBuilderPtr = NativeCreateBuilder(String(NULL), 0);
}

FontFamily::FontFamily(
    /* [in] */ const String& lang,
    /* [in] */ const String& variant)
{
    Int32 varEnum = 0;
    if (variant.Equals("compact")) {
        varEnum = 1;
    } else if (variant.Equals("elegant")) {
        varEnum = 2;
    }
    mBuilderPtr = NativeCreateBuilder(lang, varEnum);
}

FontFamily::~FontFamily()
{
    NativeUnrefFamily(mNativePtr);
    NativeAbort(mBuilderPtr);
}

ECode FontFamily::AddFont(
    /* [in] */ const String& path,
    /* [in] */ Int32 ttcIndex,
    /* [in] */ ArrayOf<IFontVariationAxis*>* axes,
    /* [in] */ Int32 weight,
    /* [in] */ Int32 italic,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IFileInputStream> file;
    CFileInputStream::New(path, (IFileInputStream**)&file);
    AutoPtr<IFileChannel> fileChannel;
    file->GetChannel((IFileChannel**)&fileChannel);
    Int64 fontSize;
    fileChannel->GetSize(&fontSize);
    AutoPtr<IMappedByteBuffer> mbb;
    fileChannel->Map(FileChannelMapMode_READ_ONLY, 0, fontSize, (IMappedByteBuffer**)&mbb);
    IByteBuffer* fontBuffer = IByteBuffer::Probe(mbb);
    if (axes != NULL) {
        for (Int32 i = 0; i < axes->GetLength(); i++) {
            AutoPtr<IFontVariationAxis> axis = (*axes)[i];
            Int32 tagValue;
            axis->GetOpenTypeTagValue(&tagValue);
            Float styleValue;
            axis->GetStyleValue(&styleValue);
            NativeAddAxisValue(mBuilderPtr, tagValue, styleValue);
        }
    }
    *result = NativeAddFont(mBuilderPtr, fontBuffer, ttcIndex, weight, italic);
    return NOERROR;
}

ECode FontFamily::AddFontFromBuffer(
    /* [in] */ IByteBuffer* font,
    /* [in] */ Int32 ttcIndex,
    /* [in] */ ArrayOf<IFontVariationAxis*>* axes,
    /* [in] */ Int32 weight,
    /* [in] */ Int32 italic,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mBuilderPtr == 0) {
        // throw new IllegalStateException("Unable to call addFontWeightStyle after freezing.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (axes != NULL) {
        for (Int32 i = 0; i < axes->GetLength(); i++) {
            AutoPtr<IFontVariationAxis> axis = (*axes)[i];
            Int32 tagValue;
            axis->GetOpenTypeTagValue(&tagValue);
            Float styleValue;
            axis->GetStyleValue(&styleValue);
            NativeAddAxisValue(mBuilderPtr, tagValue, styleValue);
        }
    }
    *result = NativeAddFontWeightStyle(mBuilderPtr, font, ttcIndex, weight, italic);
    return NOERROR;
}

ECode FontFamily::AddFontFromAsset(
    /* [in] */ IAssetManager* mgr,
    /* [in] */ const String& path,
    /* [in] */ Int32 cookie,
    /* [in] */ Boolean isAsset,
    /* [in] */ Int32 ttcIndex,
    /* [in] */ Int32 weight,
    /* [in] */ Int32 isItalic,
    /* [in] */ ArrayOf<IFontVariationAxis*>* axes,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mBuilderPtr == 0) {
        // throw new IllegalStateException("Unable to call addFontFromAsset after freezing.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (axes != NULL) {
        for (Int32 i = 0; i < axes->GetLength(); i++) {
            AutoPtr<IFontVariationAxis> axis = (*axes)[i];
            Int32 tagValue;
            axis->GetOpenTypeTagValue(&tagValue);
            Float styleValue;
            axis->GetStyleValue(&styleValue);
            NativeAddAxisValue(mBuilderPtr, tagValue, styleValue);
        }
    }
    *result = NativeAddFontFromAsset(mNativePtr, mgr, path, cookie, isAsset, ttcIndex, weight,
            isItalic);
    return NOERROR;
}

ECode FontFamily::Freeze(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mBuilderPtr == 0) {
        // throw new IllegalStateException("Unable to call freeze after freezing.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mNativePtr = NativeCreateFamily(mBuilderPtr);
    mBuilderPtr = 0;
    *result = mNativePtr != 0;
    return NOERROR;
}

ECode FontFamily::AbortCreation()
{
    if (mBuilderPtr == 0) {
        // throw new IllegalStateException("Unable to call abortCreation after freezing.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    NativeAbort(mBuilderPtr);
    mBuilderPtr = 0;
    return NOERROR;
}

struct NativeFamilyBuilder
{
    NativeFamilyBuilder(uint32_t langId, int variant)
        : langId(langId), variant(variant), allowUnsupportedFont(false) {}
    uint32_t langId;
    int variant;
    bool allowUnsupportedFont;
    std::vector<minikin::Font> fonts;
    std::vector<minikin::FontVariation> axes;
};

Int64 FontFamily::NativeCreateBuilder(
    /* [in] */ const String& lang,
    /* [in] */ Int32 variant)
{
    NativeFamilyBuilder* builder;
    if (!lang.IsNull()) {
        builder = new NativeFamilyBuilder(
                minikin::FontStyle::registerLanguageList(lang.string()), variant);
    } else {
        builder = new NativeFamilyBuilder(minikin::FontStyle::registerLanguageList(""), variant);
    }
    return reinterpret_cast<Int64>(builder);
}

void FontFamily::NativeAbort(
    /* [in] */ Int64 builderPtr)
{
    NativeFamilyBuilder* builder = reinterpret_cast<NativeFamilyBuilder*>(builderPtr);
    delete builder;
}

Int64 FontFamily::NativeCreateFamily(
    /* [in] */ Int64 builderPtr)
{
    if (builderPtr == 0) {
        return 0;
    }
    std::unique_ptr<NativeFamilyBuilder> builder(
            reinterpret_cast<NativeFamilyBuilder*>(builderPtr));
    std::shared_ptr<minikin::FontFamily> family = std::make_shared<minikin::FontFamily>(
            builder->langId, builder->variant, std::move(builder->fonts));
    if (family->getCoverage().length() == 0 && !builder->allowUnsupportedFont) {
        return 0;
    }
    return reinterpret_cast<Int64>(new FontFamilyWrapper(std::move(family)));
}

void FontFamily::NativeUnrefFamily(
    /* [in] */ Int64 familyPtr)
{
    FontFamilyWrapper* family = reinterpret_cast<FontFamilyWrapper*>(familyPtr);
    delete family;
}

static void release_global_ref(const void* /*data*/, void* context)
{
    IByteBuffer* font = reinterpret_cast<IByteBuffer*>(context);
    REFCOUNT_RELEASE(font);
}

static Boolean AddSkTypeface(NativeFamilyBuilder* builder, sk_sp<SkData>&& data, int ttcIndex,
        Int32 givenWeight, Boolean givenItalic)
{
    android::uirenderer::FatVector<SkFontMgr::FontParameters::Axis, 2> skiaAxes;
    for (const auto& axis : builder->axes) {
        skiaAxes.emplace_back(SkFontMgr::FontParameters::Axis{axis.axisTag, axis.value});
    }

    const size_t fontSize = data->size();
    const void* fontPtr = data->data();
    std::unique_ptr<SkStreamAsset> fontData(new SkMemoryStream(std::move(data)));

    SkFontMgr::FontParameters params;
    params.setCollectionIndex(ttcIndex);
    params.setAxes(skiaAxes.data(), skiaAxes.size());

    sk_sp<SkFontMgr> fm(SkFontMgr::RefDefault());
    sk_sp<SkTypeface> face(fm->createFromStream(fontData.release(), params));
    if (face == NULL) {
        Logger::E("fontFamily", "addFont failed to create font, invalid request");
        builder->axes.clear();
        return FALSE;
    }
    std::shared_ptr<minikin::MinikinFont> minikinFont =
            std::make_shared<android::MinikinFontSkia>(std::move(face), fontPtr, fontSize, ttcIndex,
                    builder->axes);

    int weight = givenWeight / 100;
    bool italic = givenItalic;

    builder->fonts.push_back(minikin::Font(minikinFont, minikin::FontStyle(weight, italic)));
    builder->axes.clear();
    return TRUE;
}

Boolean FontFamily::NativeAddFont(
    /* [in] */ Int64 builderPtr,
    /* [in] */ IByteBuffer* font,
    /* [in] */ Int32 ttcIndex,
    /* [in] */ Int32 weight,
    /* [in] */ Int32 isItalic)
{
    NativeFamilyBuilder* builder = reinterpret_cast<NativeFamilyBuilder*>(builderPtr);
    Handle64 address;
    IBuffer::Probe(font)->GetEffectiveDirectAddress(&address);
    const void* fontPtr = reinterpret_cast<void*>(address);
    if (fontPtr == NULL) {
        Logger::E("FontFamily", "addFont failed to create font, buffer invalid");
        builder->axes.clear();
        return FALSE;
    }
    Int32 fontSize;
    IBuffer::Probe(font)->GetCapacity(&fontSize);
    if (fontSize < 0) {
        Logger::E("FontFamily", "addFont failed to create font, buffer size invalid");
        builder->axes.clear();
        return FALSE;
    }
    REFCOUNT_ADD(font);
    sk_sp<SkData> data(SkData::MakeWithProc(fontPtr, fontSize,
            release_global_ref, reinterpret_cast<void*>(font)));
    return AddSkTypeface(builder, std::move(data), ttcIndex, weight, isItalic);
}

Boolean FontFamily::NativeAddFontWeightStyle(
    /* [in] */ Int64 builderPtr,
    /* [in] */ IByteBuffer* font,
    /* [in] */ Int32 ttcIndex,
    /* [in] */ Int32 weight,
    /* [in] */ Boolean isItalic)
{
    NativeFamilyBuilder* builder = reinterpret_cast<NativeFamilyBuilder*>(builderPtr);
    Handle64 address;
    IBuffer::Probe(font)->GetEffectiveDirectAddress(&address);
    const void* fontPtr = reinterpret_cast<void*>(address);
    if (fontPtr == NULL) {
        Logger::E("FontFamily", "addFont failed to create font, buffer invalid");
        builder->axes.clear();
        return FALSE;
    }
    Int32 fontSize;
    IBuffer::Probe(font)->GetCapacity(&fontSize);
    if (fontSize < 0) {
        Logger::E("FontFamily", "addFont failed to create font, buffer size invalid");
        builder->axes.clear();
        return FALSE;
    }
    REFCOUNT_ADD(font);
    sk_sp<SkData> data(SkData::MakeWithProc(fontPtr, fontSize,
            release_global_ref, reinterpret_cast<void*>(font)));
    return AddSkTypeface(builder, std::move(data), ttcIndex, weight, isItalic);
}

static void releaseAsset(const void* ptr, void* context)
{
    delete static_cast<android::Asset*>(context);
}

Boolean FontFamily::NativeAddFontFromAsset(
    /* [in] */ Int64 builderPtr,
    /* [in] */ IAssetManager* assetMgr,
    /* [in] */ const String& path,
    /* [in] */ Int32 cookie,
    /* [in] */ Boolean isAsset,
    /* [in] */ Int32 ttcIndex,
    /* [in] */ Int32 weight,
    /* [in] */ Int32 isItalic)
{
    NativeFamilyBuilder* builder = reinterpret_cast<NativeFamilyBuilder*>(builderPtr);
    if (assetMgr == NULL || path.IsNull()) {
        builder->axes.clear();
        return FALSE;
    }

    android::AssetManager* mgr = (android::AssetManager*)((CAssetManager*)assetMgr)->Ni();
    if (NULL == mgr) {
        builder->axes.clear();
        return FALSE;
    }

    android::Asset* asset;
    if (isAsset) {
        asset = mgr->open(path.string(), android::Asset::ACCESS_BUFFER);
    }
    else {
        asset = cookie ? mgr->openNonAsset(static_cast<int32_t>(cookie), path.string(),
                android::Asset::ACCESS_BUFFER) : mgr->openNonAsset(path.string(), android::Asset::ACCESS_BUFFER);
    }

    if (NULL == asset) {
        builder->axes.clear();
        return FALSE;
    }

    const void* buf = asset->getBuffer(false);
    if (NULL == buf) {
        delete asset;
        builder->axes.clear();
        return FALSE;
    }

    sk_sp<SkData> data(SkData::MakeWithProc(buf, asset->getLength(), releaseAsset, asset));
    return AddSkTypeface(builder, std::move(data), ttcIndex, weight, isItalic);
}

void FontFamily::NativeAddAxisValue(
    /* [in] */ Int64 builderPtr,
    /* [in] */ Int32 tag,
    /* [in] */ Float value)
{
    NativeFamilyBuilder* builder = reinterpret_cast<NativeFamilyBuilder*>(builderPtr);
    builder->axes.push_back({static_cast<minikin::AxisTag>(tag), value});
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

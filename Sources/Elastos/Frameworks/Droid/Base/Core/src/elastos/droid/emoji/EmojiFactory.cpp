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

#include "elastos/droid/emoji/EmojiFactory.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <emoji/EmojiFactory.h>

#include <dlfcn.h>
// #include <utils/Log.h>
// #include <utils/String8.h>
// #include <utils/String16.h>
#include <skia/core/SkTypes.h>
#include <skia/core/SkImageDecoder.h>

using Elastos::Droid::Graphics::GraphicsNative;
using Elastos::Droid::Graphics::EIID_IBitmap;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
// using Elastos::Core::IInteger32;
// using Elastos::Core::CInteger32;

namespace Elastos {
namespace Droid {
namespace Emoji {

//=======================================================
// mojiFactory::CustomLinkedHashMap
//=======================================================
EmojiFactory::CustomLinkedHashMap::CustomLinkedHashMap(
    /* [in] */ EmojiFactory* host)
    : mHost(host)
{
    LinkedHashMap::constructor(16, 0.75, TRUE);
}

Boolean EmojiFactory::CustomLinkedHashMap::RemoveEldestEntry(
    /* [in] */ IMapEntry* eldest)
{
    Int32 size;
    GetSize(&size);
    return size > mHost->mCacheSize;
}


//=======================================================
// mojiFactory
//=======================================================
CAR_INTERFACE_IMPL(EmojiFactory, Object, IEmojiFactory);

EmojiFactory::EmojiFactory()
    : mCacheSize(100)
    , mNativeEmojiFactory(0)
{}

EmojiFactory::~EmojiFactory()
{
    NativeDestructor(mNativeEmojiFactory);
}

ECode  EmojiFactory::constructor(
    /* [in] */ Int64 nativeEmojiFactory,
    /* [in] */ const String& name)
{
    mNativeEmojiFactory = nativeEmojiFactory;
    mName = name;
    mCache = new CustomLinkedHashMap(this);
    return NOERROR;
}

ECode EmojiFactory::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode EmojiFactory::GetBitmapFromAndroidPua(
    /* [in] */ Int32 pua,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    AutoLock lock(this);

    AutoPtr<IInterface> cache;
    mCache->Get(CoreUtils::Convert(pua), (IInterface**)&cache);
    if (IWeakReference::Probe(cache) == NULL) {
        AutoPtr<IBitmap> ret = NativeGetBitmapFromAndroidPua(mNativeEmojiFactory, pua);
        // There is no need to cache returned null, since in most cases it means there
        // is no map from the AndroidPua to a specific image. In other words, it usually does
        // not include the cost of creating Bitmap object.
        if (ret != NULL) {
            AutoPtr<IWeakReference> wr;
            IWeakReferenceSource::Probe(ret)->GetWeakReference((IWeakReference**)&wr);
            mCache->Put(CoreUtils::Convert(pua), wr);
        }
        *bitmap = ret;
        REFCOUNT_ADD(*bitmap);
        return NOERROR;
    }
    else {
        AutoPtr<IBitmap> tmp;
        IWeakReference::Probe(cache)->Resolve(EIID_IBitmap, (IInterface**)&tmp);
        if (tmp == NULL) {
            AutoPtr<IBitmap> ret = NativeGetBitmapFromAndroidPua(mNativeEmojiFactory, pua);
            AutoPtr<IWeakReference> wr;
            IWeakReferenceSource::Probe(ret)->GetWeakReference((IWeakReference**)&wr);
            mCache->Put(CoreUtils::Convert(pua), wr);
            *bitmap = ret;
            REFCOUNT_ADD(*bitmap);
            return NOERROR;
        }
        else {
            *bitmap = tmp;
            REFCOUNT_ADD(*bitmap);
            return NOERROR;
        }
    }
}

ECode EmojiFactory::GetBitmapFromVendorSpecificSjis(
    /* [in] */ Char32 sjis,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    AutoLock lock(this);

    Int32 vsp;
    GetAndroidPuaFromVendorSpecificSjis(sjis, &vsp);
    return GetBitmapFromAndroidPua(vsp, bitmap);
}

ECode EmojiFactory::GetBitmapFromVendorSpecificPua(
    /* [in] */ Int32 vsp,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    AutoLock lock(this);

    Int32 pua;
    GetAndroidPuaFromVendorSpecificPua(vsp, &pua);
    return GetBitmapFromAndroidPua(pua, bitmap);
}

ECode EmojiFactory::GetAndroidPuaFromVendorSpecificSjis(
    /* [in] */ Char32 sjis,
    /* [out] */ Int32* pua)
{
    VALIDATE_NOT_NULL(pua);
    *pua = NativeGetAndroidPuaFromVendorSpecificSjis(mNativeEmojiFactory, sjis);
    return NOERROR;
}

ECode EmojiFactory::GetVendorSpecificSjisFromAndroidPua(
    /* [in] */ Int32 pua,
    /* [out] */ Int32* vsp)
{
    VALIDATE_NOT_NULL(vsp);
    *vsp = NativeGetVendorSpecificSjisFromAndroidPua(mNativeEmojiFactory, pua);
    return NOERROR;
}

ECode EmojiFactory::GetAndroidPuaFromVendorSpecificPua(
    /* [in] */ Int32 vsp,
    /* [out] */ Int32* pua)
{
    VALIDATE_NOT_NULL(pua);
    *pua = NativeGetAndroidPuaFromVendorSpecificPua(mNativeEmojiFactory, vsp);
    return NOERROR;
}

ECode EmojiFactory::GetAndroidPuaFromVendorSpecificPua(
    /* [in] */ const String& vspString,
    /* [out] */ String* puaString)
{
    VALIDATE_NOT_NULL(puaString);

    if (vspString.IsNull()) {
        *puaString = NULL;
        return NOERROR;
    }
    Int32 minVsp = NativeGetMinimumVendorSpecificPua(mNativeEmojiFactory);
    Int32 maxVsp = NativeGetMaximumVendorSpecificPua(mNativeEmojiFactory);
    Int32 len = vspString.GetLength();
    AutoPtr< ArrayOf<Char32> > codePoints = ArrayOf<Char32>::Alloc(len);

    Int32 new_len = 0;
    for (Int32 i = 0; i < len; i++, new_len++) {
        Int32 codePoint = (Int32)vspString.GetChar(i);
        if (minVsp <= codePoint && codePoint <= maxVsp) {
            Int32 newCodePoint;
            GetAndroidPuaFromVendorSpecificPua(codePoint, &newCodePoint);
            if (newCodePoint > 0) {
                (*codePoints)[new_len] = newCodePoint;
                continue;
            }
        }
        (*codePoints)[new_len] = codePoint;
    }
    *puaString = String(*codePoints, 0, new_len);
    return NOERROR;
}

ECode EmojiFactory::GetVendorSpecificPuaFromAndroidPua(
    /* [in] */ Int32 pua,
    /* [out] */ Int32* vsp)
{
    VALIDATE_NOT_NULL(vsp);
    *vsp = NativeGetVendorSpecificPuaFromAndroidPua(mNativeEmojiFactory, pua);
    return NOERROR;
}

ECode EmojiFactory::GetVendorSpecificPuaFromAndroidPua(
    /* [in] */ const String& puaString,
    /* [out] */ String* vspString)
{
    VALIDATE_NOT_NULL(vspString);

    if (puaString.IsNull()) {
        *vspString = NULL;
        return NOERROR;
    }
    Int32 minVsp = NativeGetMinimumAndroidPua(mNativeEmojiFactory);
    Int32 maxVsp = NativeGetMaximumAndroidPua(mNativeEmojiFactory);
    Int32 len = puaString.GetLength();
    AutoPtr< ArrayOf<Char32> > codePoints = ArrayOf<Char32>::Alloc(len);

    Int32 new_len = 0;
    for (Int32 i = 0; i < len; i++, new_len++) {
        Int32 codePoint = (Int32)puaString.GetChar(i);
        if (minVsp <= codePoint && codePoint <= maxVsp) {
            Int32 newCodePoint;
            GetVendorSpecificPuaFromAndroidPua(codePoint, &newCodePoint);
            if (newCodePoint > 0) {
                (*codePoints)[new_len] = newCodePoint;
                continue;
            }
        }
        (*codePoints)[new_len] = codePoint;
    }
    *vspString= String(*codePoints, 0, new_len);
    return NOERROR;
}

class EmojiFactoryCaller
{
public:
    EmojiFactoryCaller() {}
    virtual ~EmojiFactoryCaller();
    Boolean Init();
    android::EmojiFactory* TryCallGetImplementation(const char* name);
    android::EmojiFactory* TryCallGetAvailableImplementation();

private:
    void *m_handle;
    android::EmojiFactory* (*m_get_implementation)(const char*);
    android::EmojiFactory* (*m_get_available_implementation)();
};

Boolean EmojiFactoryCaller::Init()
{
    const char* error_msg = NULL;
    m_handle = dlopen("libemoji.so", RTLD_LAZY | RTLD_LOCAL);

    if (m_handle == NULL) {
        error_msg = "Failed to load libemoji.so";
        goto FAIL;
    }

    m_get_implementation =
        reinterpret_cast<android::EmojiFactory *(*)(const char*)>(dlsym(m_handle, "GetImplementation"));
    if (m_get_implementation == NULL) {
        error_msg = "Failed to get symbol of GetImplementation";
        goto FAIL;
    }

    m_get_available_implementation =
        reinterpret_cast<android::EmojiFactory *(*)()>(dlsym(m_handle,"GetAvailableImplementation"));
    if (m_get_available_implementation == NULL) {
        error_msg = "Failed to get symbol of GetAvailableImplementation";
        goto FAIL;
    }

    return TRUE;

FAIL:
    const char* error_str = dlerror();
    if (error_str == NULL) {
        error_str = "unknown reason";
    }

    ALOGE("%s: %s", error_msg, error_str);
    if (m_handle != NULL) {
        dlclose(m_handle);
        m_handle = NULL;
    }
    return FALSE;
}

EmojiFactoryCaller::~EmojiFactoryCaller()
{
    if (m_handle) {
        dlclose(m_handle);
    }
}

android::EmojiFactory* EmojiFactoryCaller::TryCallGetImplementation(const char* name)
{
    if (NULL == m_handle) {
        return NULL;
    }
    return m_get_implementation(name);
}

android::EmojiFactory* EmojiFactoryCaller::TryCallGetAvailableImplementation()
{
    if (NULL == m_handle) {
        return NULL;
    }
    return m_get_available_implementation();
}

static EmojiFactoryCaller* gCaller;
static pthread_once_t g_once = PTHREAD_ONCE_INIT;
static Boolean lib_emoji_factory_is_ready;

static void InitializeCaller()
{
    gCaller = new EmojiFactoryCaller();
    lib_emoji_factory_is_ready = gCaller->Init();
}

ECode EmojiFactory::NewInstance(
    /* [in] */ const String& class_name,
    /* [out] */ IEmojiFactory** emojiFactory)
{
    VALIDATE_NOT_NULL(emojiFactory);

    if (class_name.IsNull()) {
        *emojiFactory = NULL;
        return NOERROR;
    }
    pthread_once(&g_once, InitializeCaller);
    if (!lib_emoji_factory_is_ready) {
        *emojiFactory = NULL;
        return NOERROR;
    }

    android::EmojiFactory *factory = gCaller->TryCallGetImplementation(class_name.string());
    if (NULL == factory) {
        *emojiFactory = NULL;
       return NOERROR;
    }

    EmojiFactory* ef = new EmojiFactory();
    ef->constructor(reinterpret_cast<Int64>(factory), class_name);
    *emojiFactory = ef;
    REFCOUNT_ADD(*emojiFactory);
    return NOERROR;
}

ECode EmojiFactory::NewAvailableInstance(
    /* [out] */ IEmojiFactory** emojiFactory)
{
    VALIDATE_NOT_NULL(emojiFactory);

    pthread_once(&g_once, InitializeCaller);
    if (!lib_emoji_factory_is_ready) {
        *emojiFactory = NULL;
        return NOERROR;
    }

    android::EmojiFactory *factory = gCaller->TryCallGetAvailableImplementation();
    if (NULL == factory) {
        *emojiFactory = NULL;
        return NOERROR;
    }

    String class_name(factory->Name());
    if (class_name.IsNull()) {
        *emojiFactory = NULL;
        return NOERROR;
    }

    EmojiFactory* ef = new EmojiFactory();
    ef->constructor(reinterpret_cast<Int64>(factory), class_name);
    *emojiFactory = ef;
    REFCOUNT_ADD(*emojiFactory);
    return NOERROR;
}

ECode EmojiFactory::GetMinimumAndroidPua(
    /* [out] */ Int32* pua )
{
    VALIDATE_NOT_NULL(pua);
    *pua = NativeGetMinimumAndroidPua(mNativeEmojiFactory);
    return NOERROR;
}

ECode EmojiFactory::GetMaximumAndroidPua(
    /* [out] */ Int32* pua)
{
    VALIDATE_NOT_NULL(pua);
    *pua = NativeGetMaximumAndroidPua(mNativeEmojiFactory);
    return NOERROR;
}

void EmojiFactory::NativeDestructor(
    /* [in] */ Int64 nativeEmojiFactory)
{
    /*
      // Must not delete this object!!
      EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
      delete factory;
    */
}

AutoPtr<IBitmap> EmojiFactory::NativeGetBitmapFromAndroidPua(
    /* [in] */ Int64 nativeEmojiFactory,
    /* [in] */ Int32 AndroidPua)
{
     android::EmojiFactory *factory = reinterpret_cast<android::EmojiFactory *>(nativeEmojiFactory);

     int size;
     const char *bytes = factory->GetImageBinaryFromAndroidPua(AndroidPua, &size);
     if (bytes == NULL) {
        return NULL;
     }

    SkBitmap *bitmap = new SkBitmap;
    if (!SkImageDecoder::DecodeMemory(bytes, size, bitmap)) {
        ALOGE("SkImageDecoder::DecodeMemory() failed.");
        return NULL;
    }

    return GraphicsNative::CreateBitmap(bitmap,
            GraphicsNative::kBitmapCreateFlag_Premultiplied, NULL);
}

Int32 EmojiFactory::NativeGetAndroidPuaFromVendorSpecificSjis(
    /* [in] */ Int64 nativeEmojiFactory,
    /* [in] */ Char32 sjis)
{
    android::EmojiFactory *factory = reinterpret_cast<android::EmojiFactory *>(nativeEmojiFactory);
    return factory->GetAndroidPuaFromVendorSpecificSjis(sjis);
}

Int32 EmojiFactory::NativeGetVendorSpecificSjisFromAndroidPua(
    /* [in] */ Int64 nativeEmojiFactory,
    /* [in] */ Int32 pua)
{
    android::EmojiFactory *factory = reinterpret_cast<android::EmojiFactory *>(nativeEmojiFactory);
    return factory->GetVendorSpecificSjisFromAndroidPua(pua);
}

Int32 EmojiFactory::NativeGetAndroidPuaFromVendorSpecificPua(
    /* [in] */ Int64 nativeEmojiFactory,
    /* [in] */ Int32 vsp)
{
    android::EmojiFactory *factory = reinterpret_cast<android::EmojiFactory *>(nativeEmojiFactory);
    return factory->GetAndroidPuaFromVendorSpecificPua(vsp);
}

Int32 EmojiFactory::NativeGetVendorSpecificPuaFromAndroidPua(
    /* [in] */ Int64 nativeEmojiFactory,
    /* [in] */ Int32 pua)
{
    android::EmojiFactory *factory = reinterpret_cast<android::EmojiFactory *>(nativeEmojiFactory);
    return factory->GetVendorSpecificPuaFromAndroidPua(pua);
}

Int32 EmojiFactory::NativeGetMaximumVendorSpecificPua(
    /* [in] */ Int64 nativeEmojiFactory)
{
    android::EmojiFactory *factory = reinterpret_cast<android::EmojiFactory *>(nativeEmojiFactory);
    return factory->GetMaximumVendorSpecificPua();
}

Int32 EmojiFactory::NativeGetMinimumVendorSpecificPua(
    /* [in] */ Int64 nativeEmojiFactory)
{
    android::EmojiFactory *factory = reinterpret_cast<android::EmojiFactory *>(nativeEmojiFactory);
    return factory->GetMinimumVendorSpecificPua();
}

Int32 EmojiFactory::NativeGetMinimumAndroidPua(
    /* [in] */ Int64 nativeEmojiFactory)
{
    android::EmojiFactory *factory = reinterpret_cast<android::EmojiFactory *>(nativeEmojiFactory);
    return factory->GetMinimumAndroidPua();
}

Int32 EmojiFactory::NativeGetMaximumAndroidPua(
   /* [in] */ Int64 nativeEmojiFactory)
{
    android::EmojiFactory *factory = reinterpret_cast<android::EmojiFactory *>(nativeEmojiFactory);
    return factory->GetMaximumAndroidPua();
}

} // namespace Emoji
} // namepsace Droid
} // namespace Elastos

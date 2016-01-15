#include "elastos/droid/emoji/EmojiFactory.h"

#include <dlfcn.h>
#include <utils/Log.h>
#include <utils/String8.h>
#include <utils/String16.h>
//#include "SkTypes.h"
//#include "SkImageDecoder.h"
#include "elastos/droid/emoji/CEmojiFactory.h"

using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Droid::Emoji::CEmojiFactory;

namespace Elastos {
namespace Droid {
namespace Emoji {

CAR_INTERFACE_IMPL(EmojiFactory, Object, IEmojiFactory);

//For navtive used class
class EmojiFactoryCaller
{
public:
    EmojiFactoryCaller() {}
    virtual ~EmojiFactoryCaller();

    Boolean Init();

    EmojiFactory* TryCallGetImplementation(const char* name);
    EmojiFactory* TryCallGetAvailableImplementation();

private:
    EmojiFactory* (*m_get_implementation)(const char*);
    EmojiFactory* (*m_get_available_implementation)();

    void *m_handle;
};

//EmojiFactoryCaller function
Boolean EmojiFactoryCaller::Init()
{
/*
    const char* error_msg = NULL;
    m_handle = dlopen("libemoji.so", RTLD_LAZY | RTLD_LOCAL);

    if (m_handle == NULL) {
        error_msg = "Failed to load libemoji.so";
        goto FAIL;
    }

    m_get_implementation =
        reinterpret_cast<EmojiFactory *(*)(const char*)>(dlsym(m_handle, "GetImplementation"));
    if (m_get_implementation == NULL) {
        error_msg = "Failed to get symbol of GetImplementation";
        goto FAIL;
    }

    m_get_available_implementation =
        reinterpret_cast<EmojiFactory *(*)()>(dlsym(m_handle,"GetAvailableImplementation"));
    if (m_get_available_implementation == NULL) {
        error_msg = "Failed to get symbol of GetAvailableImplementation";
        goto FAIL;
    }
*/
    return true;
/*
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
    return false;
*/
}

EmojiFactoryCaller::~EmojiFactoryCaller()
{
/*
    if (m_handle) {
        dlclose(m_handle);
    }
*/
}

EmojiFactory *EmojiFactoryCaller::TryCallGetImplementation(const char* name)
{
    if (NULL == m_handle) {
        return NULL;
    }
    return m_get_implementation(name);
}

EmojiFactory *EmojiFactoryCaller::TryCallGetAvailableImplementation()
{
    if (NULL == m_handle) {
        return NULL;
    }
    return m_get_available_implementation();
}

static EmojiFactoryCaller* gCaller;
static pthread_once_t g_once = PTHREAD_ONCE_INIT;

void EmojiFactory::InitializeCaller()
{
    gCaller = new EmojiFactoryCaller();
    lib_emoji_factory_is_ready = gCaller->Init();
}


/*
 * class EmojiFactory
 */
Boolean EmojiFactory::lib_emoji_factory_is_ready = FALSE;

//For navtive used class
EmojiFactory::EmojiFactory()
    : sCacheSize(100)
{}

EmojiFactory::~EmojiFactory()
{}

ECode EmojiFactory::GetBitmapFromAndroidPua(
    /* [in] */ Int32 pua,
    /* [out] */ IBitmap** retBitmap)
{
    VALIDATE_NOT_NULL(retBitmap);

    AutoPtr<IBitmap> cache;
    AutoPtr<IInteger32> I32;
    CInteger32::New(pua, (IInteger32**)&I32);

    mCache->Get(I32, (IInterface**)&cache);

    if (cache == NULL) {
        AutoPtr<IBitmap> ret;

        ret = NativeGetBitmapFromAndroidPua(mNativeEmojiFactory, pua);
        mCache->Put(I32, (IInterface*)ret);
    }

    *retBitmap = cache;
    REFCOUNT_ADD(*retBitmap);
    return NOERROR;
}

ECode EmojiFactory::GetBitmapFromVendorSpecificSjis(
    /* [in] */ Char32 sjis,
    /* [out] */ IBitmap** retBitmap)
{
    Int32 RetValue;

    GetAndroidPuaFromVendorSpecificSjis(sjis, &RetValue);
    return GetBitmapFromAndroidPua(RetValue, retBitmap);
}

ECode EmojiFactory::GetBitmapFromVendorSpecificPua(
    /* [in] */ Int32 vsp,
    /* [out] */ IBitmap** retBitmap)
{
    Int32 RetValue;

    GetAndroidPuaFromVendorSpecificPua(vsp, &RetValue);
    return GetBitmapFromAndroidPua(RetValue, retBitmap);
}

ECode EmojiFactory::GetAndroidPuaFromVendorSpecificSjis(
    /* [in] */ Char32 sjis,
    /* [out] */ Int32* RetValue)
{
    VALIDATE_NOT_NULL(RetValue);
    *RetValue = NativeGetAndroidPuaFromVendorSpecificSjis(mNativeEmojiFactory, sjis);
    return NOERROR;
}

ECode EmojiFactory::GetVendorSpecificSjisFromAndroidPua(
    /* [in] */ Int32 pua,
    /* [out] */ Int32* RetValue)
{
    VALIDATE_NOT_NULL(RetValue);
    *RetValue = NativeGetVendorSpecificSjisFromAndroidPua(mNativeEmojiFactory, pua);
    return NOERROR;
}

ECode EmojiFactory::GetAndroidPuaFromVendorSpecificPua(
    /* [in] */ Int32 vsp,
    /* [out] */ Int32* RetValue)
{
    VALIDATE_NOT_NULL(RetValue);
    *RetValue = NativeGetAndroidPuaFromVendorSpecificPua(mNativeEmojiFactory, vsp);
    return NOERROR;
}

ECode EmojiFactory::GetAndroidPuaFromVendorSpecificPua(
    /* [in] */ const String& vspString,
    /* [out] */ String* RetValue)
{
    VALIDATE_NOT_NULL(RetValue);
    *RetValue = NULL;

    if (vspString.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    Int32 minVsp = NativeGetMinimumVendorSpecificPua(mNativeEmojiFactory);
    Int32 maxVsp = NativeGetMaximumVendorSpecificPua(mNativeEmojiFactory);

    Int32 len = vspString.GetLength();
    AutoPtr<ArrayOf<Char32> > codePoints = ArrayOf<Char32>::Alloc(len);
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

    *RetValue = String((const ArrayOf<Char32>&)codePoints, 0, new_len);
    return NOERROR;
}

ECode EmojiFactory::GetVendorSpecificPuaFromAndroidPua(
    /* [in] */ Int32 pua,
    /* [out] */ Int32* RetValue)
{
    VALIDATE_NOT_NULL(RetValue);
    *RetValue = NativeGetVendorSpecificPuaFromAndroidPua(mNativeEmojiFactory, pua);
    return NOERROR;
}

ECode EmojiFactory::GetVendorSpecificPuaFromAndroidPua(
    /* [in] */ const String& puaString,
    /* [out] */ String* RetValue)
{
    VALIDATE_NOT_NULL(RetValue);
    *RetValue = NULL;

    if (puaString == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    Int32 minVsp = NativeGetMinimumAndroidPua(mNativeEmojiFactory);
    Int32 maxVsp = NativeGetMaximumAndroidPua(mNativeEmojiFactory);
    Int32 len = puaString.GetLength();
    AutoPtr<ArrayOf<Char32> > codePoints = ArrayOf<Char32>::Alloc(len);
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
    *RetValue= String((const ArrayOf<Char32>&)codePoints, 0, new_len);
    return NOERROR;
}

ECode EmojiFactory::NewInstance(
    /* [in] */ const String& class_name,
    /* [out] */ IEmojiFactory** emojiFactory)
{
    VALIDATE_NOT_NULL(emojiFactory);
    VALIDATE_NOT_NULL(class_name);
    *emojiFactory = NULL;

    pthread_once(&g_once, InitializeCaller);
    if (!lib_emoji_factory_is_ready) {
        return E_NULL_POINTER_EXCEPTION;
    }

    EmojiFactory *factory = gCaller->TryCallGetImplementation(class_name.string());
    if (NULL == factory) {
       return E_NULL_POINTER_EXCEPTION;
    }

    CEmojiFactory::New(reinterpret_cast<Int64>(factory), class_name, emojiFactory);
    REFCOUNT_ADD(*emojiFactory);
    return NOERROR;
}

ECode EmojiFactory::NewAvailableInstance(
    /* [out] */ IEmojiFactory** emojiFactory)
{
    *emojiFactory = NULL;

    pthread_once(&g_once, InitializeCaller);
    if (!lib_emoji_factory_is_ready) {
        return E_NULL_POINTER_EXCEPTION;
    }

    EmojiFactory *factory = gCaller->TryCallGetAvailableImplementation();
    if (NULL == factory) {
        return E_NULL_POINTER_EXCEPTION;
    }

    String tempname;
    factory->GetName(&tempname);
    if (NULL == tempname) {
        return E_NULL_POINTER_EXCEPTION;
    }

    CEmojiFactory::New(reinterpret_cast<Int64>(factory), tempname, emojiFactory);
    REFCOUNT_ADD(*emojiFactory);
    return NOERROR;
}

ECode EmojiFactory::GetMinimumAndroidPua(
    /* [out] */ Int32* RetValue )
{
    VALIDATE_NOT_NULL(RetValue);
    *RetValue = NativeGetMinimumAndroidPua(mNativeEmojiFactory);
    return NOERROR;
}

ECode EmojiFactory::GetMaximumAndroidPua(
    /* [out] */ Int32* RetValue)
{
    VALIDATE_NOT_NULL(RetValue);
    *RetValue = NativeGetMaximumAndroidPua(mNativeEmojiFactory);
    return NOERROR;
}

ECode EmojiFactory::GetName(
    /* [out] */ String* RetValue)
{
    VALIDATE_NOT_NULL(RetValue);
    *RetValue = mName;
    return NOERROR;
}

ECode EmojiFactory::GetMaximumVendorSpecificPua(
    /* [out] */ Int32* RetValue)
{
    VALIDATE_NOT_NULL(RetValue);
    *RetValue = NativeGetMaximumVendorSpecificPua(mNativeEmojiFactory);
    return NOERROR;
}


ECode EmojiFactory::GetMinimumVendorSpecificPua(
    /* [out] */ Int32* RetValue)
{
    VALIDATE_NOT_NULL(RetValue);
    *RetValue = NativeGetMinimumVendorSpecificPua(mNativeEmojiFactory);
    return NOERROR;
}

ECode  EmojiFactory::constructor()
{
    return NOERROR;
}

ECode  EmojiFactory::constructor(
    /* [in] */ Int64 nativeEmojiFactory,
    /* [in] */ const String& name)
{
    mNativeEmojiFactory = nativeEmojiFactory;
    mName = name;

    CHashMap::New((IHashMap**)&mCache);

    return NOERROR;
}

#if 0
vold EmojiFactory::CustomLinkedHashMap::CustomLinkedHashMap()
{
    mHashMap = new HashMap<Int32, AutoPre<IBitmap> >();
    //this.HashMap(16, 0.75f);
}

Boolean EmojiFactory::CustomLinkedHashMap::removeEldestEntry(Int32 eldest)
{
    return mHashMap->GetSize() > sCacheSize;
}
#endif

Int32 EmojiFactory::NativeGetMinimumAndroidPua(
    /* [in] */ Int64 nativeEmojiFactory)
{
    Int32 i;
    EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);

    factory->GetMinimumAndroidPua(&i);
    return i;
}

Int32 EmojiFactory::NativeGetMaximumAndroidPua(
   /* [in] */ Int64 nativeEmojiFactory)
{
    Int32 i;

    EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);

    factory->GetMaximumAndroidPua(&i);
    return i;
}

char *EmojiFactory::GetImageBinaryFromAndroidPua(int pua, int *size)
{
    return NULL;
}

void EmojiFactory::NativeDestructor(
    /* [in] */ Int64 nativeEmojiFactory)
{
    //Don't implement
    return;
}

AutoPtr<IBitmap> EmojiFactory::NativeGetBitmapFromAndroidPua(
    /* [in] */ Int64 nativeEmojiFactory,
    /* [in] */ Int32 AndroidPua)
{
     EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);

     int size;

     const char *bytes = factory->GetImageBinaryFromAndroidPua(AndroidPua, &size);
     if (bytes == NULL) {
        return NULL;
     }

     /*
     SkBitmap *bitmap = new SkBitmap;
     if (!SkImageDecoder::DecodeMemory(bytes, size, bitmap)) {
        ALOGE("SkImageDecoder::DecodeMemory() failed.");
        return NULL;
     }

    GraphicsJNI::createBitmap(env, bitmap,
          GraphicsJNI::kBitmapCreateFlag_Premultiplied, NULL);

     AutoPtr<IBitmap> obj;
     ECode ec = CBitmap::New(NULL, false, NULL, -1, (IBitmap**)&obj);
     if (FAILED(ec)) {
        return NULL;
     }

     return obj;
    */
     return NULL;
}

Int32 EmojiFactory::NativeGetAndroidPuaFromVendorSpecificSjis(
    /* [in] */ Int64 nativeEmojiFactory,
    /* [in] */ char sjis)
{
    Int32 RetValue;

    EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
    factory->GetAndroidPuaFromVendorSpecificSjis(sjis, &RetValue);

    return RetValue;
}

Int32 EmojiFactory::NativeGetVendorSpecificSjisFromAndroidPua(
    /* [in] */ Int64 nativeEmojiFactory,
    /* [in] */ Int32 pua)
{
    Int32 RetValue;

    EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
    factory->GetVendorSpecificSjisFromAndroidPua(pua, &RetValue);

    return RetValue;
}

Int32 EmojiFactory::NativeGetAndroidPuaFromVendorSpecificPua(
    /* [in] */ Int64 nativeEmojiFactory,
    /* [in] */ Int32 vsp)
{
    Int32 RetValue;

    EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
    factory->GetAndroidPuaFromVendorSpecificPua(vsp, &RetValue);

    return RetValue;
}

Int32 EmojiFactory::NativeGetVendorSpecificPuaFromAndroidPua(
    /* [in] */ Int64 nativeEmojiFactory,
    /* [in] */ Int32 pua)
{
    Int32 RetValue;

    EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
    factory->GetVendorSpecificPuaFromAndroidPua(pua, &RetValue);

    return RetValue;
}

Int32 EmojiFactory::NativeGetMaximumVendorSpecificPua(
    /* [in] */ Int64 nativeEmojiFactory)
{
    Int32 RetValue;

    EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
    factory->GetMaximumVendorSpecificPua(&RetValue);

    return RetValue;
}

Int32 EmojiFactory::NativeGetMinimumVendorSpecificPua(
    /* [in] */ Int64 nativeEmojiFactory)
{
    Int32 RetValue;

    EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
    factory->GetMinimumVendorSpecificPua(&RetValue);

    return RetValue;
}

} // namespace Emoji
} // namepsace Droid
} // namespace Elastos

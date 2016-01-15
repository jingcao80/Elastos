#include "elastos/droid/media/CMediaCodecList.h"
#include "elastos/droid/media/CMediaCodecInfo.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/Arrays.h>
//TODO: Need jni head
//#include <media/stagefright/MediaCodecList.h>

using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Media {

const String CMediaCodecList::TAG("MediaCodecList");

Object CMediaCodecList::sInitLock;
AutoPtr<ArrayOf<IMediaCodecInfo*> > CMediaCodecList::sAllCodecInfos;
AutoPtr<ArrayOf<IMediaCodecInfo*> > CMediaCodecList::sRegularCodecInfos;

CAR_INTERFACE_IMPL(CMediaCodecList, Object, IMediaCodecList)

CAR_OBJECT_IMPL(CMediaCodecList)

CMediaCodecList::CMediaCodecList()
{
}

CMediaCodecList::~CMediaCodecList()
{
}

ECode CMediaCodecList::constructor()
{
    return constructor(IMediaCodecList::REGULAR_CODECS);
}

ECode CMediaCodecList::constructor(
    /* [in] */ Int32 kind)
{
    InitCodecList();
    if (kind == IMediaCodecList::REGULAR_CODECS) {
        mCodecInfos = sRegularCodecInfos;
    } else {
        mCodecInfos = sAllCodecInfos;
    }
    return NOERROR;
}

ECode CMediaCodecList::GetCodecInfos(
    /* [out, callee] */ ArrayOf<IMediaCodecInfo*>** result)
{
    VALIDATE_NOT_NULL(result)

    return Arrays::CopyOf(mCodecInfos, mCodecInfos->GetLength(), result);
}

ECode CMediaCodecList::FindDecoderForFormat(
    /* [in] */ IMediaFormat* format,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FindCodecForFormat(FALSE /* encoder */, format);
    return NOERROR;
}

ECode CMediaCodecList::FindEncoderForFormat(
    /* [in] */ IMediaFormat* format,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FindCodecForFormat(TRUE /* encoder */, format);
    return NOERROR;
}

ECode CMediaCodecList::GetCodecCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    InitCodecList();
    *result = sRegularCodecInfos->GetLength();
    return NOERROR;
}

ECode CMediaCodecList::GetCodecInfoAt(
    /* [in] */ Int32 index,
    /* [out] */ IMediaCodecInfo** result)
{
    VALIDATE_NOT_NULL(result)

    InitCodecList();
    if (index < 0 || index > sRegularCodecInfos->GetLength()) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = (*sRegularCodecInfos)[index];
    return NOERROR;
}

ECode CMediaCodecList::GetInfoFor(
    /* [in] */ const String& codec,
    /* [out] */ IMediaCodecInfo** result)
{
    VALIDATE_NOT_NULL(result)

    InitCodecList();
    Int32 val;
    NativeFindCodecByName(codec, &val);
    *result = (*sAllCodecInfos)[val];
    return NOERROR;
}

String CMediaCodecList::FindCodecForFormat(
    /* [in] */ Boolean encoder,
    /* [in] */ IMediaFormat* format)
{
    String mime;
    format->GetString(IMediaFormat::KEY_MIME, &mime);
    for (Int32 i = 0; i < mCodecInfos->GetLength(); i++) {
        AutoPtr<IMediaCodecInfo> info = (*mCodecInfos)[i];
        Boolean b;
        info->IsEncoder(&b);
        if (b != encoder) {
            continue;
        }
        // try {
        AutoPtr<IMediaCodecInfoCodecCapabilities> caps;
        info->GetCapabilitiesForType(mime, (IMediaCodecInfoCodecCapabilities**)&caps);
        if (caps != NULL && (caps->IsFormatSupported(format, &b), b)) {
            String str;
            info->GetName(&str);
            return str;
        }
        // } catch (IllegalArgumentException e) {
        //     // type is not supported
        // }
    }
    return String(NULL);
}

void CMediaCodecList::InitCodecList()
{
    synchronized(sInitLock) {
        if (sRegularCodecInfos == NULL) {
            Int32 count;
            NativeGetCodecCount(&count);
            AutoPtr<IArrayList> regulars;
            CArrayList::New((IArrayList**)&regulars);
            AutoPtr<IArrayList> all;
            CArrayList::New((IArrayList**)&all);
            for (Int32 index = 0; index < count; index++) {
                // try {
                AutoPtr<IMediaCodecInfo> info = GetNewCodecInfoAt(index);
                all->Add(info);
                info->MakeRegular((IMediaCodecInfo**)&info);
                if (info != NULL) {
                    regulars->Add(info);
                }
                // } catch (Exception e) {
                //     Log.e(TAG, "Could not get codec capabilities", e);
                // }
            }
            AutoPtr<ArrayOf<IInterface*> > array;
            regulars->ToArray((ArrayOf<IInterface*>**)&array);
            for (Int32 i = 0; i < array->GetLength(); i++) {
                sRegularCodecInfos->Set(i, IMediaCodecInfo::Probe((*array)[i]));
            }
            array = NULL;
            all->ToArray((ArrayOf<IInterface*>**)&array);
            for (Int32 i = 0; i < array->GetLength(); i++) {
                sAllCodecInfos->Set(i, IMediaCodecInfo::Probe((*array)[i]));
            }
        }
    }
}

AutoPtr<IMediaCodecInfo> CMediaCodecList::GetNewCodecInfoAt(
    /* [in] */ Int32 index)
{
    AutoPtr<ArrayOf<String> > supportedTypes;
    NativeGetSupportedTypes(index, (ArrayOf<String>**)&supportedTypes);
    AutoPtr<ArrayOf<IMediaCodecInfoCodecCapabilities*> > caps =
        ArrayOf<IMediaCodecInfoCodecCapabilities*>::Alloc(supportedTypes->GetLength());
    Int32 typeIx = 0;
    for (Int32 i = 0; i < supportedTypes->GetLength(); i++) {
        String type = (*supportedTypes)[i];
        AutoPtr<IMediaCodecInfoCodecCapabilities> c;
        NativeGetCodecCapabilities(index, type, (IMediaCodecInfoCodecCapabilities**)&c);
        caps->Set(typeIx++, c);
    }
    String str;
    NativeGetCodecName(index, &str);
    Boolean b;
    NativeIsEncoder(index, &b);
    AutoPtr<IMediaCodecInfo> info;
    CMediaCodecInfo::New(str, b, caps, (IMediaCodecInfo**)&info);
    return info;
}

//TODO: Need jni head
ECode CMediaCodecList::NativeGetCodecCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    // sp<IMediaCodecList> mcl = MediaCodecList::getInstance();
    // if (mcl == NULL) {
    //     // This should never happen unless something is really wrong
    //     // jniThrowException(
    //     //             env, "java/lang/RuntimeException", "cannot get MediaCodecList");
    //     return E_RUNTIME_EXCEPTION;
    // }

    // *result = mcl->countCodecs();
    return NOERROR;
}

ECode CMediaCodecList::NativeGetCodecName(
    /* [in] */ Int32 index,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);
    // sp<IMediaCodecList> mcl = MediaCodecList::getInstance();
    // if (mcl == NULL) {
    //     // This should never happen unless something is really wrong
    //     // jniThrowException(
    //     //             env, "java/lang/RuntimeException", "cannot get MediaCodecList");
    //     return E_RUNTIME_EXCEPTION;
    // }

    // const sp<MediaCodecInfo> &info = mcl->getCodecInfo(index);
    // if (info == NULL) {
    //     // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
    //     return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // }

    // const char *name = info->getCodecName();
    // *result = name;
    return NOERROR;
}

ECode CMediaCodecList::NativeIsEncoder(
    /* [in] */ Int32 index,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // sp<IMediaCodecList> mcl = MediaCodecList::getInstance();
    // if (mcl == NULL) {
    //     // This should never happen unless something is really wrong
    //     // jniThrowException(
    //     //             env, "java/lang/RuntimeException", "cannot get MediaCodecList");
    //     return E_RUNTIME_EXCEPTION;
    // }

    // const sp<MediaCodecInfo> &info = mcl->getCodecInfo(index);
    // if (info == NULL) {
    //     // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
    //     return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // }

    // *result = info->isEncoder();
    return NOERROR;
}

ECode CMediaCodecList::NativeGetSupportedTypes(
    /* [in] */ Int32 index,
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // sp<IMediaCodecList> mcl = MediaCodecList::getInstance();
    // if (mcl == NULL) {
    //     // This should never happen unless something is really wrong
    //     // jniThrowException(
    //     //             env, "java/lang/RuntimeException", "cannot get MediaCodecList");
    //     return E_RUNTIME_EXCEPTION;
    // }

    // const sp<MediaCodecInfo> &info = mcl->getCodecInfo(index);
    // if (info == NULL) {
    //     // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
    //     return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // }

    // Vector<AString> types;
    // info->getSupportedMimes(&types);

    // AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(types.size());
    // for (size_t i = 0; i < types.size(); ++i) {
    //     array->Set(i, types.itemAt(i).c_str());
    // }

    // *result = array;
    // REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecList::NativeGetCodecCapabilities(
    /* [in] */ Int32 index,
    /* [in] */ const String& type,
    /* [out] */ IMediaCodecInfoCodecCapabilities** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // if (type == NULL) {
    //     // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
    //     return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // }

    // sp<IMediaCodecList> mcl = MediaCodecList::getInstance();
    // if (mcl == NULL) {
    //     // This should never happen unless something is really wrong
    //     // jniThrowException(
    //     //             env, "java/lang/RuntimeException", "cannot get MediaCodecList");
    //     return E_RUNTIME_EXCEPTION;
    // }

    // const sp<MediaCodecInfo> &info = mcl->getCodecInfo(index);
    // if (info == NULL) {
    //     // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
    //     return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // }

    // const char *typeStr = type.string();

// TODO: Need
    // Vector<MediaCodecInfo::ProfileLevel> profileLevels;
    // Vector<uint32_t> colorFormats;

    // sp<AMessage> defaultFormat = new AMessage();
    // defaultFormat->setString("mime", typeStr);

    // // TODO query default-format also from codec/codec list
    // const sp<MediaCodecInfo::Capabilities> &capabilities =
    //     info->getCapabilitiesFor(typeStr);
    // env->ReleaseStringUTFChars(type, typeStr);
    // typeStr = NULL;
    // if (capabilities == NULL) {
    //     jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
    //     return NULL;
    // }

    // capabilities->getSupportedColorFormats(&colorFormats);
    // capabilities->getSupportedProfileLevels(&profileLevels);
    // uint32_t flags = capabilities->getFlags();
    // sp<AMessage> details = capabilities->getDetails();
    // bool isEncoder = info->isEncoder();

    // jobject defaultFormatObj = NULL;
    // if (ConvertMessageToMap(env, defaultFormat, &defaultFormatObj)) {
    //     return NULL;
    // }

    // jobject infoObj = NULL;
    // if (ConvertMessageToMap(env, details, &infoObj)) {
    //     env->DeleteLocalRef(defaultFormatObj);
    //     return NULL;
    // }

    // jclass capsClazz =
    //     env->FindClass("android/media/MediaCodecInfo$CodecCapabilities");
    // CHECK(capsClazz != NULL);

    // jclass profileLevelClazz =
    //     env->FindClass("android/media/MediaCodecInfo$CodecProfileLevel");
    // CHECK(profileLevelClazz != NULL);

    // jobjectArray profileLevelArray =
    //     env->NewObjectArray(profileLevels.size(), profileLevelClazz, NULL);

    // jfieldID profileField =
    //     env->GetFieldID(profileLevelClazz, "profile", "I");

    // jfieldID levelField =
    //     env->GetFieldID(profileLevelClazz, "level", "I");

    // for (size_t i = 0; i < profileLevels.size(); ++i) {
    //     const MediaCodecInfo::ProfileLevel &src = profileLevels.itemAt(i);

    //     jobject profileLevelObj = env->AllocObject(profileLevelClazz);

    //     env->SetIntField(profileLevelObj, profileField, src.mProfile);
    //     env->SetIntField(profileLevelObj, levelField, src.mLevel);

    //     env->SetObjectArrayElement(profileLevelArray, i, profileLevelObj);

    //     env->DeleteLocalRef(profileLevelObj);
    //     profileLevelObj = NULL;
    // }

    // jintArray colorFormatsArray = env->NewIntArray(colorFormats.size());

    // for (size_t i = 0; i < colorFormats.size(); ++i) {
    //     jint val = colorFormats.itemAt(i);
    //     env->SetIntArrayRegion(colorFormatsArray, i, 1, &val);
    // }

    // jmethodID capsConstructID = env->GetMethodID(capsClazz, "<init>",
    //         "([Landroid/media/MediaCodecInfo$CodecProfileLevel;[IZI"
    //         "Ljava/util/Map;Ljava/util/Map;)V");

    // jobject caps = env->NewObject(capsClazz, capsConstructID,
    //         profileLevelArray, colorFormatsArray, isEncoder, flags,
    //         defaultFormatObj, infoObj);

    // env->DeleteLocalRef(profileLevelArray);
    // profileLevelArray = NULL;

    // env->DeleteLocalRef(colorFormatsArray);
    // colorFormatsArray = NULL;

    // env->DeleteLocalRef(defaultFormatObj);
    // defaultFormatObj = NULL;

    // env->DeleteLocalRef(infoObj);
    // infoObj = NULL;

    // return caps;
    return NOERROR;
}

ECode CMediaCodecList::NativeFindCodecByName(
    /* [in] */ const String& name,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    // if (name == NULL) {
    //     // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
    //     return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // }

    // sp<IMediaCodecList> mcl = MediaCodecList::getInstance();
    // if (mcl == NULL) {
    //     // This should never happen unless something is really wrong
    //     // jniThrowException(
    //     //             env, "java/lang/RuntimeException", "cannot get MediaCodecList");
    //     return E_RUNTIME_EXCEPTION;
    // }

    // Int32 ret = mcl->findCodecByName(nameStr);
    // *reuslt = ret;
    return NOERROR;
}

void CMediaCodecList::NativeInit()
{
    return;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos

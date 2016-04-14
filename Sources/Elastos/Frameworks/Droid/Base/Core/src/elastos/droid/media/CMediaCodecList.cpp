#include "elastos/droid/media/CMediaCodecList.h"
#include "elastos/droid/media/CMediaCodecInfo.h"
#include "elastos/droid/media/CMediaCodecInfoCodecCapabilities.h"
#include "elastos/droid/media/CMediaCodecInfoCodecProfileLevel.h"
#include "elastos/droid/media/Media_Utils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/Arrays.h>

#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/MediaCodecList.h>

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
    REFCOUNT_ADD(*result)
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
    REFCOUNT_ADD(*result)
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

ECode CMediaCodecList::NativeGetCodecCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    android::sp<android::IMediaCodecList> mcl = android::MediaCodecList::getInstance();
    if (mcl == NULL) {
        // This should never happen unless something is really wrong
        // jniThrowException(
        //             env, "java/lang/RuntimeException", "cannot get MediaCodecList");
        return E_RUNTIME_EXCEPTION;
    }

    *result = mcl->countCodecs();
    return NOERROR;
}

ECode CMediaCodecList::NativeGetCodecName(
    /* [in] */ Int32 index,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);
    android::sp<android::IMediaCodecList> mcl = android::MediaCodecList::getInstance();
    if (mcl == NULL) {
        // This should never happen unless something is really wrong
        // jniThrowException(
        //             env, "java/lang/RuntimeException", "cannot get MediaCodecList");
        return E_RUNTIME_EXCEPTION;
    }

    const android::sp<android::MediaCodecInfo> &info = mcl->getCodecInfo(index);
    if (info == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    const char *name = info->getCodecName();
    *result = name;
    return NOERROR;
}

ECode CMediaCodecList::NativeIsEncoder(
    /* [in] */ Int32 index,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    android::sp<android::IMediaCodecList> mcl = android::MediaCodecList::getInstance();
    if (mcl == NULL) {
        // This should never happen unless something is really wrong
        // jniThrowException(
        //             env, "java/lang/RuntimeException", "cannot get MediaCodecList");
        return E_RUNTIME_EXCEPTION;
    }

    const android::sp<android::MediaCodecInfo> &info = mcl->getCodecInfo(index);
    if (info == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *result = info->isEncoder();
    return NOERROR;
}

ECode CMediaCodecList::NativeGetSupportedTypes(
    /* [in] */ Int32 index,
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    android::sp<android::IMediaCodecList> mcl = android::MediaCodecList::getInstance();
    if (mcl == NULL) {
        // This should never happen unless something is really wrong
        // jniThrowException(
        //             env, "java/lang/RuntimeException", "cannot get MediaCodecList");
        return E_RUNTIME_EXCEPTION;
    }

    const android::sp<android::MediaCodecInfo> &info = mcl->getCodecInfo(index);
    if (info == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::Vector<android::AString> types;
    info->getSupportedMimes(&types);

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(types.size());
    for (size_t i = 0; i < types.size(); ++i) {
        array->Set(i, String(types.itemAt(i).c_str()));
    }

    *result = array;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecList::NativeGetCodecCapabilities(
    /* [in] */ Int32 index,
    /* [in] */ const String& type,
    /* [out] */ IMediaCodecInfoCodecCapabilities** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (type == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::sp<android::IMediaCodecList> mcl = android::MediaCodecList::getInstance();
    if (mcl == NULL) {
        // This should never happen unless something is really wrong
        // jniThrowException(
        //             env, "java/lang/RuntimeException", "cannot get MediaCodecList");
        return E_RUNTIME_EXCEPTION;
    }

    const android::sp<android::MediaCodecInfo> &info = mcl->getCodecInfo(index);
    if (info == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    const char *typeStr = type.string();

    android::Vector<android::MediaCodecInfo::ProfileLevel> profileLevels;
    android::Vector<uint32_t> colorFormats;

    android::sp<android::AMessage> defaultFormat = new android::AMessage();
    defaultFormat->setString("mime", typeStr);

    // TODO query default-format also from codec/codec list
    const android::sp<android::MediaCodecInfo::Capabilities> &capabilities =
        info->getCapabilitiesFor(typeStr);

    if (capabilities == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    capabilities->getSupportedColorFormats(&colorFormats);
    capabilities->getSupportedProfileLevels(&profileLevels);
    uint32_t flags = capabilities->getFlags();
    android::sp<android::AMessage> details = capabilities->getDetails();
    Boolean isEncoder = info->isEncoder();

    AutoPtr<IMap> defaultFormatObj;
    if (Media_Utils::ConvertMessageToMap(defaultFormat, (IMap**)&defaultFormatObj)) {
        return NOERROR;
    }

    AutoPtr<IMap> infoObj;
    if (Media_Utils::ConvertMessageToMap(details, (IMap**)&infoObj)) {
        return NOERROR;
    }

    AutoPtr<ArrayOf<IMediaCodecInfoCodecProfileLevel*> > profileLevelArray =
            ArrayOf<IMediaCodecInfoCodecProfileLevel*>::Alloc(profileLevels.size());

    for (size_t i = 0; i < profileLevels.size(); ++i) {
        const android::MediaCodecInfo::ProfileLevel &src = profileLevels.itemAt(i);

        AutoPtr<CMediaCodecInfoCodecProfileLevel> profileLevelObj;
        CMediaCodecInfoCodecProfileLevel::NewByFriend((CMediaCodecInfoCodecProfileLevel**)&profileLevelObj);

        profileLevelObj->mProfile = src.mProfile;
        profileLevelObj->mLevel = src.mLevel;

        profileLevelArray->Set(i, IMediaCodecInfoCodecProfileLevel::Probe(profileLevelObj));
    }

    AutoPtr<ArrayOf<Int32> > colorFormatsArray = ArrayOf<Int32>::Alloc(colorFormats.size());

    for (size_t i = 0; i < colorFormats.size(); ++i) {
        Int32 val = colorFormats.itemAt(i);
        colorFormatsArray->Set(i, val);
    }

    AutoPtr<IMediaCodecInfoCodecCapabilities> caps;
    CMediaCodecInfoCodecCapabilities::New(profileLevelArray, colorFormatsArray,
            isEncoder, flags, defaultFormatObj, infoObj,
            (IMediaCodecInfoCodecCapabilities**)&caps);

    *result = caps;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecList::NativeFindCodecByName(
    /* [in] */ const String& name,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if (name == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    const char *nameStr = name.string();
    if (nameStr == NULL) {
        // Out of memory exception already pending.
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::sp<android::IMediaCodecList> mcl = android::MediaCodecList::getInstance();
    if (mcl == NULL) {
        // This should never happen unless something is really wrong
        // jniThrowException(
        //             env, "java/lang/RuntimeException", "cannot get MediaCodecList");
        return E_RUNTIME_EXCEPTION;
    }

    Int32 ret = mcl->findCodecByName(nameStr);
    *result = ret;
    return NOERROR;
}

void CMediaCodecList::NativeInit()
{
    return;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos


#include "elastos/droid/media/CMediaFormat.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CFloat;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IFloat;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::IO::IByteBuffer;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaFormat, Object, IMediaFormat)

CAR_OBJECT_IMPL(CMediaFormat)

CMediaFormat::CMediaFormat()
{
}

CMediaFormat::~CMediaFormat()
{
}

ECode CMediaFormat::constructor()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);
    mMap = IMap::Probe(map);
    return NOERROR;
}

ECode CMediaFormat::constructor(
    /* [in] */ IMap* map)
{
    mMap = map;
    return NOERROR;
}

ECode CMediaFormat::ContainsKey(
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICharSequence> temp;
    CString::New(name, (ICharSequence**)&temp);
    return IHashMap::Probe(mMap)->ContainsKey(temp, result);
}

ECode CMediaFormat::GetInt32(
    /* [in] */ const String& name,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> temp;
    CString::New(name, (ICharSequence**)&temp);
    AutoPtr<IInterface> tmpObj;
    mMap->Get(temp, (IInterface**)&tmpObj);
    AutoPtr<IInteger32> obj = IInteger32::Probe(tmpObj);
    if (obj) {
        return obj->GetValue(result);
    }
    else {
        *result = 0;
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode CMediaFormat::GetInt32(
    /* [in] */ const String& name,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* result)
{
    // try {
    return GetInt32(name, result);
    // }
    // catch (NullPointerException  e) { /* no such field */ }
    // catch (ClassCastException e) { /* field of different type */ }
    // return defaultValue;
}

ECode CMediaFormat::GetInt64(
    /* [in] */ const String& name,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> temp;
    CString::New(name, (ICharSequence**)&temp);
    AutoPtr<IInterface> tmpObj;
    mMap->Get(temp, (IInterface**)&tmpObj);
    AutoPtr<IInteger64> obj = IInteger64::Probe(tmpObj);
    if (obj) {
        return obj->GetValue(result);
    }
    else {
        *result = 0;
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode CMediaFormat::GetFloat(
    /* [in] */ const String& name,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> temp;
    CString::New(name, (ICharSequence**)&temp);
    AutoPtr<IInterface> tmpObj;
    mMap->Get(temp, (IInterface**)&tmpObj);
    AutoPtr<IFloat> obj = IFloat::Probe(tmpObj);
    if (obj) {
        return obj->GetValue(result);
    }
    else {
        *result = 0;
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode CMediaFormat::GetString(
    /* [in] */ const String& name,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> temp;
    CString::New(name, (ICharSequence**)&temp);
    AutoPtr<IInterface> tmpObj;
    mMap->Get(temp, (IInterface**)&tmpObj);
    AutoPtr<ICharSequence> obj = ICharSequence::Probe(tmpObj);
    if (obj) {
        return obj->ToString(result);
    }
    else {
        *result = String(NULL);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode CMediaFormat::GetByteBuffer(
    /* [in] */ const String& name,
    /* [out] */ IByteBuffer** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> temp;
    CString::New(name, (ICharSequence**)&temp);
    AutoPtr<IInterface> tmpObj;
    mMap->Get(temp, (IInterface**)&tmpObj);
    *result = IByteBuffer::Probe(tmpObj);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaFormat::GetFeatureEnabled(
    /* [in] */ const String& feature,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<ICharSequence> temp;
    CString::New(KEY_FEATURE_ + feature, (ICharSequence**)&temp);
    AutoPtr<IInterface> tmpObj;
    mMap->Get(temp, (IInterface**)&tmpObj);
    AutoPtr<IInteger32> enabled = IInteger32::Probe(tmpObj);
    if (enabled == NULL) {
        // throw new IllegalArgumentException("feature is not specified");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = enabled != 0;
    return NOERROR;
}

ECode CMediaFormat::SetInt32(
    /* [in] */ const String& name,
    /* [in] */ Int32 value)
{
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<IInteger32> temp;
    CInteger32::New(value, (IInteger32**)&temp);
    return mMap->Put(cs, temp);
}

ECode CMediaFormat::SetInt64(
    /* [in] */ const String& name,
    /* [in] */ Int64 value)
{
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<IInteger64> temp;
    CInteger64::New(value, (IInteger64**)&temp);
    return mMap->Put(cs,temp);
}

ECode CMediaFormat::SetFloat(
    /* [in] */ const String& name,
    /* [in] */ Float value)
{
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<IFloat> temp;
    CFloat::New(value, (IFloat**)&temp);
    return mMap->Put(cs,temp);
}

ECode CMediaFormat::SetString(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<ICharSequence> temp;
    CString::New(value, (ICharSequence**)&temp);
    return mMap->Put(cs,temp);
}

ECode CMediaFormat::SetByteBuffer(
    /* [in] */ const String& name,
    /* [in] */ IByteBuffer* bytes)
{
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    return IHashMap::Probe(mMap)->Put(cs, bytes);
}

ECode CMediaFormat::SetFeatureEnabled(
    /* [in] */ const String& feature,
    /* [in] */ Boolean enabled)
{
    return SetInt32(KEY_FEATURE_ + feature, enabled ? 1 : 0);
}

ECode CMediaFormat::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // return mMap.toString();
    *result = String("TODO:CMediaFormat:E_NOT_IMPLEMENTED");
    return E_NOT_IMPLEMENTED;
}

ECode CMediaFormat::CreateAudioFormat(
    /* [in] */ const String& mime,
    /* [in] */ Int32 sampleRate,
    /* [in] */ Int32 channelCount,
    /* [out] */ IMediaFormat** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IMediaFormat> format;
    FAIL_RETURN(CMediaFormat::New((IMediaFormat**)&format));

    FAIL_RETURN(format->SetString(KEY_MIME, mime));
    FAIL_RETURN(format->SetInt32(KEY_SAMPLE_RATE, sampleRate));
    FAIL_RETURN(format->SetInt32(KEY_CHANNEL_COUNT, channelCount));

    *result = format;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaFormat::CreateSubtitleFormat(
    /* [in] */ const String& mime,
    /* [in] */ const String& language,
    /* [out] */ IMediaFormat** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IMediaFormat> format;
    CMediaFormat::New((IMediaFormat**)&format);
    format->SetString(KEY_MIME, mime);
    format->SetString(KEY_LANGUAGE, language);

    *result = format;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaFormat::CreateVideoFormat(
    /* [in] */ const String& mime,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IMediaFormat** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IMediaFormat> format;
    FAIL_RETURN(CMediaFormat::New((IMediaFormat**)&format));

    FAIL_RETURN(format->SetString(KEY_MIME, mime));
    FAIL_RETURN(format->SetInt32(KEY_WIDTH, width));
    FAIL_RETURN(format->SetInt32(KEY_HEIGHT, height));

    *result = format;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaFormat::GetMap(
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result);
     *result = mMap;
     REFCOUNT_ADD(*result);
     return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos


#include "elastos/droid/net/nsd/DnsSdTxtRecord.h"
#include "elastos/droid/net/nsd/CDnsSdTxtRecord.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::Build;

using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Nsd {

const Byte DnsSdTxtRecord::mSeperator = '=';

CAR_INTERFACE_IMPL_2(DnsSdTxtRecord, Object, IParcelable, IDnsSdTxtRecord)

ECode DnsSdTxtRecord::constructor()
{
    mData = ArrayOf<Byte>::Alloc(0);
    return NOERROR;
}

ECode DnsSdTxtRecord::constructor(
    /* [in] */ ArrayOf<Byte>* data)
{
    mData = data->Clone();
    return NOERROR;
}

ECode DnsSdTxtRecord::constructor(
    /* [in] */ IDnsSdTxtRecord* src)
{
    if (src != NULL) {
        src->GetRawData((ArrayOf<Byte>**)&mData);
    }
    return NOERROR;
}

ECode DnsSdTxtRecord::Set(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    if (key.IsNull()) {
        Slogger::E("CDnsSdTxtRecord", "key can not be null.");
        return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > keyBytes;
    AutoPtr<ArrayOf<Byte> > valBytes;
    Int32 valLen = 0;

    if (!value.IsNull()) {
        valBytes = ArrayOf<Byte>::Alloc((Byte*)value.string(), value.GetByteLength());
        valLen = valBytes->GetLength();
    }

    // try {
        keyBytes = ArrayOf<Byte>::Alloc((Byte*)key.string(), key.GetByteLength());
    // }
    // catch (java.io.UnsupportedEncodingException e) {
    //     Logger::E("key should be US-ASCII");
    //     return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // }

    for (Int32 i = 0; i < keyBytes->GetLength(); i++) {
        if ((*keyBytes)[i] == '=') {
            Slogger::E("CDnsSdTxtRecord", "= is not a valid character in key");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    if (keyBytes->GetLength() + valLen >= 255) {
        Slogger::E("CDnsSdTxtRecord", "Key and Value length cannot exceed 255 bytes");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 currentLoc;
    Remove(key, &currentLoc);
    if (currentLoc == -1)
        KeyCount(&currentLoc);

    Insert(keyBytes, valBytes, currentLoc);
    return NOERROR;
}

ECode DnsSdTxtRecord::Get(
    /* [in] */ const String& key,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ArrayOf<Byte> > val;
    GetValue(key, (ArrayOf<Byte>**)&val);
    if (val != NULL) {
        *result = String((const char*)val->GetPayload());
    }
    else {
        *result = String(NULL);
    }
    return NOERROR;
}

ECode DnsSdTxtRecord::Remove(
    /* [in] */ const String& key,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    Int32 avStart = 0, avLen, offset;
    Int32 keyByteLength = key.GetByteLength();
    for (Int32 i = 0; avStart < mData->GetLength(); i++) {
        avLen = (*mData)[avStart];
        if (keyByteLength <= avLen &&
                (keyByteLength == avLen || (*mData)[avStart + keyByteLength + 1] == mSeperator)) {
            String s((char *)mData->GetPayload() + avStart + 1, keyByteLength);
            if (key.EqualsIgnoreCase(s)) {
                AutoPtr<ArrayOf<Byte> > oldBytes = mData;
                mData = ArrayOf<Byte>::Alloc(oldBytes->GetLength() - avLen - 1);
                mData->Copy(oldBytes, avStart);
                offset = avStart + avLen + 1;
                mData->Copy(avStart, oldBytes, offset, oldBytes->GetLength() - offset);
                *result = i;
                return NOERROR;
            }
        }
        avStart += (0xFF & (avLen + 1));
    }

    return NOERROR;
}

ECode DnsSdTxtRecord::KeyCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 count = 0, nextKey;
    for (nextKey = 0; nextKey < mData->GetLength(); count++) {
        nextKey += (0xFF & ((*mData)[nextKey] + 1));
    }
    *result = count;
    return NOERROR;
}

ECode DnsSdTxtRecord::Contains(
    /* [in] */ const String& key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    String s;
    for (Int32 i = 0; (GetKey(i, &s), !s.IsNull()); i++) {
        if (key.EqualsIgnoreCase(s)) {
            *result = TRUE;
            return NOERROR;
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode DnsSdTxtRecord::GetSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mData->GetLength();
    return NOERROR;
}

ECode DnsSdTxtRecord::GetRawData(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mData->Clone();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode DnsSdTxtRecord::Insert(
    /* [in] */ ArrayOf<Byte>* keyBytes,
    /* [in] */ ArrayOf<Byte>* value,
    /* [in] */ Int32 index)
{
    AutoPtr<ArrayOf<Byte> > oldBytes = mData;
    Int32 valLen = (value != NULL) ? value->GetLength() : 0;
    Int32 insertion = 0;
    Int32 newLen, avLen;

    for (Int32 i = 0; i < index && insertion < mData->GetLength(); i++) {
        insertion += (0xFF & ((*mData)[insertion] + 1));
    }

    avLen = keyBytes->GetLength() + valLen + (value != NULL ? 1 : 0);
    newLen = avLen + oldBytes->GetLength() + 1;

    mData = ArrayOf<Byte>::Alloc(newLen);
    mData->Copy(oldBytes, insertion);


    Int32 secondHalfLen = oldBytes->GetLength() - insertion;
    mData->Copy(newLen - secondHalfLen, oldBytes, insertion, secondHalfLen);
    (*mData)[insertion] = (Byte) avLen;
    mData->Copy(insertion + 1, keyBytes, 0, keyBytes->GetLength());

    if (value != NULL) {
        (*mData)[insertion + 1 + keyBytes->GetLength()] = mSeperator;
        mData->Copy(insertion + keyBytes->GetLength() + 2, value, 0, valLen);
    }
    return NOERROR;
}

ECode DnsSdTxtRecord::GetKey(
    /* [in] */ Int32 index,
    /* [out] */ String* result)
{
    Int32 avStart = 0;

    for (Int32 i=0; i < index && avStart < mData->GetLength(); i++) {
        avStart += (*mData)[avStart] + 1;
    }

    if (avStart < mData->GetLength()) {
        Int32 avLen = (*mData)[avStart];
        Int32 aLen = 0;

        for (aLen=0; aLen < avLen; aLen++) {
            if ((*mData)[avStart + aLen + 1] == mSeperator) break;
        }

        *result = String(*mData, avStart + 1, aLen);
        return NOERROR;
    }
    *result = String(NULL);
    return NOERROR;
}

ECode DnsSdTxtRecord::GetValue(
    /* [in] */ Int32 index,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 avStart = 0;
    for (Int32 i=0; i < index && avStart < mData->GetLength(); i++) {
        avStart += (*mData)[avStart] + 1;
    }

    AutoPtr<ArrayOf<Byte> > array;
    if (avStart < mData->GetLength()) {
        Int32 avLen = (*mData)[avStart];
        Int32 aLen = 0;

        for (aLen=0; aLen < avLen; aLen++) {
            if ((*mData)[avStart + aLen + 1] == mSeperator) {
                array = ArrayOf<Byte>::Alloc(avLen - aLen - 1);
                array->Copy(mData,  avStart + aLen + 2, avLen - aLen - 1);
                break;
            }
        }
    }

    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode DnsSdTxtRecord::GetValueAsString(
    /* [in] */ Int32 index,
    /* [out] */ String* result)
{
    AutoPtr<ArrayOf<Byte> > rev;
    GetValue(index, (ArrayOf<Byte>**)&rev);
    *result = rev != NULL ? String((const char*)rev->GetPayload()) : String(NULL);
    return NOERROR;
}

ECode DnsSdTxtRecord::GetValue(
    /* [in] */ const String& forKey,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);

    String s;
    for (Int32 i = 0; (GetKey(i, &s), !s.IsNull()); i++) {
        if (forKey.EqualsIgnoreCase(s)) {
            return GetValue(i, result);
        }
    }

    return NOERROR;
}

ECode DnsSdTxtRecord::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    String a, res, val;

    StringBuilder av;
    for (Int32 i = 0; (GetKey(i, &a), !a.IsNull()); i++) {
        if (i != 0) {
            av += ", ";
        }
        av += "{";
        av += a;
        GetValueAsString(i, &val);
        if (!val.IsNull()) {
            av += "=";
            av += val;
            av += "}";
        }
        else
            av += "}";
    }

    res = av.ToString();
    *result = !res.IsNull() ? res : String("");
    return NOERROR;
}

ECode DnsSdTxtRecord::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(o)) FUNC_RETURN(TRUE)

    if (IDnsSdTxtRecord::Probe(o)) {
        *result = TRUE;
        return NOERROR;
    }

    DnsSdTxtRecord* record = (DnsSdTxtRecord*) IDnsSdTxtRecord::Probe(o);
    if (record == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    *result = Arrays::Equals(record->mData, mData);
    return NOERROR;
}

ECode DnsSdTxtRecord::GetHashCode(
        /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    *val = (Int32)mData.Get();
    return NOERROR;
}

ECode DnsSdTxtRecord::WriteToParcel(
        /* [in] */ IParcel* dest)
{
    return dest->WriteArrayOf((Handle32)mData.Get());
}

ECode DnsSdTxtRecord::ReadFromParcel(
        /* [in] */ IParcel* source)
{
    mData = NULL;
    return source->ReadArrayOf((Handle32*)&mData);
}

} // namespace Nsd
} // namespace Net
} // namespace Droid
} // namespace Elastos

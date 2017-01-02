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

#include "elastos/droid/internal/telephony/RILParcel.h"
#include "utils/String8.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using android::NO_ERROR;
using android::NO_MEMORY;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

RILParcel::RILParcel()
    : parcel(new android::Parcel())
{
}

RILParcel::~RILParcel()
{
    delete parcel;
}

ECode RILParcel::WriteInt32(
    /* [in] */ Int32 val)
{
    const android::status_t err = parcel->writeInt32(val);
    if (err != NO_ERROR) {
        Logger::E("RILParcel", "TODO RILParcel::WriteInt32 error happen");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode RILParcel::Marshall(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // do not marshall if there are binder objects in the parcel
    if(parcel->objectsCount()) {
        //Tried to marshall a Parcel that contained Binder objects.
        Logger::E("RILParcel", "TODO RILParcel::Marshall error happen");
        return E_RUNTIME_EXCEPTION;
    }

    Int32 size = parcel->dataSize();
    AutoPtr<ArrayOf<Byte> > ret = ArrayOf<Byte>::Alloc(size);

    if (ret != NULL) {
        Byte* array = ret->GetPayload();
        if (array != NULL) {
            memcpy(array, parcel->data(), parcel->dataSize());
            *result = ret;
            REFCOUNT_ADD(*result);
        }
    }

    return NOERROR;
}

ECode RILParcel::WriteString(
    /* [in] */ const String& val)
{
    android::status_t err = NO_MEMORY;
    if (val) {
        android::String16 str(val.string(), val.GetLength());
        err = parcel->writeString16(str.string(), str.size());
    } else {
        err = parcel->writeString16(NULL, 0);
    }
    if (err != NO_ERROR) {
        Logger::E("RILParcel", "TODO RILParcel::WriteString error happen");
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

ECode RILParcel::WriteByteArray(
    /* [in] */ ArrayOf<Byte>* array)
{
    if (array == NULL) {
        WriteInt32(-1);
        return NOERROR;
    }
    Int32 offset = 0;
    Int32 length = array->GetLength();

    const android::status_t err = parcel->writeInt32(length);
    if (err != NO_ERROR) {
        Logger::E("RILParcel", "TODO RILParcel::WriteByteArray error happen");
        return E_RUNTIME_EXCEPTION;
    }

    void* dest = parcel->writeInplace(length);
    if (dest == NULL) {
        //signalExceptionForError(env, clazz, NO_MEMORY);
        Logger::E("RILParcel", "TODO RILParcel::WriteByteArray 2 error happen");
        return E_RUNTIME_EXCEPTION;
    }

    Byte* ar = array->GetPayload();
    if (ar) {
        memcpy(dest, ar + offset, length);
    }
    return NOERROR;
}

ECode RILParcel::WriteByte(
    /* [in] */ Byte val)
{
    return WriteInt32(val);
}

ECode RILParcel::WriteStringArray(
    /* [in] */ ArrayOf<String>* array)
{
    if (array != NULL) {
        Int32 N = array->GetLength();
        WriteInt32(N);
        for (Int32 i = 0; i < N; i++) {
            WriteString((*array)[i]);
        }
    }
    else {
        WriteInt32(-1);
    }
    return NOERROR;
}

ECode RILParcel::WriteInt32Array(
    /* [in] */ ArrayOf<Int32>* val)
{
    if (val != NULL) {
        Int32 N = val->GetLength();
        WriteInt32(N);
        for (Int32 i=0; i<N; i++) {
            WriteInt32((*val)[i]);
        }
    } else {
        WriteInt32(-1);
    }
    return NOERROR;
}

ECode RILParcel::Unmarshall(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    if (length < 0) {
        return NOERROR;
    }

    Byte* array = data->GetPayload();
    if (array) {
        parcel->setDataSize(length);
        parcel->setDataPosition(0);

        void* raw = parcel->writeInplace(length);
        memcpy(raw, (array + offset), length);
    }
    return NOERROR;
}

ECode RILParcel::SetDataPosition(
    /* [in] */ Int32 pos)
{
    parcel->setDataPosition(pos);
    return NOERROR;
}

ECode RILParcel::ReadInt32(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = parcel->readInt32();
    return NOERROR;
}

ECode RILParcel::ReadInt64(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = parcel->readInt64();
    return NOERROR;
}

ECode RILParcel::ReadString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    size_t len;
    const char16_t* str = parcel->readString16Inplace(&len);
    if (str) {
        android::String8 str8(str, len);
        *result = String(str8.string());
    }
    return NOERROR;
}

ECode RILParcel::ReadStringArray(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 length = 0;
    ReadInt32(&length);
    if (length >= 0) {
        AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(length);
        for (Int32 i = 0; i < length; ++i) {
            String str;
            ReadString(&str);
            array->Set(i, str);
        }
        *result = array;
        REFCOUNT_ADD(*result);
    }
    return NOERROR;
}

ECode RILParcel::CreateByteArray(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    int32_t len = parcel->readInt32();

    // sanity check the stored length against the true data size
    if (len >= 0 && len <= (int32_t)parcel->dataAvail()) {
        AutoPtr<ArrayOf<Byte> > ret = ArrayOf<Byte>::Alloc(len);

        if (ret != NULL) {
            Byte* a2 = ret->GetPayload();
            if (a2) {
                const void* data = parcel->readInplace(len);
                memcpy(a2, data, len);
            }
            *result = ret;
            REFCOUNT_ADD(*result);
        }
    }
    return NOERROR;
}

ECode RILParcel::DataAvail(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    if (parcel)
        *result = parcel->dataAvail();
    return NOERROR;
}

} // Telephony
} // Internal
} // Droid
} // Elastos

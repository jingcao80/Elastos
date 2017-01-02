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

#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableString.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Core::CoreUtils;
using Elastos::Core::ECLSID_CString;
using Elastos::IO::IBuffer;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::CCharsetHelper;
using Elastos::IO::Charset::ICharsetHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableString::MarshalerString::MarshalerString(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableString* host)
{
    Marshaler::constructor(host, typeReference, nativeType);
}

ECode MarshalQueryableString::MarshalerString::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<ICharSequence> chars = ICharSequence::Probe(value);
    String str;
    chars->ToString(&str);
    AutoPtr<ArrayOf<Byte> > arr = str.GetBytes(/*UTF8_CHARSET*/);
    buffer->Put(arr);
    return buffer->Put(NUL); // metadata strings are NUL-terminated
}

ECode MarshalQueryableString::MarshalerString::CalculateMarshalSize(
    /* [in] */ IInterface* value,
    /* [out] */ Int32* outvalue)
{
    VALIDATE_NOT_NULL(outvalue);

    AutoPtr<ICharSequence> chars = ICharSequence::Probe(value);
    String str;
    chars->ToString(&str);
    AutoPtr<ArrayOf<Byte> > arr = str.GetBytes(/*UTF8_CHARSET*/);

    *outvalue = arr->GetLength() + 1; // metadata strings are NUL-terminated
    return NOERROR;
}

ECode MarshalQueryableString::MarshalerString::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    IBuffer::Probe(buffer)->Mark(); // save the current position

    Boolean foundNull = FALSE;
    Int32 stringLength = 0;

    Boolean res;
    while (IBuffer::Probe(buffer)->HasRemaining(&res), res) {
        Byte value;
        buffer->Get(&value);
        if (value == NUL) {
            foundNull = TRUE;
            break;
        }

        stringLength++;
    }

    if (VERBOSE) {
        Logger::V(TAG, "unmarshal - scanned %d characters; found null? %d", stringLength, foundNull);
    }

    if (!foundNull) {
        Logger::E(TAG, "Strings must be null-terminated");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    IBuffer::Probe(buffer)->Reset(); // go back to the previously marked position

    AutoPtr<ArrayOf<Byte> > strBytes = ArrayOf<Byte>::Alloc(stringLength + 1);
    buffer->Get(strBytes, /*dstOffset*/0, stringLength + 1); // including null character

    // not including null character
    String str(*strBytes, /*offset*/0, stringLength/*, UTF8_CHARSET*/);
    AutoPtr<ICharSequence> chars = CoreUtils::Convert(str);
    *outface = TO_IINTERFACE(chars);
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableString::MarshalerString::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = IMarshaler::NATIVE_SIZE_DYNAMIC;
    return NOERROR;
}

static AutoPtr<ICharset> initCharset()
{
    AutoPtr<ICharsetHelper> helper;
    CCharsetHelper::AcquireSingleton((ICharsetHelper**)&helper);
    AutoPtr<ICharset> charset;
    helper->ForName(String("UTF-8"), (ICharset**)&charset);
    return charset;
}

const String MarshalQueryableString::TAG("MarshalQueryableString");// = MarshalQueryableString.class.getSimpleName();
const Boolean MarshalQueryableString::VERBOSE = FALSE;// Log.isLoggable(TAG, Log.VERBOSE);

const AutoPtr<ICharset> MarshalQueryableString::UTF8_CHARSET = initCharset();
const Byte MarshalQueryableString::NUL = (Byte)'\0'; // used as string terminator

CAR_INTERFACE_IMPL_2(MarshalQueryableString, Object, IMarshalQueryableString, IMarshalQueryable)


ECode MarshalQueryableString::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerString(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableString::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    if (nativeType == ICameraMetadataNative::TYPE_BYTE) {
        ClassID cls;
        managedType->GetClassType(&cls);
        if (cls == ECLSID_CString) {
            *value = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

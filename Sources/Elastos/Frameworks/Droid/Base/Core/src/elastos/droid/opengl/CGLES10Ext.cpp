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

#include "elastos/droid/opengl/CGLES10Ext.h"
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <elastos/utility/logging/Slogger.h>

#define LOGD(msg) SLOGGERD("CGLES10Ext", msg)

using Elastos::IO::CNIOAccess;
using Elastos::IO::INIOAccess;

namespace Elastos {
namespace Droid {
namespace Opengl {

CAR_INTERFACE_IMPL(CGLES10Ext, Singleton, IGLES10Ext)

CAR_SINGLETON_IMPL(CGLES10Ext)

ECode CGLES10Ext::GlQueryMatrixxOES(
    /* [in] */ ArrayOf<Int32>* mantissa_ref,
    /* [in] */ Int32 mantissaOffset,
    /* [in] */ ArrayOf<Int32>* exponent_ref,
    /* [in] */ Int32 exponentOffset,
    /* [out] */ Int32* matrixx)
{
    VALIDATE_NOT_NULL(matrixx)

    *matrixx = -1;
    GLfixed *mantissa_base = (GLfixed *) 0;
    Int32 _mantissaRemaining;
    GLfixed *mantissa = (GLfixed *) 0;
    GLint *exponent_base = (GLint *) 0;
    Int32 _exponentRemaining;
    GLint *exponent = (GLint *) 0;

    if (!mantissa_ref) {
        LOGD("GlQueryMatrixxOES: mantissa == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mantissaOffset < 0) {
        LOGD("GlQueryMatrixxOES: mantissaOffset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _mantissaRemaining = mantissa_ref->GetLength() - mantissaOffset;
    if (_mantissaRemaining < 16) {
        LOGD("GlQueryMatrixxOES: length - mantissaOffset < 16 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mantissa_base = (GLfixed *) mantissa_ref->GetPayload();
    mantissa = mantissa_base + mantissaOffset;

    if (!exponent_ref) {
        LOGD("GlQueryMatrixxOES: exponent == null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (exponentOffset < 0) {
        LOGD("GlQueryMatrixxOES: exponentOffset < 0")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    _exponentRemaining = exponent_ref->GetLength() - exponentOffset;
    if (_exponentRemaining < 16) {
        LOGD("GlQueryMatrixxOES: length - exponentOffset < 16 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    exponent_base = (GLint *) mantissa_ref->GetPayload();;
    exponent = exponent_base + exponentOffset;

    *matrixx = glQueryMatrixxOES(
        (GLfixed *)mantissa,
        (GLint *)exponent
    );

    return NOERROR;
}

ECode CGLES10Ext::GlQueryMatrixxOES(
    /* [in] */ Elastos::IO::IInt32Buffer* mantissa_buf,
    /* [in] */ Elastos::IO::IInt32Buffer* exponent_buf,
    /* [out] */ Int32* matrixx)
{
    VALIDATE_NOT_NULL(matrixx)

    Handle64 _mantissaArray = (Handle64) 0;
    Int32 _mantissaBufferOffset = (Int32) 0;
    Handle64 _exponentArray = (Handle64) 0;
    Int32 _exponentBufferOffset = (Int32) 0;
    GLbitfield _returnValue = -1;
    *matrixx = -1;
    Int32 _mantissaRemaining;
    GLfixed *mantissa = (GLfixed *) 0;
    Handle64 mantissaTmp;
    Int32 _exponentRemaining;
    GLint *exponent = (GLint *) 0;
    Handle64 exponentTmp;

    FAIL_RETURN(GetPointer(IBuffer::Probe(mantissa_buf), &_mantissaArray, &_mantissaRemaining, &_mantissaBufferOffset, &mantissaTmp));
    mantissa = (GLfixed *) mantissaTmp;
    if (_mantissaRemaining < 16) {
        LOGD("GlQueryMatrixxOESEx: remaining() < 16 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(GetPointer(IBuffer::Probe(exponent_buf), &_exponentArray, &_exponentRemaining, &_exponentBufferOffset, &exponentTmp));
    exponent = (GLint *) exponentTmp;
    if (_exponentRemaining < 16) {
        LOGD("GlQueryMatrixxOESEx: remaining() < 16 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mantissa == NULL) {
        char * _mantissaBase = reinterpret_cast<char *>(_mantissaArray);
        mantissa = (GLfixed *) (_mantissaBase + _mantissaBufferOffset);
    }
    if (exponent == NULL) {
        char * _exponentBase = reinterpret_cast<char *>(_exponentArray);
        exponent = (GLint *) (_exponentBase + _exponentBufferOffset);
    }
    _returnValue = glQueryMatrixxOES(
        (GLfixed *)mantissa,
        (GLint *)exponent
    );

    *matrixx = _returnValue;
    return NOERROR;
}

ECode CGLES10Ext::GetPointer(
    /* [in] */ IBuffer* buffer,
    /* [in, out] */ Handle64* array,
    /* [in, out] */ Int32* remaining,
    /* [in, out] */ Int32* offset,
    /* [out] */ Handle64* rst)
{
    VALIDATE_NOT_NULL(rst)

    Int32 position;
    Int32 limit;
    Int32 elementSizeShift;
    Int64 pointer;
    buffer->GetPosition(&position);
    buffer->GetLimit(&limit);
    buffer->GetElementSizeShift(&elementSizeShift);
    *remaining = (limit - position) << elementSizeShift;

    AutoPtr<INIOAccess> helper;
    CNIOAccess::AcquireSingleton((INIOAccess**)&helper);

    helper->GetBasePointer(buffer, &pointer);
    if (pointer != 0L) {
        *array = 0;
        *rst = (Handle64)pointer;
        return NOERROR;
    }

    Boolean hasArray;
    if (buffer->HasArray(&hasArray), hasArray) {
        buffer->GetPrimitiveArray(array);
    } else {
        *array = 0;
    }
    helper->GetBaseArrayOffset(buffer, offset);

    *rst = 0;
    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos


#include "CGLES10Ext.h"
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <cmdef.h>
#include <elastos/utility/logging/Slogger.h>

#define LOGD(msg) SLOGGERD("CGLES10Ext", msg)

using Elastos::IO::CNIOAccessHelper;
using Elastos::IO::INIOAccessHelper;

namespace Elastos {
namespace Droid {
namespace Opengl {

ECode CGLES10Ext::glQueryMatrixxOES(
    /* [in] */ ArrayOf<Int32>* mantissa_ref,
    /* [in] */ Int32 mantissaOffset,
    /* [in] */ ArrayOf<Int32>* exponent_ref,
    /* [in] */ Int32 exponentOffset,
    /* [out] */ Int32* matrixx)
{
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

ECode CGLES10Ext::glQueryMatrixxOES(
    /* [in] */ Elastos::IO::IInt32Buffer* mantissa_buf,
    /* [in] */ Elastos::IO::IInt32Buffer* exponent_buf,
    /* [out] */ Int32* matrixx)
{
    Handle32 _mantissaArray = (Handle32) 0;
    Int32 _mantissaBufferOffset = (Int32) 0;
    Handle32 _exponentArray = (Handle32) 0;
    Int32 _exponentBufferOffset = (Int32) 0;
    GLbitfield _returnValue = -1;
    *matrixx = -1;
    Int32 _mantissaRemaining;
    GLfixed *mantissa = (GLfixed *) 0;
    Handle32 mantissaTmp;
    Int32 _exponentRemaining;
    GLint *exponent = (GLint *) 0;
    Handle32 exponentTmp;

    FAIL_RETURN(GetPointer(mantissa_buf, &_mantissaArray, &_mantissaRemaining, &_mantissaBufferOffset, &mantissaTmp));
    mantissa = (GLfixed *) mantissaTmp;
    if (_mantissaRemaining < 16) {
        LOGD("GlQueryMatrixxOESEx: remaining() < 16 < needed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(GetPointer(exponent_buf, &_exponentArray, &_exponentRemaining, &_exponentBufferOffset, &exponentTmp));
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
    /* [in, out] */ Handle32* array,
    /* [in, out] */ Int32* remaining,
    /* [in, out] */ Int32* offset,
    /* [out] */ Handle32* rst)
{
    Int32 position;
    Int32 limit;
    Int32 elementSizeShift;
    Int64 pointer;
    buffer->GetPosition(&position);
    buffer->GetLimit(&limit);
    buffer->GetElementSizeShift(&elementSizeShift);
    *remaining = (limit - position) << elementSizeShift;

    AutoPtr<INIOAccessHelper> helper;
    CNIOAccessHelper::AcquireSingleton((INIOAccessHelper**)&helper);

    helper->GetBasePointer(buffer, &pointer);
    if (pointer != 0L) {
        *array = 0;
        *rst = (Handle32)pointer;
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


#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "org/apache/http/protocol/HTTP.h"
#include "elastos/utility/logging/Logger.h"
#include "elastos/core/Math.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Protocol::HTTP;

namespace Org {
namespace Apache {
namespace Http {
namespace Utility {

CCharArrayBuffer::CCharArrayBuffer()
    : mLen(0)
{}

CAR_OBJECT_IMPL(CCharArrayBuffer)

CAR_INTERFACE_IMPL(CCharArrayBuffer, Object, ICharArrayBuffer)

void CCharArrayBuffer::Expand(
    /* [in] */ Int32 newlen)
{
    using Elastos::Core::Math;

    AutoPtr< ArrayOf<Char32> > newbuffer = ArrayOf<Char32>::Alloc(Math::Max(mBuffer->GetLength() << 1, newlen));
    newbuffer->Copy(0, mBuffer, 0, mLen);
    mBuffer = newbuffer;
}

ECode CCharArrayBuffer::Append(
    /* [in] */ ArrayOf<Char32>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    if (b == NULL) {
        return NOERROR;
    }
    if ((off < 0) || (off > b->GetLength()) || (len < 0) ||
            ((off + len) < 0) || ((off + len) > b->GetLength())) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (len == 0) {
        return NOERROR;
    }
    Int32 newlen = mLen + len;
    if (newlen > mBuffer->GetLength()) {
        Expand(newlen);
    }
    mBuffer->Copy(mLen, b, off, len);
    mLen = newlen;
    return NOERROR;
}

ECode CCharArrayBuffer::Append(
    /* [in] */ const String& _str)
{
    String str = _str;
    if (str.IsNull()) {
        str = String("null");
    }
    Int32 strlen = str.GetByteLength();
    Int32 newlen = mLen + strlen;
    if (newlen > mBuffer->GetLength()) {
        Expand(newlen);
    }
    strncat((char*)mBuffer->GetPayload(), (const char*)str, strlen);
    mLen = newlen;
    return NOERROR;
}

ECode CCharArrayBuffer::Append(
    /* [in] */ ICharArrayBuffer* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    if (b == NULL) {
        return NOERROR;
    }
    AutoPtr<CCharArrayBuffer> buffer = (CCharArrayBuffer*)b;
    return Append(buffer->mBuffer, off, len);
}

ECode CCharArrayBuffer::Append(
    /* [in] */ ICharArrayBuffer* b)
{
    if (b == NULL) {
        return NOERROR;
    }
    AutoPtr<CCharArrayBuffer> buffer = (CCharArrayBuffer*)b;
    return Append(buffer->mBuffer, 0, buffer->mLen);
}

ECode CCharArrayBuffer::Append(
    /* [in] */ Char32 ch)
{
    Int32 newlen = mLen + 1;
    if (newlen > mBuffer->GetLength()) {
        Expand(newlen);
    }
    (*mBuffer)[mLen] = ch;
    mLen = newlen;
    return NOERROR;
}

ECode CCharArrayBuffer::Append(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    if (b == NULL) {
        return NOERROR;
    }
    if ((off < 0) || (off > b->GetLength()) || (len < 0) ||
            ((off + len) < 0) || ((off + len) > b->GetLength())) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (len == 0) {
        return NOERROR;
    }
    Int32 oldlen = mLen;
    Int32 newlen = oldlen + len;
    if (newlen > mBuffer->GetLength()) {
        Expand(newlen);
    }
    for (Int32 i1 = off, i2 = oldlen; i2 < newlen; i1++, i2++) {
        Int32 ch = (*b)[i1];
        if (ch < 0) {
            ch = 256 + ch;
        }
        (*mBuffer)[i2] = (Char32)ch;
    }
    mLen = newlen;
    return NOERROR;
}

ECode CCharArrayBuffer::Append(
    /* [in] */ IByteArrayBuffer* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    if (b == NULL) {
        return NOERROR;
    }
    AutoPtr< ArrayOf<Byte> > buf;
    b->GetBuffer((ArrayOf<Byte>**)&buf);
    return Append(buf, off, len);
}

ECode CCharArrayBuffer::Append(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IObject> o = IObject::Probe(obj);
    String str;
    o->ToString(&str);
    return Append(str);
}

ECode CCharArrayBuffer::Clear()
{
    mLen = 0;
    return NOERROR;
}

ECode CCharArrayBuffer::ToCharArray(
    /* [out, callee] */ ArrayOf<Char32>** charArray)
{
    VALIDATE_NOT_NULL(charArray)
    AutoPtr< ArrayOf<Char32> > b = ArrayOf<Char32>::Alloc(mLen);
    if (mLen > 0) {
        b->Copy(0, mBuffer, 0, mLen);
    }
    *charArray = b;
    REFCOUNT_ADD(*charArray)
    return NOERROR;
}

ECode CCharArrayBuffer::CharAt(
    /* [in] */ Int32 i,
    /* [out] */ Char32* charAt)
{
    VALIDATE_NOT_NULL(charAt)
    *charAt = (*mBuffer)[i];
    return NOERROR;
}

ECode CCharArrayBuffer::GetBuffer(
    /* [out, callee] */ ArrayOf<Char32>** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = mBuffer;
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode CCharArrayBuffer::GetCapacity(
    /* [out] */ Int32* capacity)
{
    VALIDATE_NOT_NULL(capacity)
    *capacity = mBuffer->GetLength();
    return NOERROR;
}

ECode CCharArrayBuffer::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    *length = mLen;
    return NOERROR;
}

ECode CCharArrayBuffer::EnsureCapacity(
    /* [in] */ Int32 required)
{
    Int32 available = mBuffer->GetLength() - mLen;
    if (required > available) {
        Expand(mLen + required);
    }
    return NOERROR;
}

ECode CCharArrayBuffer::SetLength(
    /* [in] */ Int32 len)
{
    if (len < 0 || len > mBuffer->GetLength()) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    mLen = len;
    return NOERROR;
}

ECode CCharArrayBuffer::IsEmpty(
    /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty)
    *isEmpty = mLen == 0;
    return NOERROR;
}

ECode CCharArrayBuffer::IsFull(
    /* [out] */ Boolean* isFull)
{
    VALIDATE_NOT_NULL(isFull)
    *isFull = mBuffer->GetLength() == mLen;
    return NOERROR;
}

ECode CCharArrayBuffer::IndexOf(
    /* [in] */ Int32 ch,
    /* [in] */ Int32 beginIndex,
    /* [in] */ Int32 endIndex,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    if (beginIndex < 0) {
        beginIndex = 0;
    }
    if (endIndex > mLen) {
        endIndex = mLen;
    }
    if (beginIndex > endIndex) {
        *index = -1;
        return NOERROR;
    }
    for (Int32 i = beginIndex; i < endIndex; i++) {
        if ((Int32)(*mBuffer)[i] == ch) {
            *index = i;
            return NOERROR;
        }
    }
    *index = -1;
    return NOERROR;
}

ECode CCharArrayBuffer::IndexOf(
    /* [in] */ Int32 ch,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    return IndexOf(ch, 0, mLen, index);
}

ECode CCharArrayBuffer::Substring(
    /* [in] */ Int32 beginIndex,
    /* [in] */ Int32 endIndex,
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string)
    if (beginIndex < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (endIndex > mLen) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (beginIndex > endIndex) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    *string = String(*mBuffer, beginIndex, endIndex - beginIndex);
    return NOERROR;
}

ECode CCharArrayBuffer::SubstringTrimmed(
    /* [in] */ Int32 beginIndex,
    /* [in] */ Int32 endIndex,
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string)
    if (beginIndex < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (endIndex > mLen) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (beginIndex > endIndex) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    while (beginIndex < endIndex && HTTP::IsWhitespace((*mBuffer)[beginIndex])) {
        beginIndex++;
    }
    while (endIndex > beginIndex && HTTP::IsWhitespace((*mBuffer)[endIndex - 1])) {
        endIndex--;
    }
    *string = String(*mBuffer, beginIndex, endIndex - beginIndex);
    return NOERROR;
}

ECode CCharArrayBuffer::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(*mBuffer, 0, mLen);
    return NOERROR;
}

ECode CCharArrayBuffer::constructor(
    /* [in] */ Int32 capacity)
{
    if (capacity < 0) {
        Logger::E("CCharArrayBuffer", "Buffer capacity may not be negative");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mBuffer = ArrayOf<Char32>::Alloc(capacity);
    return NOERROR;
}

} // namespace Utility
} // namespace Http
} // namespace Apache
} // namespace Org

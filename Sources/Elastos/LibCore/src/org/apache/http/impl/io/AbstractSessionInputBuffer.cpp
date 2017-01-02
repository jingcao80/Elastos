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

#include "org/apache/http/impl/io/AbstractSessionInputBuffer.h"
#include "org/apache/http/params/HttpProtocolParams.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "org/apache/http/utility/CByteArrayBuffer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IO::EIID_ISessionInputBuffer;
using Org::Apache::Http::Params::HttpProtocolParams;
using Org::Apache::Http::Params::ICoreConnectionPNames;
using Org::Apache::Http::Protocol::IHTTP;
using Org::Apache::Http::Utility::CCharArrayBuffer;
using Org::Apache::Http::Utility::CByteArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

AbstractSessionInputBuffer::AbstractSessionInputBuffer()
    : mBufferpos(0)
    , mBufferlen(0)
    , mCharset(IHTTP::US_ASCII)
    , mAscii(TRUE)
    , mMaxLineLen(-1)
{}

CAR_INTERFACE_IMPL(AbstractSessionInputBuffer, Object, ISessionInputBuffer)

ECode AbstractSessionInputBuffer::Init(
    /* [in] */ IInputStream* instream,
    /* [in] */ Int32 buffersize,
    /* [in] */ IHttpParams* params)
{
    if (instream == NULL) {
        Logger::E("AbstractSessionInputBuffer", "Input stream may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (buffersize <= 0) {
        Logger::E("AbstractSessionInputBuffer", "Buffer size may not be negative or zero");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        Logger::E("AbstractSessionInputBuffer", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mInstream = instream;
    mBuffer = ArrayOf<Byte>::Alloc(buffersize);
    mBufferpos = 0;
    mBufferlen = 0;
    mLinebuffer = NULL;
    CByteArrayBuffer::New(buffersize, (IByteArrayBuffer**)&mLinebuffer);
    HttpProtocolParams::GetHttpElementCharset(params, &mCharset);
    mAscii = mCharset.EqualsIgnoreCase(IHTTP::US_ASCII) || mCharset.EqualsIgnoreCase(IHTTP::ASCII);
    params->GetInt32Parameter(ICoreConnectionPNames::MAX_LINE_LENGTH, -1, &mMaxLineLen);
    mMetrics = new HttpTransportMetricsImpl();
    return NOERROR;
}

Int32 AbstractSessionInputBuffer::FillBuffer()
{
    // compact the buffer if necessary
    if (mBufferpos > 0) {
        Int32 len = mBufferlen - mBufferpos;
        if (len > 0) {
            AutoPtr< ArrayOf<Byte> > temp = ArrayOf<Byte>::Alloc(len);
            temp->Copy(mBuffer, mBufferpos, len);
            mBuffer = temp;
        }
        mBufferpos = 0;
        mBufferlen = len;
    }
    Int32 l;
    Int32 off = mBufferlen;
    Int32 len = mBuffer->GetLength() - off;
    mInstream->Read(mBuffer, off, len, &l);
    if (l == -1) {
        return -1;
    }
    else {
        mBufferlen = off + l;
        mMetrics->IncrementBytesTransferred(l);
        return l;
    }
}

Boolean AbstractSessionInputBuffer::HasBufferedData()
{
    return mBufferpos < mBufferlen;
}

ECode AbstractSessionInputBuffer::Read(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    Int32 noRead = 0;
    while (!HasBufferedData()) {
        noRead = FillBuffer();
        if (noRead == -1) {
            *count = -1;
            return NOERROR;
        }
    }
    *count = (*mBuffer)[mBufferpos++] & 0xff;
    return NOERROR;
}

ECode AbstractSessionInputBuffer::Read(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    if (b == NULL) {
        *count = 0;
        return NOERROR;
    }
    Int32 noRead = 0;
    while (!HasBufferedData()) {
        noRead = FillBuffer();
        if (noRead == -1) {
            *count = -1;
            return NOERROR;
        }
    }
    Int32 chunk = mBufferlen - mBufferpos;
    if (chunk > len) {
        chunk = len;
    }
    b->Copy(off, mBuffer, mBufferpos, chunk);
    mBufferpos += chunk;
    *count = chunk;
    return NOERROR;
}

ECode AbstractSessionInputBuffer::Read(
    /* [in] */ ArrayOf<Byte>* b,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    if (b == NULL) {
        *count = 0;
    }
    return Read(b, 0, b->GetLength(), count);
}

Int32 AbstractSessionInputBuffer::LocateLF()
{
    for (Int32 i = mBufferpos; i < mBufferlen; i++) {
        if ((*mBuffer)[i] == IHTTP::LF) {
            return i;
        }
    }
    return -1;
}

ECode AbstractSessionInputBuffer::ReadLine(
    /* [in] */ ICharArrayBuffer* charbuffer,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;
    if (charbuffer == NULL) {
        Logger::E("AbstractSessionInputBuffer", "Char array buffer may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mLinebuffer->Clear();
    Int32 noRead = 0;
    Boolean retry = TRUE;
    while (retry) {
        // attempt to find end of line (LF)
        Int32 i = LocateLF();
        if (i != -1) {
            // end of line found.
            Boolean isEmpty;
            if (mLinebuffer->IsEmpty(&isEmpty), isEmpty) {
                // the entire line is preset in the read buffer
                return LineFromReadBuffer(charbuffer, i, count);
            }
            retry = FALSE;
            Int32 len = i + 1 - mBufferpos;
            mLinebuffer->Append(mBuffer, mBufferpos, len);
            mBufferpos = i + 1;
        }
        else {
            // end of line not found
            if (HasBufferedData()) {
                Int32 len = mBufferlen - mBufferpos;
                mLinebuffer->Append(mBuffer, mBufferpos, len);
                mBufferpos = mBufferlen;
            }
            noRead = FillBuffer();
            if (noRead == -1) {
                retry = FALSE;
            }
        }
        Int32 len;
        if (mMaxLineLen > 0 && (mLinebuffer->GetLength(&len), len >= mMaxLineLen)) {
            Logger::E("AbstractSessionInputBuffer", "Maximum line length limit exceeded");
            return E_IO_EXCEPTION;
        }
    }
    Boolean isEmpty;
    if (noRead == -1 && (mLinebuffer->IsEmpty(&isEmpty), isEmpty)) {
        // indicate the end of stream
        *count = -1;
        return NOERROR;
    }
    return LineFromLineBuffer(charbuffer, count);
}

ECode AbstractSessionInputBuffer::LineFromLineBuffer(
    /* [in] */ ICharArrayBuffer* charbuffer,
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len)
    // discard LF if found
    Int32 l;
    mLinebuffer->GetLength(&l);
    if (l > 0) {
        Int32 b;
        if (mLinebuffer->ByteAt(l - 1, &b), b == IHTTP::LF) {
            l--;
            mLinebuffer->SetLength(l);
        }
        // discard CR if found
        if (l > 0) {
            if (mLinebuffer->ByteAt(l - 1, &b), b == IHTTP::CR) {
                l--;
                mLinebuffer->SetLength(l);
            }
        }
    }
    mLinebuffer->GetLength(&l);
    if (mAscii) {
        charbuffer->Append(mLinebuffer, 0, l);
    }
    else {
        // This is VERY memory inefficient, BUT since non-ASCII charsets are
        // NOT meant to be used anyway, there's no point optimizing it
        AutoPtr< ArrayOf<Byte> > bytes;
        mLinebuffer->GetBuffer((ArrayOf<Byte>**)&bytes);
        assert(0);
        // String s = new String(bytes, 0, l, this.charset);
        // charbuffer->Append(s);
    }
    *len = l;
    return NOERROR;
}

ECode AbstractSessionInputBuffer::LineFromReadBuffer(
    /* [in] */ ICharArrayBuffer* charbuffer,
    /* [in] */ Int32 pos,
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    Int32 off = mBufferpos;
    Int32 len;
    mBufferpos = pos + 1;
    // BEGIN android-changed
    // The first test below was fixed to not try to skip beyond the
    // start of the live part of the buffer.
    if (pos > off && (*mBuffer)[pos - 1] == IHTTP::CR) {
        // skip CR if found
        pos--;
    }
    // END android-changed
    len = pos - off;
    if (mAscii) {
        charbuffer->Append(mBuffer, off, len);
    }
    else {
        // This is VERY memory inefficient, BUT since non-ASCII charsets are
        // NOT meant to be used anyway, there's no point optimizing it
        assert(0);
        // String s = new String(this.buffer, off, len, this.charset);
        // charbuffer->Append(s);
    }
    *length = len;
    return NOERROR;
}

ECode AbstractSessionInputBuffer::ReadLine(
    /* [out] */ String* line)
{
    VALIDATE_NOT_NULL(line)
    AutoPtr<ICharArrayBuffer> charbuffer;
    CCharArrayBuffer::New(64, (ICharArrayBuffer**)&charbuffer);
    Int32 l;
    ReadLine(charbuffer, &l);
    if (l != -1) {
        return IObject::Probe(charbuffer)->ToString(line);
    }
    else {
        *line = String(NULL);
    }
    return NOERROR;
}

ECode AbstractSessionInputBuffer::GetMetrics(
    /* [out] */ IHttpTransportMetrics** metrics)
{
    VALIDATE_NOT_NULL(metrics)
    *metrics = (IHttpTransportMetrics*)mMetrics;
    REFCOUNT_ADD(*metrics)
    return NOERROR;
}

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org
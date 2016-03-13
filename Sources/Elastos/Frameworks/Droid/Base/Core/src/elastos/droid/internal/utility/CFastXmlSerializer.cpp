
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/internal/utility/CFastXmlSerializer.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::CCharBufferHelper;
using Elastos::IO::Charset::CCharsetHelper;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::ICharsetHelper;
using Elastos::IO::Charset::ICoderResult;
using Elastos::IO::COutputStreamWriter;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::IBuffer;
using Elastos::IO::ICharBuffer;
using Elastos::IO::ICharBufferHelper;
using Elastos::IO::IFlushable;
using Elastos::IO::IOutputStreamWriter;
using Elastos::Utility::Logging::Logger;
using Org::Xmlpull::V1::EIID_IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

static AutoPtr<ArrayOf<String> > InitESCAPE_TABLE()
{
    AutoPtr<ArrayOf<String> > strings = ArrayOf<String>::Alloc(64);
    for (Int32 i = 0; i < strings->GetLength(); ++i) {
        (*strings)[i] = String(NULL);
    }

    (*strings)[34] = String("&quot;");
    (*strings)[38] = String("&amp;");
    (*strings)[60] = String("&lt;");
    (*strings)[62] = String("&gt;");
    return strings;
}

const AutoPtr<ArrayOf<String> > CFastXmlSerializer::ESCAPE_TABLE = InitESCAPE_TABLE();
const Int32 CFastXmlSerializer::BUFFER_LEN = 8192;
String CFastXmlSerializer::sSpace("                                                              ");

CAR_INTERFACE_IMPL_2(CFastXmlSerializer, Object, IFastXmlSerializer, IXmlSerializer)

CAR_OBJECT_IMPL(CFastXmlSerializer)

CFastXmlSerializer::CFastXmlSerializer()
    : mPos(0)
    , mIndent(FALSE)
    , mInTag(FALSE)
    , mNesting(0)
    , mLineStart(TRUE)
{
}

ECode CFastXmlSerializer::constructor()
{
    AutoPtr<IByteBufferHelper> helper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    FAIL_RETURN(helper->Allocate(BUFFER_LEN, (IByteBuffer**)&mBytes))

    mText = ArrayOf<Char32>::Alloc(BUFFER_LEN);
    return NOERROR;
}

ECode CFastXmlSerializer::Append(
    /* [in] */ Char32 ch)
{
    Int32 pos = mPos;
    if (pos >= (BUFFER_LEN - 1)) {
        FAIL_RETURN(Flush());
        pos = mPos;
    }
    (*mText)[pos] = ch;
    mPos = pos+1;
    return NOERROR;
}

ECode CFastXmlSerializer::Append(
    /* [in] */ const String& str,
    /* [in] */ Int32 i,
    /* [in] */ Int32 length)
{
    Int32 pos;
    StringBuilder sb;
    if (length > BUFFER_LEN) {
        Int32 end = i + length;
        while (i < end) {
            Int32 next = i + BUFFER_LEN;
            FAIL_RETURN(Append(str, i, next<end ? BUFFER_LEN : (end-i)));
            i = next;
        }
        return NOERROR;
    }
    pos = mPos;
    if ((pos + length) > BUFFER_LEN) {
        FAIL_RETURN(Flush());
        pos = mPos;
    }

    sb += str;
    sb.GetChars(i, i + length, mText, pos);
    mPos = pos + length;

    return NOERROR;
}

ECode CFastXmlSerializer::Append(
    /* [in] */ ArrayOf<Char32>* buf,
    /* [in] */ Int32 i,
    /* [in] */ Int32 length)
{
    if (length > BUFFER_LEN) {
        Int32 end = i + length;
        Int32 next;
        while (i < end) {
            next = i + BUFFER_LEN;
            FAIL_RETURN(Append(buf, i, next<end ? BUFFER_LEN : (end-i)));
            i = next;
        }
        return NOERROR;
    }
    Int32 pos = mPos;
    if ((pos+length) > BUFFER_LEN) {
        FAIL_RETURN(Flush());
        pos = mPos;
    }
    mText->Copy(pos, buf, i, length);
    // System.arraycopy(buf, i, mText, pos, length);
    mPos = pos + length;
    return NOERROR;
}

ECode CFastXmlSerializer::Append(
    /* [in] */ const String& str)
{
    return Append(str, 0, str.GetLength());
}

ECode CFastXmlSerializer::AppendIndent(
    /* [in] */ Int32 indent)
{
    indent *= 4;
    if (indent > sSpace.GetLength()) {
        indent = sSpace.GetLength();
    }
    return Append(sSpace, 0, indent);
}

ECode CFastXmlSerializer::EscapeAndAppendString(
    /* [in] */ const String& string)
{
    AutoPtr<ArrayOf<Char32> > charArray = string.GetChars();
    Int32 N = charArray->GetLength();
    Char32 NE = (Char32)ESCAPE_TABLE->GetLength();
    AutoPtr<ArrayOf<String> > escapes = ESCAPE_TABLE;
    Int32 lastPos = 0;
    Int32 pos;
    Char32 c;
    String escape;
    for (pos=0; pos < N; pos++) {
        c = (*charArray)[pos];
        if (c >= NE) continue;
        escape = (*escapes)[c];
        if (escape.IsNull()) continue;
        if (lastPos < pos)
            FAIL_RETURN(Append(string, lastPos, pos-lastPos));
        lastPos = pos + 1;
        FAIL_RETURN(Append(escape));
    }
    if (lastPos < pos)
        FAIL_RETURN(Append(string, lastPos, pos-lastPos));
    return NOERROR;
}

ECode CFastXmlSerializer::EscapeAndAppendString(
    /* [in] */ ArrayOf<Char32>* buf,
    /* [in] */ Int32 start,
    /* [in] */ Int32 len)
{
    Char32 NE = (Char32)ESCAPE_TABLE->GetLength();
    AutoPtr<ArrayOf<String> > escapes = ESCAPE_TABLE;
    Int32 end = start+len;
    Int32 lastPos = start;
    Int32 pos;
    Char32 c;
    for (pos=start; pos<end; pos++) {
        c = (*buf)[pos];
        if (c >= NE) continue;
        String escape = (*escapes)[c];
        if (escape.IsNull()) continue;
        if (lastPos < pos)
            FAIL_RETURN(Append(buf, lastPos, pos-lastPos));
        lastPos = pos + 1;
        FAIL_RETURN(Append(escape));
    }
    if (lastPos < pos)
        FAIL_RETURN(Append(buf, lastPos, pos-lastPos));
    return NOERROR;
}

ECode CFastXmlSerializer::WriteAttribute(
    /* [in] */ const String& nsps,
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    FAIL_RETURN(Append(' '));
    if (!nsps.IsNull()) {
        FAIL_RETURN(Append(nsps));
        FAIL_RETURN(Append(':'));
    }
    FAIL_RETURN(Append(name));
    FAIL_RETURN(Append(String("=\"")));

    FAIL_RETURN(EscapeAndAppendString(value));
    FAIL_RETURN(Append('"'));
    mLineStart = FALSE;
    return NOERROR;
}

ECode CFastXmlSerializer::WriteCdSect(
    /* [in] */ const String& text)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CFastXmlSerializer::WriteComment(
    /* [in] */ const String& text)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CFastXmlSerializer::WriteDocDecl(
    /* [in] */ const String& text)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CFastXmlSerializer::EndDocument()
{
    return Flush();
}

ECode CFastXmlSerializer::WriteEndTag(
    /* [in] */ const String& ns,
    /* [in] */ const String& name)
{
    ECode ec = NOERROR;
    mNesting--;
    if (mInTag) {
        FAIL_RETURN((Append(String(" />\n"))));
    }
    else {
        if (mIndent && mLineStart) {
            FAIL_RETURN(AppendIndent(mNesting));
        }
        FAIL_RETURN(Append(String("</")));
        if (!ns.IsNull()) {
            FAIL_RETURN(Append(ns));
            FAIL_RETURN(Append(':'));
        }
        FAIL_RETURN(Append(name));
        FAIL_RETURN(Append(String(">\n")));
    }

    mLineStart = TRUE;
    mInTag = FALSE;
    return NOERROR;
}

ECode CFastXmlSerializer::WriteEntityRef(
    /* [in] */ const String& text)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CFastXmlSerializer::FlushBytes()
{
    Int32 position;
    IBuffer::Probe(mBytes)->GetPosition(&position);

    if (position > 0) {
        FAIL_RETURN(IBuffer::Probe(mBytes)->Flip());
        AutoPtr<ArrayOf<Byte> > bytes;
        mBytes->GetArray((ArrayOf<Byte>**)&bytes);

        FAIL_RETURN(mOutputStream->Write(bytes, 0, position));
        FAIL_RETURN(IBuffer::Probe(mBytes)->Clear());
    }
    return NOERROR;
}

ECode CFastXmlSerializer::Flush()
{
    //Log.i("PackageManager", "flush mPos=" + mPos);
    if (mPos > 0) {
        if (mOutputStream != NULL) {
            AutoPtr<ICharBufferHelper> helper;
            CCharBufferHelper::AcquireSingleton((ICharBufferHelper**)&helper);
            AutoPtr<ICharBuffer> charBuffer;
            helper->Wrap(mText, 0, mPos, (ICharBuffer**)&charBuffer);
            AutoPtr<ICoderResult> result;
            mCharset->Encode(charBuffer, mBytes, TRUE, (ICoderResult**)&result);
            Boolean bValue;
            while (TRUE) {
                if (result->IsError(&bValue), bValue) {
                    //throw new IOException(result.toString());
                    return E_IO_EXCEPTION;
                }
                else if (result->IsOverflow(&bValue), bValue) {
                    FAIL_RETURN(FlushBytes());
                    result = NULL;
                    mCharset->Encode(charBuffer, mBytes, TRUE, (ICoderResult**)&result);
                    continue;
                }
                break;
            }
            FAIL_RETURN(FlushBytes());
            IFlushable* flushable = IFlushable::Probe(mOutputStream);
            FAIL_RETURN(flushable->Flush());
        }
        else {
            FAIL_RETURN(mWriter->Write(mText, 0, mPos));
            IFlushable* flushable = IFlushable::Probe(mWriter);
            FAIL_RETURN(flushable->Flush());
        }
        mPos = 0;
    }
    return NOERROR;
}

ECode CFastXmlSerializer::GetDepth(
    /* [out] */ Int32* depth)
{
    VALIDATE_NOT_NULL(depth);
    *depth = 0;
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CFastXmlSerializer::GetFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CFastXmlSerializer::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = String(NULL);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CFastXmlSerializer::GetNamespace(
    /* [out] */ String* nsp)
{
    VALIDATE_NOT_NULL(nsp);
    *nsp = String(NULL);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CFastXmlSerializer::GetPrefix(
    /* [in] */ const String& ns,
    /* [in] */ Boolean generatePrefix,
    /* [out] */ String* prefix)
{
    VALIDATE_NOT_NULL(prefix);
    *prefix = String(NULL);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CFastXmlSerializer::GetProperty(
    /* [in] */ const String& name,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    *value = NULL;
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CFastXmlSerializer::WriteIgnorableWhitespace(
    /* [in] */ const String& text)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CFastXmlSerializer::WriteProcessingInstruction(
    /* [in] */ const String& text)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CFastXmlSerializer::SetFeature(
    /* [in] */ const String& name,
    /* [in] */ Boolean state)
{
    if (name.Equals("http://xmlpull.org/v1/doc/features.html#indent-output")) {
        mIndent = TRUE;
        return NOERROR;
    }
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CFastXmlSerializer::SetOutput(
    /* [in] */ IOutputStream* os,
    /* [in] */ const String& encoding)
{
    VALIDATE_NOT_NULL(os);
    if (TRUE) {
        // try {
        AutoPtr<ICharsetHelper> helper;
        CCharsetHelper::AcquireSingleton((ICharsetHelper**)&helper);
        AutoPtr<ICharset> charset;
        FAIL_RETURN(helper->ForName(encoding, (ICharset**)&charset));
        mCharset = NULL;
        FAIL_RETURN(charset->NewEncoder((ICharsetEncoder**)&mCharset));
        // } catch (IllegalCharsetNameException e) {
        //     throw (UnsupportedEncodingException) (new UnsupportedEncodingException(
        //             encoding).initCause(e));
        // } catch (UnsupportedCharsetException e) {
        //     throw (UnsupportedEncodingException) (new UnsupportedEncodingException(
        //             encoding).initCause(e));
        // }
        mOutputStream = os;
    }
    else {
        AutoPtr<IOutputStreamWriter> opsr;
        if (encoding.IsNull()) {
            COutputStreamWriter::New(os, (IOutputStreamWriter**)&opsr);
        }
        else {
            COutputStreamWriter::New(os, encoding, (IOutputStreamWriter**)&opsr);
        }

        return SetOutput(IWriter::Probe(opsr.Get()));
    }
    return NOERROR;
}

ECode CFastXmlSerializer::SetOutput(
    /* [in] */ IWriter* writer)
{
    mWriter = writer;
    return NOERROR;
}

ECode CFastXmlSerializer::SetPrefix(
    /* [in] */ const String& prefix,
    /* [in] */ const String& ns)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CFastXmlSerializer::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ IInterface* value)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CFastXmlSerializer::StartDocument(
   /* [in] */ const String& encoding,
   /* [in] */ Boolean standalone)
{
    StringBuilder sb("<?xml version='1.0' encoding='utf-8' standalone='");
    sb += (standalone ? "yes" : "no");
    sb += "' ?>\n";
    FAIL_RETURN(Append(sb.ToString()));
    mLineStart = TRUE;
    return NOERROR;
}

ECode CFastXmlSerializer::WriteStartTag(
    /* [in] */ const String& ns,
    /* [in] */ const String& name)
{
    if (mInTag) {
        FAIL_RETURN(Append(String(">\n")));
    }
    if (mIndent) {
        FAIL_RETURN(AppendIndent(mNesting));
    }
    mNesting++;
    FAIL_RETURN(Append('<'));
    if (!ns.IsNull()) {
        FAIL_RETURN(Append(ns));
        FAIL_RETURN(Append(':'));
    }
    FAIL_RETURN(Append(name));
    mInTag = TRUE;
    mLineStart = FALSE;
    return NOERROR;
}


ECode CFastXmlSerializer::WriteText(
    /* [in] */ ArrayOf<Char32>* buf,
    /* [in] */ Int32 start,
    /* [in] */ Int32 len)
{
    if (mInTag) {
        FAIL_RETURN(Append('>'));
        mInTag = FALSE;
    }
    FAIL_RETURN(EscapeAndAppendString(buf, start, len));
    if (mIndent) {
        mLineStart = (*buf)[start+len-1] == '\n';
    }
    return NOERROR;
}

ECode CFastXmlSerializer::WriteText(
    /* [in] */ const String& text)
{
    if (mInTag) {
        FAIL_RETURN(Append('>'));
        mInTag = FALSE;
    }
    FAIL_RETURN(EscapeAndAppendString(text));
    if (mIndent) {
        mLineStart = text.GetLength() > 0 && (text.GetChar(text.GetLength()-1) == '\n');
    }
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

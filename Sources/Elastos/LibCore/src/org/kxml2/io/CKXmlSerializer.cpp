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

#include "CKXmlSerializer.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include "CBufferedWriter.h"
#include "COutputStreamWriter.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CBufferedWriter;
using Elastos::IO::IOutputStreamWriter;
using Elastos::IO::COutputStreamWriter;
using Elastos::IO::IFlushable;
using Org::Xmlpull::V1::IXmlSerializer;
using Org::Xmlpull::V1::EIID_IXmlSerializer;

namespace Org {
namespace Kxml2 {
namespace IO {

const Int32 CKXmlSerializer::WRITE_BUFFER_SIZE;

CAR_INTERFACE_IMPL(CKXmlSerializer, Object, IXmlSerializer)

CAR_OBJECT_IMPL(CKXmlSerializer)

CKXmlSerializer::CKXmlSerializer()
    : mAuto(0)
    , mDepth(0)
    , mPending(FALSE)
    , mUnicode(FALSE)
{
    mElementStack = ArrayOf<String>::Alloc(12);
    mNspCounts = ArrayOf<Int32>::Alloc(4);
    mNspStack = ArrayOf<String>::Alloc(8);
    mIndent = ArrayOf<Boolean>::Alloc(4);
}

CKXmlSerializer::~CKXmlSerializer()
{
    mWriter = NULL;
}

ECode CKXmlSerializer::Check(
    /* [in] */ Boolean close)
{
    if (!mPending) {
        return NOERROR;
    }

    mDepth++;
    mPending = FALSE;

    if (mIndent->GetLength() <= mDepth) {
        AutoPtr<ArrayOf<Boolean> > hlp = ArrayOf<Boolean>::Alloc(mDepth + 4);
        hlp->Copy(mIndent);
        mIndent = hlp;
    }
    (*mIndent)[mDepth] = (*mIndent)[mDepth - 1];

    for (Int32 i = (*mNspCounts)[mDepth - 1]; i < (*mNspCounts)[mDepth]; i++) {
        String ns;
        FAIL_RETURN(IWriter::Probe(mWriter)->Write(' '));
        FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("xmlns")));
        if (!(*mNspStack)[i * 2].IsEmpty()) {
            FAIL_RETURN(IWriter::Probe(mWriter)->Write(':'));
            FAIL_RETURN(IWriter::Probe(mWriter)->Write((*mNspStack)[i * 2]));
        }
        else if ((GetNamespace(&ns), ns.IsEmpty()) && !(*mNspStack)[i * 2 + 1].IsEmpty()) {
//            throw new IllegalStateException("Cannot set default namespace for elements in no namespace");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("=\"")));
        FAIL_RETURN(WriteEscaped((*mNspStack)[i * 2 + 1], '"'));
        FAIL_RETURN(IWriter::Probe(mWriter)->Write('"'));
    }

    if (mNspCounts->GetLength() <= mDepth + 1) {
        AutoPtr<ArrayOf<Int32> > hlp = ArrayOf<Int32>::Alloc(mDepth + 8);
        hlp->Copy(mNspCounts);
        mNspCounts = hlp;
    }

    (*mNspCounts)[mDepth + 1] = (*mNspCounts)[mDepth];
    //   nspCounts[depth + 2] = nspCounts[depth];

    return IWriter::Probe(mWriter)->Write(close ? String(" />") : String(">"));
}

ECode CKXmlSerializer::WriteEscaped(
    /* [in] */ const String& s,
    /* [in] */ Int32 quot)
{
    AutoPtr<ArrayOf<Char32> > charArray = s.GetChars();
    Int32 N = charArray->GetLength();
    Char32 c;
    for (Int32 i = 0; i < N; i++) {
        c = (*charArray)[i];
        switch (c) {
            case '\n':
            case '\r':
            case '\t':
                if(quot == -1) {
                    FAIL_RETURN(IWriter::Probe(mWriter)->Write(c));
                }
                else {
                    StringBuilder sb;
                    sb.Append(String("&#"));
                    sb.AppendChar(c);
                    sb.Append(String(";"));
                    String str = sb.ToString();
                    FAIL_RETURN(IWriter::Probe(mWriter)->Write(str));
                }
                break;
            case '&' :
                FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("&amp;")));
                break;
            case '>' :
                FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("&gt;")));
                break;
            case '<' :
                FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("&lt;")));
                break;
            default :
                if (c == (Char32)quot) {
                    FAIL_RETURN(IWriter::Probe(mWriter)->Write(
                        c == '"' ? String("&quot;") : String("&apos;")));
                    break;
                }

                // BEGIN android-changed: refuse to output invalid characters
                // See http://www.w3.org/TR/REC-xml/#charsets for definition.
                // No other Java XML mWriter we know of does this, but no Java
                // XML reader we know of is able to parse the bad output we'd
                // otherwise generate.
                // Note: tab, newline, and carriage return have already been
                // handled above.
                Boolean valid = (c >= 0x20 && c <= 0xd7ff) || (c >= 0xe000 && c <= 0xfffd);
                if (!valid) {
                    return ReportInvalidCharacter(c);
                }
                if (mUnicode || c < 127) {
                    FAIL_RETURN(IWriter::Probe(mWriter)->Write(c));
                }
                else {
                    StringBuilder sb;
                    sb.Append(String("&#"));
                    sb.AppendChar(c);
                    sb.Append(String(";"));
                    String str = sb.ToString();
                    FAIL_RETURN(IWriter::Probe(mWriter)->Write(str));
                }
                // END android-changed
        }
    }
    return NOERROR;
}

// BEGIN android-added
ECode CKXmlSerializer::ReportInvalidCharacter(
    /* [in] */ Char32 ch)
{
//    throw new IllegalArgumentException("Illegal character (" + Integer.toHexString((int) ch) + ")");
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}
// END android-added

ECode CKXmlSerializer::WriteDocDecl(
    /* [in] */ const String& dd)
{
    FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("<!DOCTYPE")));
    FAIL_RETURN(IWriter::Probe(mWriter)->Write(dd));
    return IWriter::Probe(mWriter)->Write(String(">"));
}

ECode CKXmlSerializer::EndDocument()
{
    while (mDepth > 0) {
        FAIL_RETURN(WriteEndTag(
            (*mElementStack)[mDepth * 3 - 3],
            (*mElementStack)[mDepth * 3 - 1]));
    }
    return Flush();
}

ECode CKXmlSerializer::WriteEntityRef(
    /* [in] */ const String& text)
{
    FAIL_RETURN(Check(FALSE));
    FAIL_RETURN(IWriter::Probe(mWriter)->Write('&'));
    FAIL_RETURN(IWriter::Probe(mWriter)->Write(text));
    return IWriter::Probe(mWriter)->Write(';');
}

ECode CKXmlSerializer::GetFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    //return false;
    name.Equals("http://xmlpull.org/v1/doc/features.html#indent-output")
        ? *value = (*mIndent)[mDepth]
        : *value = FALSE;
    return NOERROR;
}

ECode CKXmlSerializer::GetPrefix(
    /* [in] */ const String& ns,
    /* [in] */ Boolean generatePrefix,
    /* [out] */ String* prefix)
{
    VALIDATE_NOT_NULL(prefix);

//    try {
    if (FAILED(GetPrefix(ns, FALSE, generatePrefix, prefix))) {
//        throw new RuntimeException(e.toString());
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
//    }
//    catch (IOException e) {
//        throw new RuntimeException(e.toString());
//    }
}

ECode CKXmlSerializer::GetPrefix(
    /* [in] */ const String& ns,
    /* [in] */ Boolean includeDefault,
    /* [in] */ Boolean create,
    /* [out] */ String* prefix)
{
    assert(prefix != NULL);

    for (Int32 i = (*mNspCounts)[mDepth + 1] * 2 - 2; i >= 0; i -= 2) {
        if ((*mNspStack)[i + 1].Equals(ns)
            && (includeDefault || !(*mNspStack)[i].IsEmpty())) {
            String cand = (*mNspStack)[i];
            for (Int32 j = i + 2; j < (*mNspCounts)[mDepth + 1] * 2; j++) {
                if ((*mNspStack)[j].Equals(cand)) {
                    cand = NULL;
                    break;
                }
            }
            if (!cand.IsNull()) {
                *prefix = cand;
                return NOERROR;
            }
        }
    }

    if (!create) {
        *prefix = NULL;
        return NOERROR;
    }

    String _prefix;

    if (ns.IsEmpty()) {
        _prefix = "";
    }
    else {
        do {
            _prefix = String::Format("n%d", (mAuto++));
            for (Int32 i = (*mNspCounts)[mDepth + 1] * 2 - 2; i >= 0; i -= 2) {
                if (_prefix.Equals((*mNspStack)[i])) {
                    _prefix = NULL;
                    break;
                }
            }
        }
        while (_prefix.IsNull());
    }

    Boolean p = mPending;
    mPending = FALSE;
    FAIL_RETURN(SetPrefix(_prefix, ns));
    mPending = p;
    *prefix = _prefix;
    return NOERROR;
}

ECode CKXmlSerializer::GetProperty(
    /* [in] */ const String& name,
    /* [out] */ IInterface** value)
{
//    throw new RuntimeException("Unsupported property");
    return E_RUNTIME_EXCEPTION;
}

ECode CKXmlSerializer::WriteIgnorableWhitespace(
    /* [in] */ const String& text)
{
    return WriteText(text);
}

ECode CKXmlSerializer::SetFeature(
    /* [in] */ const String& name,
    /* [in] */ Boolean state)
{
    if (name.Equals("http://xmlpull.org/v1/doc/features.html#indent-output")) {
        (*mIndent)[mDepth] = state;
        return NOERROR;
    }
    else {
//        throw new RuntimeException("Unsupported Feature");
        return E_RUNTIME_EXCEPTION;
    }
}

ECode CKXmlSerializer::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ IInterface* value)
{
//    throw new RuntimeException(
//            "Unsupported Property:" + value);
    return E_RUNTIME_EXCEPTION;
}

ECode CKXmlSerializer::SetPrefix(
    /* [in] */ const String& _prefix,
    /* [in] */ const String& _ns)
{
    String prefix = _prefix;
    String ns = _ns;
    FAIL_RETURN(Check(FALSE));
    if (prefix.IsNull()) {
        prefix = "";
    }
    if (ns.IsNull()) {
        ns = "";
    }

    String def;
    FAIL_RETURN(GetPrefix(ns, TRUE, FALSE, &def));

    // boil out if already defined

    if (prefix.Equals(def)) {
        return NOERROR;
    }

    Int32 pos = ((*mNspCounts)[mDepth + 1]++) << 1;

    if (mNspStack->GetLength() < pos + 1) {
        AutoPtr<ArrayOf<String> > hlp = ArrayOf<String>::Alloc(mNspStack->GetLength() + 16);
        hlp->Copy(mNspStack, pos);
        mNspStack = hlp;
    }

    (*mNspStack)[pos++] = prefix;
    (*mNspStack)[pos] = ns;
    return NOERROR;
}

ECode CKXmlSerializer::SetOutput(
    /* [in] */ IWriter* writer)
{
    // BEGIN android-changed
    // Guarantee that the mWriter is always buffered.
    if (IBufferedWriter::Probe(writer) != NULL) {
        mWriter = IBufferedWriter::Probe(writer);
    }
    else {
        mWriter = NULL;
        CBufferedWriter::New(writer, WRITE_BUFFER_SIZE, (IBufferedWriter**)&mWriter);
    }
    // END android-changed

    // elementStack = new String[12]; //nsp/prefix/name
    //nspCounts = new int[4];
    //nspStack = new String[8]; //prefix/nsp
    //indent = new boolean[4];

    (*mNspCounts)[0] = 2;
    (*mNspCounts)[1] = 2;
    (*mNspStack)[0] = "";
    (*mNspStack)[1] = "";
    (*mNspStack)[2] = "xml";
    (*mNspStack)[3] = "http://www.w3.org/XML/1998/namespace";
    mPending = FALSE;
    mAuto = 0;
    mDepth = 0;

    mUnicode = FALSE;
    return NOERROR;
}

ECode CKXmlSerializer::SetOutput(
    /* [in] */ IOutputStream* os,
    /* [in] */ const String& encoding)
{
    if (os == NULL) {
//        throw new IllegalArgumentException("os == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IWriter> writer;
    if (encoding.IsNull()) {
        COutputStreamWriter::New(os, (IWriter**)&writer);
    }
    else {
        COutputStreamWriter::New(os, encoding, (IWriter**)&writer);
    }
    SetOutput(writer);
    mEncoding = encoding;
    if (!encoding.IsNull()
        && encoding.StartWithIgnoreCase("utf")) { /*encoding.ToLowerCase(Locale.US).StartWith("utf")*/
        mUnicode = TRUE;
    }
    return NOERROR;
}

ECode CKXmlSerializer::StartDocument(
    /* [in] */ const String& encoding,
    /* [in] */ Boolean standalone)
{
    FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("<?xml version='1.0' ")));

    if (!encoding.IsNull()) {
        mEncoding = encoding;
        if (encoding.StartWithIgnoreCase("utf")) { /*encoding.ToLowerCase(Locale.US).StartWith("utf")*/
            mUnicode = TRUE;
        }
    }

    if (!mEncoding.IsNull()) {
        FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("encoding='")));
        FAIL_RETURN(IWriter::Probe(mWriter)->Write(mEncoding));
        FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("' ")));
    }

    FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("standalone='")));
    FAIL_RETURN(IWriter::Probe(mWriter)->Write(
        standalone ? String("yes") : String("no")));
    FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("' ")));

    return IWriter::Probe(mWriter)->Write(String("?>"));
}

ECode CKXmlSerializer::WriteStartTag(
    /* [in] */ const String& ns,
    /* [in] */ const String& name)
{
    FAIL_RETURN(Check(FALSE));

    //        if (namespace == null)
    //            namespace = "";

    if ((*mIndent)[mDepth]) {
        FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("\r\n")));
        for (Int32 i = 0; i < mDepth; i++) {
            FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("  ")));
        }
    }

    Int32 esp = mDepth * 3;

    if (mElementStack->GetLength() < esp + 3) {
        AutoPtr<ArrayOf<String> > hlp = ArrayOf<String>::Alloc(mElementStack->GetLength() + 12);
        hlp->Copy(mNspStack, esp);
        mElementStack = hlp;
    }

    String prefix("");
    if (!ns.IsNull()) {
        FAIL_RETURN(GetPrefix(ns, TRUE, TRUE, &prefix));
    }

    if (!ns.IsNull() && ns.IsEmpty()) {
        for (Int32 i = (*mNspCounts)[mDepth]; i < (*mNspCounts)[mDepth + 1]; i++) {
            if ((*mNspStack)[i * 2].IsEmpty() && !(*mNspStack)[i * 2 + 1].IsEmpty()) {
//                throw new IllegalStateException("Cannot set default namespace for elements in no namespace");
                return E_ILLEGAL_STATE_EXCEPTION;
            }
        }
    }

    (*mElementStack)[esp++] = ns;
    (*mElementStack)[esp++] = prefix;
    (*mElementStack)[esp] = name;

    FAIL_RETURN(IWriter::Probe(mWriter)->Write('<'));
    if (!prefix.IsEmpty()) {
        FAIL_RETURN(IWriter::Probe(mWriter)->Write(prefix));
        FAIL_RETURN(IWriter::Probe(mWriter)->Write(':'));
    }

    FAIL_RETURN(IWriter::Probe(mWriter)->Write(name));

    mPending = TRUE;

    return NOERROR;
}

ECode CKXmlSerializer::WriteAttribute(
    /* [in] */ const String& _ns,
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    String ns = _ns;
    if (!mPending) {
//        throw new IllegalStateException("illegal position for attribute");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    //        int cnt = nspCounts[depth];

    if (ns.IsNull()) {
        ns = "";
    }

    //        depth--;
    //        pending = false;

    String prefix("");
    if (!ns.IsEmpty()) {
        FAIL_RETURN(GetPrefix(ns, FALSE, TRUE, &prefix));
    }

    //        pending = true;
    //        depth++;

    /*        if (cnt != nspCounts[depth]) {
                mWriter.write(' ');
                mWriter.write("xmlns");
                if (nspStack[cnt * 2] != null) {
                    mWriter.write(':');
                    mWriter.write(nspStack[cnt * 2]);
                }
                mWriter.write("=\"");
                writeEscaped(nspStack[cnt * 2 + 1], '"');
                mWriter.write('"');
            }
            */

    FAIL_RETURN(IWriter::Probe(mWriter)->Write(' '));
    if (!prefix.IsEmpty()) {
        FAIL_RETURN(IWriter::Probe(mWriter)->Write(prefix));
        FAIL_RETURN(IWriter::Probe(mWriter)->Write(':'));
    }
    FAIL_RETURN(IWriter::Probe(mWriter)->Write(name));
    FAIL_RETURN(IWriter::Probe(mWriter)->Write('='));
    Char32 q = value.IndexOf('"') == -1 ? '"' : '\'';
    FAIL_RETURN(IWriter::Probe(mWriter)->Write(q));
    FAIL_RETURN(WriteEscaped(value, q));
    return IWriter::Probe(mWriter)->Write(q);
}

ECode CKXmlSerializer::Flush()
{
    FAIL_RETURN(Check(FALSE));
    assert(IFlushable::Probe(mWriter));
    return IFlushable::Probe(mWriter)->Flush();
}

ECode CKXmlSerializer::WriteEndTag(
    /* [in] */ const String& ns,
    /* [in] */ const String& name)
{
    if (!mPending) {
        mDepth--;
    }
    //        if (namespace == null)
    //          namespace = "";

    if ((ns.IsNull()
        && !(*mElementStack)[mDepth * 3].IsNull())
        || (!ns.IsNull()
            && !ns.Equals((*mElementStack)[mDepth * 3]))
        || !(*mElementStack)[mDepth * 3 + 2].Equals(name)) {
//        throw new IllegalArgumentException("</{"+namespace+"}"+name+"> does not match start");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mPending) {
        FAIL_RETURN(Check(TRUE));
        mDepth--;
    }
    else {
        if ((*mIndent)[mDepth + 1]) {
            FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("\r\n")));
            for (Int32 i = 0; i < mDepth; i++) {
                FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("  ")));
            }
        }

        FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("</")));
        String prefix = (*mElementStack)[mDepth * 3 + 1];
        if (!prefix.IsEmpty()) {
            FAIL_RETURN(IWriter::Probe(mWriter)->Write(prefix));
            FAIL_RETURN(IWriter::Probe(mWriter)->Write(':'));
        }
        FAIL_RETURN(IWriter::Probe(mWriter)->Write(name));
        FAIL_RETURN(IWriter::Probe(mWriter)->Write('>'));
    }

    (*mNspCounts)[mDepth + 1] = (*mNspCounts)[mDepth];
    return NOERROR;
}

ECode CKXmlSerializer::GetNamespace(
    /* [out] */ String* ns)
{
    VALIDATE_NOT_NULL(ns);
    Int32 depth;
    GetDepth(&depth);
    *ns = depth == 0 ? String(NULL) : (*mElementStack)[depth * 3 - 3];
    return NOERROR;
}

ECode CKXmlSerializer::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    Int32 depth;
    GetDepth(&depth);
    *name = depth == 0 ? String(NULL) : (*mElementStack)[depth * 3 - 1];
    return NOERROR;
}

ECode CKXmlSerializer::GetDepth(
    /* [out] */ Int32* depth)
{
    VALIDATE_NOT_NULL(depth);
    *depth = mPending ? mDepth + 1 : mDepth;
    return NOERROR;
}

ECode CKXmlSerializer::WriteText(
    /* [in] */ const String& text)
{
    FAIL_RETURN(Check(FALSE));
    (*mIndent)[mDepth] = FALSE;
    return WriteEscaped(text, -1);
}

ECode CKXmlSerializer::WriteText(
    /* [in] */ ArrayOf<Char32>* buf,
    /* [in] */ Int32 start,
    /* [in] */ Int32 len)
{
    String str = String(*buf, start, len);
    return WriteText(str);
}

ECode CKXmlSerializer::WriteCdSect(
    /* [in] */ const String& text)
{
    FAIL_RETURN(Check(FALSE));
    // BEGIN android-changed: ]]> is not allowed within a CDATA,
    // so break and start a new one when necessary.
    String data;
    StringUtils::ReplaceAll(text, String("]]>"), String("]]]]><![CDATA[>"), &data);
    // We also aren't allowed any invalid characters.
    AutoPtr<ArrayOf<Char32> > charArray = data.GetChars();
    Char32 ch;
    Boolean valid;
    Int32 N = charArray->GetLength();
    for (Int32 i = 0; i < N; i++) {
        ch = (*charArray)[i];
        valid = (ch >= 0x20 && ch <= 0xd7ff) ||
            (ch == '\t' || ch == '\n' || ch == '\r') ||
            (ch >= 0xe000 && ch <= 0xfffd);
        if (!valid) {
            return ReportInvalidCharacter(ch);
        }
    }
    FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("<![CDATA[")));
    FAIL_RETURN(IWriter::Probe(mWriter)->Write(data));
    return IWriter::Probe(mWriter)->Write(String("]]>"));
    // END android-changed
}

ECode CKXmlSerializer::WriteComment(
    /* [in] */ const String& text)
{
    FAIL_RETURN(Check(FALSE));
    FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("<!--")));
    FAIL_RETURN(IWriter::Probe(mWriter)->Write(text));
    return IWriter::Probe(mWriter)->Write(String("-->"));
}

ECode CKXmlSerializer::WriteProcessingInstruction(
    /* [in] */ const String& text)
{
    FAIL_RETURN(Check(FALSE));
    FAIL_RETURN(IWriter::Probe(mWriter)->Write(String("<?")));
    FAIL_RETURN(IWriter::Probe(mWriter)->Write(text));
    return IWriter::Probe(mWriter)->Write(String("?>"));
}

} //namespace IO
} //namespace Kxml2
} //namespace Org

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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CFASTXMLSERIALIZER_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CFASTXMLSERIALIZER_H__

#include <Elastos.CoreLibrary.IO.h>
#include "_Elastos_Droid_Internal_Utility_CFastXmlSerializer.h"
#include <elastos/core/Object.h>

using Elastos::Core::IBoolean;
using Elastos::IO::IWriter;
using Elastos::IO::IOutputStream;
using Elastos::IO::IByteBuffer;
using Elastos::IO::Charset::ICharsetEncoder;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CFastXmlSerializer)
    , public Object
    , public IFastXmlSerializer
    , public IXmlSerializer
{
public:
    CFastXmlSerializer();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI SetFeature(
        /* [in] */ const String& name,
        /* [in] */ Boolean state);

    CARAPI GetFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean* value);

    CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ IInterface* value);

    CARAPI GetProperty(
        /* [in] */ const String& name,
        /* [out] */ IInterface** value);

    CARAPI SetOutput(
        /* [in] */ IOutputStream* os,
        /* [in] */ const String& encoding);

    CARAPI SetOutput(
        /* [in] */ IWriter* writer);

    CARAPI StartDocument(
       /* [in] */ const String& encoding,
       /* [in] */ Boolean standalone);

    CARAPI EndDocument();

    CARAPI SetPrefix(
        /* [in] */ const String& prefix,
        /* [in] */ const String& ns);

    CARAPI GetPrefix(
        /* [in] */ const String& ns,
        /* [in] */ Boolean generatePrefix,
        /* [out] */ String* prefix);

    CARAPI GetDepth(
        /* [out] */ Int32* depth);

    CARAPI GetNamespace(
        /* [out] */ String* nsp);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI WriteStartTag(
        /* [in] */ const String& ns,
        /* [in] */ const String& name);

    CARAPI WriteAttribute(
        /* [in] */ const String& ns,
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI WriteEndTag(
        /* [in] */ const String& ns,
        /* [in] */ const String& name);

    CARAPI WriteText(
        /* [in] */ const String& text);

    CARAPI WriteText(
        /* [in] */ ArrayOf<Char32>* buf,
        /* [in] */ Int32 start,
        /* [in] */ Int32 len);

    CARAPI WriteCdSect(
        /* [in] */ const String& text);

    CARAPI WriteEntityRef(
        /* [in] */ const String& text);

    CARAPI WriteProcessingInstruction(
        /* [in] */ const String& text);

    CARAPI WriteComment(
        /* [in] */ const String& text);

    CARAPI WriteDocDecl(
        /* [in] */ const String& text);

    CARAPI WriteIgnorableWhitespace(
        /* [in] */ const String& text);

    CARAPI Flush();

private:
    CARAPI Append(
        /* [in] */ Char32 ch);

    CARAPI Append(
        /* [in] */ const String& str,
        /* [in] */ Int32 i,
        /* [in] */ Int32 length);

    CARAPI Append(
        /* [in] */ ArrayOf<Char32>* buf,
        /* [in] */ Int32 i,
        /* [in] */ Int32 length);

    CARAPI Append(
        /* [in] */ const String& str);

    CARAPI AppendIndent(
        /* [in] */ Int32 indent);

    CARAPI EscapeAndAppendString(
        /* [in] */ const String& str);

    CARAPI EscapeAndAppendString(
        /* [in] */ ArrayOf<Char32>* buf,
        /* [in] */ Int32 start,
        /* [in] */ Int32 length);

    CARAPI FlushBytes();

private:
    static const AutoPtr<ArrayOf<String> > ESCAPE_TABLE;
    static const Int32 BUFFER_LEN;

    static String sSpace;

    AutoPtr<ArrayOf<Char32> > mText;

    Int32 mPos;

    AutoPtr<IWriter> mWriter;

    AutoPtr<IOutputStream> mOutputStream;
    AutoPtr<ICharsetEncoder> mCharset;
    AutoPtr<IByteBuffer> mBytes;

    Boolean mIndent;
    Boolean mInTag;

    Int32 mNesting;
    Boolean mLineStart;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CFASTXMLSERIALIZER_H__

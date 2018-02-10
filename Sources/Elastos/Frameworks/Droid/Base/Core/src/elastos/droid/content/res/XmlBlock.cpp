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

#include "elastos/droid/content/res/XmlBlock.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include <elastos/core/Math.h>
#include <elastos/core/Character.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>
#include <androidfw/ResourceTypes.h>

using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Utility::EIID_IAttributeSet;
using Elastos::Core::StringUtils;
using Elastos::Core::Character;
using Elastos::IO::EIID_ICloseable;
using Elastos::Utility::Logging::Slogger;
using Org::Xmlpull::V1::EIID_IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

//=================================================================================
// XmlBlock::Parser
//=================================================================================
CAR_INTERFACE_IMPL_4(XmlBlock::Parser, Object, IXmlResourceParser, IXmlPullParser, IAttributeSet, ICloseable)

XmlBlock::Parser::Parser(
    /* [in] */ Int64 parseState,
    /* [in] */ XmlBlock* block)
    : mParseState(parseState)
    , mHost(block)
    , mStarted(FALSE)
    , mDecNextDepth(FALSE)
    , mDepth(0)
    , mEventType(START_DOCUMENT)
{
    assert(block);
    block->mOpenCount++;
}

XmlBlock::Parser::~Parser()
{
    Close();
}

ECode XmlBlock::Parser::SetFeature(
    /* [in] */ const String& name,
    /* [in] */ Boolean state)
{
    if (FEATURE_PROCESS_NAMESPACES.Equals(name) && state) {
        return NOERROR;
    }
    if (FEATURE_REPORT_NAMESPACE_ATTRIBUTES.Equals(name) && state) {
        return NOERROR;
    }
    Slogger::E(TAG, "Unsupported feature: %s" , (const char*)name);
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlBlock::Parser::GetFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    if (FEATURE_PROCESS_NAMESPACES.Equals(name)) {
        *value = TRUE;
        return NOERROR;
    }
    if (FEATURE_REPORT_NAMESPACE_ATTRIBUTES.Equals(name)) {
        *value = TRUE;
        return NOERROR;
    }
    *value = FALSE;
    return NOERROR;
}

ECode XmlBlock::Parser::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ IInterface* value)
{
    Slogger::E("XmlBlock::Parser", "setProperty() not supported");
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlBlock::Parser::GetProperty(
    /* [in] */ const String& name,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    *value = NULL;
    return NOERROR;
}

ECode XmlBlock::Parser::SetInput(
    /* [in]*/ IReader* reader)
{
    Slogger::E("XmlBlock::Parser", "setInput() not supported");
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlBlock::Parser::SetInput(
    /* [in] */ IInputStream * inputStream,
    /* [in] */ const String& inputEncoding)
{
    Slogger::E("XmlBlock::Parser", "setInput() not supported");
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlBlock::Parser::DefineEntityReplacementText(
    /* [in] */ const String& entityName,
    /* [in] */ const String& replacementText)
{
    Slogger::E("XmlBlock::Parser", "defineEntityReplacementText() not supported");
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlBlock::Parser::GetNamespacePrefix(
    /* [in] */ Int32 pos,
    /* [out] */ String* prefix)
{
    Slogger::E("XmlBlock::Parser", "getNamespacePrefix() not supported");
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlBlock::Parser::GetInputEncoding(
    /* [out] */ String* inputEncoding)
{
    VALIDATE_NOT_NULL(inputEncoding);
    *inputEncoding = NULL;
    return NOERROR;
}

ECode XmlBlock::Parser::GetNamespace(
    /* [in] */ const String& prefix,
    /* [out] */ String* ns)
{
    Slogger::E("XmlBlock::Parser", "getNamespace() not supported");
    return E_RUNTIME_EXCEPTION;
}

ECode XmlBlock::Parser::GetNamespaceCount(
    /* [in] */ Int32 depth,
    /* [out] */ Int32* count)
{
    Slogger::E("XmlBlock::Parser", "getNamespaceCount() not supported");
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlBlock::Parser::GetPositionDescription(
    /* [out] */ String* des)
{
    VALIDATE_NOT_NULL(des);
    Int32 line = 0;
    GetLineNumber(&line);
    *des = String("Binary XML file line #") + StringUtils::ToString(line);
    return NOERROR;
}

ECode XmlBlock::Parser::GetNamespaceUri(
    /* [in] */ Int32 pos,
    /* [out] */ String* uri)
{
    Slogger::E("XmlBlock::Parser", "getNamespaceUri() not supported");
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlBlock::Parser::GetColumnNumber(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = -1;
    return NOERROR;
}

ECode XmlBlock::Parser::GetDepth(
    /* [out] */ Int32* depth)
{
    VALIDATE_NOT_NULL(depth);
    *depth = mDepth;
    return NOERROR;
}

ECode XmlBlock::Parser::GetText(
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text);
    *text = String(NULL);

    Int32 id = mHost->NativeGetText(mParseState);
    if (id >= 0) {
        AutoPtr<ICharSequence> cs = mHost->mStrings->Get(id);
        return cs->ToString(text);
    }

    return NOERROR;
}

ECode XmlBlock::Parser::GetLineNumber(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = mHost->NativeGetLineNumber(mParseState);
    return NOERROR;
}

ECode XmlBlock::Parser::GetEventType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mEventType;
    return NOERROR;
}

ECode XmlBlock::Parser::IsWhitespace(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // whitespace was stripped by aapt.
    *result = FALSE;
    return NOERROR;
}

ECode XmlBlock::Parser::GetPrefix(
    /* [out] */ String* prefix)
{
    Slogger::E("XmlBlock::Parser", "getPrefix not supported");
    return E_RUNTIME_EXCEPTION;
}

ECode XmlBlock::Parser::GetTextCharacters(
    /* [out] */ ArrayOf<Int32>* holderForStartAndLength,
    /* [out, callee] */ ArrayOf<Char32>** textChars)
{
    VALIDATE_NOT_NULL(textChars)
    *textChars = NULL;
    VALIDATE_NOT_NULL(holderForStartAndLength)

    String txt;
    GetText(&txt);
    AutoPtr< ArrayOf<Char32> > chars;
    if (!txt.IsNull()) {
        (*holderForStartAndLength)[0] = 0;
        (*holderForStartAndLength)[1] = txt.GetLength();
        chars = txt.GetChars();
    }
    *textChars = chars;
    REFCOUNT_ADD(*textChars);
    return NOERROR;
}

ECode XmlBlock::Parser::GetNamespace(
    /* [out] */ String* ns)
{
    VALIDATE_NOT_NULL(ns);
    *ns = String("");

    Int32 id = NativeGetNamespace(mParseState);
    if (id >= 0) {
        AutoPtr<ICharSequence> cs = mHost->mStrings->Get(id);
        return cs->ToString(ns);
    }

    return NOERROR;
}

ECode XmlBlock::Parser::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = String (NULL);

    Int32 id = NativeGetName(mParseState);
    if (id >= 0) {
        AutoPtr<ICharSequence> cs = mHost->mStrings->Get(id);
        return cs->ToString(name);
    }

    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeNamespace(
    /* [in] */ Int32 index,
    /* [out] */ String* attrNamespace)
{
    VALIDATE_NOT_NULL(attrNamespace);
    *attrNamespace = String(NULL);

    Int32 id = NativeGetAttributeNamespace(mParseState, index);
    // if (DEBUG) System.out.println("getAttributeNamespace of " + index + " = " + id);
    if (id >= 0) {
        AutoPtr<ICharSequence> cs = mHost->mStrings->Get(id);
        return cs->ToString(attrNamespace);
    }
    else if (id == -1) {
        *attrNamespace = "";
        return NOERROR;
    }

    Slogger::E("XmlBlock::Parser", "getAttributeNamespace of index %d = %d", index, id);
    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode XmlBlock::Parser::GetAttributeName(
    /* [in] */ Int32 index,
    /* [out] */ String* attrName)
{
    VALIDATE_NOT_NULL(attrName);
    *attrName = String(NULL);

    Int32 id = NativeGetAttributeName(mParseState, index);
    // if (DEBUG) System.out.println("getAttributeName of " + index + " = " + id);
    if (id >= 0) {
        AutoPtr<ICharSequence> cs = mHost->mStrings->Get(id);
        return cs->ToString(attrName);
    }
    Slogger::E("XmlBlock::Parser", "getAttributeName of index %d = %d", index, id);
    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode XmlBlock::Parser::GetAttributePrefix(
    /* [in] */ Int32 index,
    /* [out] */ String* attrPrefix)
{
    Slogger::E("XmlBlock::Parser", "getAttributePrefix not supported");
    return E_RUNTIME_EXCEPTION;
}

ECode XmlBlock::Parser::IsEmptyElementTag(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // XXX Need to detect this.
    *result = FALSE;
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeCount(
    /* [out] */ Int32* attrCount)
{
    VALIDATE_NOT_NULL(attrCount);
    *attrCount = mEventType == START_TAG ? mHost->NativeGetAttributeCount(mParseState) : -1;
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeValue(
    /* [in] */ Int32 index,
    /* [out] */ String* attrValue)
{
    VALIDATE_NOT_NULL(attrValue);

    Int32 id = mHost->NativeGetAttributeStringValue(mParseState, index);
    // if (DEBUG) System.out.println("getAttributeValue of " + index + " = " + id);
    if (id >= 0) {
        AutoPtr<ICharSequence> csq = mHost->mStrings->Get(id);
        return csq->ToString(attrValue);
    }

    // May be some other type...  check and try to convert if so.
    Int32 t = mHost->NativeGetAttributeDataType(mParseState, index);
    if (t == ITypedValue::TYPE_NULL) {
        Slogger::E("XmlBlock::Parser", "%d", index);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 v = mHost->NativeGetAttributeData(mParseState, index);
    *attrValue = CTypedValue::CoerceToString(t, v);
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeType(
    /* [in] */ Int32 index,
    /* [out] */ String* attrType)
{
    VALIDATE_NOT_NULL(attrType);
    *attrType = "CDATA";
    return NOERROR;
}

ECode XmlBlock::Parser::IsAttributeDefault(
    /* [in] */ Int32 index,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode XmlBlock::Parser::NextToken(
    /* [out] */ Int32* token)
{
    VALIDATE_NOT_NULL(token);
    return Next(token);
}

ECode XmlBlock::Parser::GetAttributeValue(
    /* [in] */ const String& ns,
    /* [in] */ const String& name,
    /* [out] */ String* attrValue)
{
    VALIDATE_NOT_NULL(attrValue);

    Int32 idx = mHost->NativeGetAttributeIndex(mParseState, ns, name);
    if (idx >= 0) {
          // if (DEBUG) System.out.println("getAttributeName of "
          //           + namespace + ":" + name + " index = " + idx);
          // if (DEBUG) System.out.println(
          //           "Namespace=" + getAttributeNamespace(idx)
          //           + "Name=" + getAttributeName(idx)
          //           + ", Value=" + getAttributeValue(idx));
        return GetAttributeValue(idx, attrValue);
    }
    *attrValue = NULL;
    return NOERROR;
}

ECode XmlBlock::Parser::Next(
    /* [out] */ Int32* event)
{
    VALIDATE_NOT_NULL(event);

    if (!mStarted) {
        mStarted = TRUE;
        *event = START_DOCUMENT;
        return NOERROR;
    }
    if (mParseState == 0) {
        *event = END_DOCUMENT;
        return NOERROR;
    }
    Int32 ev = mHost->NativeNext(mParseState);
    if (mDecNextDepth) {
        mDepth--;
        mDecNextDepth = FALSE;
    }
    switch (ev) {
        case START_TAG:
            mDepth++;
            break;
        case END_TAG:
            mDecNextDepth = TRUE;
            break;
    }
    mEventType = ev;
    if (ev == END_DOCUMENT) {
        // Automatically close the parse when we reach the end of
        // a document, since the standard XmlPullParser interface
        // doesn't have such an API so most clients will leave us
        // dangling.
        Close();
    }
    *event = ev;
    return NOERROR;
}

ECode XmlBlock::Parser::Require(
    /* [in] */ Int32 type,
    /* [in] */ const String& ns,
    /* [in] */ const String& name)
{
    Int32 t;
    String n1, n2;
    if ((GetEventType(&t), type != t)
        || (!ns.IsNull() && (GetNamespace(&n1), !ns.Equals(n1)))
        || (!name.IsNull() && (GetName(&n2), !name.Equals(n2)))) {
        String des;
        GetPositionDescription(&des);
        Slogger::E("XmlBlock::Parser", "expected %s" , /*IXmlPullParser::TYPES[type] + */ (const char*)des);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    return NOERROR;
}

ECode XmlBlock::Parser::NextText(
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text);

    Int32 t;
    if(GetEventType(&t), t != START_TAG) {
        String des;
        GetPositionDescription(&des);
        Slogger::E("XmlBlock::Parser", "%s: parser must be on START_TAG to read next text %p",
                (const char*)des, this);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    Int32 eventType;
    Next(&eventType);
    if(eventType == TEXT) {
        String result;
        GetText(&result);
        Next(&eventType);
        if(eventType != END_TAG) {
            String des;
            GetPositionDescription(&des);
            Slogger::E("XmlBlock::Parser", "%s: event TEXT it must be immediately followed by END_TAG %p",
                    (const char*)des, this);
            return E_XML_PULL_PARSER_EXCEPTION;
        }
        *text = result;
        return NOERROR;
    }
    else if(eventType == END_TAG) {
       *text = "";
       return NOERROR;
    }
    else {
        String des;
        GetPositionDescription(&des);
        Slogger::E("XmlBlock::Parser", "%s: parser must be on START_TAG or TEXT to read text %p",
                (const char*)des, this);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
}

ECode XmlBlock::Parser::NextTag(
    /* [out] */ Int32* tag)
{
    VALIDATE_NOT_NULL(tag);

    Int32 eventType;
    Next(&eventType);
    Boolean isWhitespace;
    if (eventType == TEXT && (IsWhitespace(&isWhitespace), isWhitespace)) {   // skip whitespace
        Next(&eventType);
    }
    if (eventType != START_TAG && eventType != END_TAG) {
        String des;
        GetPositionDescription(&des);
        Slogger::E("XmlBlock::Parser", "%s: expected start or end tag %p",
                (const char*)des, this);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    *tag = eventType;
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeNameResource(
    /* [in] */ Int32 index,
    /* [out] */ Int32* resId)
{
    VALIDATE_NOT_NULL(resId);
    *resId = mHost->NativeGetAttributeResource(mParseState, index);
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeListValue(
    /* [in] */ const String& ns,
    /* [in] */ const String& attribute,
    /* [in] */ ArrayOf<String>* options,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 idx = mHost->NativeGetAttributeIndex(mParseState, ns, attribute);
    if (idx >= 0) {
        return GetAttributeListValue(idx, options, defaultValue, value);
    }
    *value = defaultValue;
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeBooleanValue(
    /* [in] */ const String& ns,
    /* [in] */ const String& attribute,
    /* [in] */ Boolean defaultValue,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 idx = mHost->NativeGetAttributeIndex(mParseState, ns, attribute);
    if (idx >= 0) {
        return GetAttributeBooleanValue(idx, defaultValue, value);
    }
    *value = defaultValue;
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeResourceValue(
    /* [in] */ const String& ns,
    /* [in] */ const String& attribute,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 idx = mHost->NativeGetAttributeIndex(mParseState, ns, attribute);
    if (idx >= 0) {
        return GetAttributeResourceValue(idx, defaultValue, value);
    }
    *value = defaultValue;
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeIntValue(
    /* [in] */ const String& ns,
    /* [in] */ const String& attribute,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 idx = mHost->NativeGetAttributeIndex(mParseState, ns, attribute);
    if (idx >= 0) {
        return GetAttributeIntValue(idx, defaultValue, value);
    }
    *value = defaultValue;
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeUnsignedIntValue(
    /* [in] */ const String& ns,
    /* [in] */ const String& attribute,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 idx = mHost->NativeGetAttributeIndex(mParseState, ns, attribute);
    if (idx >= 0) {
        return GetAttributeUnsignedIntValue(idx, defaultValue, value);
    }
    *value = defaultValue;
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeFloatValue(
    /* [in] */ const String& ns,
    /* [in] */ const String& attribute,
    /* [in] */ Float defaultValue,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 idx = mHost->NativeGetAttributeIndex(mParseState, ns, attribute);
    if (idx >= 0) {
        return GetAttributeFloatValue(idx, defaultValue, value);
    }
    *value = defaultValue;
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeListValue(
    /* [in] */ Int32 idx,
    /* [in] */ ArrayOf<String>* options,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 t = mHost->NativeGetAttributeDataType(mParseState, idx);
    Int32 v = mHost->NativeGetAttributeData(mParseState, idx);
    if (t == ITypedValue::TYPE_STRING) {
        AutoPtr<ICharSequence> csq = mHost->mStrings->Get(v);
        *value = XmlUtils::ConvertValueToList(csq, *options, defaultValue);
        return NOERROR;
    }
    *value = v;
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeBooleanValue(
    /* [in] */ Int32 idx,
    /* [in] */ Boolean defaultValue,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 t = mHost->NativeGetAttributeDataType(mParseState, idx);
    // Note: don't attempt to convert any other types, because
    // we want to count on appt doing the conversion for us.
    if (t >= ITypedValue::TYPE_FIRST_INT &&
        t <= ITypedValue::TYPE_LAST_INT) {
        *value = mHost->NativeGetAttributeData(mParseState, idx) != 0;
        return NOERROR;
    }
    *value = defaultValue;
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeResourceValue(
    /* [in] */ Int32 idx,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 t = mHost->NativeGetAttributeDataType(mParseState, idx);
    // Note: don't attempt to convert any other types, because
    // we want to count on appt doing the conversion for us.
    if (t == ITypedValue::TYPE_REFERENCE) {
        *value = mHost->NativeGetAttributeData(mParseState, idx);
        return NOERROR;
    }
    *value = defaultValue;
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeIntValue(
    /* [in] */ Int32 idx,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 t = mHost->NativeGetAttributeDataType(mParseState, idx);
    // Note: don't attempt to convert any other types, because
    // we want to count on appt doing the conversion for us.
    if (t >= ITypedValue::TYPE_FIRST_INT &&
        t <= ITypedValue::TYPE_LAST_INT) {
        *value = mHost->NativeGetAttributeData(mParseState, idx);
        return NOERROR;
    }
    *value = defaultValue;
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeUnsignedIntValue(
    /* [in] */ Int32 idx,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 t = mHost->NativeGetAttributeDataType(mParseState, idx);
    // Note: don't attempt to convert any other types, because
    // we want to count on appt doing the conversion for us.
    if (t >= ITypedValue::TYPE_FIRST_INT &&
        t <= ITypedValue::TYPE_LAST_INT) {
        *value = mHost->NativeGetAttributeData(mParseState, idx);
        return NOERROR;
    }
    *value = defaultValue;
    return NOERROR;
}

ECode XmlBlock::Parser::GetAttributeFloatValue(
    /* [in] */ Int32 idx,
    /* [in] */ Float defaultValue,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 t = mHost->NativeGetAttributeDataType(mParseState, idx);
    // Note: don't attempt to convert any other types, because
    // we want to count on appt doing the conversion for us.
    if (t == ITypedValue::TYPE_FLOAT) {
        *value = Elastos::Core::Math::Int32BitsToFloat(
            mHost->NativeGetAttributeData(mParseState, idx));
        return NOERROR;
    }
    Slogger::E("XmlBlock::Parser", "not a float!");
    return E_RUNTIME_EXCEPTION;
}

ECode XmlBlock::Parser::GetIdAttribute(
    /* [out] */ String* attr)
{
    VALIDATE_NOT_NULL(attr);
    *attr = String(NULL);

    Int32 id = mHost->NativeGetIdAttribute(mParseState);
    if (id >= 0) {
        AutoPtr<ICharSequence> csq = mHost->mStrings->Get(id);
        return csq->ToString(attr);
    }

    return NOERROR;
}

ECode XmlBlock::Parser::GetClassAttribute(
    /* [out] */ String* attr)
{
    VALIDATE_NOT_NULL(attr);
    *attr = String(NULL);

    Int32 id = mHost->NativeGetClassAttribute(mParseState);
    if (id >= 0) {
        AutoPtr<ICharSequence> csq = mHost->mStrings->Get(id);
        return csq->ToString(attr);
    }

    return NOERROR;
}

ECode XmlBlock::Parser::GetIdAttributeResourceValue(
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    //todo: create and use native method
    return GetAttributeResourceValue(String(NULL), String("id"), defaultValue, value);
}

ECode XmlBlock::Parser::GetStyleAttribute(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mHost->NativeGetStyleAttribute(mParseState);
    return NOERROR;
}

ECode XmlBlock::Parser::Close()
{
    AutoLock lock(mHost->mSyncLock);

    if (mParseState != 0) {
        mHost->NativeDestroyParseState(mParseState);
        mParseState = 0;
        mHost->DecOpenCountLocked();
    }
    return NOERROR;
}

ECode XmlBlock::Parser::GetPooledString(
    /* [in] */ Int32 id,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq);
    AutoPtr<ICharSequence> temp = mHost->mStrings->Get(id);
    *csq = temp;
    REFCOUNT_ADD(*csq);
    return NOERROR;
}

//=================================================================================
// XmlBlock
//=================================================================================
const String XmlBlock::TAG("XmlBlock");
const Boolean XmlBlock::DEBUG = FALSE;

XmlBlock::XmlBlock(
    /* [in] */ const ArrayOf<Byte>& data)
    : mOpenCount(1)
    , mOpen(TRUE)
{
    mNative = NativeCreate(data, 0, data.GetLength());
    mStrings = new StringBlock(NativeGetStringBlock(mNative), FALSE);
}

XmlBlock::XmlBlock(
    /* [in] */ const ArrayOf<Byte>& data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size)
    : mOpenCount(1)
    , mOpen(TRUE)
{
    mNative = NativeCreate(data, offset, size);
    mStrings = new StringBlock(NativeGetStringBlock(mNative), FALSE);
}

XmlBlock::XmlBlock(
    /* [in] */ IAssetManager* assets,
    /* [in] */ Int64 xmlBlock)
    : mAssets(assets)
    , mNative(xmlBlock)
    , mOpenCount(1)
    , mOpen(TRUE)
{
    mStrings = new StringBlock(NativeGetStringBlock(xmlBlock), FALSE);
}

XmlBlock::~XmlBlock()
{
    Close();
    mStrings = NULL;
}

void XmlBlock::Close()
{
    AutoLock lock(mSyncLock);

    if (mOpen) {
        mOpen = FALSE;
        DecOpenCountLocked();
    }
}

void XmlBlock::DecOpenCountLocked()
{
    mOpenCount--;
    if (mOpenCount == 0) {
        if (mNative != 0) {
            NativeDestroy(mNative);
            mNative = 0;
        }

        if (mAssets != NULL) {
            Int32 hash;
            GetHashCode(&hash);
            ((CAssetManager*)mAssets.Get())->XmlBlockGone(hash);
            mAssets = NULL;
        }
    }
}

AutoPtr<IXmlResourceParser> XmlBlock::NewParser()
{
    AutoLock lock(mSyncLock);

    AutoPtr<IXmlResourceParser> parser;
    if (mNative != 0) {
        parser = new Parser(NativeCreateParseState(mNative), this);
    }
    return parser;
}

Int64 XmlBlock::NativeCreate(
    /* [in] */ const ArrayOf<Byte>& data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size)
{
    Int32 bLen = data.GetLength();
    if (offset < 0 || offset >= bLen || size < 0 || size > bLen || (offset + size) > bLen) {
        // doThrow(env, "java/lang/IndexOutOfBoundsException");
        assert(0);
        return 0;
    }

    android::ResXMLTree* osb = new android::ResXMLTree();
    if (osb) {
        osb->setTo(data.GetPayload() + offset, size, TRUE);
    }
    if (osb == NULL || osb->getError() != android::NO_ERROR) {
        // doThrow(env, "java/lang/IllegalArgumentException");
        assert(0);
        return 0;
    }

    return reinterpret_cast<Int64>(osb);
}

Int64 XmlBlock::NativeGetStringBlock(
    /* [in] */ Int64 xmlTree)
{
    assert(xmlTree);
    return reinterpret_cast<Int64>(&((android::ResXMLTree*)xmlTree)->getStrings());
}

Int64 XmlBlock::NativeCreateParseState(
    /* [in] */ Int64 xmlTree)
{
    assert(xmlTree);

    android::ResXMLTree* osb = (android::ResXMLTree*)xmlTree;
    android::ResXMLParser* st = new android::ResXMLParser(*osb);
    assert(st);

    st->restart();

    return reinterpret_cast<Int64>(st);
}

Int32 XmlBlock::NativeNext(
    /* [in] */ Int64 parser)
{
    android::ResXMLParser* st = (android::ResXMLParser*)parser;
    if (st == NULL) {
        return android::ResXMLParser::END_DOCUMENT;
    }

    do {
        Int32 code = (Int32)st->next();
        switch (code) {
            case android::ResXMLParser::START_TAG:
                return 2;
            case android::ResXMLParser::END_TAG:
                return 3;
            case android::ResXMLParser::TEXT:
                return 4;
            case android::ResXMLParser::START_DOCUMENT:
                return 0;
            case android::ResXMLParser::END_DOCUMENT:
                return 1;
            case android::ResXMLParser::BAD_DOCUMENT:
                goto bad;
        }
    }
    while (TRUE);

bad:
    // doThrow(env, "org/xmlpull/v1/XmlPullParserException",
    //         "Corrupt XML binary file");
    return android::ResXMLParser::BAD_DOCUMENT;
}

Int32 XmlBlock::NativeGetNamespace(
    /* [in] */ Int64 parser)
{
    android::ResXMLParser* st = (android::ResXMLParser*)parser;
    if (st == NULL) {
        return -1;
    }

    return (Int32)st->getElementNamespaceID();
}

Int32 XmlBlock::NativeGetName(
    /* [in] */ Int64 parser)
{
    android::ResXMLParser* st = (android::ResXMLParser*)parser;
    if (st == NULL) {
        return -1;
    }

    return (Int32)st->getElementNameID();
}

Int32 XmlBlock::NativeGetText(
    /* [in] */ Int64 parser)
{
    android::ResXMLParser* st = (android::ResXMLParser*)parser;
    if (st == NULL) {
        return -1;
    }

    return (Int32)st->getTextID();
}

Int32 XmlBlock::NativeGetLineNumber(
    /* [in] */ Int64 parser)
{
    assert(parser);
    return (Int32)((android::ResXMLParser*)parser)->getLineNumber();
}

Int32 XmlBlock::NativeGetAttributeCount(
    /* [in] */ Int64 parser)
{
    assert(parser);
    return (Int32)((android::ResXMLParser*)parser)->getAttributeCount();
}

Int32 XmlBlock::NativeGetAttributeNamespace(
    /* [in] */ Int64 parser,
    /* [in] */ Int32 idx)
{
    assert(parser);
    return (Int32)((android::ResXMLParser*)parser)->getAttributeNamespaceID(idx);
}

Int32 XmlBlock::NativeGetAttributeName(
    /* [in] */ Int64 parser,
    /* [in] */ Int32 idx)
{
    assert(parser);
    return (Int32)((android::ResXMLParser*)parser)->getAttributeNameID(idx);
}

Int32 XmlBlock::NativeGetAttributeResource(
    /* [in] */ Int64 parser,
    /* [in] */ Int32 idx)
{
    assert(parser);
    return (Int32)((android::ResXMLParser*)parser)->getAttributeNameResID(idx);
}

Int32 XmlBlock::NativeGetAttributeDataType(
    /* [in] */ Int64 parser,
    /* [in] */ Int32 idx)
{
    assert(parser);
    return (Int32)((android::ResXMLParser*)parser)->getAttributeDataType(idx);
}

Int32 XmlBlock::NativeGetAttributeData(
    /* [in] */ Int64 parser,
    /* [in] */ Int32 idx)
{
    assert(parser);
    return (Int32)((android::ResXMLParser*)parser)->getAttributeData(idx);
}

Int32 XmlBlock::NativeGetAttributeStringValue(
    /* [in] */ Int64 parser,
    /* [in] */ Int32 idx)
{
    assert(parser);
    return (Int32)((android::ResXMLParser*)parser)->getAttributeValueStringID(idx);
}

Int32 XmlBlock::NativeGetIdAttribute(
    /* [in] */ Int64 parser)
{
    assert(parser);
    android::ResXMLParser* st = (android::ResXMLParser*)parser;
    ssize_t idx = st->indexOfID();
    return idx >= 0 ? (Int32)st->getAttributeValueStringID(idx) : -1;
}

Int32 XmlBlock::NativeGetClassAttribute(
    /* [in] */ Int64 parser)
{
    assert(parser);
    android::ResXMLParser* st = (android::ResXMLParser*)parser;
    ssize_t idx = st->indexOfClass();
    return idx >= 0 ? (Int32)st->getAttributeValueStringID(idx) : -1;
}

Int32 XmlBlock::NativeGetStyleAttribute(
    /* [in] */ Int64 parser)
{
    assert(parser);

    android::ResXMLParser* st = (android::ResXMLParser*)parser;
    ssize_t idx = st->indexOfStyle();
    if (idx < 0) {
        return 0;
    }

    android::Res_value value;
    if (st->getAttributeValue(idx, &value) < 0) {
        return 0;
    }

    return value.dataType == value.TYPE_REFERENCE
        || value.dataType == value.TYPE_ATTRIBUTE
        ? value.data : 0;
}

Int32 XmlBlock::NativeGetAttributeIndex(
    /* [in] */ Int64 parser,
    /* [in] */ const String& ns,
    /* [in] */ const String& name)
{
    assert(parser);
    assert(!name.IsNull());
    return (Int32)((android::ResXMLParser*)parser)->indexOfAttribute(
            (const char*)ns, (const char*)name);
}

void XmlBlock::NativeDestroyParseState(
    /* [in] */ Int64 parser)
{
    assert(parser);
    delete (android::ResXMLParser*)parser;
}

void XmlBlock::NativeDestroy(
    /* [in] */ Int64 xmlTree)
{
    assert(xmlTree);
    delete (android::ResXMLTree*)xmlTree;
}

ECode XmlBlock::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash = (Int32)this;
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos


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

#include "Elastos.CoreLibrary.External.h"
#include "org/apache/harmony/xml/ExpatAttributes.h"
#include <elastos/utility/logging/Logger.h>

using Org::Xml::Sax::EIID_IAttributes;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {

CAR_INTERFACE_IMPL_2(ExpatAttributes, Object, IExpatAttributes, IAttributes)
const String ExpatAttributes::CDATA("CDATA");

ECode ExpatAttributes::GetURI(
    /* [in] */ Int32 index,
    /* [out] */ String* URI)
{
    VALIDATE_NOT_NULL(URI);
    Int32 len;
    GetLength(&len);
    if (index < 0 || index >= len) {
        *URI = String(NULL);
        return NOERROR;
    }
    Int64 parserPointer;
    GetParserPointer(&parserPointer);
    Int64 pointer;
    GetPointer(&pointer);
    *URI = GetURI(parserPointer, pointer, index);
    return NOERROR;
}

ECode ExpatAttributes::GetLocalName(
    /* [in] */ Int32 index,
    /* [out] */ String* localName)
{
    VALIDATE_NOT_NULL(localName);
    Int32 len;
    GetLength(&len);
    if(index < 0 || index >= len) {
        *localName = String(NULL);
        return NOERROR;
    }

    Int64 parserPointer;
    GetParserPointer(&parserPointer);
    Int64 pointer;
    GetPointer(&pointer);

    *localName = GetLocalName(parserPointer, pointer, index);
    return NOERROR;
}

ECode ExpatAttributes::GetQName(
    /* [in] */ Int32 index,
    /* [out] */ String* qName)
{
    VALIDATE_NOT_NULL(qName);
    Int32 len;
    GetLength(&len);
    if(index < 0 || index >= len) {
        *qName = String(NULL);
        return NOERROR;
    }
    Int64 parserPointer;
    GetParserPointer(&parserPointer);
    Int64 pointer;
    GetPointer(&pointer);
    *qName = GetQName(parserPointer, pointer, index);
    return NOERROR;
}

ECode ExpatAttributes::GetType(
    /* [in] */ Int32 index,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);
    Int32 len;
    GetLength(&len);
    if(index < 0 || index >= len) {
        *type = String(NULL);
        return NOERROR;
    }
    *type = CDATA;
    return NOERROR;
}

ECode ExpatAttributes::GetValue(
    /* [in] */  Int32 index,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    Int32 len;
    GetLength(&len);
    if (index < 0 || index >= len) {
        *value = String(NULL);
        return NOERROR;
    }
    Int64 pointer;
    GetPointer(&pointer);
    *value = GetValueByIndex(pointer, index);
    return NOERROR;
}

ECode ExpatAttributes::GetIndex(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    Int32 len;
    GetLength(&len);
    if (uri.IsNull()) {
        //throw new NullPointerException("uri == null");
        Logger::E("ExpatAttributes", "uri == null");
        assert(0);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (localName.IsNull()) {
        //throw new NullPointerException("localName == null");
        Logger::E("ExpatAttributes", "localName == null");
        assert(0);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int64 pointer;
    GetPointer(&pointer);
    if (pointer == 0) {
        *index = -1;
        return NOERROR;
    }
    *index = GetIndex(pointer, uri, localName);
    return NOERROR;
}

ECode ExpatAttributes::GetIndex(
    /* [in] */ const String& qName,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    if (qName.IsNull()) {
        //throw new NullPointerException("qName == null");
        Logger::E("ExpatAttributes", "qName == null");
        assert(0);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int64 pointer;
    GetPointer(&pointer);
    if (pointer == 0) {
        *index = -1;
        return NOERROR;
    }
    *index = GetIndexForQName(pointer, qName);
    return NOERROR;
}

ECode ExpatAttributes::GetType(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);
    if (uri.IsNull()) {
        //throw new NullPointerException("uri == null");
        Logger::E("ExpatAttributes", "uri == null");
        assert(0);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (localName.IsNull()) {
        //throw new NullPointerException("localName == null");
        Logger::E("ExpatAttributes", "localName == null");
        assert(0);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 index;
    GetIndex(uri, localName, &index);
    if (index == -1) {
        *type = String(NULL);
        return NOERROR;
    }
    *type = CDATA;
    return NOERROR;
}

ECode ExpatAttributes::GetType(
    /* [in] */ const String& qName,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);
    Int32 index;
    GetIndex(qName, &index);
    if (index == -1) {
        *type = String(NULL);
        return NOERROR;
    }
    *type = CDATA;
    return NOERROR;
}

ECode ExpatAttributes::GetValue(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    if (uri.IsNull()) {
        //throw new NullPointerException("uri == null");
        Logger::E("ExpatAttributes", "uri == null");
        assert(0);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (localName.IsNull()) {
        //throw new NullPointerException("localName == null");
        Logger::E("ExpatAttributes", "localName == null");
        assert(0);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 pointer;
    GetPointer(&pointer);
    if (pointer == 0) {
        *value = String(NULL);
        return NOERROR;
    }

    *value = GetValue(pointer, uri, localName);
    return NOERROR;
}

ECode ExpatAttributes::GetValue(
    /* [in] */ const String& qName,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    if (qName.IsNull()) {
        //throw new NullPointerException("qName == null");
        Logger::E("ExpatAttributes", "qname == null");
        assert(0);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int64 pointer;
    GetPointer(&pointer);
    if (pointer == 0) {
        *value = String(NULL);
        return NOERROR;
    }
    *value = GetValueForQName(pointer, qName);
    return NOERROR;
}

String ExpatAttributes::GetURI(
    /* [in] */ Int64 pointer,
    /* [in] */ Int64 attributePointer,
    /* [in] */ Int32 index)
{
    assert(0 && "TODO");
    return String(NULL);
}

String ExpatAttributes::GetLocalName(
    /* [in] */ Int64 pointer,
    /* [in] */ Int64 attributePointer,
    /* [in] */ Int32 index)
{
    assert(0 && "TODO");
    return String(NULL);
}

String ExpatAttributes::GetQName(
    /* [in] */ Int64 pointer,
    /* [in] */ Int64 attributePointer,
    /* [in] */ Int32 index)
{
    assert(0 && "TODO");
    return String(NULL);
}

String ExpatAttributes::GetValueByIndex(
    /* [in] */ Int64 attributePointer,
    /* [in] */ Int32 index)
{
    assert(0 && "TODO");
    return String(NULL);
}

Int32 ExpatAttributes::GetIndex(
    /* [in] */ Int64 attributePointer,
    /* [in] */ const String& uri,
    /* [in] */ const String& localName)
{
    assert(0 && "TODO");
    return 0;
}

Int32 ExpatAttributes::GetIndexForQName(
    /* [in] */ Int64 attributePointer,
    /* [in] */ const String& qName)
{
    assert(0 && "TODO");
    return 0;
}

String ExpatAttributes::GetValue(
    /* [in] */ Int64 attributePointer,
    /* [in] */ const String& uri,
    /* [in] */ const String& localName)
{
    assert(0 && "TODO");
    return String(NULL);
}

String ExpatAttributes::GetValueForQName(
    /* [in] */ Int64 attributePointer,
    /* [in] */ const String& qName)
{
    assert(0 && "TODO");
    return String(NULL);
}

void ExpatAttributes::FreeAttributes(
    /* [in] */ Int64 pointer)
{
    assert(0 && "TODO");
}

} // namespace Xml
} // namespace Harmony
} // namespace Apache
} // namespace Org

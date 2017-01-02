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

#include "org/apache/harmony/xml/dom/CharacterDataImpl.h"
#include "org/apache/harmony/xml/dom/CDocumentImpl.h"
#include "org/apache/harmony/xml/dom/InnerNodeImpl.h"

//import org.w3c.dom.CharacterData;
//import org.w3c.dom.DOMException;
using Org::W3c::Dom::EIID_ICharacterData;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {


CAR_INTERFACE_IMPL_2(CharacterDataImpl, LeafNodeImpl, ICharacterData, ICharacterDataImpl);

CharacterDataImpl::CharacterDataImpl()
{
}

ECode CharacterDataImpl::constructor(
    /* [in] */ IDocument* document,
    /* [in] */ const String& data)
{
    LeafNodeImpl::constructor(document);
    SetData(data);
    return NOERROR;
}

ECode CharacterDataImpl::AppendData(
    /* [in] */ const String& arg)
{
    buffer->Append(arg);
    return NOERROR;
}

ECode CharacterDataImpl::DeleteData(
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    buffer->Delete(offset, offset + count);
    return NOERROR;
}

ECode CharacterDataImpl::GetData(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = buffer->ToString();
    return NOERROR;
}

ECode CharacterDataImpl::AppendDataTo(
    /* [in] */ IStringBuilder* stringBuilder)
{
    stringBuilder->Append(buffer->ToString());
    return NOERROR;
}

ECode CharacterDataImpl::GetLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = buffer->GetLength();
    return NOERROR;
}

ECode CharacterDataImpl::GetNodeValue(
    /* [out] */ String* result)
{
    return GetData(result);
}

ECode CharacterDataImpl::InsertData(
    /* [in] */ Int32 offset,
    /* [in] */ const String& arg)
{
    //try {
    return buffer->Insert(offset, arg);
    //} catch (ArrayIndexOutOfBoundsException ex) {
    //    throw new DOMException(DOMException.INDEX_SIZE_ERR, null);
    //}
}

ECode CharacterDataImpl::ReplaceData(
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ const String& arg)
{
    //try {
    return buffer->Replace(offset, offset + count, arg);
    //} catch (ArrayIndexOutOfBoundsException ex) {
    //    throw new DOMException(DOMException.INDEX_SIZE_ERR, null);
    //}
}

ECode CharacterDataImpl::SetData(
    /* [in] */ const String& data)
{
    buffer = new StringBuffer(data);
    return NOERROR;
}

ECode CharacterDataImpl::SubstringData(
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    *result = buffer->Substring(offset, offset + count);
    return NOERROR;
    //} catch (ArrayIndexOutOfBoundsException ex) {
    //    throw new DOMException(DOMException.INDEX_SIZE_ERR, null);
    //}
}

} // Dom
} // Xml
} // Harmony
} // Apache
} // Org

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

#include "org/apache/http/message/CHeaderGroup.h"
#include "org/apache/http/message/CBasicListHeaderIterator.h"
#include "org/apache/http/message/CBasicHeader.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/utility/CArrayList.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::ILocale;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Org::Apache::Http::Utility::ICharArrayBuffer;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_INTERFACE_IMPL_2(CHeaderGroup, Object, IHeaderGroup, ICloneable)

CAR_OBJECT_IMPL(CHeaderGroup)

ECode CHeaderGroup::Clear()
{
    mHeaders->Clear();
    return NOERROR;
}

ECode CHeaderGroup::AddHeader(
    /* [in] */ IHeader* header)
{
    if (header == NULL) {
        return NOERROR;
    }
    return mHeaders->Add(header);
}

ECode CHeaderGroup::RemoveHeader(
    /* [in] */ IHeader* header)
{
    if (header == NULL) {
        return NOERROR;
    }
    return mHeaders->Remove(header);
}

ECode CHeaderGroup::UpdateHeader(
    /* [in] */ IHeader* header)
{
    if (header == NULL) {
        return NOERROR;
    }
    Int32 size;
    mHeaders->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> value;
        mHeaders->Get(i, (IInterface**)&value);
        AutoPtr<IHeader> current = IHeader::Probe(value);
        String currentN, name;
        current->GetName(&currentN);
        header->GetName(&name);
        if (currentN.EqualsIgnoreCase(name)) {
            mHeaders->Set(i, header);
            return NOERROR;
        }
    }
    return mHeaders->Add(header);
}

ECode CHeaderGroup::SetHeaders(
    /* [in] */ ArrayOf<IHeader*>* headers)
{
    Clear();
    if (headers == NULL) {
        return NOERROR;
    }
    for (Int32 i = 0; i < headers->GetLength(); i++) {
        mHeaders->Add((*headers)[i]);
    }
    return NOERROR;
}

ECode CHeaderGroup::GetCondensedHeader(
    /* [in] */ const String& name,
    /* [out] */ IHeader** condensedHeader)
{
    VALIDATE_NOT_NULL(condensedHeader)
    AutoPtr< ArrayOf<IHeader*> > headers;
    GetHeaders(name, (ArrayOf<IHeader*>**)&headers);

    if (headers->GetLength() == 0) {
        *condensedHeader = NULL;
        return NOERROR;
    }
    else if (headers->GetLength() == 1) {
        *condensedHeader = (*headers)[0];
        REFCOUNT_ADD(*condensedHeader)
        return NOERROR;
    }
    else {
        AutoPtr<ICharArrayBuffer> valueBuffer;
        CCharArrayBuffer::New(128, (ICharArrayBuffer**)&valueBuffer);
        String value;
        (*headers)[0]->GetValue(&value);
        valueBuffer->Append(value);
        for (Int32 i = 1; i < headers->GetLength(); i++) {
            valueBuffer->Append(String(", "));
            (*headers)[i]->GetValue(&value);
            valueBuffer->Append(value);
        }

        String str;
        IObject::Probe(valueBuffer)->ToString(&str);
        return CBasicHeader::New(name.ToLowerCase(/*ILocale::ENGLISH*/), str, condensedHeader);
    }
}

ECode CHeaderGroup::GetHeaders(
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<IHeader*>** headers)
{
    VALIDATE_NOT_NULL(headers)
    AutoPtr<IArrayList> al;
    CArrayList::New((IArrayList**)&al);
    AutoPtr<IList> headersFound = IList::Probe(al);
    Int32 size;
    mHeaders->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> value;
        mHeaders->Get(i, (IInterface**)&value);
        AutoPtr<IHeader> header = IHeader::Probe(value);
        String n;
        header->GetName(&n);
        if (n.EqualsIgnoreCase(name)) {
            headersFound->Add(header);
        }
    }

    headersFound->GetSize(&size);
    AutoPtr< ArrayOf<IInterface*> > in = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr< ArrayOf<IInterface*> > out;
    headersFound->ToArray(in, (ArrayOf<IInterface*>**)&out);
    AutoPtr< ArrayOf<IHeader*> > result = ArrayOf<IHeader*>::Alloc(out->GetLength());
    for (Int32 i = 0; i < out->GetLength(); ++i) {
        AutoPtr<IHeader> h = IHeader::Probe((*out)[i]);
        result->Set(i, h);
    }
    *headers = result;
    REFCOUNT_ADD(*headers);
    return NOERROR;
}

ECode CHeaderGroup::GetFirstHeader(
    /* [in] */ const String& name,
    /* [out] */ IHeader** firstHeader)
{
    VALIDATE_NOT_NULL(firstHeader)
    Int32 size;
    mHeaders->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> value;
        mHeaders->Get(i, (IInterface**)&value);
        AutoPtr<IHeader> header = IHeader::Probe(value);
        String n;
        header->GetName(&n);
        if (n.EqualsIgnoreCase(name)) {
            *firstHeader = header;
            REFCOUNT_ADD(*firstHeader)
            return NOERROR;
        }
    }
    *firstHeader = NULL;
    return NOERROR;
}

ECode CHeaderGroup::GetLastHeader(
    /* [in] */ const String& name,
    /* [out] */ IHeader** lastHeader)
{
    VALIDATE_NOT_NULL(lastHeader)
    // start at the end of the list and work backwards
    Int32 size;
    mHeaders->GetSize(&size);
    for (Int32 i = size; i >= 0; i--) {
        AutoPtr<IInterface> value;
        mHeaders->Get(i, (IInterface**)&value);
        AutoPtr<IHeader> header = IHeader::Probe(value);
        String n;
        header->GetName(&n);
        if (n.EqualsIgnoreCase(name)) {
            *lastHeader = header;
            REFCOUNT_ADD(*lastHeader)
            return NOERROR;
        }
    }
    *lastHeader = NULL;
    return NOERROR;
}

ECode CHeaderGroup::GetAllHeaders(
    /* [out, callee] */ ArrayOf<IHeader*>** allHeaders)
{
    VALIDATE_NOT_NULL(allHeaders)
    Int32 size;
    mHeaders->GetSize(&size);
    AutoPtr< ArrayOf<IInterface*> > in = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr< ArrayOf<IInterface*> > out;
    mHeaders->ToArray(in, (ArrayOf<IInterface*>**)&out);
    AutoPtr< ArrayOf<IHeader*> > result = ArrayOf<IHeader*>::Alloc(out->GetLength());
    for (Int32 i = 0; i < out->GetLength(); ++i) {
        AutoPtr<IHeader> h = IHeader::Probe((*out)[i]);
        result->Set(i, h);
    }
    *allHeaders = result;
    REFCOUNT_ADD(*allHeaders);
    return NOERROR;
}

ECode CHeaderGroup::ContainsHeader(
    /* [in] */ const String& name,
    /* [out] */ Boolean* containsHeader)
{
    VALIDATE_NOT_NULL(containsHeader)
    Int32 size;
    mHeaders->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> value;
        mHeaders->Get(i, (IInterface**)&value);
        AutoPtr<IHeader> header = IHeader::Probe(value);
        String n;
        header->GetName(&n);
        if (n.EqualsIgnoreCase(name)) {
            *containsHeader = TRUE;
            return NOERROR;
        }
    }

    *containsHeader = FALSE;
    return NOERROR;
}

ECode CHeaderGroup::GetIterator(
    /* [out] */ IHeaderIterator** headerIterator)
{
    VALIDATE_NOT_NULL(headerIterator)
    AutoPtr<IBasicListHeaderIterator> it;
    CBasicListHeaderIterator::New(mHeaders, String(NULL), (IBasicListHeaderIterator**)&it);
    *headerIterator = IHeaderIterator::Probe(it);
    REFCOUNT_ADD(*headerIterator)
    return NOERROR;
}

ECode CHeaderGroup::GetIterator(
    /* [in] */ const String& name,
    /* [out] */ IHeaderIterator** headerIterator)
{
    VALIDATE_NOT_NULL(headerIterator)
    AutoPtr<IBasicListHeaderIterator> it;
    CBasicListHeaderIterator::New(mHeaders, name, (IBasicListHeaderIterator**)&it);
    *headerIterator = IHeaderIterator::Probe(it);
    REFCOUNT_ADD(*headerIterator)
    return NOERROR;
}

ECode CHeaderGroup::Copy(
    /* [out] */ IHeaderGroup** headerGroup)
{
    VALIDATE_NOT_NULL(headerGroup)
    AutoPtr<CHeaderGroup> clone;
    CHeaderGroup::NewByFriend((CHeaderGroup**)&clone);
    clone->mHeaders->AddAll(0, ICollection::Probe(mHeaders));
    *headerGroup = clone;
    REFCOUNT_ADD(*headerGroup)
    return NOERROR;
}

ECode CHeaderGroup::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<CHeaderGroup> clone;
    CHeaderGroup::NewByFriend((CHeaderGroup**)&clone);
    clone->mHeaders->AddAll(0, ICollection::Probe(mHeaders));
    *obj = clone->Probe(EIID_IInterface);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CHeaderGroup::constructor()
{
    AutoPtr<IArrayList> al;
    CArrayList::New(16, (IArrayList**)&al);
    mHeaders = IList::Probe(al);
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

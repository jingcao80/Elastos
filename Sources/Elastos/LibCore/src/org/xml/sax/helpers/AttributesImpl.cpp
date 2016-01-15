
#include "AttributesImpl.h"
#include <utils/Log.h>

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

CAR_INTERFACE_IMPL_2(AttributesImpl, Object, IAttributes, IAttributesImpl)

AttributesImpl::AttributesImpl()
    : mLength(0)
{}

ECode AttributesImpl::constructor()
{
    return NOERROR;
}

ECode AttributesImpl::constructor(
    /* [in] */ IAttributes* atts)
{
    return SetAttributes(atts);;
}

ECode AttributesImpl::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);

    *length = mLength;

    return NOERROR;
}

ECode AttributesImpl::GetURI(
    /* [in] */ Int32 index,
    /* [out] */ String* URI)
{
    VALIDATE_NOT_NULL(URI);

    if (index >= 0 && index < mLength) {
        *URI = (*mData)[index * 5];
    } else {
        *URI = NULL;
    }

    return NOERROR;
}

ECode AttributesImpl::GetLocalName(
    /* [in] */ Int32 index,
    /* [out] */ String* localName)
{
    VALIDATE_NOT_NULL(localName);

    if (index >= 0 && index < mLength) {
        *localName = (*mData)[index * 5 + 1];
    } else {
        *localName = NULL;
    }

    return NOERROR;
}

ECode AttributesImpl::GetQName(
    /* [in] */ Int32 index,
    /* [out] */ String* qName)
{
    VALIDATE_NOT_NULL(qName);

    if (index >= 0 && index < mLength) {
        *qName = (*mData)[index * 5 + 2];
    } else {
        *qName = NULL;
    }

    return NOERROR;
}

ECode AttributesImpl::GetType(
    /* [in] */ Int32 index,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);

    if (index >= 0 && index < mLength) {
        *type = (*mData)[index * 5 + 3];
    } else {
        *type = NULL;
    }

    return NOERROR;
}

ECode AttributesImpl::GetValue(
    /* [in] */ Int32 index,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    if (index >= 0 && index < mLength) {
        *value = (*mData)[index * 5 + 4];
    } else {
        *value = NULL;
    }

    return NOERROR;
}

ECode AttributesImpl::GetIndex(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    Int32 max = mLength * 5;

    *index = -1;

    for (Int32 i = 0; i < max; i += 5) {
        if ((*mData)[i].Equals(uri) && (*mData)[i + 1].Equals(localName)) {
            *index = i / 5;
            break;
        }
    }

    return NOERROR;
}

ECode AttributesImpl::GetIndex(
    /* [in] */ const String& qName,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    Int32 max = mLength * 5;
    *index = -1;

    for (Int32 i = 0; i < max; i += 5) {

        if ((*mData)[i + 2].Equals(qName)) {
            *index = i / 5;
            break;
        }
    }

    return NOERROR;
}

ECode AttributesImpl::GetType(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);

    Int32 max = mLength * 5;

    *type = NULL;

    for (Int32 i = 0; i < max; i += 5) {
        if ((*mData)[i].Equals(uri) && (*mData)[i + 1].Equals(localName)) {
            *type = (*mData)[i + 3];
            break;
        }
    }

    return NOERROR;
}

ECode AttributesImpl::GetType(
    /* [in] */ const String& qName,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);

    Int32 max = mLength * 5;

    *type = NULL;

    for (Int32 i = 0; i < max; i += 5) {
        if ((*mData)[i + 2].Equals(qName)) {
            *type = (*mData)[i + 3];
            break;
        }
    }

    return NOERROR;
}

ECode AttributesImpl::GetValue(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 max = mLength * 5;

    *value = NULL;

    for (Int32 i = 0; i < max; i += 5) {
        if ((*mData)[i].Equals(uri) && (*mData)[i + 1].Equals(localName)) {
            *value = (*mData)[i + 4];
            break;
        }
    }

    return NOERROR;
}

ECode AttributesImpl::GetValue(
    /* [in] */ const String& qName,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 max = mLength * 5;

    *value = NULL;

    for (Int32 i = 0; i < max; i += 5) {
        if ((*mData)[i + 2].Equals(qName)) {
            *value = (*mData)[i + 4];
            break;
        }
    }

    return NOERROR;
}

ECode AttributesImpl::Clear()
{
    if (mData != NULL) {
        for (Int32 i = 0; i < (mLength * 5); i++) {
            (*mData)[i] = NULL;
        }
    }

    mLength = 0;

    return NOERROR;
}

ECode AttributesImpl::SetAttributes(
    /* [in] */ IAttributes* atts)
{
    Clear();
    atts->GetLength(&mLength);

    if (mLength > 0) {
        mData = ArrayOf<String>::Alloc(mLength * 5);
        String value;

        for (Int32 i = 0; i < mLength; i++) {
            atts->GetURI(i, &value);
            (*mData)[i*5] = value;

            atts->GetLocalName(i, &value);
            (*mData)[i*5+1] = value;

            atts->GetQName(i, &value);
            (*mData)[i*5+2] = value;

            atts->GetType(i, &value);
            (*mData)[i*5+3] = value;

            atts->GetValue(i, &value);
            (*mData)[i*5+4] = value;
        }
     }

    return NOERROR;
}

ECode AttributesImpl::AddAttribute(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName,
    /* [in] */ const String& type,
    /* [in] */ const String& value)
{
    EnsureCapacity(mLength + 1);

    (*mData)[mLength * 5] = uri;
    (*mData)[mLength * 5 + 1] = localName;
    (*mData)[mLength * 5 + 2] = qName;
    (*mData)[mLength * 5 + 3] = type;
    (*mData)[mLength * 5 + 4] = value;

    mLength++;

    return NOERROR;
}

ECode AttributesImpl::SetAttribute(
    /* [in] */ Int32 index,
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName,
    /* [in] */ const String& type,
    /* [in] */ const String& value)
{
    if (index >= 0 && index < mLength) {
        (*mData)[index*5] = uri;
        (*mData)[index*5+1] = localName;
        (*mData)[index*5+2] = qName;
        (*mData)[index*5+3] = type;
        (*mData)[index*5+4] = value;
    } else {
        return BadIndex(index);
    }

    return NOERROR;
}

ECode AttributesImpl::RemoveAttribute(
    /* [in] */ Int32 index)
{
    if (index >= 0 && index < mLength) {
        if (index < mLength - 1) {

            for(Int32 i = index * 5; i < (mLength - 1) * 5; i++) {
                (*mData)[i] = (*mData)[i + 5];
            }
            //System.Arraycopy(data, (index+1)*5, data, index*5, (mLength-index-1) * 5);
        }

        index = (mLength - 1) * 5;

        (*mData)[index++] = NULL;
        (*mData)[index++] = NULL;
        (*mData)[index++] = NULL;
        (*mData)[index++] = NULL;
        (*mData)[index] = NULL;

        mLength--;
    } else {
        return BadIndex(index);
    }

    return NOERROR;
}

ECode AttributesImpl::SetURI(
    /* [in] */ Int32 index,
    /* [in] */ const String& uri)
{
    if (index >= 0 && index < mLength) {
        (*mData)[index*5] = uri;
    } else {
        return BadIndex(index);
    }

    return NOERROR;
}

ECode AttributesImpl::SetLocalName(
    /* [in] */ Int32 index,
    /* [in] */ const String& localName)
{
    if (index >= 0 && index < mLength) {
        (*mData)[index * 5 + 1] = localName;
    } else {
        return BadIndex(index);
    }

    return NOERROR;
}

ECode AttributesImpl::SetQName(
    /* [in] */ Int32 index,
    /* [in] */ const String& qName)
{
    if (index >= 0 && index < mLength) {
        (*mData)[index*5+2] = qName;
    } else {
        return BadIndex(index);
    }

    return NOERROR;
}

ECode AttributesImpl::SetType(
    /* [in] */ Int32 index,
    /* [in] */ const String& type)
{
    if (index >= 0 && index < mLength) {
        (*mData)[index*5+3] = type;
    } else {
        return BadIndex(index);
    }

    return NOERROR;
}

ECode AttributesImpl::SetValue(
    /* [in] */ Int32 index,
    /* [in] */ const String& value)
{
    if (index >= 0 && index < mLength) {
        (*mData)[index*5+4] = value;
    } else {
        return BadIndex(index);
    }

    return NOERROR;
}

void AttributesImpl::EnsureCapacity(
    /* [in] */ Int32 n)
{
    if (n <= 0) {
        return;
    }
    Int32 max;
    if (mData == NULL || mData->GetLength() == 0) {
        max = 25;
    }
    else if (mData->GetLength() >= n * 5) {
        return;
    }
    else {
        max = mData->GetLength();
    }

    while (max < n * 5) {
        max *= 2;
    }

    AutoPtr<ArrayOf<String> > newArray = ArrayOf<String>::Alloc(max);
    if (mLength > 0) {
        newArray->Copy(mData->GetPayload(), mLength * 5);
    }
    mData = newArray;
}

ECode AttributesImpl::BadIndex (
    /* [in] */ Int32 index)
{
    ALOGE("Attempt to modify attribute at illegal index: %d", index);
    return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org

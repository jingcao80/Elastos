
#include "CAttributes2Impl.h"

namespace Org {
namespace Xml {
namespace Sax {
namespace Ext {

CAR_INTERFACE_IMPL(CAttributes2Impl, AttributesImpl, IAttributes2Impl)

CAR_OBJECT_IMPL(CAttributes2Impl)

ECode CAttributes2Impl::SetAttributes(
    /* [in] */ IAttributes* atts)
{
    Int32 length = 0;
    atts->GetLength(&length);

    AttributesImpl::SetAttributes(atts);

    mDeclared = ArrayOf<Boolean>::Alloc(length);
    mSpecified = ArrayOf<Boolean>::Alloc(length);

    IAttributes2* atts2 = IAttributes2::Probe(atts);
//    if (atts instanceof Attributes2) {
    if (atts2 != NULL){
        for (int i = 0; i < length; i++) {
            Boolean result = FALSE;
            atts2->IsDeclared (i, &result);
            (*mDeclared)[i] = result;

            result = FALSE;
            atts2->IsSpecified (i, &result);
            (*mSpecified)[i] = result;
        }
    }
    else {
        for (int i = 0; i < length; i++) {
            String type("");
            atts->GetType (i, &type);

            (*mDeclared) [i] = !String("CDATA").Equals (type);
            (*mSpecified)[i] = TRUE;
        }
    }

    return NOERROR;
}

ECode CAttributes2Impl::AddAttribute(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName,
    /* [in] */ const String& type,
    /* [in] */ const String& value)
{
    AttributesImpl::AddAttribute (uri, localName, qName, type, value);

    int length = 0;
    GetLength(&length);

    if (length > mSpecified->GetLength()) {

        AutoPtr<ArrayOf<Boolean> > newFlags = ArrayOf<Boolean>::Alloc(length);
//        System.arraycopy (declared, 0, newFlags, 0, declared.length);
        newFlags->Copy(mDeclared->GetPayload(), mDeclared->GetLength());
        mDeclared = newFlags;


        newFlags = ArrayOf<Boolean>::Alloc(length);
//        System.arraycopy (specified, 0, newFlags, 0, specified.length);
        newFlags->Copy(mSpecified->GetPayload(), mSpecified->GetLength());
        mSpecified = newFlags;
    }

    (*mSpecified)[length - 1] = true;
    (*mDeclared)[length - 1] = !String("CDATA").Equals (type);

    return NOERROR;
}

ECode CAttributes2Impl::SetAttribute(
    /* [in] */ Int32 index,
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName,
    /* [in] */ const String& type,
    /* [in] */ const String& value)
{
    return AttributesImpl::SetAttribute(index, uri, localName, qName, type, value);
}

ECode CAttributes2Impl::RemoveAttribute(
    /* [in] */ Int32 index)
{
    Int32 origMax = 0;
    GetLength(&origMax);
    origMax -= 1;

    AttributesImpl::RemoveAttribute (index);

    if (index != origMax) {
//        System.arraycopy (declared, index + 1, declared, index, origMax - index);
        for(Int32 i = index; i < origMax; i++){
            (*mDeclared)[i] = (*mDeclared)[i + 1];
        }
//        System.arraycopy (specified, index + 1, specified, index, origMax - index);
        for(Int32 i = index; i < origMax; i++){
            (*mSpecified)[i] = (*mSpecified)[i + 1];
        }
    }

    return NOERROR;
}

ECode CAttributes2Impl::IsDeclared(
    /* [in] */ Int32 index,
    /* [out] */ Boolean * isDeclared)
{
    Int32 length = 0;
    GetLength (&length);

    if (index < 0 || index >= length){
//        throw new ArrayIndexOutOfBoundsException (
//        "No attribute at index: " + index);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *isDeclared = (*mDeclared)[index];
    return NOERROR;
}

ECode CAttributes2Impl::IsDeclared(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ Boolean * isDeclared)
{
    int index = 0;
    GetIndex(uri, localName, &index);

    if (index < 0){
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
//        throw new IllegalArgumentException (
//        "No such attribute: local=" + localName
//        + ", namespace=" + uri);
    }

    *isDeclared = (*mDeclared)[index];
    return NOERROR;
}

ECode CAttributes2Impl::IsDeclared(
    /* [in] */ const String& qName,
    /* [out] */ Boolean * isDeclared)
{
    int index = 0;
    GetIndex(qName, &index);

    if (index < 0){
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
//        throw new IllegalArgumentException (
//        "No such attribute: " + qName);
    }

    *isDeclared = (*mDeclared)[index];
    return NOERROR;
}

ECode CAttributes2Impl::IsSpecified(
    /* [in] */ Int32 index,
    /* [out] */ Boolean* isSpecified)
{
    int length = 0;
    GetLength(&length);

    if (index < 0 || index >= length){
//        throw new ArrayIndexOutOfBoundsException (
//        "No attribute at index: " + index);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *isSpecified = (*mSpecified)[index];
    return NOERROR;
}

ECode CAttributes2Impl::IsSpecified(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ Boolean* isSpecified)
{
    int index = 0;
    GetIndex(uri, localName, &index);

    if (index < 0){
//        throw new IllegalArgumentException (
//        "No such attribute: local=" + localName
//        + ", namespace=" + uri);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *isSpecified = (*mSpecified)[index];
    return NOERROR;
}

ECode CAttributes2Impl::IsSpecified(
    /* [in] */ const String& qName,
    /* [out] */ Boolean* isSpecified)
{
    int index = 0;
    GetIndex(qName, &index);

    if (index < 0){
//        throw new IllegalArgumentException (
//        "No such attribute: " + qName);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *isSpecified = (*mSpecified)[index];
    return NOERROR;
}

ECode CAttributes2Impl::SetDeclared(
    /* [in] */ Int32 index,
    /* [in] */ Boolean value)
{
    int length = 0;
    GetLength(&length);

    if (index < 0 || index >= length){
//        throw new ArrayIndexOutOfBoundsException (
//        "No attribute at index: " + index);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    (*mDeclared)[index] = value;
    return NOERROR;
}

ECode CAttributes2Impl::SetSpecified(
    /* [in] */ Int32 index,
    /* [in] */ Boolean value)
{
    int length = 0;
    GetLength(&length);

    if (index < 0 || index >= length){
//        throw new ArrayIndexOutOfBoundsException (
//        "No attribute at index: " + index);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    (*mSpecified)[index] = value;
    return NOERROR;
}

ECode CAttributes2Impl::constructor()
{
    mDeclared = ArrayOf<Boolean>::Alloc(0);
    mSpecified = ArrayOf<Boolean>::Alloc(0);
    return NOERROR;
}

ECode CAttributes2Impl::constructor(
    /* [in] */ IAttributes* atts)
{
    AttributesImpl::constructor (atts);
    return NOERROR;
}

} // namespace Ext
} // namespace Sax
} // namespace Xml
} // namespace Org


#include "org/apache/harmony/xml/dom/CDOMErrorImpl.h"

using Org::W3c::Dom::EIID_IDOMLocator;
using Org::W3c::Dom::EIID_IDOMError;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_INTERFACE_IMPL(CDOMErrorImpl::InnerDOMLocator, Object, IDOMLocator);

ECode CDOMErrorImpl::InnerDOMLocator::GetLineNumber(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    return NOERROR;
}

ECode CDOMErrorImpl::InnerDOMLocator::GetColumnNumber(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    return NOERROR;
}

ECode CDOMErrorImpl::InnerDOMLocator::GetByteOffset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    return NOERROR;
}

ECode CDOMErrorImpl::InnerDOMLocator::GetUtf16Offset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    return NOERROR;
}

ECode CDOMErrorImpl::InnerDOMLocator::GetRelatedNode(
    /* [out] */ INode** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode CDOMErrorImpl::InnerDOMLocator::GetUri(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}


CAR_OBJECT_IMPL(CDOMErrorImpl);
CAR_INTERFACE_IMPL(CDOMErrorImpl, Object, IDOMError);

AutoPtr<IDOMLocator> CDOMErrorImpl::NULL_DOM_LOCATOR = new InnerDOMLocator();

ECode CDOMErrorImpl::GetSeverity(
    /* [out] */ Int16 * pValue)
{
    VALIDATE_NOT_NULL(pValue);
    *pValue = severity;
    return NOERROR;
}

ECode CDOMErrorImpl::GetMessage(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = type;
    return NOERROR;
}

ECode CDOMErrorImpl::GetType(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = type;
    return NOERROR;
}

ECode CDOMErrorImpl::GetRelatedException(
    /* [out] */ IObject ** ppObj)
{
    VALIDATE_NOT_NULL(ppObj);
    *ppObj = NULL;
    return NOERROR;
}

ECode CDOMErrorImpl::GetRelatedData(
    /* [out] */ IObject ** ppObj)
{
    VALIDATE_NOT_NULL(ppObj);
    *ppObj = NULL;
    return NOERROR;
}

ECode CDOMErrorImpl::GetLocation(
    /* [out] */ Org::W3c::Dom::IDOMLocator ** ppDomloc)
{
    VALIDATE_NOT_NULL(ppDomloc);
    *ppDomloc = NULL_DOM_LOCATOR;
    REFCOUNT_ADD(*ppDomloc);
    return NOERROR;
}

ECode CDOMErrorImpl::constructor(
    /* [in] */ Int32 severity,
    /* [in] */ const String& type)
{
    this->severity = severity;
    this->type = type;
    return NOERROR;
}

}
}
}
}
}



#include "Elastos.CoreLibrary.External.h"
#include "CSAXResult.h"

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Sax {

CAR_OBJECT_IMPL(CSAXResult)
CAR_INTERFACE_IMPL_2(CSAXResult, Object, ISAXResult, IResult)

CSAXResult::CSAXResult()
{
}

ECode CSAXResult::constructor(
    /* [in] */ IContentHandler* handler)
{
    return SetHandler(handler);
}

ECode CSAXResult::SetHandler(
    /* [in] */ IContentHandler* handler)
{
    mHandler = handler;
    return NOERROR;
}

ECode CSAXResult::GetHandler(
    /* [out] */ IContentHandler** handler)
{
    VALIDATE_NOT_NULL(handler);
    *handler = mHandler;
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

ECode CSAXResult::SetLexicalHandler(
    /* [in] */ ILexicalHandler* handler)
{
    mLexhandler = handler;
    return NOERROR;
}

ECode CSAXResult::GetLexicalHandler(
    /* [out] */ ILexicalHandler** lexhandler)
{
    VALIDATE_NOT_NULL(lexhandler);
    *lexhandler = mLexhandler;
    REFCOUNT_ADD(*lexhandler);
    return NOERROR;
}

ECode CSAXResult::SetSystemId(
    /* [in] */ const String& systemId)
{
    mSystemId = systemId;
    return NOERROR;
}

ECode CSAXResult::GetSystemId(
    /* [out] */ String* systemId)
{
    VALIDATE_NOT_NULL(systemId);
    *systemId = mSystemId;
    return NOERROR;
}

} // namespace Sax
} // namespace Transform
} // namespace Xml
} // namespace Elastosx

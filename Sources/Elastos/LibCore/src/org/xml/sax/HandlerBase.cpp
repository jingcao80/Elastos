
#include "HandlerBase.h"

namespace Org {
namespace Xml {
namespace Sax {

CAR_INTERFACE_IMPL_4(HandlerBase, Object, IEntityResolver, IDTDHandler, IDocumentHandler, IErrorHandler)

ECode HandlerBase::ResolveEntity(
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId,
    /* [out] */ IInputSource** src)
{
    VALIDATE_NOT_NULL(src);

    *src = NULL;

    return NOERROR;
}

ECode HandlerBase::NotationDecl(
    /* [in] */ const String& name,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId)
{
    return NOERROR;
}

ECode HandlerBase::UnparsedEntityDecl(
    /* [in] */ const String& name,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId,
    /* [in] */ const String& notationName)
{
    return NOERROR;
}

ECode HandlerBase::SetDocumentLocator(
    /* [in] */ ILocator* locator)
{
    return NOERROR;
}

ECode HandlerBase::StartDocument()
{
    return NOERROR;
}

ECode HandlerBase::EndDocument()
{
    return NOERROR;
}

ECode HandlerBase::StartElement(
    /* [in] */ const String& name,
    /* [in] */ IAttributeList* atts)
{
    return NOERROR;
}

ECode HandlerBase::EndElement(
    /* [in] */ const String& name)
{
    return NOERROR;
}

ECode HandlerBase::Characters(
    /* [in] */ ArrayOf<Char32>* ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    return NOERROR;
}

ECode HandlerBase::IgnorableWhitespace(
    /* [in] */ ArrayOf<Char32>* ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    return NOERROR;
}

ECode HandlerBase::ProcessingInstruction(
    /* [in] */ const String& target,
    /* [in] */ const String& data)
{
    return NOERROR;
}

ECode HandlerBase::Warning(
    /* [in] */ ECode exception)
{
    return NOERROR;
}

ECode HandlerBase::Error(
    /* [in] */ ECode exception)
{
    return NOERROR;
}

ECode HandlerBase::FatalError(
    /* [in] */ ECode exception)
{
    return exception;
}

} // namespace Sax
} // namespace Xml
} // namespace Org

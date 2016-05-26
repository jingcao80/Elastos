#include "elastos/droid/sax/CRootElement.h"
#include "elastos/droid/sax/CElement.h"

namespace Elastos {
namespace Droid {
namespace Sax {

CAR_INTERFACE_IMPL_4(CRootElement::Handler, Object, IEntityResolver, \
    IDTDHandler, IContentHandler, IErrorHandler);

CAR_INTERFACE_IMPL(CRootElement, Object, IRootElement);

CAR_OBJECT_IMPL(CRootElement)

CRootElement::Handler::~Handler()
{
}

ECode CRootElement::Handler::SetDocumentLocator(
    /* [in] */ ILocator* locator)
{
    mLocator = locator;
    return NOERROR;
}

ECode CRootElement::Handler::StartElement(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName,
    /* [in] */ IAttributes* attributes)
{
    Int32 depth = ++mDepth;

    if (depth == 0) {
        // This is the root element.
        return StartRoot(uri, localName, attributes);
    }

    // Prohibit mixed text and elements.
    if (mBodyBuilder != NULL) {
        //throw new BadXmlException("Encountered mixed content"
        //        + " within text element named " + current + ".",
        //        locator);
        return E_BAD_XML_EXCEPTION;
    }

    // If we're one level below the current element.
    CElement* ccurrent =  (CElement*)mCurrent.Get();
    if (depth == ccurrent->mDepth + 1) {
        // Look for a child to push onto the stack.
        AutoPtr<IChildren> children = ccurrent->mChildren;
        if (children != NULL) {
            AutoPtr<IElement> child;
            FAIL_RETURN(children->Get(uri, localName, (IElement**)&child))
            if (child != NULL) {
                FAIL_RETURN(Start(child, attributes))
            }
        }
    }
    return NOERROR;
}

ECode CRootElement::Handler::StartRoot(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ IAttributes* attributes)
{
    if (mHost->mUri.Compare(uri) != 0
            || mHost->mLocalName.Compare(localName) != 0) {
        //throw new BadXmlException("Root element name does"
        //        + " not match. Expected: " + root + ", Got: "
        //        + Element.toString(uri, localName), locator);
        return E_BAD_XML_EXCEPTION;
    }

    return Start(mHost, attributes);
}

ECode CRootElement::Handler::Start(
    /* [in] */ IElement* e,
    /* [in] */ IAttributes* attributes)
{
    // Push element onto the stack.
    mCurrent = e;

    CElement* ce =  (CElement*)e;
    if (ce->mStartElementListener != NULL) {
        FAIL_RETURN(ce->mStartElementListener->Start(attributes))
    }

    if (ce->mEndTextElementListener != NULL) {
        mBodyBuilder = new StringBuilder();
    }

    FAIL_RETURN(e->ResetRequiredChildren())
    ce->mVisited = TRUE;

    return NOERROR;
}

ECode CRootElement::Handler::Characters(
    /* [in] */ const ArrayOf<Char32>& buffer,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    if (mBodyBuilder != NULL) {
        mBodyBuilder->Append(buffer, start, length);
    }
    return NOERROR;
}

ECode CRootElement::Handler::EndElement(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName)
{
    AutoPtr<IElement> current = mCurrent;

    CElement* ccurrent =  (CElement*)mCurrent.Get();
    // If we've ended the current element...
    if (mDepth == ccurrent->mDepth) {
        FAIL_RETURN(current->CheckRequiredChildren(mLocator))

        // Invoke end element listener.
        if (ccurrent->mEndElementListener != NULL) {
            FAIL_RETURN(ccurrent->mEndElementListener->End())
        }

        // Invoke end text element listener.
        if (mBodyBuilder != NULL) {
            String body;
            body = Object::ToString(mBodyBuilder);
            mBodyBuilder = NULL;

            // We can assume that this listener is present.
            FAIL_RETURN(ccurrent->mEndTextElementListener->End(body))
        }

        // Pop element off the stack.
        mCurrent = ccurrent->mParent;
    }

    mDepth--;
    return NOERROR;
}

ECode CRootElement::Handler::ResolveEntity(
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId,
    /* [out] */ IInputSource** src)
{
    VALIDATE_NOT_NULL(src);

    *src = NULL;

    return NOERROR;
}

ECode CRootElement::Handler::NotationDecl(
    /* [in] */ const String& name,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId)
{
    return NOERROR;
}

ECode CRootElement::Handler::UnparsedEntityDecl(
    /* [in] */ const String& name,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId,
    /* [in] */ const String& notationName)
{
    return NOERROR;
}

ECode CRootElement::Handler::StartDocument()
{
    return NOERROR;
}

ECode CRootElement::Handler::EndDocument()
{
    return NOERROR;
}

ECode CRootElement::Handler::StartPrefixMapping(
    /* [in] */ const String& prefix,
    /* [in] */ const String& uri)
{
    return NOERROR;
}

ECode CRootElement::Handler::EndPrefixMapping(
    /* [in] */ const String& prefix)
{
    return NOERROR;
}

ECode CRootElement::Handler::Characters(
    /* [out] */ ArrayOf<Char32>* ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    return NOERROR;
}

ECode CRootElement::Handler::IgnorableWhitespace(
    /* [out] */ ArrayOf<Char32>* ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    return NOERROR;
}

ECode CRootElement::Handler::ProcessingInstruction(
    /* [in] */ const String& target,
    /* [in] */ const String& data)
{
    return NOERROR;
}

ECode CRootElement::Handler::SkippedEntity(
    /* [in] */ const String& name)
{
    return NOERROR;
}

ECode CRootElement::Handler::Warning(
    /* [in] */ ECode exception)
{
    return NOERROR;
}

ECode CRootElement::Handler::Error(
    /* [in] */ ECode exception)
{
    return NOERROR;
}

ECode CRootElement::Handler::FatalError(
    /* [in] */ ECode exception)
{
    return exception;
}

CRootElement::~CRootElement()
{
}

ECode CRootElement::constructor(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName)
{
    mHandler = new Handler(this);
    return Element::constructor(NULL, uri, localName, 0);
}

ECode CRootElement::constructor(
    /* [in] */ const String& localName)
{
    return constructor(String(""), localName);
}

ECode CRootElement::GetContentHandler(
    /* [out] */ IContentHandler** handler)
{
    VALIDATE_NOT_NULL(handler);

    *handler = (IContentHandler*)mHandler;
    REFCOUNT_ADD(*handler);

    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos

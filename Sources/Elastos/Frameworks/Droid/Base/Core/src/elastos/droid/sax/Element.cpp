#include "elastos/droid/sax/Element.h"
#include "elastos/droid/sax/CChildren.h"
#include "elastos/droid/sax/CElement.h"
#include <elastos/utility/etl/Algorithm.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Sax {

CAR_INTERFACE_IMPL(Element, Object, IElement);

Element::Element()
    : mVisited(FALSE)
{
}

Element::~Element()
{
}

ECode Element::constructor(
    /* [in] */ IElement* parent,
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ Int32 depth)
{
    mParent = parent;
    mUri = uri;
    mLocalName = localName;
    mDepth = depth;

    return NOERROR;
}

ECode Element::GetChild(
    /* [in] */ const String& localName,
    /* [out] */ IElement** result)
{
    return GetChild(String(""), localName, result);
}

ECode Element::GetChild(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ IElement** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (mEndTextElementListener != NULL) {
        Slogger::E("sax::Element", "This element already has an end"
            " text element listener. It cannot have children.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (mChildren == NULL) {
        FAIL_RETURN(CChildren::New((IChildren**)&mChildren))
    }

    return mChildren->GetOrCreate(IElement::Probe(this), uri, localName, result);
}

ECode Element::RequireChild(
    /* [in] */ const String& localName,
    /* [out] */ IElement** result)
{
    return RequireChild(String(""), localName, result);
}

ECode Element::RequireChild(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [out] */ IElement** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IElement> child;
    FAIL_RETURN(GetChild(uri, localName, (IElement**)&child))

    if (mRequiredChilden == NULL) {
        mRequiredChilden = new List<AutoPtr<IElement> >();
        mRequiredChilden->PushBack(child);
    }
    else {
        List<AutoPtr<IElement> >::Iterator it = Find(mRequiredChilden->Begin(), mRequiredChilden->End(), child);
        if (it == mRequiredChilden->End()) {
            mRequiredChilden->PushBack(child);
        }
    }

    *result = child;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Element::SetElementListener(
    /* [in] */ IElementListener* elementListener)
{
    SetStartElementListener(IStartElementListener::Probe(elementListener));
    return SetEndElementListener(IEndElementListener::Probe(elementListener));
}

ECode Element::SetTextElementListener(
    /* [in] */ ITextElementListener* elementListener)
{
    SetStartElementListener(IStartElementListener::Probe(elementListener));
    return SetEndTextElementListener(IEndTextElementListener::Probe(elementListener));
}

ECode Element::SetStartElementListener(
    /* [in] */ IStartElementListener* startElementListener)
{
    if (mStartElementListener != NULL) {
        Slogger::E("sax::Element", "Start element listener has already been set.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mStartElementListener = startElementListener;
    return NOERROR;
}

ECode Element::SetEndElementListener(
    /* [in] */ IEndElementListener* endElementListener)
{
    if (mEndElementListener != NULL) {
        Slogger::E("sax::Element", "End element listener has already been set.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mEndElementListener = endElementListener;
    return NOERROR;
}

ECode Element::SetEndTextElementListener(
    /* [in] */ IEndTextElementListener* endTextElementListener)
{
    if (mEndTextElementListener != NULL) {
        Slogger::E("sax::Element", "End text element listener has already been set.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (mChildren != NULL) {
        Slogger::E("sax::Element", "This element already has children."
            " It cannot have an end text element listener.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mEndTextElementListener = endTextElementListener;
    return NOERROR;
}

ECode Element::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = ToString(mUri, mLocalName);
    return NOERROR;
}

String Element::ToString(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName)
{
    String info;
    if (uri.IsNullOrEmpty()) {
        info.AppendFormat("'%s'", localName.string());
    }
    else {
        info.AppendFormat("'%s:%s'", uri.string(), localName.string());
    }
    return info;
}

ECode Element::ResetRequiredChildren()
{
    AutoPtr<List<AutoPtr<IElement> > > requiredChildren = mRequiredChilden;
    if (requiredChildren != NULL) {
        for (Int32 i = requiredChildren->GetSize() - 1; i >= 0; i--) {
            CElement* celement = (CElement*)(*requiredChildren)[i].Get();
            celement->mVisited = FALSE;
        }
    }
    return NOERROR;
}

ECode Element::CheckRequiredChildren(
    /* [in] */ ILocator* locator)
{
    AutoPtr<List<AutoPtr<IElement> > > requiredChildren = mRequiredChilden;
    if (requiredChildren != NULL) {
        String thisStr;
        ToString(&thisStr);
        for (Int32 i = requiredChildren->GetSize() - 1; i >= 0; i--) {
            CElement* celement = (CElement*)(*requiredChildren)[i].Get();
            if (!celement->mVisited) {
                Slogger::E("sax::Element", "Element named %s is missing required child element named %p",
                    thisStr.string(), locator);
               return E_BAD_XML_EXCEPTION;
            }
        }
    }

    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos

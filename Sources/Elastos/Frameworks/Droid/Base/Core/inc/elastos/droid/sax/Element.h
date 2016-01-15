#ifndef __ELASTOS_DROID_SAX_ELEMENT_H__
#define __ELASTOS_DROID_SAX_ELEMENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Sax.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::Object;
using Elastos::Utility::Etl::List;
using Org::Xml::Sax::ILocator;

namespace Elastos {
namespace Droid {
namespace Sax {

/**
 * An XML element. Provides access to child elements and hooks to listen
 * for events related to this element.
 *
 * @see RootElement
 */
class Element
    : public Object
    , public IElement
{
public:
    CAR_INTERFACE_DECL();

    Element();

    virtual ~Element();

    CARAPI constructor(
        /* [in] */ IElement* parent,
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [in] */ Int32 depth);

    /**
     * Gets the child element with the given name. Uses an empty string as the
     * namespace.
     */
    CARAPI GetChild(
        /* [in] */ const String&  localName,
        /* [out] */ IElement** result);

    /**
     * Gets the child element with the given name.
     */
    CARAPI GetChild(
        /* [in] */ const String&  uri,
        /* [in] */ const String&  localName,
        /* [out] */ IElement** result);

    /**
     * Gets the child element with the given name. Uses an empty string as the
     * namespace. We will throw a {@link org.xml.sax.SAXException} at parsing
     * time if the specified child is missing. This helps you ensure that your
     * listeners are called.
     */
    CARAPI RequireChild(
        /* [in] */ const String&  localName,
        /* [out] */ IElement** result);

    /**
     * Gets the child element with the given name. We will throw a
     * {@link org.xml.sax.SAXException} at parsing time if the specified child
     * is missing. This helps you ensure that your listeners are called.
     */
    CARAPI RequireChild(
        /* [in] */ const String&  uri,
        /* [in] */ const String&  localName,
        /* [out] */ IElement** result);

    /**
     * Sets start and end element listeners at the same time.
     */
    CARAPI SetElementListener(
        /* [in] */ IElementListener* elementListener);

    /**
     * Sets start and end text element listeners at the same time.
     */
    CARAPI SetTextElementListener(
        /* [in] */ ITextElementListener* elementListener);

    /**
     * Sets a listener for the start of this element.
     */
    CARAPI SetStartElementListener(
        /* [in] */ IStartElementListener* startElementListener);

    /**
     * Sets a listener for the end of this element.
     */
    CARAPI SetEndElementListener(
        /* [in] */ IEndElementListener* endElementListener);

    /**
     * Sets a listener for the end of this text element.
     */
    CARAPI SetEndTextElementListener(
        /* [in] */ IEndTextElementListener* endTextElementListener);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    static CARAPI_(String) ToString(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName);

    /**
     * Clears flags on required children.
     */
    CARAPI ResetRequiredChildren();

    /**
     * Throws an exception if a required child was not present.
     */
    CARAPI CheckRequiredChildren(
        /* [in] */ ILocator* locator);

public:
    /* const */ String mUri;
    /* const */ String mLocalName;
    /* const */ Int32 mDepth;
    /* const */ AutoPtr<IElement> mParent;

    AutoPtr<IChildren> mChildren;
    AutoPtr<List<AutoPtr<IElement> > > mRequiredChilden;

    Boolean mVisited;

    AutoPtr<IStartElementListener> mStartElementListener;
    AutoPtr<IEndElementListener> mEndElementListener;
    AutoPtr<IEndTextElementListener> mEndTextElementListener;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SAX_ELEMENT_H__

#ifndef _ORG_APACHE_HARMONY_XML_DOM_LEAFNODEIMPL_H__
#define _ORG_APACHE_HARMONY_XML_DOM_LEAFNODEIMPL_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/harmony/xml/dom/NodeImpl.h"
//#include "org/apache/harmony/xml/dom/InnerNodeImpl.h"
//import org.w3c.dom.Node;
using Org::W3c::Dom::IDocument;
using Org::W3c::Dom::INode;

/**
 * Provides a straightforward implementation of the corresponding W3C DOM
 * interface. The class is used internally only, thus only notable members that
 * are not in the original interface are documented (the W3C docs are quite
 * extensive). Hope that's ok.
 * <p>
 * Some of the fields may have package visibility, so other classes belonging to
 * the DOM implementation can easily access them while maintaining the DOM tree
 * structure.
 * <p>
 * This class represents a Node that has a parent Node, but no children.
 */

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

class InnerNodeImpl;

class LeafNodeImpl
    : public NodeImpl
    , public ILeafNodeImpl
{
public:
    CAR_INTERFACE_DECL();

    LeafNodeImpl();

    CARAPI constructor(
        /* [in] */ IDocument* document);

    CARAPI GetNextSibling(
        /* [out] */ INode** result);

    CARAPI GetParentNode(
        /* [out] */ INode** result);

    CARAPI GetPreviousSibling(
        /* [out] */ INode** result);

    CARAPI IsParentOf(
        /* [in] */ INode* node,
        /* [out] */ Boolean* result);

protected:
    // Maintained by InnerNodeImpl.
    //AutoPtr<InnerNodeImpl> parent;
    AutoPtr<INode> parent;
    // Maintained by InnerNodeImpl.
    Int32 index;

    friend class InnerNodeImpl;
};

} // Dom
} // Xml
} // Harmony
} // Apache
} // Org

#endif // _ORG_APACHE_HARMONY_XML_DOM_LEAFNODEIMPL_H__

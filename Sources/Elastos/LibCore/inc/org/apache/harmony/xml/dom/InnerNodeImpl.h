#ifndef _ORG_APACHE_HARMONY_XML_DOM_INNERNODEIMPL_H__
#define _ORG_APACHE_HARMONY_XML_DOM_INNERNODEIMPL_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/harmony/xml/dom/LeafNodeImpl.h"
#include "org/apache/harmony/xml/dom/CNodeListImpl.h"

//import java.util.ArrayList;
//import libcore.util.Objects;
//import org.w3c.dom.DOMException;
//import org.w3c.dom.DocumentFragment;
using Org::W3c::Dom::IDocument;
using Org::W3c::Dom::INode;
using Org::W3c::Dom::INodeList;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::IList;

/**
 * Provides a straightforward implementation of the corresponding W3C DOM
 * interface. The class is used internally only, thus only notable members that
 * are not in the original interface are documented (the W3C docs are quite
 * extensive).
 *
 * <p>Some of the fields may have package visibility, so other classes belonging
 * to the DOM implementation can easily access them while maintaining the DOM
 * tree structure.
 *
 * <p>This class represents a Node that has a parent Node as well as
 * (potentially) a number of children.
 *
 * <p>Some code was adapted from Apache Xerces.
 */

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

class InnerNodeImpl
    : public LeafNodeImpl
{
public:

    InnerNodeImpl();

    CARAPI constructor(
        /* [in] */ IDocument* document);

    CARAPI AppendChild(
        /* [in] */ INode* newChild,
        /* [out] */ INode** result);

    CARAPI GetChildNodes(
        /* [out] */ INodeList** nodelist);

    CARAPI GetFirstChild(
        /* [out] */ INode** result);

    CARAPI GetLastChild(
        /* [out] */ INode** result);

    CARAPI GetNextSibling(
        /* [out] */ INode** result);

    CARAPI HasChildNodes(
        /* [out] */ Boolean* result);

    CARAPI InsertBefore(
        /* [in] */ INode* newChild,
        /* [in] */ INode* refChild,
        /* [out] */ INode** result);

    /**
     * Inserts {@code newChild} at {@code index}. If it is already child of
     * another node, it is removed from there.
     */
    CARAPI InsertChildAt(
        /* [in] */ INode* newChild,
        /* [in] */ Int32 index,
        /* [out] */ INode** result);

    CARAPI IsParentOf(
        /* [in] */ INode* node,
        /* [out] */ Boolean* result);

    /**
     * Normalize the text nodes within this subtree. Although named similarly,
     * this method is unrelated to Document.normalize.
     */
    CARAPI Normalize();

    CARAPI RemoveChild(
        /* [in] */ INode* oldChild,
        /* [out] */ INode** result);

    /**
     * Removes {@code oldChild} and adds {@code newChild} in its place. This
     * is not atomic.
     */
    CARAPI ReplaceChild(
        /* [in] */ INode* newChild,
        /* [in] */ INode* oldChild,
        /* [out] */ INode** result);

    CARAPI GetTextContent(
        /* [out] */ String* result);

    CARAPI GetTextContent(
        /* [in] */ IStringBuilder* buf);

    CARAPI HasTextContent(
        /* [in] */ INode* child,
        /* [out] */ Boolean* result);

    CARAPI GetElementsByTagName(
        /* [in] */ CNodeListImpl* out,
        /* [in] */ const String& name);

    CARAPI GetElementsByTagNameNS(
        /* [in] */ CNodeListImpl* out,
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName);

private:
    /**
     * Returns true if {@code pattern} equals either "*" or {@code s}. Pattern
     * may be {@code null}.
     */
    static CARAPI_(Boolean) MatchesNameOrWildcard(
        /* [in] */ const String& pattern,
        /* [in] */ const String& s);

    CARAPI RefreshIndices(
        /* [in] */ Int32 fromIndex);

protected:

    // Maintained by LeafNodeImpl and ElementImpl.
    //List<LeafNodeImpl> children = new ArrayList<LeafNodeImpl>();
    AutoPtr<IList> children;

    friend class LeafNodeImpl;
};

} // Dom
} // Xml
} // Harmony
} // Apache
} // Org

#endif // _ORG_APACHE_HARMONY_XML_DOM_INNERNODEIMPL_H__

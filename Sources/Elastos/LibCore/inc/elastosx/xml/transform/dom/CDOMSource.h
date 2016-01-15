
#ifndef  __ELASTOSX_XML_TRANSFORM_DOM_CDOMSOURCE_H__
#define  __ELASTOSX_XML_TRANSFORM_DOM_CDOMSOURCE_H__

#include "Elastos.CoreLibrary.External.h"
#include "_Elastosx_Xml_Transform_Dom_CDOMSource.h"
#include "Object.h"

using Elastos::Core::Object;
using Org::W3c::Dom::INode;

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Dom {

/**
 * <p>Acts as a holder for a transformation Source tree in the
 * form of a Document Object Model (DOM) tree.</p>
 *
 * <p>Note that XSLT requires namespace support. Attempting to transform a DOM
 * that was not contructed with a namespace-aware parser may result in errors.
 * Parsers can be made namespace aware by calling
 * {@link javax.xml.parsers.DocumentBuilderFactory#setNamespaceAware(boolean awareness)}.</p>
 *
 * @author <a href="Jeff.Suttor@Sun.com">Jeff Suttor</a>
 * @version $Revision: 446598 $, $Date: 2006-09-15 05:55:40 -0700 (Fri, 15 Sep 2006) $
 * @see <a href="http://www.w3.org/TR/DOM-Level-2">Document Object Model (DOM) Level 2 Specification</a>
 */
CarClass(CDOMSource)
    , public Object
    , public IDOMSource
    , public ISource
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    /**
     * <p>Zero-argument default constructor.  If this constructor is used, and
     * no DOM source is set using {@link #setNode(Node node)} , then the
     * <code>Transformer</code> will
     * create an empty source {@link org.w3c.dom.Document} using
     * {@link javax.xml.parsers.DocumentBuilder#newDocument()}.</p>
     *
     * @see javax.xml.transform.Transformer#transform(Source xmlSource, Result outputTarget)
     */
    CARAPI constructor();

    /**
     * Create a new input source with a DOM node.  The operation
     * will be applied to the subtree rooted at this node.  In XSLT,
     * a "/" pattern still means the root of the tree (not the subtree),
     * and the evaluation of global variables and parameters is done
     * from the root node also.
     *
     * @param n The DOM node that will contain the Source tree.
     */
    CARAPI constructor(
        /* [in] */ INode* n);

    /**
     * Create a new input source with a DOM node, and with the
     * system ID also passed in as the base URI.
     *
     * @param node The DOM node that will contain the Source tree.
     * @param systemID Specifies the base URI associated with node.
     */
    CARAPI constructor(
        /* [in] */ INode* node,
        /* [in] */ const String& systemID);

    /**
     * Set the node that will represents a Source DOM tree.
     *
     * @param node The node that is to be transformed.
     */
    CARAPI SetNode(
        /* [in] */ INode* node);

    /**
     * Get the node that represents a Source DOM tree.
     *
     * @return The node that is to be transformed.
     */
    CARAPI GetNode(
        /* [out] */ INode** node);

    /**
     * Set the base ID (URL or system ID) from where URLs
     * will be resolved.
     *
     * @param systemID Base URL for this DOM tree.
     */
    CARAPI SetSystemId(
        /* [in] */ const String& systemID);

    /**
     * Get the base ID (URL or system ID) from where URLs
     * will be resolved.
     *
     * @return Base URL for this DOM tree.
     */
    CARAPI GetSystemId(
        /* [out] */ String* id);

private:
    /**
     * <p><code>Node</code> to serve as DOM source.</p>
     */
    AutoPtr<INode> mNode;

    /**
     * <p>The base ID (URL or system ID) from where URLs
     * will be resolved.</p>
     */
    String mSystemID;
};

} // namespace Dom
} // namespace Transform
} // namespace Xml
} // namespace Elastosx

#endif  // __ELASTOSX_XML_TRANSFORM_DOM_CDOMSOURCE_H__

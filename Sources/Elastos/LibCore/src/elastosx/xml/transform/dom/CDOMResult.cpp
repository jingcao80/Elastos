
#include "Elastos.CoreLibrary.External.h"
#include "CDOMResult.h"

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Dom {

CAR_OBJECT_IMPL(CDOMResult);
CAR_INTERFACE_IMPL_2(CDOMResult, Object, IDOMResult, IResult);
ECode CDOMResult::constructor()
{
    SetNode(NULL);
    SetNextSibling(NULL);
    return SetSystemId(String(NULL));
}

ECode CDOMResult::constructor(
    /* [in] */ INode* node)
{
    SetNode(node);
    SetNextSibling(NULL);
    return SetSystemId(String(NULL));
}

ECode CDOMResult::constructor(
    /* [in] */ INode* node,
    /* [in] */ const String& systemId)
{
    SetNode(node);
    SetNextSibling(NULL);
    return SetSystemId(systemId);
}

ECode CDOMResult::constructor(
    /* [in] */ INode* node,
    /* [in] */ INode* nextSibling)
{

    // does the corrent parent/child relationship exist?
    if (nextSibling != NULL) {
        // cannot be a sibling of a NULL node
        if (node == NULL) {
            // throw new IllegalArgumentException("Cannot create a DOMResult when the nextSibling is contained by the \"NULL\" node.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        Int16 v = 0;
        node->CompareDocumentPosition(nextSibling, &v);
        // nextSibling contained by node?
        if ((v & INode::DOCUMENT_POSITION_CONTAINED_BY) == 0) {
            // throw new IllegalArgumentException("Cannot create a DOMResult when the nextSibling is not contained by the node.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    SetNode(node);
    SetNextSibling(nextSibling);
    return SetSystemId(String(NULL));
}

ECode CDOMResult::constructor(
    /* [in] */ INode* node,
    /* [in] */ INode* nextSibling,
    /* [in] */ const String& systemId)
{

    // does the current parent/child relationship exist?
    if (nextSibling != NULL) {
        // cannot be a sibling of a NULL node
        if (node == NULL) {
            // throw new IllegalArgumentException("Cannot create a DOMResult when the nextSibling is contained by the \"NULL\" node.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        Int16 v = 0;
        node->CompareDocumentPosition(nextSibling, &v);
        // nextSibling contained by node?
        if ((v & INode::DOCUMENT_POSITION_CONTAINED_BY) == 0) {
            // throw new IllegalArgumentException("Cannot create a DOMResult when the nextSibling is not contained by the node.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    SetNode(node);
    SetNextSibling(nextSibling);
    return SetSystemId(systemId);
}

ECode CDOMResult::SetNode(
    /* [in] */ INode* node)
{
    // does the corrent parent/child relationship exist?
    if (mNextSibling != NULL) {
        // cannot be a sibling of a NULL node
        if (node == NULL) {
            // throw new IllegalStateException("Cannot create a DOMResult when the nextSibling is contained by the \"NULL\" node.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        Int16 v = 0;
        node->CompareDocumentPosition(mNextSibling, &v);
        // nextSibling contained by node?
        if ((v & INode::DOCUMENT_POSITION_CONTAINED_BY) == 0) {
            // throw new IllegalArgumentException("Cannot create a DOMResult when the nextSibling is not contained by the node.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    mNode = node;
    return NOERROR;
}

ECode CDOMResult::GetNode(
    /* [out] */ INode** node)
{
    VALIDATE_NOT_NULL(node);
    *node = mNode;
    REFCOUNT_ADD(*node);
    return NOERROR;
}

ECode CDOMResult::SetNextSibling(
    /* [in] */ INode* nextSibling)
{
    // does the corrent parent/child relationship exist?
    if (nextSibling != NULL) {
        // cannot be a sibling of a NULL node
        if (mNode == NULL) {
            // throw new IllegalStateException("Cannot create a DOMResult when the nextSibling is contained by the \"NULL\" node.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        Int16 v = 0;
        mNode->CompareDocumentPosition(mNextSibling, &v);
        // nextSibling contained by node?
        if ((v & INode::DOCUMENT_POSITION_CONTAINED_BY) == 0) {
            // throw new IllegalArgumentException("Cannot create a DOMResult when the nextSibling is not contained by the node.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    mNextSibling = nextSibling;
    return NOERROR;
}

ECode CDOMResult::GetNextSibling(
    /* [out] */ INode** nextSibling)
{
    VALIDATE_NOT_NULL(nextSibling);
    *nextSibling = mNextSibling;
    REFCOUNT_ADD(*nextSibling);
    return NOERROR;
}

ECode CDOMResult::SetSystemId(
    /* [in] */ const String& systemId)
{
    mSystemId = systemId;
    return NOERROR;
}

ECode CDOMResult::GetSystemId(
    /* [out] */ String* systemId)
{
    VALIDATE_NOT_NULL(systemId);
    *systemId = mSystemId;
    return NOERROR;
}

} // namespace Dom
} // namespace Transform
} // namespace Xml
} // namespace Elastosx

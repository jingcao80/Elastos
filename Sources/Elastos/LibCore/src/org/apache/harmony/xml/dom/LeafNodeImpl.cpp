#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/harmony/xml/dom/LeafNodeImpl.h"
#include "org/apache/harmony/xml/dom/InnerNodeImpl.h"
#include "org/apache/harmony/xml/dom/CDocumentImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_INTERFACE_IMPL(LeafNodeImpl, NodeImpl, ILeafNodeImpl);

LeafNodeImpl::LeafNodeImpl()
{
}

ECode LeafNodeImpl::constructor(
    /* [in] */ IDocument* document)
{
    NodeImpl::constructor(document);
    return NOERROR;
}

ECode LeafNodeImpl::GetNextSibling(
    /* [out] */ INode** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Int32 size;
    InnerNodeImpl* p = (InnerNodeImpl*)parent.Get();
    if (parent == NULL || (index + 1) >= (p->children->GetSize(&size))) {
        return NOERROR;
    }

    AutoPtr<IInterface> node;
    p->children->Get(index + 1, (IInterface**)&node);
    *result = INode::Probe(node);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode LeafNodeImpl::GetParentNode(
    /* [out] */ INode** result)
{
    VALIDATE_NOT_NULL(result);
    *result = parent.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode LeafNodeImpl::GetPreviousSibling(
    /* [out] */ INode** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    if (parent == NULL || index == 0) {
        return NOERROR;
    }

    InnerNodeImpl* p = (InnerNodeImpl*)parent.Get();
    AutoPtr<IInterface> node;
    p->children->Get(index - 1, (IInterface**)&node);
    *result = INode::Probe(node);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode LeafNodeImpl::IsParentOf(
    /* [in] */ INode* node,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

} // Dom
} // Xml
} // Harmony
} // Apache
} // Org

#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/harmony/xml/dom/CNodeListImpl.h"
#include "elastos/utility/CArrayList.h"

using Org::W3c::Dom::EIID_INodeList;
using Elastos::Utility::CArrayList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_OBJECT_IMPL(CNodeListImpl);
CAR_INTERFACE_IMPL(CNodeListImpl, Object, INodeList);

CNodeListImpl::CNodeListImpl()
{
}

ECode CNodeListImpl::constructor()
{
    CArrayList::New((IList**)&children);
    return NOERROR;
}

ECode CNodeListImpl::constructor(
    /* [in] */ IList* list)
{
    children = list;
    return NOERROR;
}

ECode CNodeListImpl::Item(
    /* [in] */ Int32 index,
    /* [out] */ Org::W3c::Dom::INode ** ppNode)
{
    VALIDATE_NOT_NULL(ppNode);
    *ppNode = NULL;
    Int32 size;
    children->GetSize(&size);
    if (index >= size) {
        return NOERROR;
    } else {
        AutoPtr<IInterface> inode;
        children->Get(index, (IInterface**)&inode);
        *ppNode = INode::Probe(inode);
        REFCOUNT_ADD(*ppNode);
        return NOERROR;
    }
}

ECode CNodeListImpl::GetLength(
    /* [out] */ Int32 * pValue)
{
    return children->GetSize(pValue);
}

ECode CNodeListImpl::Add(
    /* [in] */ INode* node)
{
    return children->Add(node);
}

}
}
}
}
}


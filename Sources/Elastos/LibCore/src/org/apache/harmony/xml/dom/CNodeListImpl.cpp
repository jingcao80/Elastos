//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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


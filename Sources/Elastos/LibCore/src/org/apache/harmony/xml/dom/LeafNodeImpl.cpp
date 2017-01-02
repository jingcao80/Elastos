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

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

#include "NodeSet.h"
#include "CArrayList.h"

using Elastos::Utility::CArrayList;
using Org::W3c::Dom::EIID_INodeList;

namespace Elastos {
namespace Utility {
namespace Prefs {

CAR_INTERFACE_IMPL(NodeSet, Object, INodeList)

NodeSet::NodeSet(
    /* [in] */ IIterator* nodes)
{
    CArrayList::New((IArrayList**)&mList);
    Boolean hasNext = FALSE;
    while(nodes->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        nodes->GetNext((IInterface**)&value);
        mList->Add(value);
    }
}

ECode NodeSet::GetLength(
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len);
    return mList->GetSize(len);
}

ECode NodeSet::Item(
    /* [in] */ Int32 index,
    /* [out] */ INode** node)
{
    VALIDATE_NOT_NULL(node);
    AutoPtr<IInterface> result;
    ECode ec = mList->Get(index, (IInterface**)&result);
    if (FAILED(ec)) {
        *node = NULL;
        return NOERROR;
    }

    *node = INode::Probe(result);
    REFCOUNT_ADD(*node);
    return ec;
}

ECode NodeSet::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("Elastos.Utility.Prefs.NodeSet");
    return NOERROR;
}

} // namespace Prefs
} // namespace Utility
} // namespace Elastos

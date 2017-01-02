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

#ifndef __ELASTOS_UTILITY_PREFS_NODESET_H__
#define __ELASTOS_UTILITY_PREFS_NODESET_H__

#include "Elastos.CoreLibrary.External.h"
#include "Object.h"

using Elastos::Core::Object;
using Org::W3c::Dom::INodeList;
using Org::W3c::Dom::INode;

namespace Elastos {
namespace Utility {
namespace Prefs {

class NodeSet
    : public Object
    , public INodeList
{
public:
    CAR_INTERFACE_DECL()

    NodeSet(
        /* [in] */ IIterator* nodes);

    CARAPI GetLength(
        /* [out] */ Int32* len);

    CARAPI Item(
        /* [in] */ Int32 index,
        /* [out] */ INode** node);

    CARAPI ToString(
        /* [out] */ String* info);

protected:
    AutoPtr<IArrayList> mList;
};

} // namespace Prefs
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__PREFS_NODESET_H__

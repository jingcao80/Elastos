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

#ifndef __ORG_APACHE_HARMONY_XML_DOM_CNODELISTIMPL_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CNODELISTIMPL_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Org_Apache_Harmony_Xml_Dom_CNodeListImpl.h"
#include <elastos/core/Object.h>

using Org::W3c::Dom::INode;
using Org::W3c::Dom::INodeList;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CNodeListImpl)
    , public Object
    , public INodeList
{
public:
    CAR_OBJECT_DECL();
    CAR_INTERFACE_DECL();

    CNodeListImpl();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IList* list);

    CARAPI Item(
        /* [in] */ Int32 index,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI GetLength(
        /* [out] */ Int32 * pValue);

    CARAPI Add(
        /* [in] */ INode* node);

private:
    // <NodeImpl>
    AutoPtr<IList> children;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CNODELISTIMPL_H__

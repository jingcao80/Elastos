
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

#ifndef __ELASTOS_UTILITY_PREFS_NODESET_H__
#define __ELASTOS_UTILITY_PREFS_NODESET_H__

#include "Elastos.CoreLibrary.External.h"
#include "Object.h"

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
    CAR_INTERFACE_DECL();

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

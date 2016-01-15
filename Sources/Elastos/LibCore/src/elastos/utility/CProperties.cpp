
#include "CProperties.h"

namespace Elastos{
namespace Utility{

CAR_OBJECT_IMPL(CProperties)

ECode CProperties::constructor()
{
    return HashTable::constructor();
}

ECode CProperties::constructor(
    /* [in] */ IProperties* properties)
{
    FAIL_RETURN(HashTable::constructor());
    return Properties::constructor((Properties*)properties);
}

} // namespace Utility
} // namespace Elastos

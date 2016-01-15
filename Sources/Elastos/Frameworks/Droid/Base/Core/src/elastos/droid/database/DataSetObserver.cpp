
#include "elastos/droid/database/DataSetObserver.h"

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL(DataSetObserver, Object, IDataSetObserver)
ECode DataSetObserver::OnChanged()
{
    // Do nothing
    return NOERROR;
}

ECode DataSetObserver::OnInvalidated()
{
    // Do nothing
    return NOERROR;
}

} //Database
} //Droid
} //Elastos

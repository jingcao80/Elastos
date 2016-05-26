
#include "CRowSetEvent.h"

namespace Elastosx {
namespace Sql {

CAR_OBJECT_IMPL(CRowSetEvent)

CAR_INTERFACE_IMPL(CRowSetEvent, EventObject, IRowSetEvent)

ECode  CRowSetEvent::constructor(
    /* [in] */ IRowSet* theSource)
{
    IObject* source = IObject::Probe(theSource);
    return EventObject::constructor(source);
}

}// namespace Elastosx
}// namespace Sql
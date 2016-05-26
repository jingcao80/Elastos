
#include "CConnectionEvent.h"

namespace Elastosx {
namespace Sql {

CAR_OBJECT_IMPL(CConnectionEvent)

CAR_INTERFACE_IMPL(CConnectionEvent, EventObject, IConnectionEvent)

CConnectionEvent::CConnectionEvent()
{
    sqlException = E_SQL_EXCEPTION;
}

ECode  CConnectionEvent::constructor(
    /* [in] */ IPooledConnection* theConnection)
{
    IObject* source = IObject::Probe(theConnection);
    return EventObject::constructor(source);
}

ECode  CConnectionEvent::constructor(
    /* [in] */  IPooledConnection* theConnection,
    /* [in] */ ECode theException)
{
    theException = E_SQL_EXCEPTION;
    IObject* source = IObject::Probe(theConnection);
    FAIL_RETURN(EventObject::constructor(source))
    sqlException = theException;
    return NOERROR;
}

ECode  CConnectionEvent::GetSQLException(
    /* [out] */ ECode* sqlExcept)
{
    *sqlExcept = sqlException;
    return NOERROR;
}

}// namespace Elastosx
}// namespace Sql
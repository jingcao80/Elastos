
#include "Elastos.CoreLibrary.Sql.h"
#include "CStatementEvent.h"

namespace Elastosx {
namespace Sql {

CAR_INTERFACE_IMPL(CStatementEvent, EventObject, IStatementEvent)

CStatementEvent::CStatementEvent()
{
    exception = E_SQL_EXCEPTION;
}

ECode  CStatementEvent::constructor(
    /* [in] */  IPooledConnection* con,
    /* [in] */ IPreparedStatement* statement,
    /* [in] */ ECode theException)
{
    theException = E_SQL_EXCEPTION;
    IObject* source = IObject::Probe(con);
    FAIL_RETURN(EventObject::constructor(source))
    this->statement = statement;
    this->exception = theException;
    return NOERROR;
}

ECode  CStatementEvent::constructor(
        /* [in] */ IPooledConnection* con,
        /* [in] */ IPreparedStatement* statement)
{
    return constructor(con, statement, 0);
}

ECode CStatementEvent::GetStatement(
        /* [out] */ IPreparedStatement** statement)
{
    VALIDATE_NOT_NULL(statement)
    *statement = this->statement;
    REFCOUNT_ADD(*statement)
    return NOERROR;
}

ECode  CStatementEvent::GetSQLException(
    /* [out] */ ECode* theException)
{
    *theException = this->exception;
    return NOERROR;
}

}// namespace Elastosx
}// namespace Sql
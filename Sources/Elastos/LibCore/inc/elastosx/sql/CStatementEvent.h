#ifndef __ELASTOSX_SQL_CSTATEMENTEVENT_H__
#define __ELASTOSX_SQL_CSTATEMENTEVENT_H__

#include "Elastos.CoreLibrary.Sql.h"
#include "_Elastosx_Sql_CStatementEvent.h"
#include "utility/EventObject.h"

using Elastos::Utility::EventObject;
using Elastos::Sql::IPreparedStatement;

namespace Elastosx {
namespace Sql {

CarClass(CStatementEvent)
    , public EventObject
    , public IStatementEvent
{
public:
    CAR_INTERFACE_DECL()

    CStatementEvent();

    CARAPI constructor(
        /* [in] */  IPooledConnection* con,
        /* [in] */ IPreparedStatement* statement,
        /* [in] */ ECode theException);

    CARAPI constructor(
        /* [in] */ IPooledConnection* con,
        /* [in] */ IPreparedStatement* statement);

    CARAPI GetStatement(
        /* [out] */ IPreparedStatement** statement);

    CARAPI GetSQLException(
        /* [out] */ ECode* theException);
private:
    ECode exception;
    AutoPtr<IPreparedStatement> statement;
};

}// namespace Elastosx
}// namespace Sql
#endif // __ELASTOSX_SQL_CSTATEMENTEVENT_H__
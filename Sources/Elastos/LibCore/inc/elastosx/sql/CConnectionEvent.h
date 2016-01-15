#ifndef __ELASTOSX_SQL_CCONNECTIONEVENT_H__
#define __ELASTOSX_SQL_CCONNECTIONEVENT_H__

#include "_Elastosx_Sql_CConnectionEvent.h"
#include "utility/EventObject.h"

using Elastos::Utility::EventObject;

namespace Elastosx {
namespace Sql {

CarClass(CConnectionEvent)
    , public EventObject
    , public IConnectionEvent
{
public:
    CAR_INTERFACE_DECL()

    CConnectionEvent();

    CARAPI constructor(
        /* [in] */ IPooledConnection* theConnection);

    CARAPI constructor(
        /* [in] */  IPooledConnection* theConnection,
        /* [in] */ ECode theException);

    CARAPI GetSQLException(
        /* [out] */ ECode* sqlExcept);
private:
    ECode sqlException;
};

}// namespace Elastosx
}// namespace Sql
#endif // __ELASTOSX_SQL_CCONNECTIONEVENT_H__
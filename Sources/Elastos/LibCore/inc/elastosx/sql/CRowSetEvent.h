#ifndef __ELASTOSX_SQL_CROWSETEVENT_H__
#define __ELASTOSX_SQL_CROWSETEVENT_H__

#include "_Elastosx_Sql_CRowSetEvent.h"
#include "utility/EventObject.h"

using Elastos::Utility::EventObject;
using Elastosx::Sql::IRowSet;

namespace Elastosx {
namespace Sql {

CarClass(CRowSetEvent)
    , public EventObject
    , public IRowSetEvent
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IRowSet* theSource);
};

}// namespace Elastosx
}// namespace Sql
#endif // __ELASTOSX_SQL_CROWSETEVENT_H__
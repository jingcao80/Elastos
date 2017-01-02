//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
    CAR_OBJECT_DECL()

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
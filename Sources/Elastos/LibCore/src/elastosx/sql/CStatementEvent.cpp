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

#include "Elastos.CoreLibrary.Sql.h"
#include "CStatementEvent.h"

namespace Elastosx {
namespace Sql {

CAR_OBJECT_IMPL(CStatementEvent)

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
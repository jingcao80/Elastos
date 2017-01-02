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
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

#include "CSQLWarning.h"

namespace Elastos {
namespace Sql {

CAR_OBJECT_IMPL(CSQLWarning)

CAR_INTERFACE_IMPL(CSQLWarning, Object, ISQLWarning)

ECode CSQLWarning::constructor()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CSQLWarning::constructor(
    /* [in] */ const String& theReason)
{
    // super(theReason);
    return NOERROR;
}

ECode CSQLWarning::constructor(
    /* [in] */ const String& theReason,
    /* [in] */ const String& theSQLState)
{
    // super(theReason, theSQLState);
    return NOERROR;
}

ECode CSQLWarning::constructor(
    /* [in] */ const String& theReason,
    /* [in] */ const String& theSQLState,
    /* [in] */ Int32 theErrorCode)
{
    // super(theReason, theSQLState, theErrorCode);
    return NOERROR;
}

// ECode CSQLWarning::constructor(
//     /* [in] */ IThrowable * cause)
// {
//     // super(cause);
//     return NOERROR;
// }

// ECode CSQLWarning::constructor(
//     /* [in] */ const String& reason,
//     /* [in] */ IThrowable * cause)
// {
//     // super(reason, cause);
//     return NOERROR;
// }

// ECode CSQLWarning::constructor(
//     /* [in] */ const String& reason,
//     /* [in] */ const String& SQLState,
//     /* [in] */ IThrowable * cause)
// {
//     // super(reason, SQLState, cause);
//     return NOERROR;
// }

// ECode CSQLWarning::constructor(
//     /* [in] */ const String& reason,
//     /* [in] */ const String& SQLState,
//     /* [in] */ Int32 vendorCode,
//     /* [in] */ IThrowable * cause)
// {
//     // super(reason, SQLState, vendorCode, cause);
//     return NOERROR;
// }

ECode CSQLWarning::GetNextWarning(
    /* [out] */ ISQLWarning ** sqlwar)
{
    // ISQLException next = super.getNextException();
    // if (next == null) {
    //     return null;
    // }
    // if (next instanceof SQLWarning) {
    //     return (SQLWarning) next;
    // }
    // throw new Error("SQLWarning chain holds value that is not a SQLWarning");
    return NOERROR;
}

ECode CSQLWarning::SetNextWarning(
    /* [in] */ ISQLWarning * w)
{
    // super.setNextException(w);
    return NOERROR;
}

} // namespace Sql
} // namespace Elastos

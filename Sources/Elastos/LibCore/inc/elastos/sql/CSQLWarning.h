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

#ifndef __ELASTOS_SQL_CSQLWARNING_H__
#define __ELASTOS_SQL_CSQLWARNING_H__

#include "_Elastos_Sql_CSQLWarning.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Sql {

CarClass(CSQLWarning)
    , public Object
    , public ISQLWarning
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& theReason);

    CARAPI constructor(
        /* [in] */ const String& theReason,
        /* [in] */ const String& theSQLState);

    CARAPI constructor(
        /* [in] */ const String& theReason,
        /* [in] */ const String& theSQLState,
        /* [in] */ Int32 theErrorCode);

    // CARAPI constructor(
    //     /* [in] */ IThrowable * cause);

    // CARAPI constructor(
    //     /* [in] */ const String& reason,
    //     /* [in] */ IThrowable * cause);

    // CARAPI constructor(
    //     /* [in] */ const String& reason,
    //      [in]  const String& SQLState,
    //     /* [in] */ IThrowable * cause);

    // CARAPI constructor(
    //     /* [in] */ const String& reason,
    //     /* [in] */ const String& SQLState,
    //     /* [in] */ Int32 vendorCode,
    //     /* [in] */ IThrowable * cause);

    CARAPI GetNextWarning(
        /* [out] */ ISQLWarning ** sqlwar);

    CARAPI SetNextWarning(
        /* [in] */ ISQLWarning * w);
};

} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_CSQLWARNING_H__

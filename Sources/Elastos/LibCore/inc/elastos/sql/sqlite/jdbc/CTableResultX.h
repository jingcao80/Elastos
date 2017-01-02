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

#ifndef __ELASTOS_SQL_SQLITE_JDBC_CTABLERESULTX_H__
#define __ELASTOS_SQL_SQLITE_JDBC_CTABLERESULTX_H__

#include "_Elastos_Sql_SQLite_JDBC_CTableResultX.h"
#include "TableResult.h"

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

CarClass(CTableResultX)
    , public TableResult
    , public ITableResultX
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 maxrows);

    CARAPI constructor(
        /* [in] */ Elastos::Sql::SQLite::ITableResult* tr);

    CARAPI_(void) Sql_types(
        /* [in] */ ArrayOf<Int32>* types);

public:
    AutoPtr<ArrayOf<Int32> > sql_type;
};

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_JDBC_CTABLERESULTX_H__

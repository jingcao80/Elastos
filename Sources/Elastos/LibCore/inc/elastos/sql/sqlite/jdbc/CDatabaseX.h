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

#ifndef __ELASTOS_SQL_SQLITE_JDBC_CDATABASEX_H__
#define __ELASTOS_SQL_SQLITE_JDBC_CDATABASEX_H__

#include "_Elastos_Sql_SQLite_JDBC_CDatabaseX.h"
#include "Database.h"

using Elastos::Sql::SQLite::Database;

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

CarClass(CDatabaseX)
    , public Database
{
public:
    CAR_OBJECT_DECL()

    CDatabaseX();

    CARAPI Exec(
        /* [in] */ const String& sql,
        /* [in] */ ICallback * cb);

    CARAPI Exec(
        /* [in] */ const String& sql,
        /* [in] */ ICallback * cb,
        /* [in] */ ArrayOf<String> * args);

    CARAPI GetTable(
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<String> * args,
        /* [out] */ ITableResult ** tbl);

    CARAPI GetTable(
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<String> * args,
        /* [in] */ ITableResult * tbl);

protected:
    using Object::Wait;

    CARAPI_(void) Wait(Int32 ms);

private:
    static Object mLock;
};

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_JDBC_CDATABASEX_H__

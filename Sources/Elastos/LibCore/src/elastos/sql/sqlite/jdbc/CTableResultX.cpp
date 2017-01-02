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

#include "CTableResultX.h"
#include "CTableResult.h"
#include "CJDBCDatabaseMetaData.h"

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

CAR_OBJECT_IMPL(CTableResultX)
CAR_INTERFACE_IMPL(CTableResultX, TableResult, ITableResultX)

ECode CTableResultX::constructor()
{
    FAIL_RETURN(TableResult::constructor())
    sql_type = ArrayOf<Int32>::Alloc(mNcolumns);
    for (Int32 i = 0; i < mNcolumns; i++) {
        (*sql_type)[i] = ITypes::VARCHAR;
    }
    return NOERROR;
}

ECode CTableResultX::constructor(
    /* [in] */ Int32 maxrows)
{
    FAIL_RETURN(TableResult::constructor(maxrows))
    sql_type = ArrayOf<Int32>::Alloc(mNcolumns);
    for (Int32 i = 0; i < mNcolumns; i++) {
        (*sql_type)[i] = ITypes::VARCHAR;
    }
    return NOERROR;
}

ECode CTableResultX::constructor(
    /* [in] */ Elastos::Sql::SQLite::ITableResult* tr)
{
    CTableResult* ctr = (CTableResult*)tr;
    mColumn = ctr->mColumn;
    mRows = ctr->mRows;
    mNcolumns = ctr->mNcolumns;
    mNrows = ctr->mNrows;
    mTypes = ctr->mTypes;
    mMaxRows = ctr->mMaxRows;
    sql_type = ArrayOf<Int32>::Alloc(mNcolumns);
    for (Int32 i = 0; i < mNcolumns; i++) {
        (*sql_type)[i] = ITypes::VARCHAR;
    }
    if (ctr->mTypes != NULL) {
        for (Int32 i = 0; i < ctr->mTypes->GetLength(); i++) {
            (*sql_type)[i] = CJDBCDatabaseMetaData::MapSqlType((*ctr->mTypes)[i]);
        }
    }
    return NOERROR;
}

void CTableResultX::Sql_types(
    /* [in] */ ArrayOf<Int32>* types)
{
    sql_type = types;
}

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos

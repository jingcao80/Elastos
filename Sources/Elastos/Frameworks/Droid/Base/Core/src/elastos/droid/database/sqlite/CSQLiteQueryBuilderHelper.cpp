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

#include "elastos/droid/database/sqlite/CSQLiteQueryBuilderHelper.h"
#include "elastos/droid/database/sqlite/SQLiteQueryBuilder.h"

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

CAR_INTERFACE_IMPL(CSQLiteQueryBuilderHelper, Singleton, ISQLiteQueryBuilderHelper)

CAR_SINGLETON_IMPL(CSQLiteQueryBuilderHelper)

ECode CSQLiteQueryBuilderHelper::BuildQueryString(
    /* [in] */ Boolean distinct,
    /* [in] */ const String& tables,
    /* [in] */ ArrayOf<String>* columns,
    /* [in] */ const String& where,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [in] */ const String& orderBy,
    /* [in] */ const String& limit,
    /* [out] */ String* queryStr)
{
    VALIDATE_NOT_NULL(queryStr)
    return SQLiteQueryBuilder::BuildQueryString(distinct, tables, columns, where, groupBy,
        having, orderBy, limit, queryStr);
}

ECode CSQLiteQueryBuilderHelper::AppendColumns(
    /* [in] */ IStringBuilder* s,
    /* [in] */ const ArrayOf<String>& columns)
{
    return SQLiteQueryBuilder::AppendColumns(s, columns);
}


} //Sqlite
} //Database
} //Droid
} //Elastos
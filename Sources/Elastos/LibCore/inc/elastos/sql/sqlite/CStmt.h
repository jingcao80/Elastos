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

#ifndef __ELASTOS_SQL_SQLITE_CSTMT_H__
#define __ELASTOS_SQL_SQLITE_CSTMT_H__

#include "_Elastos_Sql_SQLite_CStmt.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CStmt)
    , public Object
    , public IStmt
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Prepare(
        /* [out] */ Boolean* value);

    CARAPI Step(
        /* [out] */ Boolean* value);

    CARAPI Close();

    CARAPI Reset();

    CARAPI ClearBindings();

    CARAPI Bind(
        /* [in] */ Int32 pos,
        /* [in] */ Int32 value);

    CARAPI Bind(
        /* [in] */ Int32 pos,
        /* [in] */ Int64 value);

    CARAPI Bind(
        /* [in] */ Int32 pos,
        /* [in] */ Double value);

    CARAPI Bind(
        /* [in] */ Int32 pos,
        /* [in] */ const ArrayOf<Byte>& value);

    CARAPI Bind(
        /* [in] */ Int32 pos,
        /* [in] */ const String& value);

    CARAPI Bind(
        /* [in] */ Int32 pos);

    CARAPI BindZeroblob(
        /* [in] */ Int32 pos,
        /* [in] */ Int32 length);

    CARAPI BindParameterCount(
        /* [out] */ Int32* count);

    CARAPI BindParameterName(
        /* [in] */ Int32 pos,
        /* [out] */ String* str);

    CARAPI BindParameterIndex(
        /* [in] */ const String& name,
        /* [out] */ Int32* index);

    CARAPI ColumnInt(
        /* [in] */ Int32 col,
        /* [out] */ Int32* value);

    CARAPI ColumnLong(
        /* [in] */ Int32 col,
        /* [out] */ Int64* value);

    CARAPI ColumnDouble(
        /* [in] */ Int32 col,
        /* [out] */ Double* value);

    CARAPI ColumnBytes(
        /* [in] */ Int32 col,
        /* [out, callee] */ ArrayOf<Byte>** array);

    CARAPI ColumnString(
        /* [in] */ Int32 col,
        /* [out] */ String* str);

    CARAPI ColumnType(
        /* [in] */ Int32 col,
        /* [out] */ Int32* type);

    CARAPI ColumnCount(
        /* [out] */ Int32* count);

    CARAPI Column(
        /* [in] */ Int32 col,
        /* [out] */ IInterface** obj);

    CARAPI ColumnTableName(
        /* [in] */ Int32 col,
        /* [out] */ String* str);

    CARAPI ColumnDatabaseName(
        /* [in] */ Int32 col,
        /* [out] */ String* str);

    CARAPI ColumnDecltype(
        /* [in] */ Int32 col,
        /* [out] */ String* str);

    CARAPI ColumnOriginName(
        /* [in] */ Int32 col,
        /* [out] */ String* str);

    CARAPI Status(
        /* [in] */ Int32 op,
        /* [in] */ Boolean flg,
        /* [out] */ Int32* value);

    CARAPI Finalize();

    CStmt();

    ~CStmt();

public:
    Int64 mHandle;
    Int32 mError_code;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_CSTMT_H__

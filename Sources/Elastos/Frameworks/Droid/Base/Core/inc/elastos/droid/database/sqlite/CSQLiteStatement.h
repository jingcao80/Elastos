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

#ifndef __ELASTOS_DROID_DATABASE_SQLITE_CSQLITESTATEMENT_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_CSQLITESTATEMENT_H__

#include "_Elastos_Droid_Database_Sqlite_CSQLiteStatement.h"
#include "elastos/droid/database/sqlite/SQLiteProgram.h"

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

CarClass(CSQLiteStatement)
    , public SQLiteProgram
    , public ISQLiteStatement
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Execute();

    CARAPI ExecuteUpdateDelete(
        /* [out] */ Int32* result);

    CARAPI ExecuteInsert(
        /* [out] */ Int64* value);

    CARAPI SimpleQueryForInt64(
        /* [out] */ Int64* value);

    CARAPI SimpleQueryForString(
        /* [out] */ String* value);

    CARAPI SimpleQueryForBlobFileDescriptor(
        /* [out] */ IParcelFileDescriptor** result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI constructor(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<IInterface*>* bindArgs);
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_SQLITE_CSQLITESTATEMENT_H__

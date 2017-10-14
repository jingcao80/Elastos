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

#ifndef __ELASTOS_DROID_DATABASE_SQLITE_SQLITEDIRECTCURSORDRIVER_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_SQLITEDIRECTCURSORDRIVER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Database.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::ICancellationSignal;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {
/**
 * A cursor driver that uses the given query directly.
 *
 * @hide
 */
class SQLiteDirectCursorDriver
    : public Object
    , public ISQLiteCursorDriver
{
public:
    CAR_INTERFACE_DECL()

    SQLiteDirectCursorDriver(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& sql,
        /* [in] */ const String& editTable,
        /* [in] */ ICancellationSignal* cancellationSignal);

    CARAPI Query(
        /* [in] */ ISQLiteDatabaseCursorFactory* factory,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ ICursor** cursor);

    CARAPI CursorClosed();

    CARAPI SetBindArguments(
        /* [in] */ ArrayOf<String>* bindArgs);

    CARAPI CursorDeactivated();

    CARAPI CursorRequeried(
        /* [in] */ ICursor* cursor);

    using Object::ToString;

    CARAPI_(String) ToString();

private:
    AutoPtr<ISQLiteDatabase> mDatabase;
    String mEditTable;
    String mSql;
    AutoPtr<ICancellationSignal> mCancellationSignal;
    AutoPtr<ISQLiteQuery> mQuery;
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_SQLITE_SQLITEDIRECTCURSORDRIVER_H__

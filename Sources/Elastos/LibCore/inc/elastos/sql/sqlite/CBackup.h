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

#ifndef __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBACKUP_H__
#define __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBACKUP_H__

#include "_Elastos_Sql_SQLite_CBackup.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CBackup)
    , public Object
    , public IBackup
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Finish();

    CARAPI Finalize();

    CARAPI NativeFinalize();

    CARAPI Step(
        /* [in] */ Int32 n,
        /* [out] */ Boolean* isCompleted);

    CARAPI Backup();

    CARAPI Remaining(
        /* [out] */ Int32* number);

    CARAPI Pagecount(
        /* [out] */ Int32* number);

    CBackup();

    virtual ~CBackup();

private:
    CARAPI NativeStep(
        /* [in] */ Int32 n,
        /* [out] */ Boolean* state);

    CARAPI_(Int32) NativeRemaining();

    CARAPI_(Int32) NativePagecount();

protected:
    Int64 mHandle;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBACKUP_H__

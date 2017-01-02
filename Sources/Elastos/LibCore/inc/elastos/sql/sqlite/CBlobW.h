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

#ifndef __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBLOBW_H__
#define __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBLOBW_H__

#include "_Elastos_Sql_SQLite_CBlobW.h"
#include "CBlob.h"
#include <OutputStream.h>

using Elastos::IO::OutputStream;

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CBlobW)
    , public OutputStream
    , public IBlobW
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBlobW();

    CARAPI Write(
        /* [in] */ Int32 oneByte);

    CARAPI WriteBytes(
        /* [in] */ ArrayOf<Byte>* buffer);

    CARAPI WriteBytes(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len);

    CARAPI Close();

    CARAPI constructor(
        /* [in] */ Elastos::Sql::SQLite::IBlob* blob);

    CARAPI Flush();

private:
    AutoPtr<CBlob> mBlob;
    Int32 mPos;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBLOBW_H__

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

#ifndef __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBLOB_H__
#define __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBLOB_H__

#include "_Elastos_Sql_SQLite_CBlob.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CBlob)
    , public Object
    , public IBlob
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CBlob();

    CARAPI GetInputStream(
        /* [out] */ IInputStream** instream);

    CARAPI GetOutputStream(
        /* [out] */ IOutputStream** outstream);

    CARAPI Close();

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 pos,
        /* [in] */ Int32 len,
        /* [out] */ Int32 * count);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 pos,
        /* [in] */ Int32 len,
        /* [out] */ Int32 * count);
public:
    Int32 mSize;

private:
    Int64 mHandle;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBLOB_H__

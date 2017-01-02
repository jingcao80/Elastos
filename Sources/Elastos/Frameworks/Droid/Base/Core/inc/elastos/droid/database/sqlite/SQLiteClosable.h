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

#ifndef __ELASTOS_DROID_DATABASE_SQLITE_SQLITECLOSABLE_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_SQLITECLOSABLE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Database.h"
#include <elastos/core/Object.h>

using Elastos::IO::ICloseable;
using Elastos::IO::EIID_ICloseable;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

/**
 * An object created from a SQLiteDatabase that can be closed.
 *
 * This class implements a primitive reference counting scheme for database objects.
 */
class SQLiteClosable
    : public Object
    , public ISQLiteClosable
    , public ICloseable
{
public:
    CAR_INTERFACE_DECL()

    SQLiteClosable();

    virtual CARAPI AcquireReference();

    virtual CARAPI ReleaseReference();

    virtual CARAPI ReleaseReferenceFromContainer();

    virtual CARAPI Close();

protected:
    virtual CARAPI_(void) OnAllReferencesReleased() = 0;

    virtual CARAPI_(void) OnAllReferencesReleasedFromContainer();

protected:
    Int32 mReferenceCount;
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_SQLITE_SQLITECLOSABLE_H__

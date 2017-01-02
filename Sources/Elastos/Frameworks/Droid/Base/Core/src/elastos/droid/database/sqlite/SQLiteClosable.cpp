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

#include "elastos/droid/database/sqlite/SQLiteClosable.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

CAR_INTERFACE_IMPL_2(SQLiteClosable, Object, ISQLiteClosable, ICloseable);

SQLiteClosable::SQLiteClosable()
    : mReferenceCount(1)
{}

void SQLiteClosable::OnAllReferencesReleasedFromContainer()
{
    return OnAllReferencesReleased();
}

ECode SQLiteClosable::AcquireReference()
{
    {
        AutoLock syncLock(this);
        if (mReferenceCount <= 0) {
            Logger::E("SQLiteClosable", "attempt to re-open an already-closed object: %p", this);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mReferenceCount++;
    }
    return NOERROR;
}

ECode SQLiteClosable::ReleaseReference()
{
    Boolean refCountIsZero = FALSE;

    {
        AutoLock syncLock(this);
        refCountIsZero = --mReferenceCount == 0;
    }

    if (refCountIsZero) {
        OnAllReferencesReleased();
    }

    return NOERROR;
}

ECode SQLiteClosable::ReleaseReferenceFromContainer()
{
    Boolean refCountIsZero = FALSE;

    {
        AutoLock syncLock(this);
        refCountIsZero = --mReferenceCount == 0;
    }

    if (refCountIsZero) {
        OnAllReferencesReleasedFromContainer();
    }
    return NOERROR;
}

ECode SQLiteClosable::Close()
{
    return ReleaseReference();
}

} //Sqlite
} //Database
} //Droid
} //Elastos

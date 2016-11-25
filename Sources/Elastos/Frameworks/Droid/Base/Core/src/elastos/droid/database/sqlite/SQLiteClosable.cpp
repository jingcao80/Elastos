
#include "elastos/droid/database/sqlite/SQLiteClosable.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
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

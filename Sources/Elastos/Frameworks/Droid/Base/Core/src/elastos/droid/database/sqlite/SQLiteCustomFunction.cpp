
#include "elastos/droid/database/sqlite/SQLiteCustomFunction.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

SQLiteCustomFunction::SQLiteCustomFunction(
    /* [in] */ const String& name,
    /* [in] */ Int32 numArgs,
    /* [in] */ ICustomFunction* callBack)
    : mNumArgs(numArgs)
    , mCallback(callBack)
{
    if (name.IsNull()) {
        Slogger::E("SQLiteCustomFunction", "name must not be null.");
        assert(0);
        // throw new IllegalArgumentException("name must not be null.");
    }
    mName = name;
    mNumArgs = numArgs;
    mCallback = callBack;
}

void SQLiteCustomFunction::DispatchCallback(
    /* [in] */ ArrayOf<String>* args)
{
    mCallback->Callback(args);
}

} //Sqlite
} //Database
} //Droid
} //Elastos


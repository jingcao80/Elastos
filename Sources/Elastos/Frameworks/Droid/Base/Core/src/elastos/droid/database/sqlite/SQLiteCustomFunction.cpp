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


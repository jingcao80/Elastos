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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/CatLog.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                                CatLog
//=====================================================================
const Boolean CatLog::DEBUG = TRUE;

ECode CatLog::D(
    /* [in] */ IInterface* caller,
    /* [in] */ const String& msg)
{
    if (!DEBUG) {
        return NOERROR;
    }

    //assert(0 && "TODO");
    //TODO String className; // = caller.getClass().getName();
    //TODO Logger::D("CAT", className.Substring(className.LastIndexOf('.') + 1) + ": "
    //        + msg);
    Logger::D(TO_CSTR(caller), "msg: %s", msg.string());
    return NOERROR;
}

ECode CatLog::D(
    /* [in] */ const String& caller,
    /* [in] */ const String& msg)
{
    if (!DEBUG) {
        return NOERROR;
    }

    Logger::D("CAT", caller + ": " + msg);
    return NOERROR;
}

ECode CatLog::E(
    /* [in] */ IInterface* caller,
    /* [in] */ const String& msg)
{
    //String className;// = caller.getClass().getName();
    //Logger::E("CAT", className.Substring(className.LastIndexOf('.') + 1) + ": "
    //        + msg);
    Logger::E(TO_CSTR(caller), "msg: %s", msg.string());
    return NOERROR;
}

ECode CatLog::E(
    /* [in] */ const String& caller,
    /* [in] */ const String& msg)
{
    Logger::E("CAT", caller + ": " + msg);
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos



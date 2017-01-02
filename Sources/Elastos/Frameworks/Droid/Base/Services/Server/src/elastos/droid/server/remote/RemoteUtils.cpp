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

#include "remote/RemoteUtils.h"
#include "remote/ResultMsg.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

const String RemoteUtils::TAG("RemoteUtils");
const Boolean RemoteUtils::DBG = TRUE;
const String RemoteUtils::SPLITER = String("94:GM");

/**
 * composeResult: Add result number in the front of result
 * format: [rltNum][SPLITER][result]
 */
String RemoteUtils::ComposeResult(
    /* [in] */ Int32 rltNum,
    /* [in] */ const String& result)
{
    StringBuilder sb(StringUtils::Int32ToString(rltNum));
    sb.Append(SPLITER);

    /** append result if there is no error */
    if(rltNum == ResultMsg::RESULTMSG_NOERROR->ToInt32()) {
        sb.Append(result);
    }

    if (DBG) Slogger::D(TAG, "ComposeResult: %s", sb.ToString().string());
    return sb.ToString();
}

Boolean RemoteUtils::IsDebug()
{
    return TRUE;
}

} // Remote
} // Server
} // Droid
} // Elastos

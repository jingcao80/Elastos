
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


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
    Logger::D("CAT", "TODO %s", msg.string());
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
    Logger::E("CAT", "TODO %s", msg.string());
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



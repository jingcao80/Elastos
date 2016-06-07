
#ifndef __ELASTOS_DROID_SERVER_TELECOM_SPAMBLOCKER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_SPAMBLOCKER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Listens for call updates and records whether calls should be blocked based on
 * caller info results.  Can also hang up calls while they are ringing, in case
 * they could not be blocked in time.
 */
class SpamBlocker
    : public CallsManagerListenerBase
{
public:
    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI ShouldBlock(
        /* [in] */ const String& number,
        /* [out] */ Boolean* result);
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_SPAMBLOCKER_H__


#ifndef __ELASTOS_DROID_SERVER_TELECOM_CONSTANTS_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CONSTANTS_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * App-wide constants for the phone app.
 *
 * Any constants that need to be shared between two or more classes within
 * the com.android.phone package should be defined here.  (Constants that
 * are private to only one class can go in that class's .java file.)
 */
class Constants
{
public:
    //
    // URI schemes
    //

    static const String SCHEME_SMSTO;

private:
    Constants();
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CONSTANTS_H__

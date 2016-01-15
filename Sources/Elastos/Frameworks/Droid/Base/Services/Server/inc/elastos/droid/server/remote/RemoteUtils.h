
#ifndef __ELASTOS_DROID_SERVER_REMOTE_REMOTEUTILS_H__
#define __ELASTOS_DROID_SERVER_REMOTE_REMOTEUTILS_H__

#include <elastos.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

class RemoteUtils
{
public:
    /**
     * composeResult: Add result number in the front of result
     * format: [rltNum][SPLITER][result]
     */
    static CARAPI_(String) ComposeResult(
        /* [in] */ Int32 rltNum,
        /* [in] */ const String& result);

    static CARAPI_(Boolean) IsDebug();

private:
    static const String TAG;

public:
    static const Boolean DBG;
    static const String SPLITER;
};

} // Remote
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_REMOTE_REMOTEUTILS_H__

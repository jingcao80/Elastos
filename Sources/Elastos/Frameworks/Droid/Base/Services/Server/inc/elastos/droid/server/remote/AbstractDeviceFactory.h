
#ifndef __ELASTOS_DROID_SERVER_REMOTE_ABSTRACTDEVICEFACTORY_H__
#define __ELASTOS_DROID_SERVER_REMOTE_ABSTRACTDEVICEFACTORY_H__

#include <ext/frameworkext.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

class AbstractDeviceFactory : public ElRefBase
{
public:
    AbstractDeviceFactory(
        /* [in] */ IContext* ctx);

protected:
    virtual CARAPI RunForResult(
        /* [in] */ const String& cmd,
        /* [out] */ String* result) = 0;

private:
    static const String TAG;
    static const Boolean DBG;

protected:
    AutoPtr<IContext> mContext;
};

} // Remote
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_REMOTE_ABSTRACTDEVICEFACTORY_H__

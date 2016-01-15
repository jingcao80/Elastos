
#ifndef __ELASTOS_DROID_SERVER_CSERIALSERVICE_H__
#define __ELASTOS_DROID_SERVER_CSERIALSERVICE_H__

#include "_Elastos_Droid_Server_CSerialService.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CSerialService)
{
public:
    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI GetSerialPorts(
        /* [out, callee] */ ArrayOf<String>** serialPorts);

    CARAPI OpenSerialPort(
        /* [in] */ const String& path,
        /* [out] */ IParcelFileDescriptor** descriptor);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI NativeOpen(
        /* [in] */ const String& path,
        /* [out] */ IParcelFileDescriptor** descriptor);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<ArrayOf<String> > mSerialPorts;

};

} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CSERIALSERVICE_H__

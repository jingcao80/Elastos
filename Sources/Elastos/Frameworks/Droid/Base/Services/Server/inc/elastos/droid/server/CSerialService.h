
#ifndef __ELASTOS_DROID_SERVER_CSERIALSERVICE_H__
#define __ELASTOS_DROID_SERVER_CSERIALSERVICE_H__

#include "_Elastos_Droid_Server_CSerialService.h"
#include <elastos/core/Object.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Hardware.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Hardware::IISerialManager;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CSerialService)
    , public Object
    , public IISerialManager
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

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

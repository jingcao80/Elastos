
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/hardware/SerialManager.h"
#include "elastos/droid/hardware/SerialPort.h"

using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Hardware {

const String SerialManager::TAG("SerialManager");

CAR_INTERFACE_IMPL(SerialManager, Object, ISerialManager)

SerialManager::SerialManager()
{
}

SerialManager::~SerialManager()
{
}

ECode SerialManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IISerialManager* service)
{
    mContext = context;
    mService = service;
    return NOERROR;
}

ECode SerialManager::GetSerialPorts(
    /* [out, callee] */ ArrayOf<String>** ports)
{
    // try {
    return mService->GetSerialPorts(ports);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "RemoteException in getSerialPorts", e);
    //     return null;
    // }
}

ECode SerialManager::OpenSerialPort(
    /* [in] */ const String& name,
    /* [in] */ Int32 speed,
    /* [out] */ ISerialPort** port)
{
    VALIDATE_NOT_NULL(port);

    // try {
    AutoPtr<IParcelFileDescriptor> pfd;
    FAIL_RETURN(mService->OpenSerialPort(name, (IParcelFileDescriptor**)&pfd));
    if (pfd != NULL) {
        AutoPtr<SerialPort> _port = new SerialPort();
        _port->constructor(name);
        *port = _port;
        REFCOUNT_ADD(*port);
        (*port)->Open(pfd, speed);
        return NOERROR;
    } else {
        // throw new IOException("Could not open serial port " + name);
        return E_IO_EXCEPTION;
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, "exception in UsbManager.openDevice", e);
    // }
    *port = NULL;
    return NOERROR;
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos



#include "Elastos.Droid.Content.h"
#include "elastos/droid/hardware/usb/UsbManager.h"
#include "elastos/droid/hardware/usb/CUsbDeviceConnection.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/os/CSystemProperties.h"
#include <elastos/core/Character.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBaseBundle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

const String UsbManager::TAG("UsbManager");

CAR_INTERFACE_IMPL(UsbManager, Object, IUsbManager);

ECode UsbManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIUsbManager* service)
{
    mContext = context;
    mService = service;

    return NOERROR;
}

ECode UsbManager::GetDeviceList(
    /* [out] */ IMap** list)
{
    VALIDATE_NOT_NULL(list);

    AutoPtr<IBundle> bundle;
    FAIL_RETURN(mService->GetDeviceList((IBundle**)&bundle));

    AutoPtr<ISet> keySet;
    FAIL_RETURN(IBaseBundle::Probe(bundle)->GetKeySet((ISet**)&keySet));

    if (keySet == NULL) {
        *list = NULL;
        return NOERROR;
    }

    AutoPtr<IMap> result;
    CArrayMap::New((IMap**)&result);

    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    String name;
    while ((it->HasNext(&hasNext), hasNext)) {
        AutoPtr<IInterface> outface;
        FAIL_RETURN(it->GetNext((IInterface**)&outface))
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
        AutoPtr<IInterface> obj;
        FAIL_RETURN(entry->GetKey((IInterface**)&obj))
        assert(ICharSequence::Probe(obj) != NULL);
        ICharSequence::Probe(obj)->ToString(&name);

        AutoPtr<IInterface> valueObj;
        FAIL_RETURN(IBaseBundle::Probe(bundle)->Get(name, (IInterface**)&valueObj))

        result->Put(obj, valueObj);
    }

    return NOERROR;
}

ECode UsbManager::OpenDevice(
    /* [in] */ IUsbDevice* device,
    /* [out] */ IUsbDeviceConnection** connection)
{
    VALIDATE_NOT_NULL(connection);

    String deviceName;
    FAIL_RETURN(device->GetDeviceName(&deviceName));

    AutoPtr<IParcelFileDescriptor> pfd;
    FAIL_RETURN(mService->OpenDevice(deviceName, (IParcelFileDescriptor**)&pfd));

    if (pfd == NULL) {
        *connection = NULL;
        return NOERROR;
    }

    AutoPtr<IUsbDeviceConnection> tempConn;
    CUsbDeviceConnection::New(device, (IUsbDeviceConnection**)&tempConn);

    Boolean result;
    FAIL_RETURN(tempConn->Open(deviceName, pfd, &result));

    if (result == FALSE) {
        *connection = NULL;
        return NOERROR;
    }

    *connection = tempConn;
    REFCOUNT_ADD(*connection);
    return NOERROR;
}

ECode UsbManager::GetAccessoryList(
    /* [out, callee] */ ArrayOf<IUsbAccessory*>** list)
{
    VALIDATE_NOT_NULL(list);

    AutoPtr<IUsbAccessory> accessory;
    FAIL_RETURN(mService->GetCurrentAccessory((IUsbAccessory**)&accessory));

    if (accessory == NULL) {
        *list = NULL;
        return NOERROR;
    }

    AutoPtr<ArrayOf<IUsbAccessory*> > tempList = ArrayOf<IUsbAccessory*>::Alloc(1);

    tempList->Set(0, accessory);
    *list = tempList;

    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode UsbManager::OpenAccessory(
    /* [in] */ IUsbAccessory* accessory,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor);
    FAIL_RETURN(mService->OpenAccessory(accessory, descriptor));
    return NOERROR;
}

ECode UsbManager::HasDevicePermission(
    /* [in] */ IUsbDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mService->HasDevicePermission(device, result));
    return NOERROR;
}

ECode UsbManager::HasAccessoryPermission(
    /* [in] */ IUsbAccessory* accessory,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mService->HasAccessoryPermission(accessory, result));
    return NOERROR;
}

ECode UsbManager::RequestDevicePermission(
    /* [in] */ IUsbDevice* device,
    /* [in] */ IPendingIntent* pi)
{
    String name;
    FAIL_RETURN(mContext->GetPackageName(&name));
    FAIL_RETURN(mService->RequestDevicePermission(device, name, pi));
    return NOERROR;
}

ECode UsbManager::RequestAccessoryPermission(
    /* [in] */ IUsbAccessory* accessory,
    /* [in] */ IPendingIntent* pi)
{
    String name;
    FAIL_RETURN(mContext->GetPackageName(&name));
    FAIL_RETURN(mService->RequestAccessoryPermission(accessory, name, pi));
    return NOERROR;
}

ECode UsbManager::IsFunctionEnabled(
    /* [in] */ const String& function,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    String property("sys.usb.config");
    *result = PropertyContainsFunction(property, function);
    return NOERROR;
}

ECode UsbManager::GetDefaultFunction(
    /* [out] */ String* function)
{
    VALIDATE_NOT_NULL(function);
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String str;
    sp->Get(String("persist.sys.usb.config"), String(""), &str);

    int commaIndex = str.IndexOf(',');

    if (commaIndex > 0) {
        *function = str.Substring(0, commaIndex);
        return NOERROR;
    }

    *function = str;
    return NOERROR;
}

ECode UsbManager::SetCurrentFunction(
    /* [in] */ const String& function,
    /* [in] */ Boolean makeDefault)
{
    FAIL_RETURN(mService->SetCurrentFunction(function, makeDefault));
    return NOERROR;
}

ECode UsbManager::SetMassStorageBackingFile(
    /* [in] */ const String& path)
{
    FAIL_RETURN(mService->SetMassStorageBackingFile(path));
    return NOERROR;
}

Boolean UsbManager::PropertyContainsFunction(
    /* [in] */ const String& property,
    /* [in] */ const String& function)
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String functions;
    sp->Get(property, String(""), &functions);
    Int32 index = functions.IndexOf(function);

    if (index < 0) {
        return FALSE;
    }

    Char32 c1 = functions.GetChar(index - 1);
    if (index > 0 && c1 != ',') {
        return FALSE;
    }

    Int32 charAfter = index + function.GetLength();
    Char32 c2 = functions.GetChar(charAfter);
    if (charAfter < (Int32)functions.GetLength() && c2 != ',') {
        return FALSE;
    }

    return TRUE;
}

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

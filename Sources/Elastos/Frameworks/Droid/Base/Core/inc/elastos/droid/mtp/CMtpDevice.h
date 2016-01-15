
#ifndef __ELASTOS_DROID_MTP_CMTPDEVICE_H__
#define __ELASTOS_DROID_MTP_CMTPDEVICE_H__

#include "Elastos.Droid.Hardware.h"
#include "_Elastos_Droid_Mtp_CMtpDevice.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Usb::IUsbDevice;
using Elastos::Droid::Hardware::Usb::IUsbDeviceConnection;

namespace Elastos {
namespace Droid {
namespace Mtp {

CarClass(CMtpDevice)
    , public Object
    , public IMtpDevice
{
public:
    CMtpDevice();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IUsbDevice* device);

    CARAPI Open(
        /* [in] */ IUsbDeviceConnection* connection,
        /* [out]*/ Boolean* result);

    CARAPI Close();

    CARAPI GetDeviceName(
        /* [out] */ String* result);

    CARAPI GetDeviceId(
        /* [out]*/ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetDeviceInfo(
        /* [out] */ IMtpDeviceInfo** result);

    CARAPI GetStorageIds(
        /* [out] */ ArrayOf<Int32>** result);

    CARAPI GetObjectHandles(
        /* [in] */ Int32 storageId,
        /* [in] */ Int32 format,
        /* [in] */ Int32 objectHandle,
        /* [out] */ ArrayOf<Int32>** result);

    CARAPI GetObject(
        /* [in] */ Int32 objectHandle,
        /* [in] */ Int32 objectSize,
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetThumbnail(
        /* [in] */ Int32 objectHandle,
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetStorageInfo(
        /* [in] */ Int32 storageId,
        /* [out] */ IMtpStorageInfo** result);

    CARAPI GetObjectInfo(
        /* [in] */ Int32 objectHandle,
        /* [out] */ IMtpObjectInfo** result);

    CARAPI DeleteObject(
        /* [in] */ Int32 objectHandle,
        /* [out] */ Boolean* result);

    CARAPI GetParent(
        /* [in] */ Int32 objectHandle,
        /* [out] */ Int64* result);

    CARAPI GetStorageId(
        /* [in] */ Int32 objectHandle,
        /* [out]*/ Int64* result);

    CARAPI ImportFile(
        /* [in] */ Int32 objectHandle,
        /* [in] */ const String& destPath,
        /* [out] */ Boolean* result);

protected:
    CARAPI_(void) Finalize();

private:
    CARAPI_(Boolean) NativeOpen(
        /* [in] */ const String& deviceName,
        /* [in] */ Int32 fd);

    CARAPI_(void) NativeClose();

    CARAPI_(AutoPtr<IMtpDeviceInfo>) NativeGetDeviceInfo();

    CARAPI_(AutoPtr<ArrayOf<Int32> >) NativeGetStorageIds();

    CARAPI_(AutoPtr<IMtpStorageInfo>) NativeGetStorageInfo(
        /* [in] */ Int32 storageId);

    CARAPI_(AutoPtr<ArrayOf<Int32> >) NativeGetObjectHandles(
        /* [in] */ Int32 storageId,
        /* [in] */ Int32 format,
        /* [in] */ Int32 objectHandle);

    CARAPI_(AutoPtr<IMtpObjectInfo>) NativeGetObjectInfo(
        /* [in] */ Int32 objectHandle);

    CARAPI_(AutoPtr<ArrayOf<Byte> >) NativeGetObject(
        /* [in]*/ Int32 objectHandle,
        /* [in]*/ Int32 objectSize);

    CARAPI_(AutoPtr<ArrayOf<Byte> >) NativeGetThumbnail(
        /* [in] */ Int32 objectHandle);

    CARAPI_(Boolean) NativeDeleteObject(
        /* [in] */ Int32 objectHandle);

    CARAPI_(Int64) NativeGetParent(
        /* [in] */ Int32 objectHandle);

    CARAPI_(Int64) NativeGetStorageId(
        /* [in] */ Int32 objectHandle);

    CARAPI_(Boolean) NativeImportFile(
        /* [in] */ Int32 objectHandle,
        /* [in] */ const String& destPath);

private:
    static const String TAG;
    AutoPtr<IUsbDevice> mDevice;
    Int64 mNativeContext;

};

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MTP_CMTPDEVICE_H__

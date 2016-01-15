
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/mtp/CMtpDevice.h"
#include "elastos/droid/mtp/CMtpDeviceInfo.h"
#include "elastos/droid/mtp/CMtpStorageInfo.h"
#include "elastos/droid/mtp/CMtpObjectInfo.h"

#include "MediaMtp/MtpTypes.h"
#include "MediaMtp/MtpDevice.h"
#include "MediaMtp/MtpDeviceInfo.h"
#include "MediaMtp/MtpStorageInfo.h"
#include "MediaMtp/MtpObjectInfo.h"

using namespace android;
using Elastos::Droid::Mtp::CMtpDeviceInfo;
using Elastos::Droid::Mtp::CMtpStorageInfo;
using Elastos::Droid::Mtp::CMtpObjectInfo;

namespace Elastos {
namespace Droid {
namespace Mtp {

const String CMtpDevice::TAG("MtpDevice");

CMtpDevice::CMtpDevice()
    : mNativeContext(0)
{}

CAR_INTERFACE_IMPL(CMtpDevice, Object, IMtpDevice)

CAR_OBJECT_IMPL(CMtpDevice)

ECode CMtpDevice::constructor(
        /* [in] */ IUsbDevice* device)
{
    mDevice = device;
    return NOERROR;
}

/**
 * Opens the MTP device.  Once the device is open it takes ownership of the
 * {@link android.hardware.usb.UsbDeviceConnection}.
 * The connection will be closed when you call {@link #close()}
 * The connection will also be closed if this method fails.
 *
 * @param connection an open {@link android.hardware.usb.UsbDeviceConnection} for the device
 * @return true if the device was successfully opened.
 */
ECode CMtpDevice::Open(
        /* [in] */ IUsbDeviceConnection* connection,
        /* [out]*/ Boolean* mResult)
{
    VALIDATE_NOT_NULL(mResult);

    String devName;
    mDevice->GetDeviceName(&devName);
    Int32 desc;
    connection->GetFileDescriptor(&desc);
    Boolean result = NativeOpen(devName, desc);
    if (!result) {
        connection->Close();
    }
    *mResult = result;
    return NOERROR;
}

/**
 * Closes all resources related to the MtpDevice object.
 * After this is called, the object can not be used until {@link #open} is called again
 * with a new {@link android.hardware.usb.UsbDeviceConnection}.
 */
ECode CMtpDevice::Close()
{
    NativeClose();
    return NOERROR;
}

void CMtpDevice::Finalize()
{
    //try {
        NativeClose();
    //} finally {
    //    super.finalize();
    //}
}

/**
 * Returns the name of the USB device
 * This returns the same value as {@link android.hardware.usb.UsbDevice#getDeviceName}
 * for the device's {@link android.hardware.usb.UsbDevice}
 *
 * @return the device name
 */
ECode CMtpDevice::GetDeviceName(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    return mDevice->GetDeviceName(result);
}

/**
 * Returns the USB ID of the USB device.
 * This returns the same value as {@link android.hardware.usb.UsbDevice#getDeviceId}
 * for the device's {@link android.hardware.usb.UsbDevice}
 *
 * @return the device ID
 */
ECode CMtpDevice::GetDeviceId(
        /* [out]*/ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    return mDevice->GetDeviceId(result);
}

ECode CMtpDevice::ToString(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    return mDevice->GetDeviceName(result);
}

/**
 * Returns the {@link MtpDeviceInfo} for this device
 *
 * @return the device info
 */
ECode CMtpDevice::GetDeviceInfo(
        /* [out] */ IMtpDeviceInfo** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IMtpDeviceInfo> temp = NativeGetDeviceInfo();
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Returns the list of IDs for all storage units on this device
 * Information about each storage unit can be accessed via {@link #getStorageInfo}.
 *
 * @return the list of storage IDs
 */
ECode CMtpDevice::GetStorageIds(
        /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ArrayOf<Int32> > temp = NativeGetStorageIds();
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Returns the list of object handles for all objects on the given storage unit,
 * with the given format and parent.
 * Information about each object can be accessed via {@link #getObjectInfo}.
 *
 * @param storageId the storage unit to query
 * @param format the format of the object to return, or zero for all formats
 * @param objectHandle the parent object to query, or zero for the storage root
 * @return the object handles
 */
ECode CMtpDevice::GetObjectHandles(
    /* [in] */ Int32 storageId,
    /* [in] */ Int32 format,
    /* [in] */ Int32 objectHandle,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ArrayOf<Int32> > temp = NativeGetObjectHandles(storageId, format, objectHandle);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Returns the data for an object as a byte array.
 * This call may block for an arbitrary amount of time depending on the size
 * of the data and speed of the devices.
 *
 * @param objectHandle handle of the object to read
 * @param objectSize the size of the object (this should match
 *      {@link MtpObjectInfo#getCompressedSize}
 * @return the object's data, or null if reading fails
 */
ECode CMtpDevice::GetObject(
    /* [in] */ Int32 objectHandle,
    /* [in] */ Int32 objectSize,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ArrayOf<Byte> > temp = NativeGetObject(objectHandle, objectSize);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Returns the thumbnail data for an object as a byte array.
 * The size and format of the thumbnail data can be determined via
 * {@link MtpObjectInfo#getThumbCompressedSize} and
 * {@link MtpObjectInfo#getThumbFormat}.
 * For typical devices the format is JPEG.
 *
 * @param objectHandle handle of the object to read
 * @return the object's thumbnail, or null if reading fails
 */
ECode CMtpDevice::GetThumbnail(
    /* [in] */ Int32 objectHandle,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ArrayOf<Byte> > temp = NativeGetThumbnail(objectHandle);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Retrieves the {@link MtpStorageInfo} for a storage unit.
 *
 * @param storageId the ID of the storage unit
 * @return the MtpStorageInfo
 */
ECode CMtpDevice::GetStorageInfo(
    /* [in] */ Int32 storageId,
    /* [out] */ IMtpStorageInfo** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IMtpStorageInfo> temp = NativeGetStorageInfo(storageId);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Retrieves the {@link MtpObjectInfo} for an object.
 *
 * @param objectHandle the handle of the object
 * @return the MtpObjectInfo
 */
ECode CMtpDevice::GetObjectInfo(
    /* [in] */ Int32 objectHandle,
    /* [out] */ IMtpObjectInfo** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IMtpObjectInfo> temp = NativeGetObjectInfo(objectHandle);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Deletes an object on the device.  This call may block, since
 * deleting a directory containing many files may take a long time
 * on some devices.
 *
 * @param objectHandle handle of the object to delete
 * @return true if the deletion succeeds
 */
ECode CMtpDevice::DeleteObject(
    /* [in] */ Int32 objectHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NativeDeleteObject(objectHandle);
    return NOERROR;
}

/**
 * Retrieves the object handle for the parent of an object on the device.
 *
 * @param objectHandle handle of the object to query
 * @return the parent's handle, or zero if it is in the root of the storage
 */
ECode CMtpDevice::GetParent(
    /* [in] */ Int32 objectHandle,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeGetParent(objectHandle);
    return NOERROR;
}

/**
 * Retrieves the ID of the storage unit containing the given object on the device.
 *
 * @param objectHandle handle of the object to query
 * @return the object's storage unit ID
 */
ECode CMtpDevice::GetStorageId(
    /* [in] */ Int32 objectHandle,
    /* [out]*/ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeGetStorageId(objectHandle);
    return NOERROR;
}

/**
 * Copies the data for an object to a file in external storage.
 * This call may block for an arbitrary amount of time depending on the size
 * of the data and speed of the devices.
 *
 * @param objectHandle handle of the object to read
 * @param destPath path to destination for the file transfer.
 *      This path should be in the external storage as defined by
 *      {@link android.os.Environment#getExternalStorageDirectory}
 * @return true if the file transfer succeeds
 */
ECode CMtpDevice::ImportFile(
    /* [in] */ Int32 objectHandle,
    /* [in] */ const String& destPath,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeImportFile(objectHandle, destPath);
    return NOERROR;
}

Boolean CMtpDevice::NativeOpen(
    /* [in] */ const String& deviceName,
    /* [in] */ Int32 fd)
{
    const char *deviceNameStr = deviceName.string();
    if (deviceNameStr == NULL) {
        return FALSE;
    }

    MtpDevice* device = MtpDevice::open(deviceNameStr, fd);

    if (device)
        mNativeContext = (Int32)device;
    return (device != NULL);
}

void CMtpDevice::NativeClose()
{
    MtpDevice* device = (MtpDevice*)mNativeContext;
    if (device) {
        device->close();
        delete device;
        mNativeContext = 0;
    }
}

AutoPtr<IMtpDeviceInfo> CMtpDevice::NativeGetDeviceInfo()
{
    MtpDevice* device = (MtpDevice*)mNativeContext;
    if (!device) {
    //    ALOGD("android_mtp_MtpDevice_get_device_info device is null");
        return NULL;
    }
    MtpDeviceInfo* deviceInfo = device->getDeviceInfo();
    if (!deviceInfo) {
    //    ALOGD("android_mtp_MtpDevice_get_device_info deviceInfo is null");
        return NULL;
    }

    AutoPtr<IMtpDeviceInfo> info;
    CMtpDeviceInfo::New((IMtpDeviceInfo**)&info);
    if (info == NULL) {
    //    ALOGE("Could not create a MtpDeviceInfo object");
        delete deviceInfo;
        return NULL;
    }

    if (deviceInfo->mManufacturer)
        info->SetManufacturer(String(deviceInfo->mManufacturer));
    if (deviceInfo->mModel)
        info->SetModel(String(deviceInfo->mModel));
    if (deviceInfo->mVersion)
        info->SetVersion(String(deviceInfo->mVersion));
    if (deviceInfo->mSerial)
        info->SetSerialNumber(String(deviceInfo->mSerial));

    delete deviceInfo;
    return info;
}

AutoPtr<ArrayOf<Int32> > CMtpDevice::NativeGetStorageIds()
{
    MtpDevice* device = (MtpDevice*)mNativeContext;
    if (!device)
        return NULL;
    MtpStorageIDList* storageIDs = device->getStorageIDs();
    if (!storageIDs)
        return NULL;

    Int32 length = storageIDs->size();
    AutoPtr< ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(length);
    // FIXME is this cast safe?
    //// env->SetIntArrayRegion(array, 0, length, (const Int32 *)storageIDs->array());
    for(Int32 i = 0; i < length; i++) {
        array->Set(i, (*storageIDs)[i]);
    }

    delete storageIDs;
    return array;
}

AutoPtr<IMtpStorageInfo> CMtpDevice::NativeGetStorageInfo(
    /* [in] */ Int32 storageId)
{
    MtpDevice* device = (MtpDevice*)mNativeContext;
    if (!device)
        return NULL;
    MtpStorageInfo* storageInfo = device->getStorageInfo(storageId);
    if (!storageInfo)
        return NULL;

    AutoPtr<IMtpStorageInfo> info;
    CMtpStorageInfo::New((IMtpStorageInfo**)&info);
    if (info == NULL) {
    //    ALOGE("Could not create a MtpStorageInfo object");
        delete storageInfo;
        return NULL;
    }

    if (storageInfo->mStorageID)
        info->SetStorageId(storageInfo->mStorageID);
    if (storageInfo->mMaxCapacity)
        info->SetMaxCapacity(storageInfo->mMaxCapacity);
    if (storageInfo->mFreeSpaceBytes)
        info->SetFreeSpace(storageInfo->mFreeSpaceBytes);
    if (storageInfo->mStorageDescription)
        info->SetDescription(String(storageInfo->mStorageDescription));
    if (storageInfo->mVolumeIdentifier)
        info->SetVolumeIdentifier(String(storageInfo->mVolumeIdentifier));

    delete storageInfo;
    return info;
}

AutoPtr<ArrayOf<Int32> > CMtpDevice::NativeGetObjectHandles(
    /* [in] */ Int32 storageId,
    /* [in] */ Int32 format,
    /* [in] */ Int32 objectHandle)
{
    MtpDevice* device = (MtpDevice*)mNativeContext;
    if (!device)
        return NULL;
    MtpObjectHandleList* handles = device->getObjectHandles(storageId, format, objectHandle);
    if (!handles)
        return NULL;

    Int32 length = handles->size();
    AutoPtr< ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(length);
    // FIXME is this cast safe?
    //// env->SetIntArrayRegion(array, 0, length, (const jint *)handles->array());
    for(Int32 i = 0; i < length; i++) {
        array->Set(i, (*handles)[i]);
    }

    delete handles;
    return array;
}

AutoPtr<IMtpObjectInfo> CMtpDevice::NativeGetObjectInfo(
    /* [in] */ Int32 objectHandle)
{
    MtpDevice* device = (MtpDevice*)mNativeContext;
    if (!device)
        return NULL;
    MtpObjectInfo* objectInfo = device->getObjectInfo(objectHandle);
    if (!objectInfo)
        return NULL;
    AutoPtr<IMtpObjectInfo> info;
    CMtpObjectInfo::New((IMtpObjectInfo**)&info);
    if (info == NULL) {
    //    ALOGE("Could not create a MtpObjectInfo object");
        delete objectInfo;
        return NULL;
    }

    if (objectInfo->mHandle)
        info->SetObjectHandle(objectInfo->mHandle);
    if (objectInfo->mStorageID)
        info->SetStorageId(objectInfo->mStorageID);
    if (objectInfo->mFormat)
        info->SetFormat(objectInfo->mFormat);
    if (objectInfo->mProtectionStatus)
        info->SetProtectionStatus(objectInfo->mProtectionStatus);
    if (objectInfo->mCompressedSize)
        info->SetCompressedSize(objectInfo->mCompressedSize);
    if (objectInfo->mThumbFormat)
        info->SetThumbFormat(objectInfo->mThumbFormat);
    if (objectInfo->mThumbCompressedSize)
        info->SetThumbCompressedSize(objectInfo->mThumbCompressedSize);
    if (objectInfo->mThumbPixWidth)
        info->SetThumbPixWidth(objectInfo->mThumbPixWidth);
    if (objectInfo->mThumbPixHeight)
        info->SetThumbPixHeight(objectInfo->mThumbPixHeight);
    if (objectInfo->mImagePixWidth)
        info->SetImagePixWidth(objectInfo->mImagePixWidth);
    if (objectInfo->mImagePixHeight)
        info->SetImagePixHeight(objectInfo->mImagePixHeight);
    if (objectInfo->mImagePixDepth)
        info->SetImagePixDepth(objectInfo->mImagePixDepth);
    if (objectInfo->mParent)
        info->SetParent(objectInfo->mParent);
    if (objectInfo->mAssociationType)
        info->SetAssociationType(objectInfo->mAssociationType);
    if (objectInfo->mAssociationDesc)
        info->SetAssociationDesc(objectInfo->mAssociationDesc);
    if (objectInfo->mSequenceNumber)
        info->SetSequenceNumber(objectInfo->mSequenceNumber);
    if (objectInfo->mName)
        info->SetName(String(objectInfo->mName));
    if (objectInfo->mDateCreated)
        info->SetDateCreated(objectInfo->mDateCreated * 1000LL);
    if (objectInfo->mDateModified)
        info->SetDateModified(objectInfo->mDateModified * 1000LL);
    if (objectInfo->mKeywords)
        info->SetKeywords(String(objectInfo->mKeywords));

    delete objectInfo;
    return info;
}

struct get_object_callback_data {
    AutoPtr<ArrayOf<Byte> > array;
};

static bool get_object_callback(void* data, int offset, int length, void* clientData)
{
    get_object_callback_data* cbData = (get_object_callback_data *)clientData;
    //// cbData->env->SetByteArrayRegion(cbData->array, offset, length, (jbyte *)data);
    for(Int32 i = offset; i < length; i++) {
        cbData->array->Set(i, ((Byte*)data)[i]);
    }

    return true;
}

AutoPtr<ArrayOf<Byte> > CMtpDevice::NativeGetObject(
    /* [in]*/ Int32 objectHandle,
    /* [in]*/ Int32 objectSize)
{
    MtpDevice* device = (MtpDevice*)mNativeContext;
    if (!device)
        return NULL;

    AutoPtr< ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(objectSize);
    if (!array) {
    //    jniThrowException(env, "java/lang/OutOfMemoryError", NULL);
        return NULL;
    }

    get_object_callback_data data;
    data.array = array;

    if (device->readObject(objectHandle, get_object_callback, objectSize, &data))
        return array;
    return NULL;
}

AutoPtr<ArrayOf<Byte> > CMtpDevice::NativeGetThumbnail(
    /* [in] */ Int32 objectHandle)
{
    MtpDevice* device = (MtpDevice*)mNativeContext;
    if (!device)
        return NULL;

    Int32 length;
    Byte* thumbnail = (Byte*)device->getThumbnail(objectHandle, length);
    if (! thumbnail)
        return NULL;
    AutoPtr< ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(length);
    //// env->SetByteArrayRegion(array, 0, length, (const jbyte *)thumbnail);
    for(Int32 i = 0; i < length; i++) {
        array->Set(i, thumbnail[i]);
    }

    free(thumbnail);
    return array;
}

Boolean CMtpDevice::NativeDeleteObject(
    /* [in] */ Int32 objectHandle)
{
    MtpDevice* device = (MtpDevice*)mNativeContext;
    if (device && device->deleteObject(objectHandle)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

Int64 CMtpDevice::NativeGetParent(
    /* [in] */ Int32 objectHandle)
{
    MtpDevice* device = (MtpDevice*)mNativeContext;
    if (device)
        return device->getParent(objectHandle);
    else
        return -1;
}

Int64 CMtpDevice::NativeGetStorageId(
    /* [in] */ Int32 objectHandle)
{
    MtpDevice* device = (MtpDevice*)mNativeContext;
    if (device)
        return device->getStorageID(objectHandle);
    else
        return -1;
}

Boolean CMtpDevice::NativeImportFile(
    /* [in] */ Int32 objectHandle,
    /* [in] */ const String& destPath)
{
    MtpDevice* device = (MtpDevice*)mNativeContext;
    if (device) {
        const char *destPathStr = destPath.string();
        if (destPathStr == NULL) {
            return FALSE;
        }

        Boolean result = device->readObject(objectHandle, destPathStr, 1015/*AID_SDCARD_RW*/, 0664);
        return result;
    }

    return FALSE;
}

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos

#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/utility/DrmConvertSession.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

//=====================================================================
//                          DrmConvertSession
//=====================================================================
CAR_INTERFACE_IMPL(DrmConvertSession, Object, IDrmConvertSession);

const String DrmConvertSession::TAG("DrmConvertSession");

AutoPtr<IDrmConvertSession> DrmConvertSession::Open(
    /* [in] */ IContext* context,
    /* [in] */ const String& mimeType)
{
    // ==================before translated======================
    // DrmManagerClient drmClient = null;
    // int convertSessionId = -1;
    // if (context != null && mimeType != null && !mimeType.equals("")) {
    //     try {
    //         drmClient = new DrmManagerClient(context);
    //         try {
    //             convertSessionId = drmClient.openConvertSession(mimeType);
    //         } catch (IllegalArgumentException e) {
    //             Log.w(TAG, "Conversion of Mimetype: " + mimeType
    //                     + " is not supported.", e);
    //         } catch (IllegalStateException e) {
    //             Log.w(TAG, "Could not access Open DrmFramework.", e);
    //         }
    //     } catch (IllegalArgumentException e) {
    //         Log.w(TAG,
    //                 "DrmManagerClient instance could not be created, context is Illegal.");
    //     } catch (IllegalStateException e) {
    //         Log.w(TAG, "DrmManagerClient didn't initialize properly.");
    //     }
    // }
    //
    // if (drmClient == null || convertSessionId < 0) {
    //     return null;
    // } else {
    //     return new DrmConvertSession(drmClient, convertSessionId);
    // }
    assert(0);
    AutoPtr<DrmConvertSession> empty;
    return empty;
}

ECode DrmConvertSession::Convert(
    /* [in] */ ArrayOf<Byte>* inBuffer,
    /* [in] */ Int32 size,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(inBuffer);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // byte[] result = null;
    // if (inBuffer != null) {
    //     DrmConvertedStatus convertedStatus = null;
    //     try {
    //         if (size != inBuffer.length) {
    //             byte[] buf = new byte[size];
    //             System.arraycopy(inBuffer, 0, buf, 0, size);
    //             convertedStatus = mDrmClient.convertData(mConvertSessionId, buf);
    //         } else {
    //             convertedStatus = mDrmClient.convertData(mConvertSessionId, inBuffer);
    //         }
    //
    //         if (convertedStatus != null &&
    //                 convertedStatus.statusCode == DrmConvertedStatus.STATUS_OK &&
    //                 convertedStatus.convertedData != null) {
    //             result = convertedStatus.convertedData;
    //         }
    //     } catch (IllegalArgumentException e) {
    //         Log.w(TAG, "Buffer with data to convert is illegal. Convertsession: "
    //                 + mConvertSessionId, e);
    //     } catch (IllegalStateException e) {
    //         Log.w(TAG, "Could not convert data. Convertsession: " +
    //                 mConvertSessionId, e);
    //     }
    // } else {
    //     throw new IllegalArgumentException("Parameter inBuffer is null");
    // }
    // return result;
    assert(0);
    return NOERROR;
}

ECode DrmConvertSession::Close(
    /* [in] */ const String& filename,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // DrmConvertedStatus convertedStatus = null;
    // int result = Downloads.Impl.STATUS_UNKNOWN_ERROR;
    // if (mDrmClient != null && mConvertSessionId >= 0) {
    //     try {
    //         convertedStatus = mDrmClient.closeConvertSession(mConvertSessionId);
    //         if (convertedStatus == null ||
    //                 convertedStatus.statusCode != DrmConvertedStatus.STATUS_OK ||
    //                 convertedStatus.convertedData == null) {
    //             result = Downloads.Impl.STATUS_NOT_ACCEPTABLE;
    //         } else {
    //             RandomAccessFile rndAccessFile = null;
    //             try {
    //                 rndAccessFile = new RandomAccessFile(filename, "rw");
    //                 rndAccessFile.seek(convertedStatus.offset);
    //                 rndAccessFile.write(convertedStatus.convertedData);
    //                 result = Downloads.Impl.STATUS_SUCCESS;
    //             } catch (FileNotFoundException e) {
    //                 result = Downloads.Impl.STATUS_FILE_ERROR;
    //                 Log.w(TAG, "File: " + filename + " could not be found.", e);
    //             } catch (IOException e) {
    //                 result = Downloads.Impl.STATUS_FILE_ERROR;
    //                 Log.w(TAG, "Could not access File: " + filename + " .", e);
    //             } catch (IllegalArgumentException e) {
    //                 result = Downloads.Impl.STATUS_FILE_ERROR;
    //                 Log.w(TAG, "Could not open file in mode: rw", e);
    //             } catch (SecurityException e) {
    //                 Log.w(TAG, "Access to File: " + filename +
    //                         " was denied denied by SecurityManager.", e);
    //             } finally {
    //                 if (rndAccessFile != null) {
    //                     try {
    //                         rndAccessFile.close();
    //                     } catch (IOException e) {
    //                         result = Downloads.Impl.STATUS_FILE_ERROR;
    //                         Log.w(TAG, "Failed to close File:" + filename
    //                                 + ".", e);
    //                     }
    //                 }
    //             }
    //         }
    //     } catch (IllegalStateException e) {
    //         Log.w(TAG, "Could not close convertsession. Convertsession: " +
    //                 mConvertSessionId, e);
    //     }
    // }
    // return result;
    assert(0);
    return NOERROR;
}

DrmConvertSession::DrmConvertSession(
    /* [in] */ IInterface/*TODO IDrmManagerClient*/* drmClient,
    /* [in] */ Int32 convertSessionId)
{
    // ==================before translated======================
    // mDrmClient = drmClient;
    // mConvertSessionId = convertSessionId;
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos



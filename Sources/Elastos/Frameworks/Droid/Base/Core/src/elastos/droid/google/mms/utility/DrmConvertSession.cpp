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
    assert(0 && "TODO");
    // AutoPtr<IDrmManagerClient> drmClient;
    // Int32 convertSessionId = -1;
    // if (context != NULL && !mimeType.IsNull() && !mimeType.Equals("")) {
    //     drmClient = new DrmManagerClient(context);
    //     convertSessionId = drmClient.openConvertSession(mimeType);

    // }

    // if (drmClient == NULL || convertSessionId < 0) {
    //     return NULL;
    // }
    // else {
    //     AutoPtr<DrmConvertSession> result = new DrmConvertSession(drmClient, convertSessionId);
    //     return result;
    // }
    return NULL;
}

ECode DrmConvertSession::Convert(
    /* [in] */ ArrayOf<Byte>* inBuffer,
    /* [in] */ Int32 size,
    /* [out] */ ArrayOf<Byte>** res)
{
    VALIDATE_NOT_NULL(inBuffer);
    VALIDATE_NOT_NULL(res);
    AutoPtr<ArrayOf<Byte> > result;
    if (inBuffer != NULL) {
        assert(0 && "TODO");
        // AutoPtr<IDrmConvertedStatus> convertedStatus;
        // if (size != inBuffer->GetLength()) {
        //     AutoPtr<ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(size);
        //     // System.arraycopy(inBuffer, 0, buf, 0, size);
        //     buf->Copy(0, inBuffer, 0, size);
        //     mDrmClient->ConvertData(mConvertSessionId, buf, (IDrmConvertedStatus**)&convertedStatus);
        // }
        // else {
        //     mDrmClient->ConvertData(mConvertSessionId, inBuffer,(IDrmConvertedStatus**)&convertedStatus);
        // }

        // if (convertedStatus != NULL &&
        //         convertedStatus->mStatusCode == IDrmConvertedStatus::STATUS_OK &&
        //         convertedStatus->mConvertedData != NULL) {
        //     result = convertedStatus->mConvertedData;
        // }
    }
    else {
        // throw new IllegalArgumentException("Parameter inBuffer is NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *res = result;
    return NOERROR;
}

ECode DrmConvertSession::Close(
    /* [in] */ const String& filename,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);
    assert(0 && "TODO");
    // AutoPtr<IDrmConvertedStatus> convertedStatus;
    // Int32 result = IDownloadsImpl::STATUS_UNKNOWN_ERROR;
    // if (mDrmClient != NULL && mConvertSessionId >= 0) {
    //     mDrmClient->CloseConvertSession(mConvertSessionId, (IDrmConvertedStatus**)&convertedStatus);
    //     if (convertedStatus == NULL ||
    //             convertedStatus->mStatusCode != IDrmConvertedStatus::STATUS_OK ||
    //             convertedStatus->mConvertedData == NULL) {
    //         result = IDownloadsImpl::STATUS_NOT_ACCEPTABLE;
    //     }
    //     else {
    //         AutoPtr<IRandomAccessFile> rndAccessFile;
    //         CRandomAccessFile::New(filename, "rw", (IRandomAccessFile**)&rndAccessFile);
    //         rndAccessFile->Seek(convertedStatus->mOffset);
    //         rndAccessFile->Write(convertedStatus->mConvertedData);
    //         result = IDownloadsImpl::STATUS_SUCCESS;
    //         if (rndAccessFile != NULL) {
    //             rndAccessFile->Close();
    //         }
    //     }
    // }
    // *res = result;
    return NOERROR;
}

DrmConvertSession::DrmConvertSession(
    /* [in] */ IInterface/*TODO IDrmManagerClient*/* drmClient,
    /* [in] */ Int32 convertSessionId)
{
    mDrmClient = drmClient;
    mConvertSessionId = convertSessionId;
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos



#include "Elastos.Droid.Google.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/google/mms/pdu/PduPart.h"

#include <elastos/core/CoreUtils.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                               PduPart
//=====================================================================
CAR_INTERFACE_IMPL(PduPart, Object, IPduPart);

const String PduPart::TAG("PduPart");

AutoPtr<ArrayOf<Byte> > PduPart::DISPOSITION_FROM_DATA = String("from-data").GetBytes();
AutoPtr<ArrayOf<Byte> > PduPart::DISPOSITION_ATTACHMENT = String("attachment").GetBytes();
AutoPtr<ArrayOf<Byte> > PduPart::DISPOSITION_INLINE = String("inline").GetBytes();

PduPart::PduPart()
{
    CHashMap::New((IHashMap**)&mPartHeader);
}

ECode PduPart::SetData(
    /* [in] */ ArrayOf<Byte>* data)
{
    VALIDATE_NOT_NULL(data);
    if (data == NULL) {
        return NOERROR;
    }

    mPartData = ArrayOf<Byte>::Alloc(data->GetLength());
    // System::Arraycopy(data, 0, mPartData, 0, data->GetLength());
    mPartData->Copy(0, data, 0, data->GetLength());
    return NOERROR;
}

ECode PduPart::GetData(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    if (mPartData == NULL) {
       *result = NULL;
       return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > byteArray = ArrayOf<Byte>::Alloc(mPartData->GetLength());
    // System::Arraycopy(mPartData, 0, byteArray, 0, mPartData->GetLength());
    byteArray->Copy(0, mPartData, 0, mPartData->GetLength());
    *result = byteArray;
    return NOERROR;
}

ECode PduPart::GetDataLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mPartData != NULL){
        *result = mPartData->GetLength();
    }
    else {
        *result = 0;
    }
    return NOERROR;
}

ECode PduPart::SetDataUri(
    /* [in] */ IUri* uri)
{
    VALIDATE_NOT_NULL(uri);
    mUri = uri;
    return NOERROR;
}

ECode PduPart::GetDataUri(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUri;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PduPart::SetContentId(
    /* [in] */ ArrayOf<Byte>* contentId)
{
    VALIDATE_NOT_NULL(contentId);
    if ((contentId == NULL) || (contentId->GetLength() == 0)) {
        // throw new IllegalArgumentException(
        //         "Content-Id may not be null or empty.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if ((contentId->GetLength() > 1)
            && ((Char32) (*contentId)[0] == '<')
            && ((Char32) (*contentId)[contentId->GetLength() - 1] == '>')) {
        mPartHeader->Put(CoreUtils::Convert(P_CONTENT_ID), CoreUtils::Convert(String(*contentId)));
        return NOERROR;
    }

    // Insert beginning '<' and trailing '>' for Content-Id.
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(contentId->GetLength() + 2);
    (*buffer)[0] = (Byte) (0xff & '<');
    (*buffer)[buffer->GetLength() - 1] = (Byte) (0xff & '>');
    // System::Arraycopy(contentId, 0, buffer, 1, contentId->GetLength());
    buffer->Copy(1, contentId, 0, contentId->GetLength());
    mPartHeader->Put(CoreUtils::Convert(P_CONTENT_ID), CoreUtils::Convert(String(*buffer)));
    return NOERROR;
}

ECode PduPart::GetContentId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mPartHeader->Get(CoreUtils::Convert(P_CONTENT_ID), (IInterface**)&p);
    AutoPtr<IArrayOf> array = IArrayOf::Probe(p);
    Int32 len = 0;
    array->GetLength(&len);
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        array->Get(i, (IInterface**)&obj);
        IByte::Probe(obj)->GetValue(&(*bytes)[i]);
    }
    *result = bytes;
    return NOERROR;
}

ECode PduPart::SetCharset(
    /* [in] */ Int32 charset)
{
    mPartHeader->Put(CoreUtils::Convert(P_CHARSET), CoreUtils::Convert(charset));
    return NOERROR;
}

ECode PduPart::GetCharset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mPartHeader->Get(CoreUtils::Convert(P_CHARSET), (IInterface**)&p);
    AutoPtr<IInteger32> charset = IInteger32::Probe(p);
    if(charset == NULL) {
        *result = 0;
        return NOERROR;
    }
    else {
        return charset->GetValue(result);
    }
    return NOERROR;
}

ECode PduPart::SetContentLocation(
    /* [in] */ ArrayOf<Byte>* contentLocation)
{
    VALIDATE_NOT_NULL(contentLocation)
    if (contentLocation == NULL) {
        // throw new NullPointerException("NULL content-location");
        return E_NULL_POINTER_EXCEPTION;
    }

    mPartHeader->Put(CoreUtils::Convert(P_CONTENT_LOCATION), CoreUtils::Convert(String(*contentLocation)));
    return NOERROR;
}

ECode PduPart::GetContentLocation(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> p;
    mPartHeader->Get(CoreUtils::Convert(P_CONTENT_LOCATION), (IInterface**)&p);
    *result = (ArrayOf<Byte>*)p.Get();
    return NOERROR;
}

ECode PduPart::SetContentDisposition(
    /* [in] */ ArrayOf<Byte>* contentDisposition)
{
    VALIDATE_NOT_NULL(contentDisposition);
    if (contentDisposition == NULL) {
        // throw new NullPointerException("NULL content-disposition");
        return E_NULL_POINTER_EXCEPTION;
    }

    mPartHeader->Put(CoreUtils::Convert(P_CONTENT_DISPOSITION), CoreUtils::Convert(String(*contentDisposition)));
    return NOERROR;
}

ECode PduPart::GetContentDisposition(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> p;
    mPartHeader->Get(CoreUtils::Convert(P_CONTENT_DISPOSITION), (IInterface**)&p);
    *result = (ArrayOf<Byte>*)p.Get();
    return NOERROR;
}

ECode PduPart::SetContentType(
    /* [in] */ ArrayOf<Byte>* contentType)
{
    VALIDATE_NOT_NULL(contentType);
    if (contentType == NULL) {
        // throw new NullPointerException("NULL content-type");
        return E_NULL_POINTER_EXCEPTION;
    }

    mPartHeader->Put(CoreUtils::Convert(P_CONTENT_TYPE), CoreUtils::Convert(String(*contentType)));
    return NOERROR;
}

ECode PduPart::GetContentType(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mPartHeader->Get(CoreUtils::Convert(P_CONTENT_TYPE), (IInterface**)&p);
    *result = (ArrayOf<Byte>*)p.Get();
    return NOERROR;
}

ECode PduPart::SetContentTransferEncoding(
    /* [in] */ ArrayOf<Byte>* contentTransferEncoding)
{
    VALIDATE_NOT_NULL(contentTransferEncoding);
    if (contentTransferEncoding == NULL) {
        // throw new NullPointerException("NULL content-transfer-encoding");
        return E_NULL_POINTER_EXCEPTION;
    }

    mPartHeader->Put(CoreUtils::Convert(P_CONTENT_TRANSFER_ENCODING), CoreUtils::Convert(String(*contentTransferEncoding)));
    return NOERROR;
}

ECode PduPart::GetContentTransferEncoding(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mPartHeader->Get(CoreUtils::Convert(P_CONTENT_TRANSFER_ENCODING), (IInterface**)&p);
    *result = (ArrayOf<Byte>*)p.Get();
    return NOERROR;
}

ECode PduPart::SetName(
    /* [in] */ ArrayOf<Byte>* name)
{
    VALIDATE_NOT_NULL(name);
    if (NULL == name) {
        // throw new NullPointerException("NULL content-id");
        return E_NULL_POINTER_EXCEPTION;
    }

    mPartHeader->Put(CoreUtils::Convert(P_NAME), CoreUtils::Convert(String(*name)));
    return NOERROR;
}

ECode PduPart::GetName(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mPartHeader->Get(CoreUtils::Convert(P_NAME), (IInterface**)&p);
    *result = (ArrayOf<Byte>*)p.Get();
    return NOERROR;
}

ECode PduPart::SetFilename(
    /* [in] */ ArrayOf<Byte>* fileName)
{
    VALIDATE_NOT_NULL(fileName);
    if (NULL == fileName) {
        // throw new NullPointerException("NULL content-id");
        return E_NULL_POINTER_EXCEPTION;
    }

    mPartHeader->Put(CoreUtils::Convert(P_FILENAME), CoreUtils::Convert(String(*fileName)));
    return NOERROR;
}

ECode PduPart::GetFilename(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mPartHeader->Get(CoreUtils::Convert(P_FILENAME), (IInterface**)&p);
    *result = (ArrayOf<Byte>*)p.Get();
    return NOERROR;
}

ECode PduPart::GenerateLocation(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // Assumption: At least one of the content-location / name / filename
    // or content-id should be set. This is guaranteed by the PduParser
    // for incoming messages and by MM composer for outgoing messages.
    AutoPtr<IInterface> pName;
    mPartHeader->Get(CoreUtils::Convert(P_NAME), (IInterface**)&pName);
    String name;
    ICharSequence::Probe(pName)->ToString(&name);
    AutoPtr<ArrayOf<Byte> > location = name.GetBytes();
    if(NULL == location) {
        AutoPtr<IInterface> pFileName;
        mPartHeader->Get(CoreUtils::Convert(P_FILENAME), (IInterface**)&pFileName);
        String fileName;
        ICharSequence::Probe(pFileName)->ToString(&fileName);
        location = fileName.GetBytes();

        if (NULL == location) {
            AutoPtr<IInterface> pLocation;
            mPartHeader->Get(CoreUtils::Convert(P_CONTENT_LOCATION), (IInterface**)&pLocation);
            String loc;
            ICharSequence::Probe(pLocation)->ToString(&loc);
            location = loc.GetBytes();
        }
    }

    if (NULL == location) {
        AutoPtr<IInterface> pContentId;
        mPartHeader->Get(CoreUtils::Convert(P_CONTENT_ID), (IInterface**)&pContentId);
        String id;
        ICharSequence::Probe(pContentId)->ToString(&id);
        AutoPtr<ArrayOf<Byte> > contentId = id.GetBytes();
        *result = String("cid:") + String(*contentId);
    }
    else {
        *result = String(*location);
    }
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

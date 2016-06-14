#include "Elastos.Droid.Google.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/google/mms/pdu/PduPart.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                               PduPart
//=====================================================================
CAR_INTERFACE_IMPL(PduPart, Object, IPduPart);

const Int32 PduPart::P_Q;
const Int32 PduPart::P_CHARSET;
const Int32 PduPart::P_LEVEL;
const Int32 PduPart::P_TYPE;
const Int32 PduPart::P_DEP_NAME;
const Int32 PduPart::P_DEP_FILENAME;
const Int32 PduPart::P_DIFFERENCES;
const Int32 PduPart::P_PADDING;
const Int32 PduPart::P_CT_MR_TYPE;
const Int32 PduPart::P_DEP_START;
const Int32 PduPart::P_DEP_START_INFO;
const Int32 PduPart::P_DEP_COMMENT;
const Int32 PduPart::P_DEP_DOMAIN;
const Int32 PduPart::P_MAX_AGE;
const Int32 PduPart::P_DEP_PATH;
const Int32 PduPart::P_SECURE;
const Int32 PduPart::P_SEC;
const Int32 PduPart::P_MAC;
const Int32 PduPart::P_CREATION_DATE;
const Int32 PduPart::P_MODIFICATION_DATE;
const Int32 PduPart::P_READ_DATE;
const Int32 PduPart::P_SIZE;
const Int32 PduPart::P_NAME;
const Int32 PduPart::P_FILENAME;
const Int32 PduPart::P_START;
const Int32 PduPart::P_START_INFO;
const Int32 PduPart::P_COMMENT;
const Int32 PduPart::P_DOMAIN;
const Int32 PduPart::P_PATH;
const Int32 PduPart::P_CONTENT_TYPE;
const Int32 PduPart::P_CONTENT_LOCATION;
const Int32 PduPart::P_CONTENT_ID;
const Int32 PduPart::P_DEP_CONTENT_DISPOSITION;
const Int32 PduPart::P_CONTENT_DISPOSITION;
const Int32 PduPart::P_CONTENT_TRANSFER_ENCODING;
const String PduPart::CONTENT_TRANSFER_ENCODING("Content-Transfer-Encoding");
const String PduPart::P_BINARY("binary");
const String PduPart::P_7BIT("7bit");
const String PduPart::P_8BIT("8bit");
const String PduPart::P_BASE64("base64");
const String PduPart::P_QUOTED_PRINTABLE("quoted-printable");
const AutoPtr<ArrayOf<Byte> > PduPart::DISPOSITION_FROM_DATA = String("from-data").GetBytes();
const AutoPtr<ArrayOf<Byte> > PduPart::DISPOSITION_ATTACHMENT = String("attachment").GetBytes();
const AutoPtr<ArrayOf<Byte> > PduPart::DISPOSITION_INLINE = String("inline").GetBytes();
const Int32 PduPart::P_DISPOSITION_FROM_DATA;
const Int32 PduPart::P_DISPOSITION_ATTACHMENT;
const Int32 PduPart::P_DISPOSITION_INLINE;
const String PduPart::TAG("PduPart");

PduPart::PduPart()
{
    // ==================before translated======================
    // mPartHeader = new HashMap<Integer, Object>();
}

ECode PduPart::SetData(
    /* [in] */ ArrayOf<Byte>* data)
{
    VALIDATE_NOT_NULL(data);
    // ==================before translated======================
    //  if(data == null) {
    //     return;
    // }
    //
    //  mPartData = new byte[data.length];
    //  System.arraycopy(data, 0, mPartData, 0, data.length);
    assert(0);
    return NOERROR;
}

ECode PduPart::GetData(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if(mPartData == null) {
    //    return null;
    // }
    //
    // byte[] byteArray = new byte[mPartData.length];
    // System.arraycopy(mPartData, 0, byteArray, 0, mPartData.length);
    // return byteArray;
    assert(0);
    return NOERROR;
}

ECode PduPart::GetDataLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if(mPartData != null){
    //     return mPartData.length;
    // } else {
    //     return 0;
    // }
    assert(0);
    return NOERROR;
}

ECode PduPart::SetDataUri(
    /* [in] */ IUri* uri)
{
    VALIDATE_NOT_NULL(uri);
    // ==================before translated======================
    // mUri = uri;
    assert(0);
    return NOERROR;
}

ECode PduPart::GetDataUri(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUri;
    assert(0);
    return NOERROR;
}

ECode PduPart::SetContentId(
    /* [in] */ ArrayOf<Byte>* contentId)
{
    VALIDATE_NOT_NULL(contentId);
    // ==================before translated======================
    // if((contentId == null) || (contentId.length == 0)) {
    //     throw new IllegalArgumentException(
    //             "Content-Id may not be null or empty.");
    // }
    //
    // if ((contentId.length > 1)
    //         && ((char) contentId[0] == '<')
    //         && ((char) contentId[contentId.length - 1] == '>')) {
    //     mPartHeader.put(P_CONTENT_ID, contentId);
    //     return;
    // }
    //
    // // Insert beginning '<' and trailing '>' for Content-Id.
    // byte[] buffer = new byte[contentId.length + 2];
    // buffer[0] = (byte) (0xff & '<');
    // buffer[buffer.length - 1] = (byte) (0xff & '>');
    // System.arraycopy(contentId, 0, buffer, 1, contentId.length);
    // mPartHeader.put(P_CONTENT_ID, buffer);
    assert(0);
    return NOERROR;
}

ECode PduPart::GetContentId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (byte[]) mPartHeader.get(P_CONTENT_ID);
    assert(0);
    return NOERROR;
}

ECode PduPart::SetCharset(
    /* [in] */ Int32 charset)
{
    // ==================before translated======================
    // mPartHeader.put(P_CHARSET, charset);
    assert(0);
    return NOERROR;
}

ECode PduPart::GetCharset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Integer charset = (Integer) mPartHeader.get(P_CHARSET);
    // if(charset == null) {
    //     return 0;
    // } else {
    //     return charset.intValue();
    // }
    assert(0);
    return NOERROR;
}

ECode PduPart::SetContentLocation(
    /* [in] */ ArrayOf<Byte>* contentLocation)
{
    VALIDATE_NOT_NULL(contentLocation);
    // ==================before translated======================
    // if(contentLocation == null) {
    //     throw new NullPointerException("null content-location");
    // }
    //
    // mPartHeader.put(P_CONTENT_LOCATION, contentLocation);
    assert(0);
    return NOERROR;
}

ECode PduPart::GetContentLocation(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (byte[]) mPartHeader.get(P_CONTENT_LOCATION);
    assert(0);
    return NOERROR;
}

ECode PduPart::SetContentDisposition(
    /* [in] */ ArrayOf<Byte>* contentDisposition)
{
    VALIDATE_NOT_NULL(contentDisposition);
    // ==================before translated======================
    // if(contentDisposition == null) {
    //     throw new NullPointerException("null content-disposition");
    // }
    //
    // mPartHeader.put(P_CONTENT_DISPOSITION, contentDisposition);
    assert(0);
    return NOERROR;
}

ECode PduPart::GetContentDisposition(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (byte[]) mPartHeader.get(P_CONTENT_DISPOSITION);
    assert(0);
    return NOERROR;
}

ECode PduPart::SetContentType(
    /* [in] */ ArrayOf<Byte>* contentType)
{
    VALIDATE_NOT_NULL(contentType);
    // ==================before translated======================
    // if(contentType == null) {
    //     throw new NullPointerException("null content-type");
    // }
    //
    // mPartHeader.put(P_CONTENT_TYPE, contentType);
    assert(0);
    return NOERROR;
}

ECode PduPart::GetContentType(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (byte[]) mPartHeader.get(P_CONTENT_TYPE);
    assert(0);
    return NOERROR;
}

ECode PduPart::SetContentTransferEncoding(
    /* [in] */ ArrayOf<Byte>* contentTransferEncoding)
{
    VALIDATE_NOT_NULL(contentTransferEncoding);
    // ==================before translated======================
    // if(contentTransferEncoding == null) {
    //     throw new NullPointerException("null content-transfer-encoding");
    // }
    //
    // mPartHeader.put(P_CONTENT_TRANSFER_ENCODING, contentTransferEncoding);
    assert(0);
    return NOERROR;
}

ECode PduPart::GetContentTransferEncoding(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (byte[]) mPartHeader.get(P_CONTENT_TRANSFER_ENCODING);
    assert(0);
    return NOERROR;
}

ECode PduPart::SetName(
    /* [in] */ ArrayOf<Byte>* name)
{
    VALIDATE_NOT_NULL(name);
    // ==================before translated======================
    // if(null == name) {
    //     throw new NullPointerException("null content-id");
    // }
    //
    // mPartHeader.put(P_NAME, name);
    assert(0);
    return NOERROR;
}

ECode PduPart::GetName(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (byte[]) mPartHeader.get(P_NAME);
    assert(0);
    return NOERROR;
}

ECode PduPart::SetFilename(
    /* [in] */ ArrayOf<Byte>* fileName)
{
    VALIDATE_NOT_NULL(fileName);
    // ==================before translated======================
    // if(null == fileName) {
    //     throw new NullPointerException("null content-id");
    // }
    //
    // mPartHeader.put(P_FILENAME, fileName);
    assert(0);
    return NOERROR;
}

ECode PduPart::GetFilename(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (byte[]) mPartHeader.get(P_FILENAME);
    assert(0);
    return NOERROR;
}

ECode PduPart::GenerateLocation(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Assumption: At least one of the content-location / name / filename
    // // or content-id should be set. This is guaranteed by the PduParser
    // // for incoming messages and by MM composer for outgoing messages.
    // byte[] location = (byte[]) mPartHeader.get(P_NAME);
    // if(null == location) {
    //     location = (byte[]) mPartHeader.get(P_FILENAME);
    //
    //     if (null == location) {
    //         location = (byte[]) mPartHeader.get(P_CONTENT_LOCATION);
    //     }
    // }
    //
    // if (null == location) {
    //     byte[] contentId = (byte[]) mPartHeader.get(P_CONTENT_ID);
    //     return "cid:" + new String(contentId);
    // } else {
    //     return new String(location);
    // }
    assert(0);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

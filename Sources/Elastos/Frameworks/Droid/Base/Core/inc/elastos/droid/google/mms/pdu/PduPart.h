#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUPART_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUPART_H__

#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Net::IUri;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

/**
 * The pdu part.
 */
class PduPart
    : public Object
    , public IPduPart
{
public:
    CAR_INTERFACE_DECL();

    /**
     * Empty Constructor.
     */
    PduPart();

    /**
     * Set part data. The data are stored as byte array.
     *
     * @param data the data
     */
    CARAPI SetData(
        /* [in] */ ArrayOf<Byte>* data);

    /**
     * @return A copy of the part data or null if the data wasn't set or
     *         the data is stored as Uri.
     * @see #getDataUri
     */
    CARAPI GetData(
        /* [out] */ ArrayOf<Byte>** result);

    /**
    * @return The length of the data, if this object have data, else 0.
    */
    CARAPI GetDataLength(
        /* [out] */ Int32* result);

    /**
     * Set data uri. The data are stored as Uri.
     *
     * @param uri the uri
     */
    CARAPI SetDataUri(
        /* [in] */ IUri* uri);

    /**
     * @return The Uri of the part data or null if the data wasn't set or
     *         the data is stored as byte array.
     * @see #getData
     */
    CARAPI GetDataUri(
        /* [out] */ IUri** result);

    /**
     * Set Content-id value
     *
     * @param contentId the content-id value
     * @throws NullPointerException if the value is null.
     */
    CARAPI SetContentId(
        /* [in] */ ArrayOf<Byte>* contentId);

    /**
     * Get Content-id value.
     *
     * @return the value
     */
    CARAPI GetContentId(
        /* [out] */ ArrayOf<Byte>** result);

    /**
     * Set Char-set value.
     *
     * @param charset the value
     */
    CARAPI SetCharset(
        /* [in] */ Int32 charset);

    /**
     * Get Char-set value
     *
     * @return the charset value. Return 0 if charset was not set.
     */
    CARAPI GetCharset(
        /* [out] */ Int32* result);

    /**
     * Set Content-Location value.
     *
     * @param contentLocation the value
     * @throws NullPointerException if the value is null.
     */
    CARAPI SetContentLocation(
        /* [in] */ ArrayOf<Byte>* contentLocation);

    /**
     * Get Content-Location value.
     *
     * @return the value
     *     return PduPart.disposition[0] instead of <Octet 128> (Form-data).
     *     return PduPart.disposition[1] instead of <Octet 129> (Attachment).
     *     return PduPart.disposition[2] instead of <Octet 130> (Inline).
     */
    CARAPI GetContentLocation(
        /* [out] */ ArrayOf<Byte>** result);

    /**
     * Set Content-Disposition value.
     * Use PduPart.disposition[0] instead of <Octet 128> (Form-data).
     * Use PduPart.disposition[1] instead of <Octet 129> (Attachment).
     * Use PduPart.disposition[2] instead of <Octet 130> (Inline).
     *
     * @param contentDisposition the value
     * @throws NullPointerException if the value is null.
     */
    CARAPI SetContentDisposition(
        /* [in] */ ArrayOf<Byte>* contentDisposition);

    /**
     * Get Content-Disposition value.
     *
     * @return the value
     */
    CARAPI GetContentDisposition(
        /* [out] */ ArrayOf<Byte>** result);

    /**
     *  Set Content-Type value.
     *
     *  @param value the value
     *  @throws NullPointerException if the value is null.
     */
    CARAPI SetContentType(
        /* [in] */ ArrayOf<Byte>* contentType);

    /**
     * Get Content-Type value of part.
     *
     * @return the value
     */
    CARAPI GetContentType(
        /* [out] */ ArrayOf<Byte>** result);

    /**
     * Set Content-Transfer-Encoding value
     *
     * @param contentId the content-id value
     * @throws NullPointerException if the value is null.
     */
    CARAPI SetContentTransferEncoding(
        /* [in] */ ArrayOf<Byte>* contentTransferEncoding);

    /**
     * Get Content-Transfer-Encoding value.
     *
     * @return the value
     */
    CARAPI GetContentTransferEncoding(
        /* [out] */ ArrayOf<Byte>** result);

    /**
     * Set Content-type parameter: name.
     *
     * @param name the name value
     * @throws NullPointerException if the value is null.
     */
    CARAPI SetName(
        /* [in] */ ArrayOf<Byte>* name);

    /**
     *  Get content-type parameter: name.
     *
     *  @return the name
     */
    CARAPI GetName(
        /* [out] */ ArrayOf<Byte>** result);

    /**
     * Get Content-disposition parameter: filename
     *
     * @param fileName the filename value
     * @throws NullPointerException if the value is null.
     */
    CARAPI SetFilename(
        /* [in] */ ArrayOf<Byte>* fileName);

    /**
     * Set Content-disposition parameter: filename
     *
     * @return the filename
     */
    CARAPI GetFilename(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GenerateLocation(
        /* [out] */ String* result);

public:
    /**
     * Value of disposition can be set to PduPart when the value is octet in
     * the PDU.
     * "from-data" instead of Form-data<Octet 128>.
     * "attachment" instead of Attachment<Octet 129>.
     * "inline" instead of Inline<Octet 130>.
     */
    static AutoPtr<ArrayOf<Byte> > DISPOSITION_FROM_DATA;
    static AutoPtr<ArrayOf<Byte> > DISPOSITION_ATTACHMENT;
    static AutoPtr<ArrayOf<Byte> > DISPOSITION_INLINE;

private:
    /**
     * Header of part.
     */
    AutoPtr<IMap> mPartHeader; // Integer, Object
    /**
     * Data uri.
     */
    AutoPtr<IUri> mUri;
    /**
     * Part data.
     */
    AutoPtr<ArrayOf<Byte> > mPartData;
    static const String TAG;
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUPART_H__


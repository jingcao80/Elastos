
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMSMSCBMESSAGE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMSMSCBMESSAGE_H__

#include <Elastos.CoreLibrary.Core.h>
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_Internal_Telephony_Gsm_CGsmSmsCbMessage.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Telephony::ISmsCbLocation;
using Elastos::Droid::Telephony::ISmsCbMessage;
using Elastos::Droid::Utility::IPair;
using Elastos::Core::IArrayOf;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * Parses a GSM or UMTS format SMS-CB message into an {@link SmsCbMessage} object. The class is
 * public because {@link #createSmsCbMessage(SmsCbLocation, byte[][])} is used by some test cases.
 */
CarClass(CGsmSmsCbMessage)
    , public Object
    , public IGsmSmsCbMessage
{
public:
    CGsmSmsCbMessage() {}

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor() {}

    /**
     * Create a new SmsCbMessage object from a header object plus one or more received PDUs.
     *
     * @param pdus PDU bytes
     */
    static CARAPI CreateSmsCbMessage(
        /* [in] */ ISmsCbHeader* header,
        /* [in] */ ISmsCbLocation* location,
        /* [in] */ IArrayOf* pdus,
        /* [out] */ ISmsCbMessage** result);

    /**
     * Create a new SmsCbMessage object from one or more received PDUs. This is used by some
     * CellBroadcastReceiver test cases, because SmsCbHeader is now package local.
     *
     * @param location the location (geographical scope) for the message
     * @param pdus PDU bytes
     */
    static CARAPI CreateSmsCbMessage(
        /* [in] */ ISmsCbLocation* location,
        /* [in] */ IArrayOf* pdus,
        /* [out] */ ISmsCbMessage** result);

private:
    /**
     * Parse and unpack the body text according to the encoding in the DCS.
     * After completing successfully this method will have assigned the body
     * text into mBody, and optionally the language code into mLanguage
     *
     * @param header the message header to use
     * @param pdu the PDU to decode
     * @return a Pair of Strings containing the language and body of the message
     */
    static CARAPI ParseBody(
        /* [in] */ ISmsCbHeader* header,
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [out] */ IPair** result);

    /**
     * Unpack body text from the pdu using the given encoding, position and
     * length within the pdu
     *
     * @param pdu The pdu
     * @param encoding The encoding, as derived from the DCS
     * @param offset Position of the first byte to unpack
     * @param length Number of bytes to unpack
     * @param hasLanguageIndicator true if the body text is preceded by a
     *            language indicator. If so, this method will as a side-effect
     *            assign the extracted language code into mLanguage
     * @param language the language to return if hasLanguageIndicator is false
     * @return a Pair of Strings containing the language and body of the message
     */
    static CARAPI UnpackBody(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ Int32 encoding,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ Boolean hasLanguageIndicator,
        /* [in] */ const String& language,
        /* [out] */ IPair** result);

private:
    /**
     * Languages in the 0000xxxx DCS group as defined in 3GPP TS 23.038, section 5.
     */
    static AutoPtr<ArrayOf<String> > LANGUAGE_CODES_GROUP_0; // = {
            // "de", "en", "it", "fr", "es", "nl", "sv", "da", "pt", "fi", "no", "el", "tr", "hu",
            // "pl", null };

    /**
     * Languages in the 0010xxxx DCS group as defined in 3GPP TS 23.038, section 5.
     */
    static AutoPtr<ArrayOf<String> > LANGUAGE_CODES_GROUP_2; // = {
            // "cs", "he", "ar", "ru", "is", null, null, null, null, null, null, null, null, null,
            // null, null };

    static const Char32 CARRIAGE_RETURN = 0x0d;

    static const Int32 PDU_BODY_PAGE_LENGTH = 82;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMSMSCBMESSAGE_H__

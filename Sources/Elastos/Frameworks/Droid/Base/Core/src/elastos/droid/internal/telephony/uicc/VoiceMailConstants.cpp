#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/VoiceMailConstants.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                          VoiceMailConstants
//=====================================================================
const String VoiceMailConstants::LOGTAG("VoiceMailConstants");
const String VoiceMailConstants::PARTNER_VOICEMAIL_PATH("etc/voicemail-conf.xml");
const Int32 VoiceMailConstants::NAME;
const Int32 VoiceMailConstants::NUMBER;
const Int32 VoiceMailConstants::TAG;
const Int32 VoiceMailConstants::SIZE;

VoiceMailConstants::VoiceMailConstants()
{
    // ==================before translated======================
    // CarrierVmMap = new HashMap<String, String[]>();
    // loadVoiceMail();
}

ECode VoiceMailConstants::ContainsCarrier(
    /* [in] */ const String& carrier,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return CarrierVmMap.containsKey(carrier);
    assert(0);
    return NOERROR;
}

ECode VoiceMailConstants::GetCarrierName(
    /* [in] */ const String& carrier,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // String[] data = CarrierVmMap.get(carrier);
    // return data[NAME];
    assert(0);
    return NOERROR;
}

ECode VoiceMailConstants::GetVoiceMailNumber(
    /* [in] */ const String& carrier,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // String[] data = CarrierVmMap.get(carrier);
    // return data[NUMBER];
    assert(0);
    return NOERROR;
}

ECode VoiceMailConstants::GetVoiceMailTag(
    /* [in] */ const String& carrier,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // String[] data = CarrierVmMap.get(carrier);
    // return data[TAG];
    assert(0);
    return NOERROR;
}

void VoiceMailConstants::LoadVoiceMail()
{
    // ==================before translated======================
    // FileReader vmReader;
    //
    // final File vmFile = new File(Environment.getRootDirectory(),
    //         PARTNER_VOICEMAIL_PATH);
    //
    // try {
    //     vmReader = new FileReader(vmFile);
    // } catch (FileNotFoundException e) {
    //     Rlog.w(LOG_TAG, "Can't open " +
    //             Environment.getRootDirectory() + "/" + PARTNER_VOICEMAIL_PATH);
    //     return;
    // }
    //
    // try {
    //     XmlPullParser parser = Xml.newPullParser();
    //     parser.setInput(vmReader);
    //
    //     XmlUtils.beginDocument(parser, "voicemail");
    //
    //     while (true) {
    //         XmlUtils.nextElement(parser);
    //
    //         String name = parser.getName();
    //         if (!"voicemail".equals(name)) {
    //             break;
    //         }
    //
    //         String[] data = new String[SIZE];
    //         String numeric = parser.getAttributeValue(null, "numeric");
    //         data[NAME]     = parser.getAttributeValue(null, "carrier");
    //         data[NUMBER]   = parser.getAttributeValue(null, "vmnumber");
    //         data[TAG]      = parser.getAttributeValue(null, "vmtag");
    //
    //         CarrierVmMap.put(numeric, data);
    //     }
    // } catch (XmlPullParserException e) {
    //     Rlog.w(LOG_TAG, "Exception in Voicemail parser " + e);
    // } catch (IOException e) {
    //     Rlog.w(LOG_TAG, "Exception in Voicemail parser " + e);
    // } finally {
    //     try {
    //         if (vmReader != null) {
    //             vmReader.close();
    //         }
    //     } catch (IOException e) {}
    // }
    assert(0);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/SpnOverride.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                             SpnOverride
//=====================================================================
CAR_INTERFACE_IMPL(SpnOverride, Object, ISpnOverride);

const String SpnOverride::LOGTAG("SpnOverride");
const String SpnOverride::PARTNER_SPN_OVERRIDE_PATH("etc/spn-conf.xml");

SpnOverride::SpnOverride()
{
    // ==================before translated======================
    // mCarrierSpnMap = new HashMap<String, String>();
    // loadSpnOverrides();
}

ECode SpnOverride::ContainsCarrier(
    /* [in] */ const String& carrier,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCarrierSpnMap.containsKey(carrier);
    assert(0);
    return NOERROR;
}

ECode SpnOverride::GetSpn(
    /* [in] */ const String& carrier,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCarrierSpnMap.get(carrier);
    assert(0);
    return NOERROR;
}

void SpnOverride::LoadSpnOverrides()
{
    // ==================before translated======================
    // FileReader spnReader;
    //
    // final File spnFile = new File(Environment.getRootDirectory(),
    //         PARTNER_SPN_OVERRIDE_PATH);
    //
    // try {
    //     spnReader = new FileReader(spnFile);
    // } catch (FileNotFoundException e) {
    //     Rlog.w(LOGTAG, "Can not open " +
    //             Environment.getRootDirectory() + "/" + PARTNER_SPN_OVERRIDE_PATH);
    //     return;
    // }
    //
    // try {
    //     XmlPullParser parser = Xml.newPullParser();
    //     parser.setInput(spnReader);
    //
    //     XmlUtils.beginDocument(parser, "spnOverrides");
    //
    //     while (true) {
    //         XmlUtils.nextElement(parser);
    //
    //         String name = parser.getName();
    //         if (!"spnOverride".equals(name)) {
    //             break;
    //         }
    //
    //         String numeric = parser.getAttributeValue(null, "numeric");
    //         String data    = parser.getAttributeValue(null, "spn");
    //
    //         mCarrierSpnMap.put(numeric, data);
    //     }
    //     spnReader.close();
    // } catch (XmlPullParserException e) {
    //     Rlog.w(LOGTAG, "Exception in spn-conf parser " + e);
    // } catch (IOException e) {
    //     Rlog.w(LOGTAG, "Exception in spn-conf parser " + e);
    // }
    assert(0);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

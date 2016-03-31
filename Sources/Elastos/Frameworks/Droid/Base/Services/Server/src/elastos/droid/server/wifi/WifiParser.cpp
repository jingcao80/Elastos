
#include "elastos/droid/server/wifi/WifiParser.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//                              WifiParser
//=====================================================================
const Int32 WifiParser::VENDOR_SPECIFIC_IE;
const Int32 WifiParser::IEEE_RSN_IE;
const Int32 WifiParser::WPA_IE_VENDOR_TYPE;

WifiParser::WifiParser()
{
}

String WifiParser::Parse_akm(
    /* [in] */ IE* full_IE[],
    /* [in] */ IBitSet* ieee_cap)
{
    // ==================before translated======================
    // boolean privacy = false;
    // boolean error = false;
    // if (ieee_cap == null)
    //     return null;
    //
    // if (full_IE == null)
    //     return null;
    //
    // privacy = ieee_cap.get(4);
    //
    // String capabilities = "";
    // boolean rsne_found = false;
    // boolean wpa_found = false;
    //
    // for (IE ie : full_IE) {
    //     String security = "";
    //     if (ie.id == IEEE_RSN_IE) {
    //         rsne_found = true;
    //         //parsing WPA2 capabilities
    //
    //         ByteBuffer buf = ByteBuffer.wrap(ie.data);
    //
    //         int total_len = ie.data.length;
    //         int offset = 2;
    //
    //         //version
    //         if ((total_len - offset) < 2) {
    //             //not enough space for version field
    //             security = "";
    //             error = true;
    //             break;
    //         }
    //         int val = 0;
    //         if (0x0100 != buf.getShort(offset)) {
    //             //incorrect version
    //             security = "";
    //             error = true;
    //             break;
    //         }
    //         offset += 2;
    //
    //
    //         //group cipher
    //         if ((total_len - offset) < 4) {
    //             security = ""; //parse error on group cipher suite
    //             error = true;
    //             break;
    //         }
    //         offset += 4; //skip the group cipher
    //
    //         security = "[WPA2"; //found the RSNE IE, hence start building the capability string
    //
    //         //pairwise cipher
    //         if ((total_len - offset) < 2) {
    //             security = ""; //parse error no pairwise cipher
    //             error = true;
    //             break;
    //         }
    //         val = buf.getShort(offset);
    //         if ((total_len - offset) < (2 + val * 4)) {
    //             security = ""; //parse error no pairwise cipher
    //             error = true;
    //             break;
    //         }
    //         offset += 2 + val * 4; //skip the pairwise ciphers
    //
    //         //AKM
    //         if ((total_len - offset) < 2) {
    //             security = ""; //parse error no AKM
    //             error = true;
    //             break;
    //         }
    //         val = buf.getShort(offset);
    //         if ((total_len - offset) < (2 + val * 4)) {
    //             security = ""; //parse error no pairwise cipher
    //             error = true;
    //             break;
    //         }
    //         offset += 2;
    //         if (val == 0) {
    //             security += "-EAP"; //default AKM
    //         }
    //         for (int i = 0; i < val; i++) {
    //             int akm = buf.getInt(offset);
    //             boolean found = false;
    //             switch (akm) {
    //                 case 0x01ac0f00:
    //                     security += found ? "+" : "-" + "EAP";
    //                     found = true;
    //                     break;
    //                 case 0x02ac0f00:
    //                     security += found ? "+" : "-" + "PSK"; //PSK as 802.11-2012 11.6.1.2
    //                     found = true;
    //                     break;
    //                 case 0x03ac0f00:
    //                     security += found ? "+" : "-" + "FT/EAP";
    //                     found = true;
    //                     break;
    //                 case 0x04ac0f00:
    //                     security += found ? "+" : "-" + "FT/PSK";
    //                     found = true;
    //                     break;
    //                 case 0x06ac0f00:
    //                     security += found ? "+" : "-" + "PSK-SHA256";
    //                     found = true;
    //                     break;
    //                 case 0x05ac0f00:
    //                     security += found ? "+" : "-" + "EAP-SHA256";
    //                     found = true;
    //                     break;
    //             }
    //             offset += 4;
    //         }
    //         //we parsed what we want at this point
    //         security += "]";
    //         capabilities += security;
    //
    //     }
    //
    //     if (ie.id == VENDOR_SPECIFIC_IE) {
    //         int total_len = ie.data.length;
    //         int offset = 2;
    //
    //         //version
    //         if ((total_len - offset) < 4) {
    //             //not enough space for OUI and type field
    //             security = "";
    //             error = true;
    //             break;
    //         }
    //
    //         ByteBuffer buf = ByteBuffer.wrap(ie.data);
    //
    //         if (buf.getInt(offset) != 0x01F25000) {
    //             //look for HS2.0 and WPA IE
    //             security = "";
    //             continue;
    //         }
    //
    //         security = "[WPA"; //prep the string for WPA
    //
    //         //version
    //         if ((total_len - offset) < 2) {
    //             //not enough space for version field
    //             security = "";
    //             error = true;
    //             break;
    //         }
    //         int val = 0;
    //         if (0x0100 != buf.getShort(offset)) {
    //             //incorrect version
    //             security = "";
    //             error = true;
    //             break;
    //         }
    //         offset += 2;
    //
    //
    //         //group cipher
    //         if ((total_len - offset) < 4) {
    //             security = ""; //parse error on group cipher suite
    //             error = true;
    //             break;
    //         }
    //         offset += 4; //skip the group cipher
    //
    //
    //         //pairwise cipher
    //         if ((total_len - offset) < 2) {
    //             security = ""; //parse error no pairwise cipher
    //             error = true;
    //             break;
    //         }
    //         val = buf.getShort(offset);
    //         if ((total_len - offset) < (2 + val * 4)) {
    //             security = ""; //parse error no pairwise cipher
    //             error = true;
    //             break;
    //         }
    //         offset += 2 + val * 4; //skip the pairwise ciphers
    //
    //         //AKM
    //         if ((total_len - offset) < 2) {
    //             security = ""; //parse error no AKM
    //             error = true;
    //             break;
    //         }
    //         val = buf.getShort(offset);
    //         if ((total_len - offset) < (2 + val * 4)) {
    //             security = ""; //parse error no pairwise cipher
    //             error = true;
    //             break;
    //         }
    //         offset += 2;
    //         if (val == 0) {
    //             security += "-EAP"; //default AKM
    //         }
    //         for (int i = 0; i < val; i++) {
    //             int akm = buf.getInt(offset);
    //             boolean found = false;
    //             switch (akm) {
    //                 case 0x01f25000:
    //                     security += found ? "+" : "-" + "EAP";
    //                     found = true;
    //                     break;
    //                 case 0x02f25000:
    //                     security += found ? "+" : "-" + "PSK"; //PSK as 802.11-2012 11.6.1.2
    //                     found = true;
    //                     break;
    //
    //             }
    //             offset += 4;
    //         }
    //         //we parsed what we want at this point
    //         security += "]";
    //     }
    // }
    //
    // if (rsne_found == false && wpa_found == false && privacy) {
    //     //private Beacon without an RSNE or WPA IE, hence WEP0
    //     capabilities += "[WEP]";
    // }
    //
    // if (error)
    //     return null;
    // else
    //     return capabilities;
    assert(0);
    return String("");
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos



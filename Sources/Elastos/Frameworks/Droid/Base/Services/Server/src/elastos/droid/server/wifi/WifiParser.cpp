#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/server/wifi/WifiParser.h"

using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;

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
    /* [in] */ ArrayOf<IE*>* full_IE,
    /* [in] */ IBitSet* ieee_cap)
{
    Boolean privacy = FALSE;
    Boolean error = FALSE;
    if (ieee_cap == NULL)
        return String(NULL);

    if (full_IE == NULL)
        return String(NULL);

    ieee_cap->Get(4, &privacy);

    String capabilities("");
    Boolean rsne_found = FALSE;
    Boolean wpa_found = FALSE;

    for (Int32 i = 0; i < full_IE->GetLength(); ++i) {
        AutoPtr<IE> ie = (*full_IE)[i];
        String security("");
        if (ie->id == IEEE_RSN_IE) {
            rsne_found = TRUE;
            //parsing WPA2 capabilities

            AutoPtr<IByteBufferHelper> bbHelper;
            CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbHelper);
            AutoPtr<IByteBuffer> buf;
            bbHelper->Wrap(ie->data, (IByteBuffer**)&buf);

            Int32 total_len = ie->data->GetLength();
            Int32 offset = 2;

            //version
            if ((total_len - offset) < 2) {
                //not enough space for version field
                security = String("");
                error = TRUE;
                break;
            }
            Int16 val = 0;
            Int16 i16;
            buf->GetInt16(offset, &i16);
            if (0x0100 != i16) {
                //incorrect version
                security = String("");
                error = TRUE;
                break;
            }
            offset += 2;


            //group cipher
            if ((total_len - offset) < 4) {
                security = String(""); //parse error on group cipher suite
                error = TRUE;
                break;
            }
            offset += 4; //skip the group cipher

            security = String("[WPA2"); //found the RSNE IE, hence start building the capability string

            //pairwise cipher
            if ((total_len - offset) < 2) {
                security = String(""); //parse error no pairwise cipher
                error = TRUE;
                break;
            }
            buf->GetInt16(offset, &val);
            if ((total_len - offset) < (2 + val * 4)) {
                security = String(""); //parse error no pairwise cipher
                error = TRUE;
                break;
            }
            offset += 2 + val * 4; //skip the pairwise ciphers

            //AKM
            if ((total_len - offset) < 2) {
                security = String(""); //parse error no AKM
                error = TRUE;
                break;
            }
            buf->GetInt16(offset, &val);
            if ((total_len - offset) < (2 + val * 4)) {
                security = String(""); //parse error no pairwise cipher
                error = TRUE;
                break;
            }
            offset += 2;
            if (val == 0) {
                security += String("-EAP"); //default AKM
            }
            for (Int32 i = 0; i < val; i++) {
                Int32 akm;
                buf->GetInt32(offset, &akm);
                Boolean found = FALSE;
                switch (akm) {
                    case 0x01ac0f00:
                        security += found ? String("+") : String("-") + String("EAP");
                        found = TRUE;
                        break;
                    case 0x02ac0f00:
                        security += found ? String("+") : String("-") + String("PSK"); //PSK as 802.11-2012 11.6.1.2
                        found = TRUE;
                        break;
                    case 0x03ac0f00:
                        security += found ? String("+") : String("-") + String("FT/EAP");
                        found = TRUE;
                        break;
                    case 0x04ac0f00:
                        security += found ? String("+") : String("-") + String("FT/PSK");
                        found = TRUE;
                        break;
                    case 0x06ac0f00:
                        security += found ? String("+") : String("-") + String("PSK-SHA256");
                        found = TRUE;
                        break;
                    case 0x05ac0f00:
                        security += found ? String("+") : String("-") + String("EAP-SHA256");
                        found = TRUE;
                        break;
                }
                offset += 4;
            }
            //we parsed what we want at this point
            security += String("]");
            capabilities += security;

        }

        if (ie->id == VENDOR_SPECIFIC_IE) {
            Int32 total_len = ie->data->GetLength();
            Int32 offset = 2;

            //version
            if ((total_len - offset) < 4) {
                //not enough space for OUI and type field
                security = String("");
                error = TRUE;
                break;
            }

            AutoPtr<IByteBufferHelper> bbHelper;
            CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbHelper);
            AutoPtr<IByteBuffer> buf;
            bbHelper->Wrap(ie->data, (IByteBuffer**)&buf);


            Int32 i32;
            if ((buf->GetInt32(offset, &i32), i32) != 0x01F25000) {
                //look for HS2.0 and WPA IE
                security = String("");
                continue;
            }

            security = String("[WPA"); //prep the string for WPA

            //version
            if ((total_len - offset) < 2) {
                //not enough space for version field
                security = String("");
                error = TRUE;
                break;
            }
            Int16 val = 0;
            Int16 i16;
            if (0x0100 != (buf->GetInt16(offset, &i16), i16)) {
                //incorrect version
                security = String("");
                error = TRUE;
                break;
            }
            offset += 2;


            //group cipher
            if ((total_len - offset) < 4) {
                security = String(""); //parse error on group cipher suite
                error = TRUE;
                break;
            }
            offset += 4; //skip the group cipher


            //pairwise cipher
            if ((total_len - offset) < 2) {
                security = String(""); //parse error no pairwise cipher
                error = TRUE;
                break;
            }
            buf->GetInt16(offset, &val);
            if ((total_len - offset) < (2 + val * 4)) {
                security = String(""); //parse error no pairwise cipher
                error = TRUE;
                break;
            }
            offset += 2 + val * 4; //skip the pairwise ciphers

            //AKM
            if ((total_len - offset) < 2) {
                security = String(""); //parse error no AKM
                error = TRUE;
                break;
            }
            buf->GetInt16(offset, &val);
            if ((total_len - offset) < (2 + val * 4)) {
                security = String(""); //parse error no pairwise cipher
                error = TRUE;
                break;
            }
            offset += 2;
            if (val == 0) {
                security += String("-EAP"); //default AKM
            }
            for (Int32 i = 0; i < val; i++) {
                Int32 akm;
                buf->GetInt32(offset, &akm);
                Boolean found = FALSE;
                switch (akm) {
                    case 0x01f25000:
                        security += found ? String("+") : String("-") + String("EAP");
                        found = TRUE;
                        break;
                    case 0x02f25000:
                        security += found ? String("+") : String("-") + String("PSK"); //PSK as 802.11-2012 11.6.1.2
                        found = TRUE;
                        break;

                }
                offset += 4;
            }
            //we parsed what we want at this point
            security += String("]");
        }
    }

    if (rsne_found == FALSE && wpa_found == FALSE && privacy) {
        //private Beacon without an RSNE or WPA IE, hence WEP0
        capabilities += String("[WEP]");
    }

    if (error)
        return String(NULL);
    else
        return capabilities;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


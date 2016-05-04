/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony;

using Elastos::Utility::IHashMap;

/**
 * Implement the WSP data type decoder.
 *
 * @hide
 */
public class WspTypeDecoder {

    private static const Int32 WAP_PDU_SHORT_LENGTH_MAX = 30;
    private static const Int32 WAP_PDU_LENGTH_QUOTE = 31;

    public static const Int32 PDU_TYPE_PUSH = 0x06;
    public static const Int32 PDU_TYPE_CONFIRMED_PUSH = 0x07;

    private static const HashMap<Integer, String> WELL_KNOWN_MIME_TYPES =
            new HashMap<Integer, String>();

    private static const HashMap<Integer, String> WELL_KNOWN_PARAMETERS =
            new HashMap<Integer, String>();

    public static const Int32 PARAMETER_ID_X_WAP_APPLICATION_ID = 0x2f;
    private static const Int32 Q_VALUE = 0x00;

    static {
        WELL_KNOWN_MIME_TYPES->Put(0x00, "*/*");
        WELL_KNOWN_MIME_TYPES->Put(0x01, "text/*");
        WELL_KNOWN_MIME_TYPES->Put(0x02, "text/html");
        WELL_KNOWN_MIME_TYPES->Put(0x03, "text/plain");
        WELL_KNOWN_MIME_TYPES->Put(0x04, "text/x-hdml");
        WELL_KNOWN_MIME_TYPES->Put(0x05, "text/x-ttml");
        WELL_KNOWN_MIME_TYPES->Put(0x06, "text/x-vCalendar");
        WELL_KNOWN_MIME_TYPES->Put(0x07, "text/x-vCard");
        WELL_KNOWN_MIME_TYPES->Put(0x08, "text/vnd.wap.wml");
        WELL_KNOWN_MIME_TYPES->Put(0x09, "text/vnd.wap.wmlscript");
        WELL_KNOWN_MIME_TYPES->Put(0x0A, "text/vnd.wap.wta-event");
        WELL_KNOWN_MIME_TYPES->Put(0x0B, "multipart/*");
        WELL_KNOWN_MIME_TYPES->Put(0x0C, "multipart/mixed");
        WELL_KNOWN_MIME_TYPES->Put(0x0D, "multipart/form-data");
        WELL_KNOWN_MIME_TYPES->Put(0x0E, "multipart/byterantes");
        WELL_KNOWN_MIME_TYPES->Put(0x0F, "multipart/alternative");
        WELL_KNOWN_MIME_TYPES->Put(0x10, "application/*");
        WELL_KNOWN_MIME_TYPES->Put(0x11, "application/java-vm");
        WELL_KNOWN_MIME_TYPES->Put(0x12, "application/x-www-form-urlencoded");
        WELL_KNOWN_MIME_TYPES->Put(0x13, "application/x-hdmlc");
        WELL_KNOWN_MIME_TYPES->Put(0x14, "application/vnd.wap.wmlc");
        WELL_KNOWN_MIME_TYPES->Put(0x15, "application/vnd.wap.wmlscriptc");
        WELL_KNOWN_MIME_TYPES->Put(0x16, "application/vnd.wap.wta-eventc");
        WELL_KNOWN_MIME_TYPES->Put(0x17, "application/vnd.wap.uaprof");
        WELL_KNOWN_MIME_TYPES->Put(0x18, "application/vnd.wap.wtls-ca-certificate");
        WELL_KNOWN_MIME_TYPES->Put(0x19, "application/vnd.wap.wtls-user-certificate");
        WELL_KNOWN_MIME_TYPES->Put(0x1A, "application/x-x509-ca-cert");
        WELL_KNOWN_MIME_TYPES->Put(0x1B, "application/x-x509-user-cert");
        WELL_KNOWN_MIME_TYPES->Put(0x1C, "image/*");
        WELL_KNOWN_MIME_TYPES->Put(0x1D, "image/gif");
        WELL_KNOWN_MIME_TYPES->Put(0x1E, "image/jpeg");
        WELL_KNOWN_MIME_TYPES->Put(0x1F, "image/tiff");
        WELL_KNOWN_MIME_TYPES->Put(0x20, "image/png");
        WELL_KNOWN_MIME_TYPES->Put(0x21, "image/vnd.wap.wbmp");
        WELL_KNOWN_MIME_TYPES->Put(0x22, "application/vnd.wap.multipart.*");
        WELL_KNOWN_MIME_TYPES->Put(0x23, "application/vnd.wap.multipart.mixed");
        WELL_KNOWN_MIME_TYPES->Put(0x24, "application/vnd.wap.multipart.form-data");
        WELL_KNOWN_MIME_TYPES->Put(0x25, "application/vnd.wap.multipart.byteranges");
        WELL_KNOWN_MIME_TYPES->Put(0x26, "application/vnd.wap.multipart.alternative");
        WELL_KNOWN_MIME_TYPES->Put(0x27, "application/xml");
        WELL_KNOWN_MIME_TYPES->Put(0x28, "text/xml");
        WELL_KNOWN_MIME_TYPES->Put(0x29, "application/vnd.wap.wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x2A, "application/x-x968-cross-cert");
        WELL_KNOWN_MIME_TYPES->Put(0x2B, "application/x-x968-ca-cert");
        WELL_KNOWN_MIME_TYPES->Put(0x2C, "application/x-x968-user-cert");
        WELL_KNOWN_MIME_TYPES->Put(0x2D, "text/vnd.wap.si");
        WELL_KNOWN_MIME_TYPES->Put(0x2E, "application/vnd.wap.sic");
        WELL_KNOWN_MIME_TYPES->Put(0x2F, "text/vnd.wap.sl");
        WELL_KNOWN_MIME_TYPES->Put(0x30, "application/vnd.wap.slc");
        WELL_KNOWN_MIME_TYPES->Put(0x31, "text/vnd.wap.co");
        WELL_KNOWN_MIME_TYPES->Put(0x32, "application/vnd.wap.coc");
        WELL_KNOWN_MIME_TYPES->Put(0x33, "application/vnd.wap.multipart.related");
        WELL_KNOWN_MIME_TYPES->Put(0x34, "application/vnd.wap.sia");
        WELL_KNOWN_MIME_TYPES->Put(0x35, "text/vnd.wap.connectivity-xml");
        WELL_KNOWN_MIME_TYPES->Put(0x36, "application/vnd.wap.connectivity-wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x37, "application/pkcs7-mime");
        WELL_KNOWN_MIME_TYPES->Put(0x38, "application/vnd.wap.hashed-certificate");
        WELL_KNOWN_MIME_TYPES->Put(0x39, "application/vnd.wap.signed-certificate");
        WELL_KNOWN_MIME_TYPES->Put(0x3A, "application/vnd.wap.cert-response");
        WELL_KNOWN_MIME_TYPES->Put(0x3B, "application/xhtml+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x3C, "application/wml+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x3D, "text/css");
        WELL_KNOWN_MIME_TYPES->Put(0x3E, "application/vnd.wap.mms-message");
        WELL_KNOWN_MIME_TYPES->Put(0x3F, "application/vnd.wap.rollover-certificate");
        WELL_KNOWN_MIME_TYPES->Put(0x40, "application/vnd.wap.locc+wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x41, "application/vnd.wap.loc+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x42, "application/vnd.syncml.dm+wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x43, "application/vnd.syncml.dm+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x44, "application/vnd.syncml.notification");
        WELL_KNOWN_MIME_TYPES->Put(0x45, "application/vnd.wap.xhtml+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x46, "application/vnd.wv.csp.cir");
        WELL_KNOWN_MIME_TYPES->Put(0x47, "application/vnd.oma.dd+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x48, "application/vnd.oma.drm.message");
        WELL_KNOWN_MIME_TYPES->Put(0x49, "application/vnd.oma.drm.content");
        WELL_KNOWN_MIME_TYPES->Put(0x4A, "application/vnd.oma.drm.rights+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x4B, "application/vnd.oma.drm.rights+wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x4C, "application/vnd.wv.csp+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x4D, "application/vnd.wv.csp+wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x4E, "application/vnd.syncml.ds.notification");
        WELL_KNOWN_MIME_TYPES->Put(0x4F, "audio/*");
        WELL_KNOWN_MIME_TYPES->Put(0x50, "video/*");
        WELL_KNOWN_MIME_TYPES->Put(0x51, "application/vnd.oma.dd2+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x52, "application/mikey");
        WELL_KNOWN_MIME_TYPES->Put(0x53, "application/vnd.oma.dcd");
        WELL_KNOWN_MIME_TYPES->Put(0x54, "application/vnd.oma.dcdc");

        WELL_KNOWN_MIME_TYPES->Put(0x0201, "application/vnd.uplanet.cacheop-wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x0202, "application/vnd.uplanet.signal");
        WELL_KNOWN_MIME_TYPES->Put(0x0203, "application/vnd.uplanet.alert-wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x0204, "application/vnd.uplanet.list-wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x0205, "application/vnd.uplanet.listcmd-wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x0206, "application/vnd.uplanet.channel-wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x0207, "application/vnd.uplanet.provisioning-status-uri");
        WELL_KNOWN_MIME_TYPES->Put(0x0208, "x-wap.multipart/vnd.uplanet.header-set");
        WELL_KNOWN_MIME_TYPES->Put(0x0209, "application/vnd.uplanet.bearer-choice-wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x020A, "application/vnd.phonecom.mmc-wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x020B, "application/vnd.nokia.syncset+wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x020C, "image/x-up-wpng");
        WELL_KNOWN_MIME_TYPES->Put(0x0300, "application/iota.mmc-wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x0301, "application/iota.mmc-xml");
        WELL_KNOWN_MIME_TYPES->Put(0x0302, "application/vnd.syncml+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x0303, "application/vnd.syncml+wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x0304, "text/vnd.wap.emn+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x0305, "text/calendar");
        WELL_KNOWN_MIME_TYPES->Put(0x0306, "application/vnd.omads-email+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x0307, "application/vnd.omads-file+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x0308, "application/vnd.omads-folder+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x0309, "text/directory;profile=vCard");
        WELL_KNOWN_MIME_TYPES->Put(0x030A, "application/vnd.wap.emn+wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x030B, "application/vnd.nokia.ipdc-purchase-response");
        WELL_KNOWN_MIME_TYPES->Put(0x030C, "application/vnd.motorola.screen3+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x030D, "application/vnd.motorola.screen3+gzip");
        WELL_KNOWN_MIME_TYPES->Put(0x030E, "application/vnd.cmcc.setting+wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x030F, "application/vnd.cmcc.bombing+wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x0310, "application/vnd.docomo.pf");
        WELL_KNOWN_MIME_TYPES->Put(0x0311, "application/vnd.docomo.ub");
        WELL_KNOWN_MIME_TYPES->Put(0x0312, "application/vnd.omaloc-supl-init");
        WELL_KNOWN_MIME_TYPES->Put(0x0313, "application/vnd.oma.group-usage-list+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x0314, "application/oma-directory+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x0315, "application/vnd.docomo.pf2");
        WELL_KNOWN_MIME_TYPES->Put(0x0316, "application/vnd.oma.drm.roap-trigger+wbxml");
        WELL_KNOWN_MIME_TYPES->Put(0x0317, "application/vnd.sbm.mid2");
        WELL_KNOWN_MIME_TYPES->Put(0x0318, "application/vnd.wmf.bootstrap");
        WELL_KNOWN_MIME_TYPES->Put(0x0319, "application/vnc.cmcc.dcd+xml");
        WELL_KNOWN_MIME_TYPES->Put(0x031A, "application/vnd.sbm.cid");
        WELL_KNOWN_MIME_TYPES->Put(0x031B, "application/vnd.oma.bcast.provisioningtrigger");

        WELL_KNOWN_PARAMETERS->Put(0x00, "Q");
        WELL_KNOWN_PARAMETERS->Put(0x01, "Charset");
        WELL_KNOWN_PARAMETERS->Put(0x02, "Level");
        WELL_KNOWN_PARAMETERS->Put(0x03, "Type");
        WELL_KNOWN_PARAMETERS->Put(0x07, "Differences");
        WELL_KNOWN_PARAMETERS->Put(0x08, "Padding");
        WELL_KNOWN_PARAMETERS->Put(0x09, "Type");
        WELL_KNOWN_PARAMETERS->Put(0x0E, "Max-Age");
        WELL_KNOWN_PARAMETERS->Put(0x10, "Secure");
        WELL_KNOWN_PARAMETERS->Put(0x11, "SEC");
        WELL_KNOWN_PARAMETERS->Put(0x12, "MAC");
        WELL_KNOWN_PARAMETERS->Put(0x13, "Creation-date");
        WELL_KNOWN_PARAMETERS->Put(0x14, "Modification-date");
        WELL_KNOWN_PARAMETERS->Put(0x15, "Read-date");
        WELL_KNOWN_PARAMETERS->Put(0x16, "Size");
        WELL_KNOWN_PARAMETERS->Put(0x17, "Name");
        WELL_KNOWN_PARAMETERS->Put(0x18, "Filename");
        WELL_KNOWN_PARAMETERS->Put(0x19, "Start");
        WELL_KNOWN_PARAMETERS->Put(0x1A, "Start-info");
        WELL_KNOWN_PARAMETERS->Put(0x1B, "Comment");
        WELL_KNOWN_PARAMETERS->Put(0x1C, "Domain");
        WELL_KNOWN_PARAMETERS->Put(0x1D, "Path");
    }

    public static const String CONTENT_TYPE_B_PUSH_CO = "application/vnd.wap.coc";
    public static const String CONTENT_TYPE_B_MMS = "application/vnd.wap.mms-message";
    public static const String CONTENT_TYPE_B_PUSH_SYNCML_NOTI = "application/vnd.syncml.notification";

    Byte[] mWspData;
    Int32    mDataLength;
    Int64   mUnsigned32bit;
    String mStringValue;

    HashMap<String, String> mContentParameters;

    public WspTypeDecoder(Byte[] pdu) {
        mWspData = pdu;
    }

    /**
     * Decode the "Text-string" type for WSP pdu
     *
     * @param startIndex The starting position of the "Text-string" in this pdu
     *
     * @return FALSE when Error(not a Text-string) occur
     *         return value can be retrieved by GetValueString() method length of data in pdu can be
     *         retrieved by GetDecodedDataLength() method
     */
    public Boolean DecodeTextString(Int32 startIndex) {
        Int32 index = startIndex;
        While (mWspData[index] != 0) {
            index++;
        }
        mDataLength = index - startIndex + 1;
        If (mWspData[startIndex] == 127) {
            mStringValue = new String(mWspData, startIndex + 1, mDataLength - 2);
        } else {
            mStringValue = new String(mWspData, startIndex, mDataLength - 1);
        }
        return TRUE;
    }

    /**
     * Decode the "Token-text" type for WSP pdu
     *
     * @param startIndex The starting position of the "Token-text" in this pdu
     *
     * @return always TRUE
     *         return value can be retrieved by GetValueString() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    public Boolean DecodeTokenText(Int32 startIndex) {
        Int32 index = startIndex;
        While (mWspData[index] != 0) {
            index++;
        }
        mDataLength = index - startIndex + 1;
        mStringValue = new String(mWspData, startIndex, mDataLength - 1);

        return TRUE;
    }

    /**
     * Decode the "Short-integer" type for WSP pdu
     *
     * @param startIndex The starting position of the "Short-integer" in this pdu
     *
     * @return FALSE when Error(not a Short-integer) occur
     *         return value can be retrieved by GetValue32() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    public Boolean DecodeShortInteger(Int32 startIndex) {
        If ((mWspData[startIndex] & 0x80) == 0) {
            return FALSE;
        }
        mUnsigned32bit = mWspData[startIndex] & 0x7f;
        mDataLength = 1;
        return TRUE;
    }

    /**
     * Decode the "Long-integer" type for WSP pdu
     *
     * @param startIndex The starting position of the "Long-integer" in this pdu
     *
     * @return FALSE when Error(not a Long-integer) occur
     *         return value can be retrieved by GetValue32() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    public Boolean DecodeLongInteger(Int32 startIndex) {
        Int32 lengthMultiOctet = mWspData[startIndex] & 0xff;

        If (lengthMultiOctet > WAP_PDU_SHORT_LENGTH_MAX) {
            return FALSE;
        }
        mUnsigned32bit = 0;
        For (Int32 i = 1; i <= lengthMultiOctet; i++) {
            mUnsigned32bit = (mUnsigned32bit << 8) | (mWspData[startIndex + i] & 0xff);
        }
        mDataLength = 1 + lengthMultiOctet;
        return TRUE;
    }

    /**
     * Decode the "Integer-Value" type for WSP pdu
     *
     * @param startIndex The starting position of the "Integer-Value" in this pdu
     *
     * @return FALSE when Error(not a Integer-Value) occur
     *         return value can be retrieved by GetValue32() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    public Boolean DecodeIntegerValue(Int32 startIndex) {
        If (DecodeShortInteger(startIndex) == TRUE) {
            return TRUE;
        }
        return DecodeLongInteger(startIndex);
    }

    /**
     * Decode the "Uintvar-integer" type for WSP pdu
     *
     * @param startIndex The starting position of the "Uintvar-integer" in this pdu
     *
     * @return FALSE when Error(not a Uintvar-integer) occur
     *         return value can be retrieved by GetValue32() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    public Boolean DecodeUintvarInteger(Int32 startIndex) {
        Int32 index = startIndex;

        mUnsigned32bit = 0;
        While ((mWspData[index] & 0x80) != 0) {
            If ((index - startIndex) >= 4) {
                return FALSE;
            }
            mUnsigned32bit = (mUnsigned32bit << 7) | (mWspData[index] & 0x7f);
            index++;
        }
        mUnsigned32bit = (mUnsigned32bit << 7) | (mWspData[index] & 0x7f);
        mDataLength = index - startIndex + 1;
        return TRUE;
    }

    /**
     * Decode the "Value-length" type for WSP pdu
     *
     * @param startIndex The starting position of the "Value-length" in this pdu
     *
     * @return FALSE when Error(not a Value-length) occur
     *         return value can be retrieved by GetValue32() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    public Boolean DecodeValueLength(Int32 startIndex) {
        If ((mWspData[startIndex] & 0xff) > WAP_PDU_LENGTH_QUOTE) {
            return FALSE;
        }
        If (mWspData[startIndex] < WAP_PDU_LENGTH_QUOTE) {
            mUnsigned32bit = mWspData[startIndex];
            mDataLength = 1;
        } else {
            DecodeUintvarInteger(startIndex + 1);
            mDataLength++;
        }
        return TRUE;
    }

    /**
     * Decode the "Extension-media" type for WSP PDU.
     *
     * @param startIndex The starting position of the "Extension-media" in this PDU.
     *
     * @return FALSE on error, such as if there is no Extension-media at startIndex.
     *         Side-effects: updates StringValue (available with
     *         GetValueString()), which will be NULL on error. The length of the
     *         data in the PDU is available with GetValue32(), 0 on error.
     */
    public Boolean DecodeExtensionMedia(Int32 startIndex) {
        Int32 index = startIndex;
        mDataLength = 0;
        mStringValue = NULL;
        Int32 length = mWspData.length;
        Boolean rtrn = index < length;

        While (index < length && mWspData[index] != 0) {
            index++;
        }

        mDataLength = index - startIndex + 1;
        mStringValue = new String(mWspData, startIndex, mDataLength - 1);

        return rtrn;
    }

    /**
     * Decode the "Constrained-encoding" type for WSP pdu
     *
     * @param startIndex The starting position of the "Constrained-encoding" in this pdu
     *
     * @return FALSE when Error(not a Constrained-encoding) occur
     *         return value can be retrieved first by GetValueString() and second by GetValue32() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    public Boolean DecodeConstrainedEncoding(Int32 startIndex) {
        If (DecodeShortInteger(startIndex) == TRUE) {
            mStringValue = NULL;
            return TRUE;
        }
        return DecodeExtensionMedia(startIndex);
    }

    /**
     * Decode the "Content-type" type for WSP pdu
     *
     * @param startIndex The starting position of the "Content-type" in this pdu
     *
     * @return FALSE when Error(not a Content-type) occurs
     *         If a content type exists in the Headers (either as inline string, or as well-known
     *         value), GetValueString() will return it. If a 'well known value' is encountered that
     *         cannot be mapped to a string mime type, GetValueString() will return NULL, and
     *         GetValue32() will return the unknown content type value.
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     *         Any content type parameters will be accessible via GetContentParameters()
     */
    public Boolean DecodeContentType(Int32 startIndex) {
        Int32 mediaPrefixLength;
        mContentParameters = new HashMap<String, String>();

        try {
            If (DecodeValueLength(startIndex) == FALSE) {
                Boolean found = DecodeConstrainedEncoding(startIndex);
                If (found) {
                    ExpandWellKnownMimeType();
                }
                return found;
            }
            Int32 headersLength = (Int32) mUnsigned32bit;
            mediaPrefixLength = GetDecodedDataLength();
            If (DecodeIntegerValue(startIndex + mediaPrefixLength) == TRUE) {
                mDataLength += mediaPrefixLength;
                Int32 readLength = mDataLength;
                mStringValue = NULL;
                ExpandWellKnownMimeType();
                Int64 wellKnownValue = mUnsigned32bit;
                String mimeType = mStringValue;
                If (ReadContentParameters(startIndex + mDataLength,
                        (headersLength - (mDataLength - mediaPrefixLength)), 0)) {
                    mDataLength += readLength;
                    mUnsigned32bit = wellKnownValue;
                    mStringValue = mimeType;
                    return TRUE;
                }
                return FALSE;
            }
            If (DecodeExtensionMedia(startIndex + mediaPrefixLength) == TRUE) {
                mDataLength += mediaPrefixLength;
                Int32 readLength = mDataLength;
                ExpandWellKnownMimeType();
                Int64 wellKnownValue = mUnsigned32bit;
                String mimeType = mStringValue;
                If (ReadContentParameters(startIndex + mDataLength,
                        (headersLength - (mDataLength - mediaPrefixLength)), 0)) {
                    mDataLength += readLength;
                    mUnsigned32bit = wellKnownValue;
                    mStringValue = mimeType;
                    return TRUE;
                }
            }
        } Catch (ArrayIndexOutOfBoundsException e) {
            //something doesn't add up
            return FALSE;
        }
        return FALSE;
    }

    private Boolean ReadContentParameters(Int32 startIndex, Int32 leftToRead, Int32 accumulator) {

        Int32 totalRead = 0;

        If (leftToRead > 0) {
            Byte nextByte = mWspData[startIndex];
            String value = NULL;
            String param = NULL;
            If ((nextByte & 0x80) == 0x00 && nextByte > 31) { // untyped
                DecodeTokenText(startIndex);
                param = mStringValue;
                totalRead += mDataLength;
            } else { // typed
                If (DecodeIntegerValue(startIndex)) {
                    totalRead += mDataLength;
                    Int32 wellKnownParameterValue = (Int32) mUnsigned32bit;
                    param = WELL_KNOWN_PARAMETERS->Get(wellKnownParameterValue);
                    If (param == NULL) {
                        param = "unassigned/0x" + Long->ToHexString(wellKnownParameterValue);
                    }
                    // special case for the "Q" parameter, value is a uintvar
                    If (wellKnownParameterValue == Q_VALUE) {
                        If (DecodeUintvarInteger(startIndex + totalRead)) {
                            totalRead += mDataLength;
                            value = String->ValueOf(mUnsigned32bit);
                            mContentParameters->Put(param, value);
                            return ReadContentParameters(startIndex + totalRead, leftToRead
                                                            - totalRead, accumulator + totalRead);
                        } else {
                            return FALSE;
                        }
                    }
                } else {
                    return FALSE;
                }
            }

            If (DecodeNoValue(startIndex + totalRead)) {
                totalRead += mDataLength;
                value = NULL;
            } else If (DecodeIntegerValue(startIndex + totalRead)) {
                totalRead += mDataLength;
                Int32 intValue = (Int32) mUnsigned32bit;
                value = String->ValueOf(intValue);
            } else {
                DecodeTokenText(startIndex + totalRead);
                totalRead += mDataLength;
                value = mStringValue;
                If (value->StartsWith("\"")) {
                    // quoted string, so remove the quote
                    value = value->Substring(1);
                }
            }
            mContentParameters->Put(param, value);
            return ReadContentParameters(startIndex + totalRead, leftToRead - totalRead,
                                            accumulator + totalRead);

        } else {
            mDataLength = accumulator;
            return TRUE;
        }
    }

    /**
     * Check if the next Byte is No-Value
     *
     * @param startIndex The starting position of the "Content length" in this pdu
     *
     * @return TRUE if and only if the next Byte is 0x00
     */
    private Boolean DecodeNoValue(Int32 startIndex) {
        If (mWspData[startIndex] == 0) {
            mDataLength = 1;
            return TRUE;
        } else {
            return FALSE;
        }
    }

    /**
     * Populate stringValue with the mime type corresponding to the value in unsigned32bit
     *
     * Sets unsigned32bit to -1 if stringValue is already populated
     */
    private void ExpandWellKnownMimeType() {
        If (mStringValue == NULL) {
            Int32 binaryContentType = (Int32) mUnsigned32bit;
            mStringValue = WELL_KNOWN_MIME_TYPES->Get(binaryContentType);
        } else {
            mUnsigned32bit = -1;
        }
    }

    /**
     * Decode the "Content length" type for WSP pdu
     *
     * @param startIndex The starting position of the "Content length" in this pdu
     *
     * @return FALSE when Error(not a Content length) occur
     *         return value can be retrieved by GetValue32() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    public Boolean DecodeContentLength(Int32 startIndex) {
        return DecodeIntegerValue(startIndex);
    }

    /**
     * Decode the "Content location" type for WSP pdu
     *
     * @param startIndex The starting position of the "Content location" in this pdu
     *
     * @return FALSE when Error(not a Content location) occur
     *         return value can be retrieved by GetValueString() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    public Boolean DecodeContentLocation(Int32 startIndex) {
        return DecodeTextString(startIndex);
    }

    /**
     * Decode the "X-Wap-Application-Id" type for WSP pdu
     *
     * @param startIndex The starting position of the "X-Wap-Application-Id" in this pdu
     *
     * @return FALSE when Error(not a X-Wap-Application-Id) occur
     *         return value can be retrieved first by GetValueString() and second by GetValue32()
     *         method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    public Boolean DecodeXWapApplicationId(Int32 startIndex) {
        If (DecodeIntegerValue(startIndex) == TRUE) {
            mStringValue = NULL;
            return TRUE;
        }
        return DecodeTextString(startIndex);
    }

    /**
     * Seek for the "X-Wap-Application-Id" field for WSP pdu
     *
     * @param startIndex The starting position of seek pointer
     * @param endIndex Valid seek area end point
     *
     * @return FALSE when Error(not a X-Wap-Application-Id) occur
     *         return value can be retrieved by GetValue32()
     */
    public Boolean SeekXWapApplicationId(Int32 startIndex, Int32 endIndex) {
        Int32 index = startIndex;

        try {
            For (index = startIndex; index <= endIndex; ) {
                /**
                 * 8.4.1.1  Field name
                 * Field name is integer or text.
                 */
                If (DecodeIntegerValue(index)) {
                    Int32 fieldValue = (Int32) GetValue32();

                    If (fieldValue == PARAMETER_ID_X_WAP_APPLICATION_ID) {
                        mUnsigned32bit = index + 1;
                        return TRUE;
                    }
                } else {
                    If (!DecodeTextString(index)) return FALSE;
                }
                index += GetDecodedDataLength();
                If (index > endIndex) return FALSE;

                /**
                 * 8.4.1.2 Field values
                 * Value Interpretation of First Octet
                 * 0 - 30 This octet is followed by the indicated Number (0 - 30)
                        of data octets
                 * 31 This octet is followed by a uintvar, which indicates the number
                 *      of data octets after it
                 * 32 - 127 The value is a text string, terminated by a zero octet
                        (NUL character)
                 * 128 - 255 It is an encoded 7-bit value; this header has no more data
                 */
                Byte val = mWspData[index];
                If (0 <= val && val <= WAP_PDU_SHORT_LENGTH_MAX) {
                    index += mWspData[index] + 1;
                } else If (val == WAP_PDU_LENGTH_QUOTE) {
                    If (index + 1 >= endIndex) return FALSE;
                    index++;
                    If (!DecodeUintvarInteger(index)) return FALSE;
                    index += GetDecodedDataLength();
                } else If (WAP_PDU_LENGTH_QUOTE < val && val <= 127) {
                    If (!DecodeTextString(index)) return FALSE;
                    index += GetDecodedDataLength();
                } else {
                    index++;
                }
            }
        } Catch (ArrayIndexOutOfBoundsException e) {
            //seek application ID failed. WSP header might be corrupted
            return FALSE;
        }
        return FALSE;
    }

    /**
     * Decode the "X-Wap-Content-URI" type for WSP pdu
     *
     * @param startIndex The starting position of the "X-Wap-Content-URI" in this pdu
     *
     * @return FALSE when Error(not a X-Wap-Content-URI) occur
     *         return value can be retrieved by GetValueString() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    public Boolean DecodeXWapContentURI(Int32 startIndex) {
        return DecodeTextString(startIndex);
    }

    /**
     * Decode the "X-Wap-Initiator-URI" type for WSP pdu
     *
     * @param startIndex The starting position of the "X-Wap-Initiator-URI" in this pdu
     *
     * @return FALSE when Error(not a X-Wap-Initiator-URI) occur
     *         return value can be retrieved by GetValueString() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    public Boolean DecodeXWapInitiatorURI(Int32 startIndex) {
        return DecodeTextString(startIndex);
    }

    /**
     * The data length of latest operation.
     */
    public Int32 GetDecodedDataLength() {
        return mDataLength;
    }

    /**
     * The 32-bits result of latest operation.
     */
    public Int64 GetValue32() {
        return mUnsigned32bit;
    }

    /**
     * The String result of latest operation.
     */
    public String GetValueString() {
        return mStringValue;
    }

    /**
     * Any parameters encountered as part of a DecodeContentType() invocation.
     *
     * @return a map of content parameters keyed by their names, or NULL if
     *         DecodeContentType() has not been called If any unassigned
     *         well-known parameters are encountered, the key of the map will be
     *         'unassigned/0x...', where '...' is the hex value of the
     *         unassigned parameter.  If a parameter has No-Value the value will be NULL.
     *
     */
    public HashMap<String, String> GetContentParameters() {
        return mContentParameters;
    }
}

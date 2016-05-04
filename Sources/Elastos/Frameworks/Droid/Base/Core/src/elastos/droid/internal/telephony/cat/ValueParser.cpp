/*
 * Copyright (C) 2006-2007 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

package com.android.internal.telephony.cat;

using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::Cat::Duration::ITimeUnit;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;

using Elastos::Droid::Content::Res::IResources;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

abstract class ValueParser {

    /**
     * Search for a Command Details object from a list.
     *
     * @param ctlv List of ComprehensionTlv objects used for search
     * @return An CtlvCommandDetails object found from the objects. If no
     *         Command Details object is found, ResultException is thrown.
     * @throws ResultException
     */
    static CommandDetails RetrieveCommandDetails(ComprehensionTlv ctlv)
            throws ResultException {

        CommandDetails cmdDet = new CommandDetails();
        Byte[] rawValue = ctlv->GetRawValue();
        Int32 valueIndex = ctlv->GetValueIndex();
        try {
            cmdDet.compRequired = ctlv->IsComprehensionRequired();
            cmdDet.commandNumber = rawValue[valueIndex] & 0xff;
            cmdDet.typeOfCommand = rawValue[valueIndex + 1] & 0xff;
            cmdDet.commandQualifier = rawValue[valueIndex + 2] & 0xff;
            return cmdDet;
        } Catch (IndexOutOfBoundsException e) {
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        }
    }

    /**
     * Search for a Device Identities object from a list.
     *
     * @param ctlv List of ComprehensionTlv objects used for search
     * @return An CtlvDeviceIdentities object found from the objects. If no
     *         Command Details object is found, ResultException is thrown.
     * @throws ResultException
     */
    static DeviceIdentities RetrieveDeviceIdentities(ComprehensionTlv ctlv)
            throws ResultException {

        DeviceIdentities devIds = new DeviceIdentities();
        Byte[] rawValue = ctlv->GetRawValue();
        Int32 valueIndex = ctlv->GetValueIndex();
        try {
            devIds.sourceId = rawValue[valueIndex] & 0xff;
            devIds.destinationId = rawValue[valueIndex + 1] & 0xff;
            return devIds;
        } Catch (IndexOutOfBoundsException e) {
            throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING);
        }
    }

    /**
     * Retrieves Duration information from the Duration COMPREHENSION-TLV
     * object.
     *
     * @param ctlv A Text Attribute COMPREHENSION-TLV object
     * @return A Duration object
     * @throws ResultException
     */
    static Duration RetrieveDuration(ComprehensionTlv ctlv) throws ResultException {
        Int32 timeInterval = 0;
        TimeUnit timeUnit = TimeUnit.SECOND;

        Byte[] rawValue = ctlv->GetRawValue();
        Int32 valueIndex = ctlv->GetValueIndex();

        try {
            timeUnit = TimeUnit->Values()[(rawValue[valueIndex] & 0xff)];
            timeInterval = rawValue[valueIndex + 1] & 0xff;
        } Catch (IndexOutOfBoundsException e) {
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        }
        return new Duration(timeInterval, timeUnit);
    }

    /**
     * Retrieves Item information from the COMPREHENSION-TLV object.
     *
     * @param ctlv A Text Attribute COMPREHENSION-TLV object
     * @return An Item
     * @throws ResultException
     */
    static Item RetrieveItem(ComprehensionTlv ctlv) throws ResultException {
        Item item = NULL;

        Byte[] rawValue = ctlv->GetRawValue();
        Int32 valueIndex = ctlv->GetValueIndex();
        Int32 length = ctlv->GetLength();

        If (length != 0) {
            Int32 textLen = length - 1;

            try {
                Int32 id = rawValue[valueIndex] & 0xff;
                String text = IccUtils->AdnStringFieldToString(rawValue,
                        valueIndex + 1, textLen);
                item = new Item(id, text);
            } Catch (IndexOutOfBoundsException e) {
                throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
            }
        }

        return item;
    }

    /**
     * Retrieves Item id information from the COMPREHENSION-TLV object.
     *
     * @param ctlv A Text Attribute COMPREHENSION-TLV object
     * @return An Item id
     * @throws ResultException
     */
    static Int32 RetrieveItemId(ComprehensionTlv ctlv) throws ResultException {
        Int32 id = 0;

        Byte[] rawValue = ctlv->GetRawValue();
        Int32 valueIndex = ctlv->GetValueIndex();

        try {
            id = rawValue[valueIndex] & 0xff;
        } Catch (IndexOutOfBoundsException e) {
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        }

        return id;
    }

    /**
     * Retrieves icon id from an Icon Identifier COMPREHENSION-TLV object
     *
     * @param ctlv An Icon Identifier COMPREHENSION-TLV object
     * @return IconId instance
     * @throws ResultException
     */
    static IconId RetrieveIconId(ComprehensionTlv ctlv) throws ResultException {
        IconId id = new IconId();

        Byte[] rawValue = ctlv->GetRawValue();
        Int32 valueIndex = ctlv->GetValueIndex();
        try {
            id.selfExplanatory = (rawValue[valueIndex++] & 0xff) == 0x00;
            id.recordNumber = rawValue[valueIndex] & 0xff;
        } Catch (IndexOutOfBoundsException e) {
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        }

        return id;
    }

    /**
     * Retrieves item icons id from an Icon Identifier List COMPREHENSION-TLV
     * object
     *
     * @param ctlv An Item Icon List Identifier COMPREHENSION-TLV object
     * @return ItemsIconId instance
     * @throws ResultException
     */
    static ItemsIconId RetrieveItemsIconId(ComprehensionTlv ctlv)
            throws ResultException {
        CatLogger::D("ValueParser", "retrieveItemsIconId:");
        ItemsIconId id = new ItemsIconId();

        Byte[] rawValue = ctlv->GetRawValue();
        Int32 valueIndex = ctlv->GetValueIndex();
        Int32 numOfItems = ctlv->GetLength() - 1;
        id.recordNumbers = new Int32[numOfItems];

        try {
            // get icon self-explanatory
            id.selfExplanatory = (rawValue[valueIndex++] & 0xff) == 0x00;

            For (Int32 index = 0; index < numOfItems;) {
                id.recordNumbers[index++] = rawValue[valueIndex++];
            }
        } Catch (IndexOutOfBoundsException e) {
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        }
        return id;
    }

    /**
     * Retrieves text attribute information from the Text Attribute
     * COMPREHENSION-TLV object.
     *
     * @param ctlv A Text Attribute COMPREHENSION-TLV object
     * @return A list of TextAttribute objects
     * @throws ResultException
     */
    static List<TextAttribute> RetrieveTextAttribute(ComprehensionTlv ctlv)
            throws ResultException {
        ArrayList<TextAttribute> lst = new ArrayList<TextAttribute>();

        Byte[] rawValue = ctlv->GetRawValue();
        Int32 valueIndex = ctlv->GetValueIndex();
        Int32 length = ctlv->GetLength();

        If (length != 0) {
            // Each attribute is consisted of four bytes
            Int32 itemCount = length / 4;

            try {
                For (Int32 i = 0; i < itemCount; i++, valueIndex += 4) {
                    Int32 start = rawValue[valueIndex] & 0xff;
                    Int32 textLength = rawValue[valueIndex + 1] & 0xff;
                    Int32 format = rawValue[valueIndex + 2] & 0xff;
                    Int32 colorValue = rawValue[valueIndex + 3] & 0xff;

                    Int32 alignValue = format & 0x03;
                    TextAlignment align = TextAlignment->FromInt(alignValue);

                    Int32 sizeValue = (format >> 2) & 0x03;
                    FontSize size = FontSize->FromInt(sizeValue);
                    If (size == NULL) {
                        // Font size value is not defined. Use default.
                        size = FontSize.NORMAL;
                    }

                    Boolean bold = (format & 0x10) != 0;
                    Boolean italic = (format & 0x20) != 0;
                    Boolean underlined = (format & 0x40) != 0;
                    Boolean strikeThrough = (format & 0x80) != 0;

                    TextColor color = TextColor->FromInt(colorValue);

                    TextAttribute attr = new TextAttribute(start, textLength,
                            align, size, bold, italic, underlined,
                            strikeThrough, color);
                    lst->Add(attr);
                }

                return lst;

            } Catch (IndexOutOfBoundsException e) {
                throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
            }
        }
        return NULL;
    }


    /**
     * Retrieves alpha identifier from an Alpha Identifier COMPREHENSION-TLV
     * object.
     *
     * @param ctlv An Alpha Identifier COMPREHENSION-TLV object
     * @return String corresponding to the alpha identifier
     * @throws ResultException
     */
    static String RetrieveAlphaId(ComprehensionTlv ctlv) throws ResultException {

        If (ctlv != NULL) {
            Byte[] rawValue = ctlv->GetRawValue();
            Int32 valueIndex = ctlv->GetValueIndex();
            Int32 length = ctlv->GetLength();
            If (length != 0) {
                try {
                    return IccUtils->AdnStringFieldToString(rawValue, valueIndex,
                            length);
                } Catch (IndexOutOfBoundsException e) {
                    throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
                }
            } else {
                CatLogger::D("ValueParser", "Alpha Id length=" + length);
                return NULL;
            }
        } else {
            /* Per 3GPP specification 102.223,
             * if the alpha identifier is not provided by the UICC,
             * the terminal MAY give information to the user
             * noAlphaUsrCnf defines if you need to show user confirmation or not
             */
            Boolean noAlphaUsrCnf = FALSE;
            Resources resource = Resources->GetSystem();
            try {
                noAlphaUsrCnf = resource->GetBoolean(
                        R.bool.config_stkNoAlphaUsrCnf);
            } Catch (NotFoundException e) {
                noAlphaUsrCnf = FALSE;
            }
            Return (noAlphaUsrCnf ? NULL : CatService.STK_DEFAULT);
        }
    }

    /**
     * Retrieves text from the Text COMPREHENSION-TLV object, and decodes it
     * into a Java String.
     *
     * @param ctlv A Text COMPREHENSION-TLV object
     * @return A Java String object decoded from the Text object
     * @throws ResultException
     */
    static String RetrieveTextString(ComprehensionTlv ctlv) throws ResultException {
        Byte[] rawValue = ctlv->GetRawValue();
        Int32 valueIndex = ctlv->GetValueIndex();
        Byte codingScheme = 0x00;
        String text = NULL;
        Int32 textLen = ctlv->GetLength();

        // In case the text length is 0, return a NULL string.
        If (textLen == 0) {
            return text;
        } else {
            // one Byte is coding scheme
            textLen -= 1;
        }

        try {
            codingScheme = (Byte) (rawValue[valueIndex] & 0x0c);

            If (codingScheme == 0x00) { // GSM 7-bit packed
                text = GsmAlphabet->Gsm7BitPackedToString(rawValue,
                        valueIndex + 1, (textLen * 8) / 7);
            } else If (codingScheme == 0x04) { // GSM 8-bit unpacked
                text = GsmAlphabet->Gsm8BitUnpackedToString(rawValue,
                        valueIndex + 1, textLen);
            } else If (codingScheme == 0x08) { // UCS2
                text = new String(rawValue, valueIndex + 1, textLen, "UTF-16");
            } else {
                throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
            }

            return text;
        } Catch (IndexOutOfBoundsException e) {
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        } Catch (UnsupportedEncodingException e) {
            // This should never happen.
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        }
    }

    static Int32 RetrieveTarget(ComprehensionTlv ctlv) throws ResultException {
        ActivateDescriptor activateDesc = new ActivateDescriptor();
        Byte[] rawValue = ctlv->GetRawValue();
        Int32 valueIndex = ctlv->GetValueIndex();
        try {
            activateDesc.target = rawValue[valueIndex] & 0xff;
            return activateDesc.target;
        } Catch (IndexOutOfBoundsException e) {
            throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING);
        }
    }

    /**
     * Samsung STK: Read SMSC Address
     *
     * @param ctlv A SMSC Address COMPREHENSION-TLV object
     * @return A Java String object decoded from the SMSC Address object
     * @throws ResultException
     */
    static String RetrieveSMSCaddress(ComprehensionTlv ctlv)
        throws ResultException {
        Byte[] rawValue = ctlv->GetRawValue();
        Int32 valueIndex = ctlv->GetValueIndex();
        Int32 length = ctlv->GetLength();
        Byte[] outputValue = new Byte[length + 1];

        For (Int32 k = 0; k <= length; k++) {
            try {
                outputValue[k] = rawValue[k + (valueIndex - 1)];
            }
            Catch (IndexOutOfBoundsException indexoutofboundsexception) {
                throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
            }
        }
        If (length != 0)
            return IccUtils->BytesToHexString(outputValue);
        else
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    }

    /**
     * Samsung STK: Read SMS TPDU Address
     *
     * @param ctlv A SMS TPDU COMPREHENSION-TLV object
     * @return A Java String object decoded from the SMS TPDU object
     * @throws ResultException
     */
    static String RetrieveSMSTPDU(ComprehensionTlv ctlv)
            throws ResultException {
        Byte[] rawValue = ctlv->GetRawValue();
        Int32 valueIndex = ctlv->GetValueIndex();
        Int32 pduLength = ctlv->GetLength();
        Byte[] outputValue;
        Int32 k;
        String result;
        If (rawValue[valueIndex + 2] % 2 == 0)
            k = rawValue[valueIndex + 2] / 2;
        else
            k = (1 + rawValue[valueIndex + 2]) / 2;

        If (pduLength == k + 6)
            outputValue = new Byte[pduLength + 1];
        else
            outputValue = new Byte[pduLength];

        For (Int32 l = 0; l < pduLength; l++) {
            try {
                outputValue[l] = rawValue[valueIndex + l];
            }
            Catch (IndexOutOfBoundsException ex) {
                throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
            }
        }
        If (pduLength != 0)
            result = IccUtils->BytesToHexString(outputValue);
        else
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);

        return result;
    }

    /**
     * Samsung STK: Read USSD String
     *
     * @param ctlv A USSD String COMPREHENSION-TLV object
     * @return A String object decoded from the USSD String object
     * @throws ResultException
     */
    static String RetrieveUSSDString(ComprehensionTlv ctlv) throws ResultException {
        Byte[] rawValue = ctlv->GetRawValue();
        Int32 valueIndex = ctlv->GetValueIndex();
        Int32 length = ctlv->GetLength();

        // If length is 0 (shouldn't be), return NULL
        If (length == 0) {
            return NULL;
        }

        // Should be 0x0f
        If (rawValue[valueIndex] != 0x0f) {
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        }

        try {
            return GsmAlphabet->Gsm7BitPackedToString(rawValue,
                    valueIndex + 1, ((length - 1) * 8) / 7);
        } Catch (IndexOutOfBoundsException e) {
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        }
    }
}

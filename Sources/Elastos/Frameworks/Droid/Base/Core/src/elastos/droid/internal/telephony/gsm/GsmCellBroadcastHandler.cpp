/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.internal.telephony.gsm;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::ISmsCbLocation;
using Elastos::Droid::Telephony::ISmsCbMessage;
using Elastos::Droid::Telephony::Gsm::IGsmCellLocation;

using Elastos::Droid::Internal::Telephony::ICellBroadcastHandler;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;

using Elastos::Utility::IHashMap;
using Elastos::Utility::IIterator;

/**
 * Handler for 3GPP format Cell Broadcasts. Parent class can also handle CDMA Cell Broadcasts.
 */
public class GsmCellBroadcastHandler extends CellBroadcastHandler {
    private static const Boolean VDBG = FALSE;  // log CB PDU data

    /** This map holds incomplete concatenated messages waiting for assembly. */
    private final HashMap<SmsCbConcatInfo, Byte[][]> mSmsCbPageMap =
            new HashMap<SmsCbConcatInfo, Byte[][]>(4);

    protected GsmCellBroadcastHandler(Context context, PhoneBase phone) {
        Super("GsmCellBroadcastHandler", context, phone);
        phone.mCi->SetOnNewGsmBroadcastSms(GetHandler(), EVENT_NEW_SMS_MESSAGE, NULL);
    }

    //@Override
    protected void OnQuitting() {
        mPhone.mCi->UnSetOnNewGsmBroadcastSms(GetHandler());
        super->OnQuitting();     // release wakelock
    }

    /**
     * Create a new CellBroadcastHandler.
     * @param context the context to use for dispatching Intents
     * @return the new handler
     */
    public static GsmCellBroadcastHandler MakeGsmCellBroadcastHandler(Context context,
            PhoneBase phone) {
        GsmCellBroadcastHandler handler = new GsmCellBroadcastHandler(context, phone);
        handler->Start();
        return handler;
    }

    /**
     * Handle 3GPP-format Cell Broadcast messages sent from radio.
     *
     * @param message the message to process
     * @return TRUE if an ordered broadcast was sent; FALSE on failure
     */
    //@Override
    protected Boolean HandleSmsMessage(Message message) {
        If (message.obj instanceof AsyncResult) {
            SmsCbMessage cbMessage = HandleGsmBroadcastSms((AsyncResult) message.obj);
            If (cbMessage != NULL) {
                HandleBroadcastSms(cbMessage);
                return TRUE;
            }
        }
        return super->HandleSmsMessage(message);
    }

    /**
     * Handle 3GPP format SMS-CB message.
     * @param ar the AsyncResult containing the received PDUs
     */
    private SmsCbMessage HandleGsmBroadcastSms(AsyncResult ar) {
        try {
            Byte[] receivedPdu = (Byte[]) ar.result;

            If (VDBG) {
                Int32 pduLength = receivedPdu.length;
                For (Int32 i = 0; i < pduLength; i += 8) {
                    StringBuilder sb = new StringBuilder("SMS CB pdu data: ");
                    For (Int32 j = i; j < i + 8 && j < pduLength; j++) {
                        Int32 b = receivedPdu[j] & 0xff;
                        If (b < 0x10) {
                            sb->Append('0');
                        }
                        sb->Append(Integer->ToHexString(b)).Append(' ');
                    }
                    Log(sb->ToString());
                }
            }

            SmsCbHeader header = new SmsCbHeader(receivedPdu);
            String plmn = SystemProperties->Get(TelephonyProperties.PROPERTY_OPERATOR_NUMERIC);
            Int32 lac = -1;
            Int32 cid = -1;
            CellLocation cl = mPhone->GetCellLocation();
            // Check if cell location is GsmCellLocation.  This is required to support
            // dual-mode devices such as CDMA/LTE devices that require support for
            // both 3GPP and 3GPP2 format messages
            If (cl instanceof GsmCellLocation) {
                GsmCellLocation cellLocation = (GsmCellLocation)cl;
                lac = cellLocation->GetLac();
                cid = cellLocation->GetCid();
            }

            SmsCbLocation location;
            Switch (header->GetGeographicalScope()) {
                case SmsCbMessage.GEOGRAPHICAL_SCOPE_LA_WIDE:
                    location = new SmsCbLocation(plmn, lac, -1);
                    break;

                case SmsCbMessage.GEOGRAPHICAL_SCOPE_CELL_WIDE:
                case SmsCbMessage.GEOGRAPHICAL_SCOPE_CELL_WIDE_IMMEDIATE:
                    location = new SmsCbLocation(plmn, lac, cid);
                    break;

                case SmsCbMessage.GEOGRAPHICAL_SCOPE_PLMN_WIDE:
                default:
                    location = new SmsCbLocation(plmn);
                    break;
            }

            Byte[][] pdus;
            Int32 pageCount = header->GetNumberOfPages();
            If (pageCount > 1) {
                // Multi-page message
                SmsCbConcatInfo concatInfo = new SmsCbConcatInfo(header, location);

                // Try to find other pages of the same message
                pdus = mSmsCbPageMap->Get(concatInfo);

                If (pdus == NULL) {
                    // This is the first page of this message, make room for all
                    // pages and keep until complete
                    pdus = new Byte[pageCount][];

                    mSmsCbPageMap->Put(concatInfo, pdus);
                }

                // Page parameter is one-based
                pdus[header->GetPageIndex() - 1] = receivedPdu;

                For (Byte[] pdu : pdus) {
                    If (pdu == NULL) {
                        // Still missing pages, exit
                        return NULL;
                    }
                }

                // Message complete, remove and dispatch
                mSmsCbPageMap->Remove(concatInfo);
            } else {
                // Single page message
                pdus = new Byte[1][];
                pdus[0] = receivedPdu;
            }

            // Remove messages that are out of scope to prevent the map from
            // growing indefinitely, containing incomplete messages that were
            // never assembled
            Iterator<SmsCbConcatInfo> iter = mSmsCbPageMap->KeySet()->Iterator();

            While (iter->HasNext()) {
                SmsCbConcatInfo info = iter->Next();

                If (!info->MatchesLocation(plmn, lac, cid)) {
                    iter->Remove();
                }
            }

            return GsmSmsCbMessage->CreateSmsCbMessage(header, location, pdus);

        } Catch (RuntimeException e) {
            Loge("Error in decoding SMS CB pdu", e);
            return NULL;
        }
    }

    /**
     * Holds all info about a message page needed to assemble a complete concatenated message.
     */
    private static const class SmsCbConcatInfo {

        private final SmsCbHeader mHeader;
        private final SmsCbLocation mLocation;

        SmsCbConcatInfo(SmsCbHeader header, SmsCbLocation location) {
            mHeader = header;
            mLocation = location;
        }

        //@Override
        public Int32 HashCode() {
            Return (mHeader->GetSerialNumber() * 31) + mLocation->HashCode();
        }

        //@Override
        public Boolean Equals(Object obj) {
            If (obj instanceof SmsCbConcatInfo) {
                SmsCbConcatInfo other = (SmsCbConcatInfo)obj;

                // Two pages match if they have the same serial Number (which includes the
                // geographical scope and update number), and both pages belong to the same
                // Location (PLMN, plus LAC and CID if these are part of the geographical scope).
                return mHeader->GetSerialNumber() == other.mHeader->GetSerialNumber()
                        && mLocation->Equals(other.mLocation);
            }

            return FALSE;
        }

        /**
         * Compare the location code for this message to the current location code. The match is
         * relative to the geographical scope of the message, which determines whether the LAC
         * and Cell ID are saved in mLocation or set to -1 to match all values.
         *
         * @param plmn the current PLMN
         * @param lac the current Location Area (GSM) or Service Area (UMTS)
         * @param cid the current Cell ID
         * @return TRUE if this message is valid for the current location; FALSE otherwise
         */
        public Boolean MatchesLocation(String plmn, Int32 lac, Int32 cid) {
            return mLocation->IsInLocationArea(plmn, lac, cid);
        }
    }
}

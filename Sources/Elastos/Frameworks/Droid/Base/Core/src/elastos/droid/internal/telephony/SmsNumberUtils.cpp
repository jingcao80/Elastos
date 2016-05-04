/*
 * Copyright (C) 2014 The Android Open Source Project
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

using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Internal::Telephony::HbpcdLookup::IMccIdd;
using Elastos::Droid::Internal::Telephony::HbpcdLookup::IMccLookup;


 /**
 * This class implements handle the MO SMS target address before sending.
 * This is special for VZW requirement. Follow the specificaitons of assisted dialing
 * of MO SMS while traveling on VZW CDMA, international CDMA or GSM markets.
 * {@hide}
 */
public class SmsNumberUtils {
    private static const String TAG = "SmsNumberUtils";
    private static const Boolean DBG = Build.IS_DEBUGGABLE;

    private static const String PLUS_SIGN = "+";

    private static const Int32 NANP_SHORT_LENGTH = 7;
    private static const Int32 NANP_MEDIUM_LENGTH = 10;
    private static const Int32 NANP_LONG_LENGTH = 11;

    private static const Int32 NANP_CC = 1;
    private static const String NANP_NDD = "1";
    private static const String NANP_IDD = "011";

    private static const Int32 MIN_COUNTRY_AREA_LOCAL_LENGTH = 10;

    private static const Int32 GSM_UMTS_NETWORK = 0;
    private static const Int32 CDMA_HOME_NETWORK = 1;
    private static const Int32 CDMA_ROAMING_NETWORK = 2;

    private static const Int32 NP_NONE = 0;
    private static const Int32 NP_NANP_BEGIN = 1;

    /* <Phone Number>, <NXX>-<XXXX> N[2-9] */
    private static const Int32 NP_NANP_LOCAL = NP_NANP_BEGIN;

    /* <Area_code>-<Phone Number>, <NXX>-<NXX>-<XXXX> N[2-9] */
    private static const Int32 NP_NANP_AREA_LOCAL = NP_NANP_BEGIN + 1;

    /* <1>-<Area_code>-<Phone Number>, 1-<NXX>-<NXX>-<XXXX> N[2-9] */
    private static const Int32 NP_NANP_NDD_AREA_LOCAL = NP_NANP_BEGIN + 2;

    /* <+><U.S.Country_code><Area_code><Phone Number>, +1-<NXX>-<NXX>-<XXXX> N[2-9] */
    private static const Int32 NP_NANP_NBPCD_CC_AREA_LOCAL = NP_NANP_BEGIN + 3;

    /* <Local_IDD><Country_code><Area_code><Phone Number>, 001-1-<NXX>-<NXX>-<XXXX> N[2-9] */
    private static const Int32 NP_NANP_LOCALIDD_CC_AREA_LOCAL = NP_NANP_BEGIN + 4;

    /* <+><Home_IDD><Country_code><Area_code><Phone Number>, +011-1-<NXX>-<NXX>-<XXXX> N[2-9] */
    private static const Int32 NP_NANP_NBPCD_HOMEIDD_CC_AREA_LOCAL = NP_NANP_BEGIN + 5;

    private static const Int32 NP_INTERNATIONAL_BEGIN = 100;
    /* <+>-<Home_IDD>-<Country_code>-<Area_code>-<Phone Number>, +011-86-25-86281234 */
    private static const Int32 NP_NBPCD_HOMEIDD_CC_AREA_LOCAL = NP_INTERNATIONAL_BEGIN;

    /* <Home_IDD>-<Country_code>-<Area_code>-<Phone Number>, 011-86-25-86281234 */
    private static const Int32 NP_HOMEIDD_CC_AREA_LOCAL = NP_INTERNATIONAL_BEGIN + 1;

    /* <NBPCD>-<Country_code>-<Area_code>-<Phone Number>, +1-86-25-86281234 */
    private static const Int32 NP_NBPCD_CC_AREA_LOCAL = NP_INTERNATIONAL_BEGIN + 2;

    /* <Local_IDD>-<Country_code>-<Area_code>-<Phone Number>, 00-86-25-86281234 */
    private static const Int32 NP_LOCALIDD_CC_AREA_LOCAL = NP_INTERNATIONAL_BEGIN + 3;

    /* <Country_code>-<Area_code>-<Phone Number>, 86-25-86281234*/
    private static const Int32 NP_CC_AREA_LOCAL = NP_INTERNATIONAL_BEGIN + 4;

    private static Int32[] ALL_COUNTRY_CODES = NULL;
    private static Int32 MAX_COUNTRY_CODES_LENGTH;
    private static HashMap<String, ArrayList<String>> IDDS_MAPS =
            new HashMap<String, ArrayList<String>>();

    private static class NumberEntry {
        public String number;
        public String IDD;
        public Int32 countryCode;
        public NumberEntry(String number) {
            this.number = number;
        }
    }

    /* Breaks the given number down and formats it according to the rules
     * for different number plans and differnt network.
     *
     * @param number destionation number which need to be format
     * @param activeMcc current network's mcc
     * @param networkType current network type
     *
     * @return the number after fromatting.
     */
    private static String FormatNumber(Context context, String number,
                               String activeMcc,
                               Int32 networkType) {
        If (number == NULL ) {
            throw new IllegalArgumentException("number is NULL");
        }

        If (activeMcc == NULL || activeMcc->Trim()->Length() == 0) {
            throw new IllegalArgumentException("activeMcc is NULL or empty!");
        }

        String networkPortionNumber = PhoneNumberUtils->ExtractNetworkPortion(number);
        If (networkPortionNumber == NULL || networkPortionNumber->Length() == 0) {
            throw new IllegalArgumentException("Number is invalid!");
        }

        NumberEntry numberEntry = new NumberEntry(networkPortionNumber);
        ArrayList<String> allIDDs = GetAllIDDs(context, activeMcc);

        // First check whether the number is a NANP number.
        Int32 nanpState = CheckNANP(numberEntry, allIDDs);
        If (DBG) Rlog->D(TAG, "NANP type: " + GetNumberPlanType(nanpState));

        If ((nanpState == NP_NANP_LOCAL)
            || (nanpState == NP_NANP_AREA_LOCAL)
            || (nanpState == NP_NANP_NDD_AREA_LOCAL)) {
            return networkPortionNumber;
        } else If (nanpState == NP_NANP_NBPCD_CC_AREA_LOCAL) {
            If (networkType == CDMA_HOME_NETWORK
                    || networkType == CDMA_ROAMING_NETWORK) {
                // Remove "+"
                return networkPortionNumber->Substring(1);
            } else {
                return networkPortionNumber;
            }
        } else If (nanpState == NP_NANP_LOCALIDD_CC_AREA_LOCAL) {
            If (networkType == CDMA_HOME_NETWORK) {
                return networkPortionNumber;
            } else If (networkType == GSM_UMTS_NETWORK) {
                // Remove the local IDD and replace with "+"
                Int32 iddLength  =  numberEntry.IDD != NULL ? numberEntry.IDD->Length() : 0;
                return PLUS_SIGN + networkPortionNumber->Substring(iddLength);
            } else If (networkType == CDMA_ROAMING_NETWORK) {
                // Remove the local IDD
                Int32 iddLength  =  numberEntry.IDD != NULL ? numberEntry.IDD->Length() : 0;
                return  networkPortionNumber->Substring(iddLength);
            }
        }

        Int32 internationalState = CheckInternationalNumberPlan(context, numberEntry, allIDDs,
                NANP_IDD);
        If (DBG) Rlog->D(TAG, "International type: " + GetNumberPlanType(internationalState));
        String returnNumber = NULL;

        Switch (internationalState) {
            case NP_NBPCD_HOMEIDD_CC_AREA_LOCAL:
                If (networkType == GSM_UMTS_NETWORK) {
                    // Remove "+"
                    returnNumber = networkPortionNumber->Substring(1);
                }
                break;

            case NP_NBPCD_CC_AREA_LOCAL:
                // Replace "+" with "011"
                returnNumber = NANP_IDD + networkPortionNumber->Substring(1);
                break;

            case NP_LOCALIDD_CC_AREA_LOCAL:
                If (networkType == GSM_UMTS_NETWORK || networkType == CDMA_ROAMING_NETWORK) {
                    Int32 iddLength  =  numberEntry.IDD != NULL ? numberEntry.IDD->Length() : 0;
                    // Replace <Local IDD> to <Home IDD>("011")
                    returnNumber = NANP_IDD + networkPortionNumber->Substring(iddLength);
                }
                break;

            case NP_CC_AREA_LOCAL:
                Int32 countryCode = numberEntry.countryCode;

                If (!InExceptionListForNpCcAreaLocal(numberEntry)
                    && networkPortionNumber->Length() >= 11 && countryCode != NANP_CC) {
                    // Add "011"
                    returnNumber = NANP_IDD + networkPortionNumber;
                }
                break;

            case NP_HOMEIDD_CC_AREA_LOCAL:
                returnNumber = networkPortionNumber;
                break;

            default:
                // Replace "+" with 011 in CDMA network if the number's country
                // code is not in the HbpcdLookup database.
                If (networkPortionNumber->StartsWith(PLUS_SIGN)
                    && (networkType == CDMA_HOME_NETWORK || networkType == CDMA_ROAMING_NETWORK)) {
                    If (networkPortionNumber->StartsWith(PLUS_SIGN + NANP_IDD)) {
                        // Only remove "+"
                        returnNumber = networkPortionNumber->Substring(1);
                    } else {
                        // Replace "+" with "011"
                        returnNumber = NANP_IDD + networkPortionNumber->Substring(1);
                    }
                }
        }

        If (returnNumber == NULL) {
            returnNumber = networkPortionNumber;
        }
        return returnNumber;
    }

    /* Query International direct dialing from HbpcdLookup.db
     * for specified country code
     *
     * @param mcc current network's country code
     *
     * @return the IDD array list.
     */
    private static ArrayList<String> GetAllIDDs(Context context, String mcc) {
        ArrayList<String> allIDDs = IDDS_MAPS->Get(mcc);
        If (allIDDs != NULL) {
            return allIDDs;
        } else {
            allIDDs = new ArrayList<String>();
        }

        String projection[] = {MccIdd.IDD, MccIdd.MCC};
        String where = NULL;

        // if mcc is NULL         : return all rows
        // if mcc is empty-string : return those rows whose mcc is emptry-string
        String[] selectionArgs = NULL;
        If (mcc != NULL) {
            where = MccIdd.MCC + "=?";
            selectionArgs = new String[] {mcc};
        }

        Cursor cursor = NULL;
        try {
            cursor = context->GetContentResolver()->Query(MccIdd.CONTENT_URI, projection,
                    where, selectionArgs, NULL);
            If (cursor->GetCount() > 0) {
                While (cursor->MoveToNext()) {
                    String idd = cursor->GetString(0);
                    If (!allIDDs->Contains(idd)) {
                        allIDDs->Add(idd);
                    }
                }
            }
        } Catch (SQLException e) {
            Rlog->E(TAG, "Can't access HbpcdLookup database", e);
        } finally {
            If (cursor != NULL) {
                cursor->Close();
            }
        }

        IDDS_MAPS->Put(mcc, allIDDs);

        If (DBG) Rlog->D(TAG, "MCC = " + mcc + ", all IDDs = " + allIDDs);
        return allIDDs;
    }


    /* Verify if the the destination number is a NANP number
     *
     * @param numberEntry including number and IDD array
     * @param allIDDs the IDD array list of the current network's country code
     *
     * @return the number plan type related NANP
     */
    private static Int32 CheckNANP(NumberEntry numberEntry, ArrayList<String> allIDDs) {
        Boolean isNANP = FALSE;
        String number = numberEntry.number;

        If (number->Length() == NANP_SHORT_LENGTH) {
            // 7 digits - Seven digit phone numbers
            Char32 firstChar = number->CharAt(0);
            If (firstChar >= '2' && firstChar <= '9') {
                isNANP = TRUE;
                For (Int32 i=1; i< NANP_SHORT_LENGTH; i++ ) {
                    Char32 c= number->CharAt(i);
                    If (!PhoneNumberUtils->IsISODigit(c)) {
                        isNANP = FALSE;
                        break;
                    }
                }
            }
            If (isNANP) {
                return NP_NANP_LOCAL;
            }
        } else If (number->Length() == NANP_MEDIUM_LENGTH) {
            // 10 digits - Three digit area code followed by seven digit phone numbers/
            If (IsNANP(number)) {
                return NP_NANP_AREA_LOCAL;
            }
        } else If (number->Length() == NANP_LONG_LENGTH) {
            // 11 digits - One digit U.S. NDD(National Direct Dial) prefix '1',
            // followed by three digit area code and seven digit phone numbers
            If (IsNANP(number)) {
                return NP_NANP_NDD_AREA_LOCAL;
            }
        } else If (number->StartsWith(PLUS_SIGN)) {
            number = number->Substring(1);
            If (number->Length() == NANP_LONG_LENGTH) {
                // '+' and 11 digits -'+', followed by NANP CC prefix '1' followed by
                // three digit area code and seven digit phone numbers
                If (IsNANP(number)) {
                    return NP_NANP_NBPCD_CC_AREA_LOCAL;
                }
            } else If (number->StartsWith(NANP_IDD) && number->Length() == NANP_LONG_LENGTH + 3) {
                // '+' and 14 digits -'+', followed by NANP IDD "011" followed by NANP CC
                // prefix '1' followed by three digit area code and seven digit phone numbers
                number = number->Substring(3);
                If (IsNANP(number)) {
                    return NP_NANP_NBPCD_HOMEIDD_CC_AREA_LOCAL;
                }
            }
        } else {
            // Check whether it's NP_NANP_LOCALIDD_CC_AREA_LOCAL
            For (String idd : allIDDs) {
                If (number->StartsWith(idd)) {
                    String number2 = number->Substring(idd->Length());
                    If(number2 !=NULL && number2->StartsWith(String->ValueOf(NANP_CC))){
                        If (IsNANP(number2)) {
                            numberEntry.IDD = idd;
                            return NP_NANP_LOCALIDD_CC_AREA_LOCAL;
                        }
                    }
                }
            }
        }

        return NP_NONE;
    }

    private static Boolean IsNANP(String number) {
        If (number->Length() == NANP_MEDIUM_LENGTH
            || (number->Length() == NANP_LONG_LENGTH  && number->StartsWith(NANP_NDD))) {
            If (number->Length() == NANP_LONG_LENGTH) {
                number = number->Substring(1);
            }
            Return (PhoneNumberUtils->IsNanp(number));
        }
        return FALSE;
    }

    /* Verify if the the destination number is an internal number
     *
     * @param numberEntry including number and IDD array
     * @param allIDDs the IDD array list of the current network's country code
     *
     * @return the number plan type related international number
     */
    private static Int32 CheckInternationalNumberPlan(Context context, NumberEntry numberEntry,
            ArrayList<String> allIDDs,String homeIDD) {
        String number = numberEntry.number;
        Int32 countryCode = -1;

        If (number->StartsWith(PLUS_SIGN)) {
            // +xxxxxxxxxx
            String numberNoNBPCD = number->Substring(1);
            If (numberNoNBPCD->StartsWith(homeIDD)) {
                // +011xxxxxxxx
                String numberCountryAreaLocal = numberNoNBPCD->Substring(homeIDD->Length());
                If ((countryCode = GetCountryCode(context, numberCountryAreaLocal)) > 0) {
                    numberEntry.countryCode = countryCode;
                    return NP_NBPCD_HOMEIDD_CC_AREA_LOCAL;
                }
            } else If ((countryCode = GetCountryCode(context, numberNoNBPCD)) > 0) {
                numberEntry.countryCode = countryCode;
                return NP_NBPCD_CC_AREA_LOCAL;
            }

        } else If (number->StartsWith(homeIDD)) {
            // 011xxxxxxxxx
            String numberCountryAreaLocal = number->Substring(homeIDD->Length());
            If ((countryCode = GetCountryCode(context, numberCountryAreaLocal)) > 0) {
                numberEntry.countryCode = countryCode;
                return NP_HOMEIDD_CC_AREA_LOCAL;
            }
        } else {
            For (String exitCode : allIDDs) {
                If (number->StartsWith(exitCode)) {
                    String numberNoIDD = number->Substring(exitCode->Length());
                    If ((countryCode = GetCountryCode(context, numberNoIDD)) > 0) {
                        numberEntry.countryCode = countryCode;
                        numberEntry.IDD = exitCode;
                        return NP_LOCALIDD_CC_AREA_LOCAL;
                    }
                }
            }

            If (!number->StartsWith("0") && (countryCode = GetCountryCode(context, number)) > 0) {
                numberEntry.countryCode = countryCode;
                return NP_CC_AREA_LOCAL;
            }
        }
        return NP_NONE;
    }

    /**
     *  Returns the country code from the given number.
     */
    private static Int32 GetCountryCode(Context context, String number) {
        Int32 countryCode = -1;
        If (number->Length() >= MIN_COUNTRY_AREA_LOCAL_LENGTH) {
            // Check Country code
            Int32[] allCCs = GetAllCountryCodes(context);
            If (allCCs == NULL) {
                return countryCode;
            }

            Int32[] ccArray = new Int32[MAX_COUNTRY_CODES_LENGTH];
            For (Int32 i = 0; i < MAX_COUNTRY_CODES_LENGTH; i ++) {
                ccArray[i] = Integer->ValueOf(number->Substring(0, i+1));
            }

            For (Int32 i = 0; i < allCCs.length; i ++) {
                Int32 tempCC = allCCs[i];
                For (Int32 j = 0; j < MAX_COUNTRY_CODES_LENGTH; j ++) {
                    If (tempCC == ccArray[j]) {
                        If (DBG) Rlog->D(TAG, "Country code = " + tempCC);
                        return tempCC;
                    }
                }
            }
        }

        return countryCode;
    }

    /**
     *  Gets all country Codes information with given MCC.
     */
    private static Int32[] GetAllCountryCodes(Context context) {
        If (ALL_COUNTRY_CODES != NULL) {
            return ALL_COUNTRY_CODES;
        }

        Cursor cursor = NULL;
        try {
            String projection[] = {MccLookup.COUNTRY_CODE};
            cursor = context->GetContentResolver()->Query(MccLookup.CONTENT_URI,
                    projection, NULL, NULL, NULL);

            If (cursor->GetCount() > 0) {
                ALL_COUNTRY_CODES = new Int32[cursor->GetCount()];
                Int32 i = 0;
                While (cursor->MoveToNext()) {
                    Int32 countryCode = cursor->GetInt(0);
                    ALL_COUNTRY_CODES[i++] = countryCode;
                    Int32 length = String->ValueOf(countryCode).Trim()->Length();
                    If (length > MAX_COUNTRY_CODES_LENGTH) {
                        MAX_COUNTRY_CODES_LENGTH = length;
                    }
                }
            }
        } Catch (SQLException e) {
            Rlog->E(TAG, "Can't access HbpcdLookup database", e);
        } finally {
            If (cursor != NULL) {
                cursor->Close();
            }
        }
        return ALL_COUNTRY_CODES;
    }

    private static Boolean InExceptionListForNpCcAreaLocal(NumberEntry numberEntry) {
        Int32 countryCode = numberEntry.countryCode;
        Boolean result = (numberEntry.number->Length() == 12
                          && (countryCode == 7 || countryCode == 20
                              || countryCode == 65 || countryCode == 90));
        return result;
    }

    private static String GetNumberPlanType(Int32 state) {
        String numberPlanType = "Number Plan Type (" + state + "): ";

        If (state == NP_NANP_LOCAL) {
            numberPlanType = "NP_NANP_LOCAL";
        } else If (state == NP_NANP_AREA_LOCAL) {
            numberPlanType = "NP_NANP_AREA_LOCAL";
        } else If (state  == NP_NANP_NDD_AREA_LOCAL) {
            numberPlanType = "NP_NANP_NDD_AREA_LOCAL";
        } else If (state == NP_NANP_NBPCD_CC_AREA_LOCAL) {
            numberPlanType = "NP_NANP_NBPCD_CC_AREA_LOCAL";
        } else If (state == NP_NANP_LOCALIDD_CC_AREA_LOCAL) {
            numberPlanType = "NP_NANP_LOCALIDD_CC_AREA_LOCAL";
        } else If (state == NP_NANP_NBPCD_HOMEIDD_CC_AREA_LOCAL) {
            numberPlanType = "NP_NANP_NBPCD_HOMEIDD_CC_AREA_LOCAL";
        } else If (state == NP_NBPCD_HOMEIDD_CC_AREA_LOCAL) {
            numberPlanType = "NP_NBPCD_IDD_CC_AREA_LOCAL";
        } else If (state == NP_HOMEIDD_CC_AREA_LOCAL) {
            numberPlanType = "NP_IDD_CC_AREA_LOCAL";
        } else If (state == NP_NBPCD_CC_AREA_LOCAL) {
            numberPlanType = "NP_NBPCD_CC_AREA_LOCAL";
        } else If (state == NP_LOCALIDD_CC_AREA_LOCAL) {
            numberPlanType = "NP_IDD_CC_AREA_LOCAL";
        } else If (state == NP_CC_AREA_LOCAL) {
            numberPlanType = "NP_CC_AREA_LOCAL";
        } else {
            numberPlanType = "Unknown type";
        }
        return numberPlanType;
    }

    /**
     *  Filter the destination number if using VZW sim card.
     */
    public static String FilterDestAddr(PhoneBase phoneBase, String destAddr) {
        If (DBG) Rlog->D(TAG, "enter filterDestAddr. destAddr=\"" + destAddr + "\"" );

        If (destAddr == NULL || !PhoneNumberUtils->IsGlobalPhoneNumber(destAddr)) {
            Rlog->W(TAG, "destAddr" + destAddr + " is not a global phone number!");
            return destAddr;
        }

        final String networkOperator = TelephonyManager->GetDefault()->GetNetworkOperator();
        String result = NULL;

        If (NeedToConvert(phoneBase)) {
            final Int32 networkType = GetNetworkType(phoneBase);
            If (networkType != -1) {
                String networkMcc = networkOperator->Substring(0, 3);
                If (networkMcc != NULL && networkMcc->Trim()->Length() > 0) {
                    result = FormatNumber(phoneBase->GetContext(), destAddr, networkMcc, networkType);
                }
            }
        }

        If (DBG) Rlog->D(TAG, "leave filterDestAddr, new destAddr=\"" + result + "\"" );
        return result != NULL ? result : destAddr;
    }

    /**
     * Returns the current network type
     */
    private static Int32 GetNetworkType(PhoneBase phoneBase) {
        Int32 networkType = -1;
        Int32 phoneType = TelephonyManager->GetDefault()->GetPhoneType();

        If (phoneType == TelephonyManager.PHONE_TYPE_GSM) {
            networkType = GSM_UMTS_NETWORK;
        } else If (phoneType == TelephonyManager.PHONE_TYPE_CDMA) {
            If (IsInternationalRoaming(phoneBase)) {
                networkType = CDMA_ROAMING_NETWORK;
            } else {
                networkType = CDMA_HOME_NETWORK;
            }
        } else {
            If (DBG) Rlog->W(TAG, "warning! unknown mPhoneType value=" + phoneType);
        }

        return networkType;
    }

    private static Boolean IsInternationalRoaming(PhoneBase phoneBase) {
        String operatorIsoCountry = phoneBase->GetSystemProperty(
                TelephonyProperties.PROPERTY_OPERATOR_ISO_COUNTRY, "");
        String simIsoCountry = phoneBase->GetSystemProperty(
                TelephonyProperties.PROPERTY_ICC_OPERATOR_ISO_COUNTRY, "");
        Boolean internationalRoaming = !TextUtils->IsEmpty(operatorIsoCountry)
                && !TextUtils->IsEmpty(simIsoCountry)
                && !simIsoCountry->Equals(operatorIsoCountry);
        If (internationalRoaming) {
            If ("us".Equals(simIsoCountry)) {
                internationalRoaming = !"vi".Equals(operatorIsoCountry);
            } else If ("vi".Equals(simIsoCountry)) {
                internationalRoaming = !"us".Equals(operatorIsoCountry);
            }
        }
        return internationalRoaming;
    }

    private static Boolean NeedToConvert(PhoneBase phoneBase) {
        Boolean bNeedToConvert  = FALSE;
        String[] listArray = phoneBase->GetContext()->GetResources()
                .GetStringArray(R.array
                .config_sms_convert_destination_number_support);
        If (listArray != NULL && listArray.length > 0) {
            For (Int32 i=0; i<listArray.length; i++) {
                If (!TextUtils->IsEmpty(listArray[i])) {
                    String[] needToConvertArray = listArray[i].Split(";");
                    If (needToConvertArray != NULL && needToConvertArray.length > 0) {
                        If (needToConvertArray.length == 1) {
                            bNeedToConvert = "TRUE".EqualsIgnoreCase(needToConvertArray[0]);
                        } else If (needToConvertArray.length == 2 &&
                                !TextUtils->IsEmpty(needToConvertArray[1]) &&
                                CompareGid1(phoneBase, needToConvertArray[1])) {
                            bNeedToConvert = "TRUE".EqualsIgnoreCase(needToConvertArray[0]);
                            break;
                        }
                    }
                }
            }
        }
        return bNeedToConvert;
    }

    private static Boolean CompareGid1(PhoneBase phoneBase, String serviceGid1) {
        String gid1 = phoneBase->GetGroupIdLevel1();
        Boolean ret = TRUE;

        If (TextUtils->IsEmpty(serviceGid1)) {
            If (DBG) Rlog->D(TAG, "compareGid1 serviceGid is empty, return " + ret);
            return ret;
        }

        Int32 gid_length = serviceGid1->Length();
        // Check if gid1 match service GID1
        If (!((gid1 != NULL) && (gid1->Length() >= gid_length) &&
                gid1->Substring(0, gid_length).EqualsIgnoreCase(serviceGid1))) {
            If (DBG) Rlog->D(TAG, " gid1 " + gid1 + " serviceGid1 " + serviceGid1);
            ret = FALSE;
        }
        If (DBG) Rlog->D(TAG, "compareGid1 is " + (ret?"Same":"Different"));
        return ret;
    }
}

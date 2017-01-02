//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <elastos/core/IntegralToString.h>
#include "elastos/droid/internal/telephony/CSmsNumberUtils.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/os/Build.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Core::IntegralToString;
//using Elastos::Utility::IArrays;
using Elastos::Utility::IList;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Droid::Telephony::ITelephonyManager;
//using Elastos::Droid::Internal::Telephony::HbpcdLookup::IMccIdd;
//using Elastos::Droid::Internal::Telephony::HbpcdLookup::IMccLookup;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

 /**
 * This class implements handle the MO SMS target address before sending.
 * This is special for VZW requirement. Follow the specificaitons of assisted dialing
 * of MO SMS while traveling on VZW CDMA, international CDMA or GSM markets.
 * {@hide}
 */
CAR_SINGLETON_IMPL(CSmsNumberUtils)

CAR_INTERFACE_IMPL(CSmsNumberUtils, Singleton, ISmsNumberUtils)

const String CSmsNumberUtils::TAG("SmsNumberUtils");
const Boolean CSmsNumberUtils::DBG = Build::IS_DEBUGGABLE;

const String CSmsNumberUtils::PLUS_SIGN("+");

const Int32 CSmsNumberUtils::NANP_SHORT_LENGTH = 7;
const Int32 CSmsNumberUtils::NANP_MEDIUM_LENGTH = 10;
const Int32 CSmsNumberUtils::NANP_LONG_LENGTH = 11;

const Int32 CSmsNumberUtils::NANP_CC = 1;
const String CSmsNumberUtils::NANP_NDD("1");
const String CSmsNumberUtils::NANP_IDD("011");

const Int32 CSmsNumberUtils::MIN_COUNTRY_AREA_LOCAL_LENGTH = 10;

const Int32 CSmsNumberUtils::GSM_UMTS_NETWORK = 0;
const Int32 CSmsNumberUtils::CDMA_HOME_NETWORK = 1;
const Int32 CSmsNumberUtils::CDMA_ROAMING_NETWORK = 2;

const Int32 CSmsNumberUtils::NP_NONE = 0;
const Int32 CSmsNumberUtils::NP_NANP_BEGIN = 1;

/* <Phone Number>, <NXX>-<XXXX> N[2-9] */
const Int32 CSmsNumberUtils::NP_NANP_LOCAL = NP_NANP_BEGIN;

/* <Area_code>-<Phone Number>, <NXX>-<NXX>-<XXXX> N[2-9] */
const Int32 CSmsNumberUtils::NP_NANP_AREA_LOCAL = NP_NANP_BEGIN + 1;

/* <1>-<Area_code>-<Phone Number>, 1-<NXX>-<NXX>-<XXXX> N[2-9] */
const Int32 CSmsNumberUtils::NP_NANP_NDD_AREA_LOCAL = NP_NANP_BEGIN + 2;

/* <+><U.S.Country_code><Area_code><Phone Number>, +1-<NXX>-<NXX>-<XXXX> N[2-9] */
const Int32 CSmsNumberUtils::NP_NANP_NBPCD_CC_AREA_LOCAL = NP_NANP_BEGIN + 3;

/* <Local_IDD><Country_code><Area_code><Phone Number>, 001-1-<NXX>-<NXX>-<XXXX> N[2-9] */
const Int32 CSmsNumberUtils::NP_NANP_LOCALIDD_CC_AREA_LOCAL = NP_NANP_BEGIN + 4;

/* <+><Home_IDD><Country_code><Area_code><Phone Number>, +011-1-<NXX>-<NXX>-<XXXX> N[2-9] */
const Int32 CSmsNumberUtils::NP_NANP_NBPCD_HOMEIDD_CC_AREA_LOCAL = NP_NANP_BEGIN + 5;

const Int32 CSmsNumberUtils::NP_INTERNATIONAL_BEGIN = 100;
/* <+>-<Home_IDD>-<Country_code>-<Area_code>-<Phone Number>, +011-86-25-86281234 */
const Int32 CSmsNumberUtils::NP_NBPCD_HOMEIDD_CC_AREA_LOCAL = NP_INTERNATIONAL_BEGIN;

/* <Home_IDD>-<Country_code>-<Area_code>-<Phone Number>, 011-86-25-86281234 */
const Int32 CSmsNumberUtils::NP_HOMEIDD_CC_AREA_LOCAL = NP_INTERNATIONAL_BEGIN + 1;

/* <NBPCD>-<Country_code>-<Area_code>-<Phone Number>, +1-86-25-86281234 */
const Int32 CSmsNumberUtils::NP_NBPCD_CC_AREA_LOCAL = NP_INTERNATIONAL_BEGIN + 2;

/* <Local_IDD>-<Country_code>-<Area_code>-<Phone Number>, 00-86-25-86281234 */
const Int32 CSmsNumberUtils::NP_LOCALIDD_CC_AREA_LOCAL = NP_INTERNATIONAL_BEGIN + 3;

/* <Country_code>-<Area_code>-<Phone Number>, 86-25-86281234*/
const Int32 CSmsNumberUtils::NP_CC_AREA_LOCAL = NP_INTERNATIONAL_BEGIN + 4;

//TODO
//HashMap<String, ArrayList<String>> IDDS_MAPS =
//        new HashMap<String, ArrayList<String>>();

String CSmsNumberUtils::FormatNumber(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ const String& activeMcc,
    /* [in] */ Int32 networkType)
{
    assert(0 && "TODO");
//    if (number == NULL ) {
//        throw new IllegalArgumentException("number is NULL");
//    }
//
//    if (activeMcc == NULL || activeMcc->Trim()->Length() == 0) {
//        throw new IllegalArgumentException("activeMcc is NULL or empty!");
//    }
//
//    String networkPortionNumber = PhoneNumberUtils->ExtractNetworkPortion(number);
//    if (networkPortionNumber == NULL || networkPortionNumber->Length() == 0) {
//        throw new IllegalArgumentException("Number is invalid!");
//    }
//
//    NumberEntry numberEntry = new NumberEntry(networkPortionNumber);
//    ArrayList<String> allIDDs = GetAllIDDs(context, activeMcc);
//
//    // First check whether the number is a NANP number.
//    Int32 nanpState = CheckNANP(numberEntry, allIDDs);
//    if (DBG) Rlog->D(TAG, "NANP type: " + GetNumberPlanType(nanpState));
//
//    if ((nanpState == NP_NANP_LOCAL)
//        || (nanpState == NP_NANP_AREA_LOCAL)
//        || (nanpState == NP_NANP_NDD_AREA_LOCAL)) {
//        return networkPortionNumber;
//    } else if (nanpState == NP_NANP_NBPCD_CC_AREA_LOCAL) {
//        if (networkType == CDMA_HOME_NETWORK
//                || networkType == CDMA_ROAMING_NETWORK) {
//            // Remove "+"
//            return networkPortionNumber->Substring(1);
//        } else {
//            return networkPortionNumber;
//        }
//    } else if (nanpState == NP_NANP_LOCALIDD_CC_AREA_LOCAL) {
//        if (networkType == CDMA_HOME_NETWORK) {
//            return networkPortionNumber;
//        } else if (networkType == GSM_UMTS_NETWORK) {
//            // Remove the local IDD and replace with "+"
//            Int32 iddLength  =  numberEntry.IDD != NULL ? numberEntry.IDD->Length() : 0;
//            return PLUS_SIGN + networkPortionNumber->Substring(iddLength);
//        } else if (networkType == CDMA_ROAMING_NETWORK) {
//            // Remove the local IDD
//            Int32 iddLength  =  numberEntry.IDD != NULL ? numberEntry.IDD->Length() : 0;
//            return  networkPortionNumber->Substring(iddLength);
//        }
//    }
//
//    Int32 internationalState = CheckInternationalNumberPlan(context, numberEntry, allIDDs,
//            NANP_IDD);
//    if (DBG) Rlog->D(TAG, "International type: " + GetNumberPlanType(internationalState));
//    String returnNumber = NULL;
//
//    Switch (internationalState) {
//        case NP_NBPCD_HOMEIDD_CC_AREA_LOCAL:
//            if (networkType == GSM_UMTS_NETWORK) {
//                // Remove "+"
//                returnNumber = networkPortionNumber->Substring(1);
//            }
//            break;
//
//        case NP_NBPCD_CC_AREA_LOCAL:
//            // Replace "+" with "011"
//            returnNumber = NANP_IDD + networkPortionNumber->Substring(1);
//            break;
//
//        case NP_LOCALIDD_CC_AREA_LOCAL:
//            if (networkType == GSM_UMTS_NETWORK || networkType == CDMA_ROAMING_NETWORK) {
//                Int32 iddLength  =  numberEntry.IDD != NULL ? numberEntry.IDD->Length() : 0;
//                // Replace <Local IDD> to <Home IDD>("011")
//                returnNumber = NANP_IDD + networkPortionNumber->Substring(iddLength);
//            }
//            break;
//
//        case NP_CC_AREA_LOCAL:
//            Int32 countryCode = numberEntry.countryCode;
//
//            if (!InExceptionListForNpCcAreaLocal(numberEntry)
//                && networkPortionNumber->Length() >= 11 && countryCode != NANP_CC) {
//                // Add "011"
//                returnNumber = NANP_IDD + networkPortionNumber;
//            }
//            break;
//
//        case NP_HOMEIDD_CC_AREA_LOCAL:
//            returnNumber = networkPortionNumber;
//            break;
//
//        default:
//            // Replace "+" with 011 in CDMA network if the number's country
//            // code is not in the HbpcdLookup database.
//            if (networkPortionNumber->StartsWith(PLUS_SIGN)
//                && (networkType == CDMA_HOME_NETWORK || networkType == CDMA_ROAMING_NETWORK)) {
//                if (networkPortionNumber->StartsWith(PLUS_SIGN + NANP_IDD)) {
//                    // Only remove "+"
//                    returnNumber = networkPortionNumber->Substring(1);
//                } else {
//                    // Replace "+" with "011"
//                    returnNumber = NANP_IDD + networkPortionNumber->Substring(1);
//                }
//            }
//    }
//
//    if (returnNumber == NULL) {
//        returnNumber = networkPortionNumber;
//    }
//    return returnNumber;
}

AutoPtr<IArrayList> CSmsNumberUtils::GetAllIDDs(
    /* [in] */ IContext* context,
    /* [in] */ const String& mcc)
{
    assert(0 && "TODO");
//    ArrayList<String> allIDDs = IDDS_MAPS->Get(mcc);
//    if (allIDDs != NULL) {
//        return allIDDs;
//    } else {
//        allIDDs = new ArrayList<String>();
//    }
//
//    String projection[] = {MccIdd.IDD, MccIdd.MCC};
//    String where = NULL;
//
//    // if mcc is NULL         : return all rows
//    // if mcc is empty-string : return those rows whose mcc is emptry-string
//    String[] selectionArgs = NULL;
//    if (mcc != NULL) {
//        where = MccIdd.MCC + "=?";
//        selectionArgs = new String[] {mcc};
//    }
//
//    Cursor cursor = NULL;
//    try {
//        cursor = context->GetContentResolver()->Query(MccIdd.CONTENT_URI, projection,
//                where, selectionArgs, NULL);
//        if (cursor->GetCount() > 0) {
//            While (cursor->MoveToNext()) {
//                String idd = cursor->GetString(0);
//                if (!allIDDs->Contains(idd)) {
//                    allIDDs->Add(idd);
//                }
//            }
//        }
//    } Catch (SQLException e) {
//        Rlog->E(TAG, "Can't access HbpcdLookup database", e);
//    } finally {
//        if (cursor != NULL) {
//            cursor->Close();
//        }
//    }
//
//    IDDS_MAPS->Put(mcc, allIDDs);
//
//    if (DBG) Rlog->D(TAG, "MCC = " + mcc + ", all IDDs = " + allIDDs);
//    return allIDDs;
}

Int32 CSmsNumberUtils::CheckNANP(
    /* [in] */ NumberEntry* numberEntry,
    /* [in] */ IArrayList* allIDDs)
{
    assert(0 && "TODO");
//    Boolean isNANP = FALSE;
//    String number = numberEntry.number;
//
//    if (number->GetLength() == NANP_SHORT_LENGTH) {
//        // 7 digits - Seven digit phone numbers
//        Char32 firstChar = number->GetCharAt(0);
//        if (firstChar >= '2' && firstChar <= '9') {
//            isNANP = TRUE;
//            For (Int32 i=1; i< NANP_SHORT_LENGTH; i++ ) {
//                Char32 c= number->CharAt(i);
//                if (!PhoneNumberUtils->IsISODigit(c)) {
//                    isNANP = FALSE;
//                    break;
//                }
//            }
//        }
//        if (isNANP) {
//            return NP_NANP_LOCAL;
//        }
//    } else if (number->Length() == NANP_MEDIUM_LENGTH) {
//        // 10 digits - Three digit area code followed by seven digit phone numbers/
//        if (IsNANP(number)) {
//            return NP_NANP_AREA_LOCAL;
//        }
//    } else if (number->Length() == NANP_LONG_LENGTH) {
//        // 11 digits - One digit U.S. NDD(National Direct Dial) prefix '1',
//        // followed by three digit area code and seven digit phone numbers
//        if (IsNANP(number)) {
//            return NP_NANP_NDD_AREA_LOCAL;
//        }
//    } else if (number->StartsWith(PLUS_SIGN)) {
//        number = number->Substring(1);
//        if (number->Length() == NANP_LONG_LENGTH) {
//            // '+' and 11 digits -'+', followed by NANP CC prefix '1' followed by
//            // three digit area code and seven digit phone numbers
//            if (IsNANP(number)) {
//                return NP_NANP_NBPCD_CC_AREA_LOCAL;
//            }
//        } else if (number->StartsWith(NANP_IDD) && number->Length() == NANP_LONG_LENGTH + 3) {
//            // '+' and 14 digits -'+', followed by NANP IDD "011" followed by NANP CC
//            // prefix '1' followed by three digit area code and seven digit phone numbers
//            number = number->Substring(3);
//            if (IsNANP(number)) {
//                return NP_NANP_NBPCD_HOMEIDD_CC_AREA_LOCAL;
//            }
//        }
//    } else {
//        // Check whether it's NP_NANP_LOCALIDD_CC_AREA_LOCAL
//        For (String idd : allIDDs) {
//            if (number->StartsWith(idd)) {
//                String number2 = number->Substring(idd->Length());
//                if(number2 !=NULL && number2->StartsWith(String->ValueOf(NANP_CC))){
//                    if (IsNANP(number2)) {
//                        numberEntry.IDD = idd;
//                        return NP_NANP_LOCALIDD_CC_AREA_LOCAL;
//                    }
//                }
//            }
//        }
//    }
//
//    return NP_NONE;
}

Boolean CSmsNumberUtils::IsNANP(
    /* [in] */ const String& number)
{
    Int32 len = number.GetLength();
    if (len == NANP_MEDIUM_LENGTH || (len == NANP_LONG_LENGTH  && number.StartWith(NANP_NDD))) {
        String str;
        if (len == NANP_LONG_LENGTH) {
            str = number.Substring(1);
        }
        else str = number;
        Boolean result;
        PhoneNumberUtils::IsNanp(str, &result);
        return result;
    }
    return FALSE;
}

Int32 CSmsNumberUtils::CheckInternationalNumberPlan(
    /* [in] */ IContext* context,
    /* [in] */ NumberEntry* numberEntry,
    /* [in] */ IArrayList* allIDDs,
    /* [in] */ const String& homeIDD)
{
    assert(0 && "TODO");
//    String number = numberEntry.number;
//    Int32 countryCode = -1;
//
//    if (number->StartsWith(PLUS_SIGN)) {
//        // +xxxxxxxxxx
//        String numberNoNBPCD = number->Substring(1);
//        if (numberNoNBPCD->StartsWith(homeIDD)) {
//            // +011xxxxxxxx
//            String numberCountryAreaLocal = numberNoNBPCD->Substring(homeIDD->Length());
//            if ((countryCode = GetCountryCode(context, numberCountryAreaLocal)) > 0) {
//                numberEntry.countryCode = countryCode;
//                return NP_NBPCD_HOMEIDD_CC_AREA_LOCAL;
//            }
//        } else if ((countryCode = GetCountryCode(context, numberNoNBPCD)) > 0) {
//            numberEntry.countryCode = countryCode;
//            return NP_NBPCD_CC_AREA_LOCAL;
//        }
//
//    } else if (number->StartsWith(homeIDD)) {
//        // 011xxxxxxxxx
//        String numberCountryAreaLocal = number->Substring(homeIDD->Length());
//        if ((countryCode = GetCountryCode(context, numberCountryAreaLocal)) > 0) {
//            numberEntry.countryCode = countryCode;
//            return NP_HOMEIDD_CC_AREA_LOCAL;
//        }
//    } else {
//        For (String exitCode : allIDDs) {
//            if (number->StartsWith(exitCode)) {
//                String numberNoIDD = number->Substring(exitCode->Length());
//                if ((countryCode = GetCountryCode(context, numberNoIDD)) > 0) {
//                    numberEntry.countryCode = countryCode;
//                    numberEntry.IDD = exitCode;
//                    return NP_LOCALIDD_CC_AREA_LOCAL;
//                }
//            }
//        }
//
//        if (!number->StartsWith("0") && (countryCode = GetCountryCode(context, number)) > 0) {
//            numberEntry.countryCode = countryCode;
//            return NP_CC_AREA_LOCAL;
//        }
//    }
//    return NP_NONE;
}

Int32 CSmsNumberUtils::GetCountryCode(
    /* [in] */ IContext* context,
    /* [in] */ const String& number)
{
    assert(0 && "TODO");
//    Int32 countryCode = -1;
//    if (number->Length() >= MIN_COUNTRY_AREA_LOCAL_LENGTH) {
//        // Check Country code
//        Int32[] allCCs = GetAllCountryCodes(context);
//        if (allCCs == NULL) {
//            return countryCode;
//        }
//
//        Int32[] ccArray = new Int32[MAX_COUNTRY_CODES_LENGTH];
//        For (Int32 i = 0; i < MAX_COUNTRY_CODES_LENGTH; i ++) {
//            ccArray[i] = Integer->ValueOf(number->Substring(0, i+1));
//        }
//
//        For (Int32 i = 0; i < allCCs.length; i ++) {
//            Int32 tempCC = allCCs[i];
//            For (Int32 j = 0; j < MAX_COUNTRY_CODES_LENGTH; j ++) {
//                if (tempCC == ccArray[j]) {
//                    if (DBG) Rlog->D(TAG, "Country code = " + tempCC);
//                    return tempCC;
//                }
//            }
//        }
//    }
//
//    return countryCode;
}

/**
 *  Gets all country Codes information with given MCC.
 */
AutoPtr<ArrayOf<Int32> > CSmsNumberUtils::GetAllCountryCodes(
    /* [in] */ IContext* context)
{
    assert(0 && "TODO");
//    if (ALL_COUNTRY_CODES != NULL) {
//        return ALL_COUNTRY_CODES;
//    }
//
//    Cursor cursor = NULL;
//    try {
//        String projection[] = {MccLookup.COUNTRY_CODE};
//        cursor = context->GetContentResolver()->Query(MccLookup.CONTENT_URI,
//                projection, NULL, NULL, NULL);
//
//        if (cursor->GetCount() > 0) {
//            ALL_COUNTRY_CODES = new Int32[cursor->GetCount()];
//            Int32 i = 0;
//            While (cursor->MoveToNext()) {
//                Int32 countryCode = cursor->GetInt(0);
//                ALL_COUNTRY_CODES[i++] = countryCode;
//                Int32 length = String->ValueOf(countryCode).Trim()->Length();
//                if (length > MAX_COUNTRY_CODES_LENGTH) {
//                    MAX_COUNTRY_CODES_LENGTH = length;
//                }
//            }
//        }
//    } Catch (SQLException e) {
//        Rlog->E(TAG, "Can't access HbpcdLookup database", e);
//    } finally {
//        if (cursor != NULL) {
//            cursor->Close();
//        }
//    }
    return ALL_COUNTRY_CODES;
}

Boolean CSmsNumberUtils::InExceptionListForNpCcAreaLocal(
    /* [in] */ NumberEntry* numberEntry)
{
    assert(0 && "TODO");
//    Int32 countryCode = numberEntry.countryCode;
//    Boolean result = (numberEntry.number->Length() == 12
//                      && (countryCode == 7 || countryCode == 20
//                          || countryCode == 65 || countryCode == 90));
//    return result;
}

String CSmsNumberUtils::GetNumberPlanType(
    /* [in] */ Int32 state)
{
    String numberPlanType = String("Number Plan Type (") + IntegralToString::ToString(state) + String("): ");

    if (state == NP_NANP_LOCAL) {
        numberPlanType = String("NP_NANP_LOCAL");
    } else if (state == NP_NANP_AREA_LOCAL) {
        numberPlanType = String("NP_NANP_AREA_LOCAL");
    } else if (state  == NP_NANP_NDD_AREA_LOCAL) {
        numberPlanType = String("NP_NANP_NDD_AREA_LOCAL");
    } else if (state == NP_NANP_NBPCD_CC_AREA_LOCAL) {
        numberPlanType = String("NP_NANP_NBPCD_CC_AREA_LOCAL");
    } else if (state == NP_NANP_LOCALIDD_CC_AREA_LOCAL) {
        numberPlanType = String("NP_NANP_LOCALIDD_CC_AREA_LOCAL");
    } else if (state == NP_NANP_NBPCD_HOMEIDD_CC_AREA_LOCAL) {
        numberPlanType = String("NP_NANP_NBPCD_HOMEIDD_CC_AREA_LOCAL");
    } else if (state == NP_NBPCD_HOMEIDD_CC_AREA_LOCAL) {
        numberPlanType = String("NP_NBPCD_IDD_CC_AREA_LOCAL");
    } else if (state == NP_HOMEIDD_CC_AREA_LOCAL) {
        numberPlanType = String("NP_IDD_CC_AREA_LOCAL");
    } else if (state == NP_NBPCD_CC_AREA_LOCAL) {
        numberPlanType = String("NP_NBPCD_CC_AREA_LOCAL");
    } else if (state == NP_LOCALIDD_CC_AREA_LOCAL) {
        numberPlanType = String("NP_IDD_CC_AREA_LOCAL");
    } else if (state == NP_CC_AREA_LOCAL) {
        numberPlanType = String("NP_CC_AREA_LOCAL");
    } else {
        numberPlanType = String("Unknown type");
    }
    return numberPlanType;
}

/**
 *  Filter the destination number if using VZW sim card.
 */
ECode CSmsNumberUtils::FilterDestAddr(
    /* [in] */ IPhoneBase* phoneBase,
    /* [in] */ const String& destAddr,
    /* [out] */ String* result)
{
    Logger::D(TAG, "[TODO] FilterDestAddr Not Implemented");
    *result = destAddr;
    return NOERROR;
//    if (DBG) Rlog->D(TAG, "enter filterDestAddr. destAddr=\"" + destAddr + "\"" );
//
//    if (destAddr == NULL || !PhoneNumberUtils->IsGlobalPhoneNumber(destAddr)) {
//        Rlog->W(TAG, "destAddr" + destAddr + " is not a global phone number!");
//        return destAddr;
//    }
//
//    final String networkOperator = TelephonyManager->GetDefault()->GetNetworkOperator();
//    String result = NULL;
//
//    if (NeedToConvert(phoneBase)) {
//        final Int32 networkType = GetNetworkType(phoneBase);
//        if (networkType != -1) {
//            String networkMcc = networkOperator->Substring(0, 3);
//            if (networkMcc != NULL && networkMcc->Trim()->Length() > 0) {
//                result = FormatNumber(phoneBase->GetContext(), destAddr, networkMcc, networkType);
//            }
//        }
//    }
//
//    if (DBG) Rlog->D(TAG, "leave filterDestAddr, new destAddr=\"" + result + "\"" );
//    return result != NULL ? result : destAddr;
}

/**
 * Returns the current network type
 */
Int32 CSmsNumberUtils::GetNetworkType(
    /* [in] */ IPhoneBase* phoneBase)
{
    assert(0 && "TODO");
//    Int32 networkType = -1;
//    Int32 phoneType = TelephonyManager->GetDefault()->GetPhoneType();
//
//    if (phoneType == TelephonyManager.PHONE_TYPE_GSM) {
//        networkType = GSM_UMTS_NETWORK;
//    } else if (phoneType == TelephonyManager.PHONE_TYPE_CDMA) {
//        if (IsInternationalRoaming(phoneBase)) {
//            networkType = CDMA_ROAMING_NETWORK;
//        } else {
//            networkType = CDMA_HOME_NETWORK;
//        }
//    } else {
//        if (DBG) Rlog->W(TAG, "warning! unknown mPhoneType value=" + phoneType);
//    }
//
//    return networkType;
}

Boolean CSmsNumberUtils::IsInternationalRoaming(
    /* [in] */ IPhoneBase* phoneBase)
{
    assert(0 && "TODO");
//    String operatorIsoCountry = phoneBase->GetSystemProperty(
//            TelephonyProperties.PROPERTY_OPERATOR_ISO_COUNTRY, "");
//    String simIsoCountry = phoneBase->GetSystemProperty(
//            TelephonyProperties.PROPERTY_ICC_OPERATOR_ISO_COUNTRY, "");
//    Boolean internationalRoaming = !TextUtils->IsEmpty(operatorIsoCountry)
//            && !TextUtils->IsEmpty(simIsoCountry)
//            && !simIsoCountry->Equals(operatorIsoCountry);
//    if (internationalRoaming) {
//        if ("us".Equals(simIsoCountry)) {
//            internationalRoaming = !"vi".Equals(operatorIsoCountry);
//        } else if ("vi".Equals(simIsoCountry)) {
//            internationalRoaming = !"us".Equals(operatorIsoCountry);
//        }
//    }
//    return internationalRoaming;
}

Boolean CSmsNumberUtils::NeedToConvert(
    /* [in] */ IPhoneBase* phoneBase)
{
    assert(0 && "TODO");
//    Boolean bNeedToConvert  = FALSE;
//    String[] listArray = phoneBase->GetContext()->GetResources()
//            .GetStringArray(R.array
//            .config_sms_convert_destination_number_support);
//    if (listArray != NULL && listArray.length > 0) {
//        For (Int32 i=0; i<listArray.length; i++) {
//            if (!TextUtils->IsEmpty(listArray[i])) {
//                String[] needToConvertArray = listArray[i].Split(";");
//                if (needToConvertArray != NULL && needToConvertArray.length > 0) {
//                    if (needToConvertArray.length == 1) {
//                        bNeedToConvert = "TRUE".EqualsIgnoreCase(needToConvertArray[0]);
//                    } else if (needToConvertArray.length == 2 &&
//                            !TextUtils->IsEmpty(needToConvertArray[1]) &&
//                            CompareGid1(phoneBase, needToConvertArray[1])) {
//                        bNeedToConvert = "TRUE".EqualsIgnoreCase(needToConvertArray[0]);
//                        break;
//                    }
//                }
//            }
//        }
//    }
//    return bNeedToConvert;
}

Boolean CSmsNumberUtils::CompareGid1(
    /* [in] */ IPhoneBase* phoneBase,
    /* [in] */ const String& serviceGid1)
{
    assert(0 && "TODO");
//    String gid1 = phoneBase->GetGroupIdLevel1();
//    Boolean ret = TRUE;
//
//    if (TextUtils->IsEmpty(serviceGid1)) {
//        if (DBG) Rlog->D(TAG, "compareGid1 serviceGid is empty, return " + ret);
//        return ret;
//    }
//
//    Int32 gid_length = serviceGid1->Length();
//    // Check if gid1 match service GID1
//    if (!((gid1 != NULL) && (gid1->Length() >= gid_length) &&
//            gid1->Substring(0, gid_length).EqualsIgnoreCase(serviceGid1))) {
//        if (DBG) Rlog->D(TAG, " gid1 " + gid1 + " serviceGid1 " + serviceGid1);
//        ret = FALSE;
//    }
//    if (DBG) Rlog->D(TAG, "compareGid1 is " + (ret?"Same":"Different"));
//    return ret;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

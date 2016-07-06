
#include "elastos/droid/teleservice/phone/CCellBroadcastSms.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "R.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/os/AsyncResult.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

const Int32 CCellBroadcastSms::NO_OF_SERVICE_CATEGORIES = 31;
const Int32 CCellBroadcastSms::NO_OF_INTS_STRUCT_1 = 3;
const Int32 CCellBroadcastSms::MAX_LENGTH_RESULT = NO_OF_SERVICE_CATEGORIES * NO_OF_INTS_STRUCT_1 + 1;

const Int32 CCellBroadcastSms::MESSAGE_ACTIVATE_CB_SMS = 1;
const Int32 CCellBroadcastSms::MESSAGE_GET_CB_SMS_CONFIG = 2;
const Int32 CCellBroadcastSms::MESSAGE_SET_CB_SMS_CONFIG = 3;

CCellBroadcastSms::MyHandler::MyHandler(
    /* [in] */ CCellBroadcastSms* host)
{
    Handler::constructor();
}

ECode CCellBroadcastSms::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MESSAGE_ACTIVATE_CB_SMS:
            //Only a log message here, because the received response is always null
            if (DBG) Logger::D(TAG, "Cell Broadcast SMS enabled/disabled.");
            break;
        case MESSAGE_GET_CB_SMS_CONFIG: {
            AutoPtr<ArrayOf<Int32> > result;
            assert(0 && "TODO");
            // result = (int[])((AsyncResult)msg.obj).result;
            // check if the actual service categoties table size on the NV is '0'
            if ((*result)[0] == 0) {
                (*result)[0] = NO_OF_SERVICE_CATEGORIES;

                ITwoStatePreference::Probe(mHost->mButtonBcSms)->SetChecked(FALSE);
                AutoPtr<IMessageHelper> helper;
                CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
                AutoPtr<IMessage> m;
                helper->Obtain(mHost->mHandler, MESSAGE_ACTIVATE_CB_SMS, (IMessage**)&m);
                mHost->mPhone->ActivateCellBroadcastSms(IRILConstants::CDMA_CELL_BROADCAST_SMS_DISABLED, m);

                AutoPtr<IContext> context;
                mHost->mPhone->GetContext((IContext**)&context);
                AutoPtr<IContentResolver> cr;
                context->GetContentResolver((IContentResolver**)&cr);

                AutoPtr<ISettingsGlobal> sHelper;
                CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sHelper);
                Boolean tmp = FALSE;
                sHelper->PutInt32(cr, ISettingsGlobal::CDMA_CELL_BROADCAST_SMS,
                        IRILConstants::CDMA_CELL_BROADCAST_SMS_DISABLED, &tmp);

                mHost->EnableDisableAllCbConfigButtons(FALSE);
            }

            CellBroadcastSmsConfig::SetCbSmsConfig(result);
            AutoPtr<ArrayOf<Int32> > configArray = CellBroadcastSmsConfig::GetCbSmsBselectedValues();
            mHost->SetAllCbConfigButtons(configArray);

            break;
        }
        case MESSAGE_SET_CB_SMS_CONFIG:
            //Only a log message here, because the received response is always null
            if (DBG) Logger::D(TAG, "Set Cell Broadcast SMS values.");
            break;
        default:
            StringBuilder sb;
            sb += "Error! Unhandled message in CellBroadcastSms.java. Message: ";
            sb += what;
            Logger::E(TAG, sb.ToString());
        break;
    }
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > CCellBroadcastSms::CellBroadcastSmsConfig::mBSelected = ArrayOf<Int32>::Alloc(NO_OF_SERVICE_CATEGORIES + 1);
AutoPtr<ArrayOf<Int32> > CCellBroadcastSms::CellBroadcastSmsConfig::mConfigDataComplete = ArrayOf<Int32>::Alloc(MAX_LENGTH_RESULT);

void CCellBroadcastSms::CellBroadcastSmsConfig::SetCbSmsConfig(
    /* [in] */ ArrayOf<Int32>* configData)
{
    if(configData == NULL) {
        Logger::E(TAG, "Error! No cell broadcast service categories returned.");
        return;
    }

    if((*configData)[0] > MAX_LENGTH_RESULT) {
        Logger::E(TAG, "Error! Wrong number of service categories returned from RIL");
        return;
    }

    //The required config values for broadcast SMS are stored in a C struct:
    //
    //  typedef struct {
    //      int size;
    //      RIL_CDMA_BcServiceInfo *entries;
    //  } RIL_CDMA_BcSMSConfig;
    //
    //  typedef struct {
    //      int uServiceCategory;
    //      int uLanguage;
    //      unsigned char bSelected;
    //  } RIL_CDMA_BcServiceInfo;
    //
    // This means, that we have to ignore the first value and check every
    // 3rd value starting with the 2nd of all. This value indicates, where we
    // will store the appropriate bSelected value, which is 2 values behind it.
    for(Int32 i = 1; i < configData->GetLength(); i += NO_OF_INTS_STRUCT_1) {
        (*mBSelected)[(*configData)[i]] = (*configData)[i +2];
    }

    //Store all values in an extra array
    mConfigDataComplete = configData;
    return;
}

void CCellBroadcastSms::CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
    /* [in] */ Boolean value,
    /* [in] */ Int32 pos)
{
    if(pos < mBSelected->GetLength()) {
        (*mBSelected)[pos] = (value == TRUE ? 1 : 0);
    }
    else {
        Logger::E(TAG,"Error! Invalid value position.");
    }
}

AutoPtr<ArrayOf<Int32> > CCellBroadcastSms::CellBroadcastSmsConfig::GetCbSmsBselectedValues()
{
    return mBSelected;
}

AutoPtr<ArrayOf<Int32> > CCellBroadcastSms::CellBroadcastSmsConfig::GetCbSmsAllValues()
{
    return mConfigDataComplete;
}

void CCellBroadcastSms::CellBroadcastSmsConfig::SetCbSmsNoOfStructs(
    /* [in] */ Int32 value)
{
    //Sets the size parameter, which contains the number of structs
    //that will be transmitted
    (*mConfigDataComplete)[0] = value;
    return;
}

void CCellBroadcastSms::CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
    /* [in] */ Boolean value,
    /* [in] */ Int32 serviceCategory)
{
    //Sets the bSelected value for a specific serviceCategory
    for(Int32 i = 1; i < mConfigDataComplete->GetLength(); i += NO_OF_INTS_STRUCT_1) {
        if((*mConfigDataComplete)[i] == serviceCategory) {
            (*mConfigDataComplete)[i + 2] = value == TRUE ? 1 : 0;
            break;
        }
    }
    return;
}

void CCellBroadcastSms::CellBroadcastSmsConfig::SetConfigDataCompleteLanguage(
    /* [in] */ Int32 language)
{
    //It is only possible to set the same language for all entries
    for(Int32 i = 2; i < mConfigDataComplete->GetLength(); i += NO_OF_INTS_STRUCT_1) {
        (*mConfigDataComplete)[i] = language;
    }
    return;
}

Int32 CCellBroadcastSms::CellBroadcastSmsConfig::GetConfigDataLanguage()
{
    Int32 language = (*mConfigDataComplete)[2];
    //2 is the language value of the first entry
    //It is only possible to set the same language for all entries
    if (language < 1 || language > 7) {
        Logger::E(TAG, "Error! Wrong language returned from RIL...defaulting to 1, english");
        return 1;
    }
    else {
        return language;
    }
}

CAR_INTERFACE_IMPL(CCellBroadcastSms::PreferenceOnPreferenceChangeListener, Object, \
        IPreferenceOnPreferenceChangeListener)
CCellBroadcastSms::PreferenceOnPreferenceChangeListener::PreferenceOnPreferenceChangeListener(
    /* [in] */ CCellBroadcastSms* host)
    : mHost(host)
{}

ECode CCellBroadcastSms::PreferenceOnPreferenceChangeListener::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* objValue,
    /* [out] */ Boolean* result)
{
    return mHost->OnPreferenceChange(preference, objValue, result);
}

const String CCellBroadcastSms::TAG("CellBroadcastSms");
const Boolean CCellBroadcastSms::DBG = FALSE;

const String CCellBroadcastSms::BUTTON_ENABLE_DISABLE_BC_SMS_KEY("button_enable_disable_cell_bc_sms");
const String CCellBroadcastSms::LIST_LANGUAGE_KEY("list_language");
const String CCellBroadcastSms::BUTTON_EMERGENCY_BROADCAST_KEY("button_emergency_broadcast");
const String CCellBroadcastSms::BUTTON_ADMINISTRATIVE_KEY("button_administrative");
const String CCellBroadcastSms::BUTTON_MAINTENANCE_KEY("button_maintenance");
const String CCellBroadcastSms::BUTTON_LOCAL_WEATHER_KEY("button_local_weather");
const String CCellBroadcastSms::BUTTON_ATR_KEY("button_atr");
const String CCellBroadcastSms::BUTTON_LAFS_KEY("button_lafs");
const String CCellBroadcastSms::BUTTON_RESTAURANTS_KEY("button_restaurants");
const String CCellBroadcastSms::BUTTON_LODGINGS_KEY("button_lodgings");
const String CCellBroadcastSms::BUTTON_RETAIL_DIRECTORY_KEY("button_retail_directory");
const String CCellBroadcastSms::BUTTON_ADVERTISEMENTS_KEY("button_advertisements");
const String CCellBroadcastSms::BUTTON_STOCK_QUOTES_KEY("button_stock_quotes");
const String CCellBroadcastSms::BUTTON_EO_KEY("button_eo");
const String CCellBroadcastSms::BUTTON_MHH_KEY("button_mhh");
const String CCellBroadcastSms::BUTTON_TECHNOLOGY_NEWS_KEY("button_technology_news");
const String CCellBroadcastSms::BUTTON_MULTI_CATEGORY_KEY("button_multi_category");

const String CCellBroadcastSms::BUTTON_LOCAL_GENERAL_NEWS_KEY("button_local_general_news");
const String CCellBroadcastSms::BUTTON_REGIONAL_GENERAL_NEWS_KEY("button_regional_general_news");
const String CCellBroadcastSms::BUTTON_NATIONAL_GENERAL_NEWS_KEY("button_national_general_news");
const String CCellBroadcastSms::BUTTON_INTERNATIONAL_GENERAL_NEWS_KEY("button_international_general_news");

const String CCellBroadcastSms::BUTTON_LOCAL_BF_NEWS_KEY("button_local_bf_news");
const String CCellBroadcastSms::BUTTON_REGIONAL_BF_NEWS_KEY("button_regional_bf_news");
const String CCellBroadcastSms::BUTTON_NATIONAL_BF_NEWS_KEY("button_national_bf_news");
const String CCellBroadcastSms::BUTTON_INTERNATIONAL_BF_NEWS_KEY("button_international_bf_news");

const String CCellBroadcastSms::BUTTON_LOCAL_SPORTS_NEWS_KEY("button_local_sports_news");
const String CCellBroadcastSms::BUTTON_REGIONAL_SPORTS_NEWS_KEY("button_regional_sports_news");
const String CCellBroadcastSms::BUTTON_NATIONAL_SPORTS_NEWS_KEY("button_national_sports_news");
const String CCellBroadcastSms::BUTTON_INTERNATIONAL_SPORTS_NEWS_KEY("button_international_sports_news");

const String CCellBroadcastSms::BUTTON_LOCAL_ENTERTAINMENT_NEWS_KEY("button_local_entertainment_news");
const String CCellBroadcastSms::BUTTON_REGIONAL_ENTERTAINMENT_NEWS_KEY("button_regional_entertainment_news");
const String CCellBroadcastSms::BUTTON_NATIONAL_ENTERTAINMENT_NEWS_KEY("button_national_entertainment_news");
const String CCellBroadcastSms::BUTTON_INTERNATIONAL_ENTERTAINMENT_NEWS_KEY("button_international_entertainment_news");

CAR_INTERFACE_IMPL(CCellBroadcastSms, PreferenceActivity, IPreferenceOnPreferenceChangeListener)
CAR_OBJECT_IMPL(CCellBroadcastSms)
ECode CCellBroadcastSms::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonBcSms)) {
        if (DBG) Logger::D(TAG, "onPreferenceTreeClick: preference == mButtonBcSms.");
        AutoPtr<IMessageHelper> helper;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
        Boolean res;
        if(ITwoStatePreference::Probe(mButtonBcSms)->IsChecked(&res), res) {
            AutoPtr<IMessage> m;
            helper->Obtain(mHandler, MESSAGE_ACTIVATE_CB_SMS, (IMessage**)&m);
            mPhone->ActivateCellBroadcastSms(IRILConstants::CDMA_CELL_BROADCAST_SMS_ENABLED, m);

            AutoPtr<IContext> context;
            mPhone->GetContext((IContext**)&context);
            AutoPtr<IContentResolver> cr;
            context->GetContentResolver((IContentResolver**)&cr);

            AutoPtr<ISettingsGlobal> helper;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
            helper->PutInt32(cr, ISettingsGlobal::CDMA_CELL_BROADCAST_SMS,
                    IRILConstants::CDMA_CELL_BROADCAST_SMS_ENABLED, &res);

            EnableDisableAllCbConfigButtons(TRUE);
        }
        else {
            AutoPtr<IMessage> m;
            helper->Obtain(mHandler, MESSAGE_ACTIVATE_CB_SMS, (IMessage**)&m);
            mPhone->ActivateCellBroadcastSms(IRILConstants::CDMA_CELL_BROADCAST_SMS_DISABLED, m);

            AutoPtr<IContext> context;
            mPhone->GetContext((IContext**)&context);
            AutoPtr<IContentResolver> cr;
            context->GetContentResolver((IContentResolver**)&cr);

            AutoPtr<ISettingsGlobal> helper;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
            helper->PutInt32(cr, ISettingsGlobal::CDMA_CELL_BROADCAST_SMS,
                    IRILConstants::CDMA_CELL_BROADCAST_SMS_DISABLED, &res);

            EnableDisableAllCbConfigButtons(FALSE);
        }
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mListLanguage)) {
        //Do nothing here, because this click will be handled in onPreferenceChange
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonEmergencyBroadcast)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonEmergencyBroadcast)->IsChecked(&res), res), 1);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonEmergencyBroadcast)->IsChecked(&res), res), 1);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonAdministrative)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonAdministrative)->IsChecked(&res), res), 2);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonAdministrative)->IsChecked(&res), res), 2);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonMaintenance)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonMaintenance)->IsChecked(&res), res), 3);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonMaintenance)->IsChecked(&res), res), 3);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonLocalWeather)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonLocalWeather)->IsChecked(&res), res), 20);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonLocalWeather)->IsChecked(&res), res), 20);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonAtr)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonAtr)->IsChecked(&res), res), 21);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonAtr)->IsChecked(&res), res), 21);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonLafs)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonLafs)->IsChecked(&res), res), 22);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonLafs)->IsChecked(&res), res), 22);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonRestaurants)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonRestaurants)->IsChecked(&res), res), 23);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonRestaurants)->IsChecked(&res), res), 23);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonLodgings)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonLodgings)->IsChecked(&res), res), 24);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonLodgings)->IsChecked(&res), res), 24);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonRetailDirectory)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonRetailDirectory)->IsChecked(&res), res), 25);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonRetailDirectory)->IsChecked(&res), res), 25);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonAdvertisements)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonAdvertisements)->IsChecked(&res), res), 26);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonAdvertisements)->IsChecked(&res), res), 26);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonStockQuotes)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonStockQuotes)->IsChecked(&res), res), 27);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonStockQuotes)->IsChecked(&res), res), 27);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonEo)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonEo)->IsChecked(&res), res), 28);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonEo)->IsChecked(&res), res), 28);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonMhh)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonMhh)->IsChecked(&res), res), 29);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonMhh)->IsChecked(&res), res), 29);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonTechnologyNews)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonTechnologyNews)->IsChecked(&res), res), 30);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonTechnologyNews)->IsChecked(&res), res), 30);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonMultiCategory)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonMultiCategory)->IsChecked(&res), res), 31);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonMultiCategory)->IsChecked(&res), res), 31);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonLocal1)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonLocal1)->IsChecked(&res), res), 4);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonLocal1)->IsChecked(&res), res), 4);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonRegional1)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonRegional1)->IsChecked(&res), res), 5);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonRegional1)->IsChecked(&res), res), 5);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonNational1)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonNational1)->IsChecked(&res), res), 6);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonNational1)->IsChecked(&res), res), 6);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonInternational1)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonInternational1)->IsChecked(&res), res), 7);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonInternational1)->IsChecked(&res), res), 7);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonLocal2)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonLocal2)->IsChecked(&res), res), 8);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonLocal2)->IsChecked(&res), res), 8);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonRegional2)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonRegional2)->IsChecked(&res), res), 9);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonRegional2)->IsChecked(&res), res), 9);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonNational2)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonNational2)->IsChecked(&res), res), 10);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonNational2)->IsChecked(&res), res), 10);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonInternational2)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonInternational2)->IsChecked(&res), res), 11);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonInternational2)->IsChecked(&res), res), 11);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonLocal3)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonLocal3)->IsChecked(&res), res), 12);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonLocal3)->IsChecked(&res), res), 12);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonRegional3)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonRegional3)->IsChecked(&res), res), 13);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonRegional3)->IsChecked(&res), res), 13);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonNational3)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonNational3)->IsChecked(&res), res), 14);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonNational3)->IsChecked(&res), res), 14);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonInternational3)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonInternational3)->IsChecked(&res), res), 15);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonInternational3)->IsChecked(&res), res), 15);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonLocal4)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonLocal4)->IsChecked(&res), res), 16);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonLocal4)->IsChecked(&res), res), 16);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonRegional4)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonRegional4)->IsChecked(&res), res), 17);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonRegional4)->IsChecked(&res), res), 17);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonNational4)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonNational4)->IsChecked(&res), res), 18);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonNational4)->IsChecked(&res), res), 18);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonInternational4)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected(
                (ITwoStatePreference::Probe(mButtonInternational4)->IsChecked(&res), res), 19);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue(
                (ITwoStatePreference::Probe(mButtonInternational4)->IsChecked(&res), res), 19);
    }
    else {
        IPreference::Probe(preferenceScreen)->SetEnabled(FALSE);
        *result = FALSE;
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode CCellBroadcastSms::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* objValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(preference) == TO_IINTERFACE(mListLanguage)) {
        // set the new language to the array which will be transmitted later
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(objValue);
        String str;
        cchar->ToString(&str);

        Int32 index;
        mListLanguage->FindIndexOfValue(str, &index);
        CellBroadcastSmsConfig::SetConfigDataCompleteLanguage(index + 1);
    }

    // always let the preference setting proceed.
    *result = TRUE;
    return NOERROR;
}

ECode CCellBroadcastSms::OnCreate(
    /* [in] */ IBundle* icicle)
{
    PreferenceActivity::OnCreate(icicle);

    AddPreferencesFromResource(Elastos::Droid::TeleService::R::xml::cell_broadcast_sms);

    mPhone = PhoneGlobals::GetPhone();
    mHandler = new MyHandler(this);

    AutoPtr<IPreferenceScreen> prefSet;
    GetPreferenceScreen((IPreferenceScreen**)&prefSet);

    AutoPtr<ICharSequence> cs;
    CString::New(BUTTON_ENABLE_DISABLE_BC_SMS_KEY, (ICharSequence**)&cs);
    AutoPtr<IPreference> preference;
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonBcSms = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(LIST_LANGUAGE_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mListLanguage = IListPreference::Probe(preference);

    // set the listener for the language list preference
    AutoPtr<PreferenceOnPreferenceChangeListener> l = new PreferenceOnPreferenceChangeListener(this);
    IPreference::Probe(mListLanguage)->SetOnPreferenceChangeListener(l);
    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_EMERGENCY_BROADCAST_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonEmergencyBroadcast = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_ADMINISTRATIVE_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonAdministrative = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_MAINTENANCE_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonMaintenance = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_LOCAL_WEATHER_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonLocalWeather = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_ATR_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonAtr = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_LAFS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonLafs = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_RESTAURANTS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonRestaurants = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_LODGINGS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonLodgings = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_RETAIL_DIRECTORY_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonRetailDirectory = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_ADVERTISEMENTS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonAdvertisements = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_STOCK_QUOTES_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonStockQuotes = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_EO_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonEo = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_MHH_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonMhh = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_TECHNOLOGY_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonTechnologyNews = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_MULTI_CATEGORY_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonMultiCategory = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_LOCAL_GENERAL_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonLocal1 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_REGIONAL_GENERAL_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonRegional1 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_NATIONAL_GENERAL_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonNational1 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_INTERNATIONAL_GENERAL_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonInternational1 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_LOCAL_BF_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonLocal2 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_REGIONAL_BF_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonRegional2 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_NATIONAL_BF_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonNational2 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_INTERNATIONAL_BF_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonInternational2 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_LOCAL_SPORTS_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonLocal3 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_REGIONAL_SPORTS_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonRegional3 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_NATIONAL_SPORTS_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonNational3 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_INTERNATIONAL_SPORTS_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonInternational3 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_LOCAL_ENTERTAINMENT_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonLocal4 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_REGIONAL_ENTERTAINMENT_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonRegional4 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_NATIONAL_ENTERTAINMENT_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonNational4 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    cs = NULL;
    CString::New(BUTTON_INTERNATIONAL_ENTERTAINMENT_NEWS_KEY, (ICharSequence**)&cs);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&preference);
    mButtonInternational4 = ICheckBoxPreference::Probe(preference);

    return NOERROR;
}

ECode CCellBroadcastSms::OnResume()
{
    PreferenceActivity::OnResume();

    AutoPtr<IPreferenceScreen> prefSet;
    GetPreferenceScreen((IPreferenceScreen**)&prefSet);
    IPreference::Probe(prefSet)->SetEnabled(TRUE);

    AutoPtr<IContext> context;
    mPhone->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<ISettingsGlobal> helper;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
    Int32 settingCbSms;
    helper->GetInt32(cr, ISettingsGlobal::CDMA_CELL_BROADCAST_SMS,
            IRILConstants::CDMA_CELL_BROADCAST_SMS_DISABLED, &settingCbSms);

    ITwoStatePreference::Probe(mButtonBcSms)->SetChecked(settingCbSms == IRILConstants::CDMA_CELL_BROADCAST_SMS_ENABLED);

    Boolean res;
    if(ITwoStatePreference::Probe(mButtonBcSms)->IsChecked(&res), res) {
        EnableDisableAllCbConfigButtons(TRUE);
    }
    else {
        EnableDisableAllCbConfigButtons(FALSE);
    }

    AutoPtr<IMessageHelper> mHelper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&mHelper);
    AutoPtr<IMessage> m;
    mHelper->Obtain(mHandler, MESSAGE_GET_CB_SMS_CONFIG, (IMessage**)&m);
    return mPhone->GetCellBroadcastSmsConfig(m);
}

ECode CCellBroadcastSms::OnPause()
{
    PreferenceActivity::OnPause();

    CellBroadcastSmsConfig::SetCbSmsNoOfStructs(NO_OF_SERVICE_CATEGORIES);

    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> m;
    helper->Obtain(mHandler, MESSAGE_SET_CB_SMS_CONFIG, (IMessage**)&m);
    AutoPtr<ArrayOf<Int32> > array = CellBroadcastSmsConfig::GetCbSmsAllValues();
    return mPhone->SetCellBroadcastSmsConfig(array, m);
}

void CCellBroadcastSms::EnableDisableAllCbConfigButtons(
    /* [in] */ Boolean enable)
{
    IPreference::Probe(mButtonEmergencyBroadcast)->SetEnabled(enable);
    IPreference::Probe(mListLanguage)->SetEnabled(enable);
    IPreference::Probe(mButtonAdministrative)->SetEnabled(enable);
    IPreference::Probe(mButtonMaintenance)->SetEnabled(enable);
    IPreference::Probe(mButtonLocalWeather)->SetEnabled(enable);
    IPreference::Probe(mButtonAtr)->SetEnabled(enable);
    IPreference::Probe(mButtonLafs)->SetEnabled(enable);
    IPreference::Probe(mButtonRestaurants)->SetEnabled(enable);
    IPreference::Probe(mButtonLodgings)->SetEnabled(enable);
    IPreference::Probe(mButtonRetailDirectory)->SetEnabled(enable);
    IPreference::Probe(mButtonAdvertisements)->SetEnabled(enable);
    IPreference::Probe(mButtonStockQuotes)->SetEnabled(enable);
    IPreference::Probe(mButtonEo)->SetEnabled(enable);
    IPreference::Probe(mButtonMhh)->SetEnabled(enable);
    IPreference::Probe(mButtonTechnologyNews)->SetEnabled(enable);
    IPreference::Probe(mButtonMultiCategory)->SetEnabled(enable);

    IPreference::Probe(mButtonLocal1)->SetEnabled(enable);
    IPreference::Probe(mButtonRegional1)->SetEnabled(enable);
    IPreference::Probe(mButtonNational1)->SetEnabled(enable);
    IPreference::Probe(mButtonInternational1)->SetEnabled(enable);

    IPreference::Probe(mButtonLocal2)->SetEnabled(enable);
    IPreference::Probe(mButtonRegional2)->SetEnabled(enable);
    IPreference::Probe(mButtonNational2)->SetEnabled(enable);
    IPreference::Probe(mButtonInternational2)->SetEnabled(enable);

    IPreference::Probe(mButtonLocal3)->SetEnabled(enable);
    IPreference::Probe(mButtonRegional3)->SetEnabled(enable);
    IPreference::Probe(mButtonNational3)->SetEnabled(enable);
    IPreference::Probe(mButtonInternational3)->SetEnabled(enable);

    IPreference::Probe(mButtonLocal4)->SetEnabled(enable);
    IPreference::Probe(mButtonRegional4)->SetEnabled(enable);
    IPreference::Probe(mButtonNational4)->SetEnabled(enable);
    IPreference::Probe(mButtonInternational4)->SetEnabled(enable);
}

void CCellBroadcastSms::SetAllCbConfigButtons(
    /* [in] */ ArrayOf<Int32>* configArray)
{
    //These buttons are in a well defined sequence. If you want to change it,
    //be sure to map the buttons to their corresponding slot in the configArray !
    ITwoStatePreference::Probe(mButtonEmergencyBroadcast)->SetChecked((*configArray)[1] != 0);
    //subtract 1, because the values are handled in an array which starts with 0 and not with 1
    mListLanguage->SetValueIndex(CellBroadcastSmsConfig::GetConfigDataLanguage() - 1);
    ITwoStatePreference::Probe(mButtonAdministrative)->SetChecked((*configArray)[2] != 0);
    ITwoStatePreference::Probe(mButtonMaintenance)->SetChecked((*configArray)[3] != 0);
    ITwoStatePreference::Probe(mButtonLocalWeather)->SetChecked((*configArray)[20] != 0);
    ITwoStatePreference::Probe(mButtonAtr)->SetChecked((*configArray)[21] != 0);
    ITwoStatePreference::Probe(mButtonLafs)->SetChecked((*configArray)[22] != 0);
    ITwoStatePreference::Probe(mButtonRestaurants)->SetChecked((*configArray)[23] != 0);
    ITwoStatePreference::Probe(mButtonLodgings)->SetChecked((*configArray)[24] != 0);
    ITwoStatePreference::Probe(mButtonRetailDirectory)->SetChecked((*configArray)[25] != 0);
    ITwoStatePreference::Probe(mButtonAdvertisements)->SetChecked((*configArray)[26] != 0);
    ITwoStatePreference::Probe(mButtonStockQuotes)->SetChecked((*configArray)[27] != 0);
    ITwoStatePreference::Probe(mButtonEo)->SetChecked((*configArray)[28] != 0);
    ITwoStatePreference::Probe(mButtonMhh)->SetChecked((*configArray)[29] != 0);
    ITwoStatePreference::Probe(mButtonTechnologyNews)->SetChecked((*configArray)[30] != 0);
    ITwoStatePreference::Probe(mButtonMultiCategory)->SetChecked((*configArray)[31] != 0);

    ITwoStatePreference::Probe(mButtonLocal1)->SetChecked((*configArray)[4] != 0);
    ITwoStatePreference::Probe(mButtonRegional1)->SetChecked((*configArray)[5] != 0);
    ITwoStatePreference::Probe(mButtonNational1)->SetChecked((*configArray)[6] != 0);
    ITwoStatePreference::Probe(mButtonInternational1)->SetChecked((*configArray)[7] != 0);

    ITwoStatePreference::Probe(mButtonLocal2)->SetChecked((*configArray)[8] != 0);
    ITwoStatePreference::Probe(mButtonRegional2)->SetChecked((*configArray)[9] != 0);
    ITwoStatePreference::Probe(mButtonNational2)->SetChecked((*configArray)[10] != 0);
    ITwoStatePreference::Probe(mButtonInternational2)->SetChecked((*configArray)[11] != 0);

    ITwoStatePreference::Probe(mButtonLocal3)->SetChecked((*configArray)[12] != 0);
    ITwoStatePreference::Probe(mButtonRegional3)->SetChecked((*configArray)[13] != 0);
    ITwoStatePreference::Probe(mButtonNational3)->SetChecked((*configArray)[14] != 0);
    ITwoStatePreference::Probe(mButtonInternational3)->SetChecked((*configArray)[15] != 0);

    ITwoStatePreference::Probe(mButtonLocal4)->SetChecked((*configArray)[16] != 0);
    ITwoStatePreference::Probe(mButtonRegional4)->SetChecked((*configArray)[17] != 0);
    ITwoStatePreference::Probe(mButtonNational4)->SetChecked((*configArray)[18] != 0);
    ITwoStatePreference::Probe(mButtonInternational4)->SetChecked((*configArray)[19] != 0);
}


} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

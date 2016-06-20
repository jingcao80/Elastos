
#include "elastos/droid/phone/CCellBroadcastSms.h"


namespace Elastos {
namespace Droid {
namespace Phone {


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
            if (DBG) Logger::D(LOG_TAG, "Cell Broadcast SMS enabled/disabled.");
            break;
        case MESSAGE_GET_CB_SMS_CONFIG:
            int result[] = (int[])((AsyncResult)msg.obj).result;

            // check if the actual service categoties table size on the NV is '0'
            if ((*result)[0] == 0) {
                (*result)[0] = NO_OF_SERVICE_CATEGORIES;

                mButtonBcSms->SetChecked(FALSE);
                AutoPtr<IMessage> m;
                Message::Obtain(mHandler, MESSAGE_ACTIVATE_CB_SMS, (IMessage**)&m);
                mPhone->ActivateCellBroadcastSms(IRILConstants::CDMA_CELL_BROADCAST_SMS_DISABLED, m);

                AutoPtr<IContext> context;
                mPhone->GetContext((IContext**)&context);
                AutoPtr<IContentResolver> cr;
                context->GetContentResolver((IContentResolver**)&cr);

                AutoPtr<ISettingsGlobal> helper;
                CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
                Int32 cdmaSubscriptionMode;
                helper->PutInt32(cr, ISettingsGlobal::CDMA_CELL_BROADCAST_SMS,
                        IRILConstants::CDMA_CELL_BROADCAST_SMS_DISABLED);

                EnableDisableAllCbConfigButtons(FALSE);
            }

            CellBroadcastSmsConfig::SetCbSmsConfig(result);
            AutoPtr<ArrayOf<Int32> > configArray;
            CellBroadcastSmsConfig::GetCbSmsBselectedValues((ArrayOf<Int32>**)&configArray);
            SetAllCbConfigButtons(configArray);

            break;
        case MESSAGE_SET_CB_SMS_CONFIG:
            //Only a log message here, because the received response is always null
            if (DBG) Logger::D(LOG_TAG, "Set Cell Broadcast SMS values.");
            break;
        default:
            StringBuilder sb;
            sb += "Error! Unhandled message in CellBroadcastSms.java. Message: ";
            sb += what;
            Logger::E(LOG_TAG, sb.ToString());
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
        Logger::E(LOG_TAG, "Error! No cell broadcast service categories returned.");
        return;
    }

    if((*configData)[0] > MAX_LENGTH_RESULT) {
        Logger::E(LOG_TAG, "Error! Wrong number of service categories returned from RIL");
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
        (*mBSelected)[configData[i]] = (*configData)[i +2];
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
        Logger::E(LOG_TAG,"Error! Invalid value position.");
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
        Logger::E(LOG_TAG, "Error! Wrong language returned from RIL...defaulting to 1, english");
        return 1;
    }
    else {
        return language;
    }
}

const String CCellBroadcastSms::LOG_TAG("CellBroadcastSms");
const Boolean CCellBroadcastSms::DBG = FALSE;

const String CCellBroadcastSms::BUTTON_ENABLE_DISABLE_BC_SMS_KEY = "button_enable_disable_cell_bc_sms";
const String CCellBroadcastSms::LIST_LANGUAGE_KEY = "list_language";
const String CCellBroadcastSms::BUTTON_EMERGENCY_BROADCAST_KEY = "button_emergency_broadcast";
const String CCellBroadcastSms::BUTTON_ADMINISTRATIVE_KEY = "button_administrative";
const String CCellBroadcastSms::BUTTON_MAINTENANCE_KEY = "button_maintenance";
const String CCellBroadcastSms::BUTTON_LOCAL_WEATHER_KEY = "button_local_weather";
const String CCellBroadcastSms::BUTTON_ATR_KEY = "button_atr";
const String CCellBroadcastSms::BUTTON_LAFS_KEY = "button_lafs";
const String CCellBroadcastSms::BUTTON_RESTAURANTS_KEY = "button_restaurants";
const String CCellBroadcastSms::BUTTON_LODGINGS_KEY = "button_lodgings";
const String CCellBroadcastSms::BUTTON_RETAIL_DIRECTORY_KEY = "button_retail_directory";
const String CCellBroadcastSms::BUTTON_ADVERTISEMENTS_KEY = "button_advertisements";
const String CCellBroadcastSms::BUTTON_STOCK_QUOTES_KEY = "button_stock_quotes";
const String CCellBroadcastSms::BUTTON_EO_KEY = "button_eo";
const String CCellBroadcastSms::BUTTON_MHH_KEY = "button_mhh";
const String CCellBroadcastSms::BUTTON_TECHNOLOGY_NEWS_KEY = "button_technology_news";
const String CCellBroadcastSms::BUTTON_MULTI_CATEGORY_KEY = "button_multi_category";

const String CCellBroadcastSms::BUTTON_LOCAL_GENERAL_NEWS_KEY = "button_local_general_news";
const String CCellBroadcastSms::BUTTON_REGIONAL_GENERAL_NEWS_KEY = "button_regional_general_news";
const String CCellBroadcastSms::BUTTON_NATIONAL_GENERAL_NEWS_KEY = "button_national_general_news";
const String CCellBroadcastSms::BUTTON_INTERNATIONAL_GENERAL_NEWS_KEY = "button_international_general_news";

const String CCellBroadcastSms::BUTTON_LOCAL_BF_NEWS_KEY = "button_local_bf_news";
const String CCellBroadcastSms::BUTTON_REGIONAL_BF_NEWS_KEY = "button_regional_bf_news";
const String CCellBroadcastSms::BUTTON_NATIONAL_BF_NEWS_KEY = "button_national_bf_news";
const String CCellBroadcastSms::BUTTON_INTERNATIONAL_BF_NEWS_KEY = "button_international_bf_news";

const String CCellBroadcastSms::BUTTON_LOCAL_SPORTS_NEWS_KEY = "button_local_sports_news";
const String CCellBroadcastSms::BUTTON_REGIONAL_SPORTS_NEWS_KEY = "button_regional_sports_news";
const String CCellBroadcastSms::BUTTON_NATIONAL_SPORTS_NEWS_KEY = "button_national_sports_news";
const String CCellBroadcastSms::BUTTON_INTERNATIONAL_SPORTS_NEWS_KEY = "button_international_sports_news";

const String CCellBroadcastSms::BUTTON_LOCAL_ENTERTAINMENT_NEWS_KEY = "button_local_entertainment_news";
const String CCellBroadcastSms::BUTTON_REGIONAL_ENTERTAINMENT_NEWS_KEY = "button_regional_entertainment_news";
const String CCellBroadcastSms::BUTTON_NATIONAL_ENTERTAINMENT_NEWS_KEY = "button_national_entertainment_news";
const String CCellBroadcastSms::BUTTON_INTERNATIONAL_ENTERTAINMENT_NEWS_KEY = "button_international_entertainment_news";

const Int32 CCellBroadcastSms::NO_OF_SERVICE_CATEGORIES = 31;
const Int32 CCellBroadcastSms::NO_OF_INTS_STRUCT_1 = 3;
const Int32 CCellBroadcastSms::MAX_LENGTH_RESULT = NO_OF_SERVICE_CATEGORIES * NO_OF_INTS_STRUCT_1 + 1;

const Int32 CCellBroadcastSms::MESSAGE_ACTIVATE_CB_SMS = 1;
const Int32 CCellBroadcastSms::MESSAGE_GET_CB_SMS_CONFIG = 2;
const Int32 CCellBroadcastSms::MESSAGE_SET_CB_SMS_CONFIG = 3;

CAR_INTERFACE_IMPL_2(CCellBroadcastSms, PreferenceActivity, ICellBroadcastSms, IPreferenceOnPreferenceChangeListener)

CAR_OBJECT_IMPL(CCellBroadcastSms)

ECode CCellBroadcastSms::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonBcSms)) {
        if (DBG) Logger::D(LOG_TAG, "onPreferenceTreeClick: preference == mButtonBcSms.");
        Boolean res;
        if(mButtonBcSms->IsChecked(&res), res) {
            AutoPtr<IMessage> m;
            Message::Obtain(mHandler, MESSAGE_ACTIVATE_CB_SMS, (IMessage**)&m);
            mPhone->ActivateCellBroadcastSms(IRILConstants::CDMA_CELL_BROADCAST_SMS_ENABLED, m);

            AutoPtr<IContext> context;
            mPhone->GetContext((IContext**)&context);
            AutoPtr<IContentResolver> cr;
            context->GetContentResolver((IContentResolver**)&cr);

            AutoPtr<ISettingsGlobal> helper;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
            helper->PutInt32(cr, ISettingsGlobal::CDMA_CELL_BROADCAST_SMS,
                    IRILConstants::CDMA_CELL_BROADCAST_SMS_ENABLED);

            EnableDisableAllCbConfigButtons(TRUE);
        }
        else {
            AutoPtr<IMessage> m;
            Message::Obtain(mHandler, MESSAGE_ACTIVATE_CB_SMS, (IMessage**)&m);
            mPhone->ActivateCellBroadcastSms(IRILConstants::CDMA_CELL_BROADCAST_SMS_DISABLED, m);

            AutoPtr<IContext> context;
            mPhone->GetContext((IContext**)&context);
            AutoPtr<IContentResolver> cr;
            context->GetContentResolver((IContentResolver**)&cr);

            AutoPtr<ISettingsGlobal> helper;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
            helper->PutInt32(cr, ISettingsGlobal::CDMA_CELL_BROADCAST_SMS,
                    IRILConstants::CDMA_CELL_BROADCAST_SMS_DISABLED);

            EnableDisableAllCbConfigButtons(FALSE);
        }
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mListLanguage)) {
        //Do nothing here, because this click will be handled in onPreferenceChange
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonEmergencyBroadcast)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonEmergencyBroadcast->IsChecked(&res), res), 1);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonEmergencyBroadcast->IsChecked(&res), res), 1);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonAdministrative)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonAdministrative->IsChecked(&res), res), 2);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonAdministrative->IsChecked(&res), res), 2);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonMaintenance)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonMaintenance->IsChecked(&res), res), 3);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonMaintenance->IsChecked(&res), res), 3);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonLocalWeather)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonLocalWeather->IsChecked(&res), res), 20);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonLocalWeather->IsChecked(&res), res), 20);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonAtr)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonAtr->IsChecked(&res), res), 21);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonAtr->IsChecked(&res), res), 21);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonLafs)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonLafs->IsChecked(&res), res), 22);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonLafs->IsChecked(&res), res), 22);
    }
    else if TO_IINTERFACE((preference) == TO_IINTERFACE(mButtonRestaurants)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonRestaurants->IsChecked(&res), res), 23);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonRestaurants->IsChecked(&res), res), 23);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonLodgings)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonLodgings->IsChecked(&res), res), 24);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonLodgings->IsChecked(&res), res), 24);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonRetailDirectory)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonRetailDirectory->IsChecked(&res), res), 25);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonRetailDirectory->IsChecked(&res), res), 25);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonAdvertisements)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonAdvertisements->IsChecked(&res), res), 26);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonAdvertisements->IsChecked(&res), res), 26);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonStockQuotes)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonStockQuotes->IsChecked(&res), res), 27);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonStockQuotes->IsChecked(&res), res), 27);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonEo)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonEo->IsChecked(&res), res), 28);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonEo->IsChecked(&res), res), 28);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonMhh)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonMhh->IsChecked(&res), res), 29);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonMhh->IsChecked(&res), res), 29);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonTechnologyNews)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonTechnologyNews->IsChecked(&res), res), 30);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonTechnologyNews->IsChecked(&res), res), 30);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonMultiCategory)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonMultiCategory->IsChecked(&res), res), 31);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonMultiCategory->IsChecked(&res), res), 31);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonLocal1)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonLocal1->IsChecked(&res), res), 4);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonLocal1->IsChecked(&res), res), 4);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonRegional1)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonRegional1->IsChecked(&res), res), 5);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonRegional1->IsChecked(&res), res), 5);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonNational1)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonNational1->IsChecked(&res), res), 6);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonNational1->IsChecked(&res), res), 6);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonInternational1)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonInternational1->IsChecked(&res), res), 7);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonInternational1->IsChecked(&res), res), 7);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonLocal2)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonLocal2->IsChecked(&res), res), 8);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonLocal2->IsChecked(&res), res), 8);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonRegional2)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonRegional2->IsChecked(&res), res), 9);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonRegional2->IsChecked(&res), res), 9);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonNational2)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonNational2->IsChecked(&res), res), 10);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonNational2->IsChecked(&res), res), 10);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonInternational2)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonInternational2->IsChecked(&res), res), 11);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonInternational2->IsChecked(&res), res), 11);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonLocal3)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonLocal3->IsChecked(&res), res), 12);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonLocal3->IsChecked(&res), res), 12);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonRegional3)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonRegional3->IsChecked(&res), res), 13);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonRegional3->IsChecked(&res), res), 13);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonNational3)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonNational3->IsChecked(&res), res), 14);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonNational3->IsChecked(&res), res), 14);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonInternational3)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonInternational3->IsChecked(&res), res), 15);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonInternational3->IsChecked(&res), res), 15);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonLocal4)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonLocal4->IsChecked(&res), res), 16);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonLocal4->IsChecked(&res), res), 16);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonRegional4)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonRegional4->IsChecked(&res), res), 17);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonRegional4->IsChecked(&res), res), 17);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonNational4)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonNational4->IsChecked(&res), res), 18);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonNational4->IsChecked(&res), res), 18);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonInternational4)) {
        Boolean res;
        CellBroadcastSmsConfig::SetConfigDataCompleteBSelected((mButtonInternational4->IsChecked(&res), res), 19);
        CellBroadcastSmsConfig::SetCbSmsBSelectedValue((mButtonInternational4->IsChecked(&res), res), 19);
    }
    else {
        preferenceScreen->SetEnabled(FALSE);
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

    AddPreferencesFromResource(R.xml.cell_broadcast_sms);

    mPhone = PhoneGlobals::GetPhone();
    mHandler = new MyHandler();

    AutoPtr<IPreferenceScreen> prefSet;
    GetPreferenceScreen((IPreferenceScreen**)&prefSet);

    AutoPtr<IPreference> preference;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_ENABLE_DISABLE_BC_SMS_KEY, (IPreference**)&preference);
    mButtonBcSms = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(LIST_LANGUAGE_KEY, (IPreference**)&preference);
    mListLanguage = IListPreference::Probe(preference);

    // set the listener for the language list preference
    mListLanguage->SetOnPreferenceChangeListener(this);
    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_EMERGENCY_BROADCAST_KEY, (IPreference**)&preference);
    mButtonEmergencyBroadcast = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_ADMINISTRATIVE_KEY, (IPreference**)&preference);
    mButtonAdministrative = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_MAINTENANCE_KEY, (IPreference**)&preference);
    mButtonMaintenance = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_LOCAL_WEATHER_KEY, (IPreference**)&preference);
    mButtonLocalWeather = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_ATR_KEY, (IPreference**)&preference);
    mButtonAtr = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_LAFS_KEY, (IPreference**)&preference);
    mButtonLafs = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_RESTAURANTS_KEY, (IPreference**)&preference);
    mButtonRestaurants = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_LODGINGS_KEY, (IPreference**)&preference);
    mButtonLodgings = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_RETAIL_DIRECTORY_KEY, (IPreference**)&preference);
    mButtonRetailDirectory = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_ADVERTISEMENTS_KEY, (IPreference**)&preference);
    mButtonAdvertisements = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_STOCK_QUOTES_KEY, (IPreference**)&preference);
    mButtonStockQuotes = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_EO_KEY, (IPreference**)&preference);
    mButtonEo = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_MHH_KEY, (IPreference**)&preference);
    mButtonMhh = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_TECHNOLOGY_NEWS_KEY, (IPreference**)&preference);
    mButtonTechnologyNews = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_MULTI_CATEGORY_KEY, (IPreference**)&preference);
    mButtonMultiCategory = ICheckBoxPreference::Probe(preference);


    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_LOCAL_GENERAL_NEWS_KEY, (IPreference**)&preference);
    mButtonLocal1 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_REGIONAL_GENERAL_NEWS_KEY, (IPreference**)&preference);
    mButtonRegional1 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_NATIONAL_GENERAL_NEWS_KEY, (IPreference**)&preference);
    mButtonNational1 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_INTERNATIONAL_GENERAL_NEWS_KEY, (IPreference**)&preference);
    mButtonInternational1 = ICheckBoxPreference::Probe(preference);


    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_LOCAL_BF_NEWS_KEY, (IPreference**)&preference);
    mButtonLocal2 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_REGIONAL_BF_NEWS_KEY, (IPreference**)&preference);
    mButtonRegional2 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_NATIONAL_BF_NEWS_KEY, (IPreference**)&preference);
    mButtonNational2 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_INTERNATIONAL_BF_NEWS_KEY, (IPreference**)&preference);
    mButtonInternational2 = ICheckBoxPreference::Probe(preference);


    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_LOCAL_SPORTS_NEWS_KEY, (IPreference**)&preference);
    mButtonLocal3 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_REGIONAL_SPORTS_NEWS_KEY, (IPreference**)&preference);
    mButtonRegional3 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_NATIONAL_SPORTS_NEWS_KEY, (IPreference**)&preference);
    mButtonNational3 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_INTERNATIONAL_SPORTS_NEWS_KEY, (IPreference**)&preference);
    mButtonInternational3 = ICheckBoxPreference::Probe(preference);


    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_LOCAL_ENTERTAINMENT_NEWS_KEY, (IPreference**)&preference);
    mButtonLocal4 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_REGIONAL_ENTERTAINMENT_NEWS_KEY, (IPreference**)&preference);
    mButtonRegional4 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_NATIONAL_ENTERTAINMENT_NEWS_KEY, (IPreference**)&preference);
    mButtonNational4 = ICheckBoxPreference::Probe(preference);

    preference = NULL;
    IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_INTERNATIONAL_ENTERTAINMENT_NEWS_KEY, (IPreference**)&preference);
    mButtonInternational4 = ICheckBoxPreference::Probe(preference);
}

ECode CCellBroadcastSms::OnResume()
{
    PreferenceActivity::OnResume();

    AutoPtr<IPreferenceScreen> prefSet;
    GetPreferenceScreen((IPreferenceScreen**)&prefSet);
    prefSet->SetEnabled(TRUE);

    AutoPtr<IContext> context;
    mPhone->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<ISettingsGlobal> helper;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
    Int32 settingCbSms;
    helper->GetInt32(cr, ISettingsGlobal::CDMA_CELL_BROADCAST_SMS,
            IRILConstants::CDMA_CELL_BROADCAST_SMS_DISABLED, &settingCbSms);

    mButtonBcSms->SetChecked(settingCbSms == IRILConstants::CDMA_CELL_BROADCAST_SMS_ENABLED);

    Boolean res;
    if(mButtonBcSms->IsChecked(&res), res) {
        EnableDisableAllCbConfigButtons(TRUE);
    }
    else {
        EnableDisableAllCbConfigButtons(FALSE);
    }

    AutoPtr<IMessage> m;
    Message::Obtain(mHandler, MESSAGE_GET_CB_SMS_CONFIG, (IMessage**)&m);
    return mPhone->GetCellBroadcastSmsConfig(m);
}

ECode CCellBroadcastSms::OnPause()
{
    PreferenceActivity::OnPause();

    CellBroadcastSmsConfig::SetCbSmsNoOfStructs(NO_OF_SERVICE_CATEGORIES);

    AutoPtr<IMessage> m;
    Message::Obtain(mHandler, MESSAGE_SET_CB_SMS_CONFIG, (IMessage**)&m);
    AutoPtr<AutoPtr<ArrayOf<Int32> > array = CellBroadcastSmsConfig::GetCbSmsAllValues();
    return mPhone->SetCellBroadcastSmsConfig(array, m);
}

void CCellBroadcastSms::EnableDisableAllCbConfigButtons(
    /* [in] */ Boolean enable)
{
    mButtonEmergencyBroadcast->SetEnabled(enable);
    mListLanguage->SetEnabled(enable);
    mButtonAdministrative->SetEnabled(enable);
    mButtonMaintenance->SetEnabled(enable);
    mButtonLocalWeather->SetEnabled(enable);
    mButtonAtr->SetEnabled(enable);
    mButtonLafs->SetEnabled(enable);
    mButtonRestaurants->SetEnabled(enable);
    mButtonLodgings->SetEnabled(enable);
    mButtonRetailDirectory->SetEnabled(enable);
    mButtonAdvertisements->SetEnabled(enable);
    mButtonStockQuotes->SetEnabled(enable);
    mButtonEo->SetEnabled(enable);
    mButtonMhh->SetEnabled(enable);
    mButtonTechnologyNews->SetEnabled(enable);
    mButtonMultiCategory->SetEnabled(enable);

    mButtonLocal1->SetEnabled(enable);
    mButtonRegional1->SetEnabled(enable);
    mButtonNational1->SetEnabled(enable);
    mButtonInternational1->SetEnabled(enable);

    mButtonLocal2->SetEnabled(enable);
    mButtonRegional2->SetEnabled(enable);
    mButtonNational2->SetEnabled(enable);
    mButtonInternational2->SetEnabled(enable);

    mButtonLocal3->SetEnabled(enable);
    mButtonRegional3->SetEnabled(enable);
    mButtonNational3->SetEnabled(enable);
    mButtonInternational3->SetEnabled(enable);

    mButtonLocal4->SetEnabled(enable);
    mButtonRegional4->SetEnabled(enable);
    mButtonNational4->SetEnabled(enable);
    mButtonInternational4->SetEnabled(enable);
}

void CCellBroadcastSms::SetAllCbConfigButtons(
    /* [in] */ ArrayOf<Int32>* configArray)
{
    //These buttons are in a well defined sequence. If you want to change it,
    //be sure to map the buttons to their corresponding slot in the configArray !
    mButtonEmergencyBroadcast->SetChecked((*configArray)[1] != 0);
    //subtract 1, because the values are handled in an array which starts with 0 and not with 1
    mListLanguage->SetValueIndex(CellBroadcastSmsConfig::GetConfigDataLanguage() - 1);
    mButtonAdministrative->SetChecked((*configArray)[2] != 0);
    mButtonMaintenance->SetChecked((*configArray)[3] != 0);
    mButtonLocalWeather->SetChecked((*configArray)[20] != 0);
    mButtonAtr->SetChecked((*configArray)[21] != 0);
    mButtonLafs->SetChecked((*configArray)[22] != 0);
    mButtonRestaurants->SetChecked((*configArray)[23] != 0);
    mButtonLodgings->SetChecked((*configArray)[24] != 0);
    mButtonRetailDirectory->SetChecked((*configArray)[25] != 0);
    mButtonAdvertisements->SetChecked((*configArray)[26] != 0);
    mButtonStockQuotes->SetChecked((*configArray)[27] != 0);
    mButtonEo->SetChecked((*configArray)[28] != 0);
    mButtonMhh->SetChecked((*configArray)[29] != 0);
    mButtonTechnologyNews->SetChecked((*configArray)[30] != 0);
    mButtonMultiCategory->SetChecked((*configArray)[31] != 0);

    mButtonLocal1->SetChecked((*configArray)[4] != 0);
    mButtonRegional1->SetChecked((*configArray)[5] != 0);
    mButtonNational1->SetChecked((*configArray)[6] != 0);
    mButtonInternational1->SetChecked((*configArray)[7] != 0);

    mButtonLocal2->SetChecked((*configArray)[8] != 0);
    mButtonRegional2->SetChecked((*configArray)[9] != 0);
    mButtonNational2->SetChecked((*configArray)[10] != 0);
    mButtonInternational2->SetChecked((*configArray)[11] != 0);

    mButtonLocal3->SetChecked((*configArray)[12] != 0);
    mButtonRegional3->SetChecked((*configArray)[13] != 0);
    mButtonNational3->SetChecked((*configArray)[14] != 0);
    mButtonInternational3->SetChecked((*configArray)[15] != 0);

    mButtonLocal4->SetChecked((*configArray)[16] != 0);
    mButtonRegional4->SetChecked((*configArray)[17] != 0);
    mButtonNational4->SetChecked((*configArray)[18] != 0);
    mButtonInternational4->SetChecked((*configArray)[19] != 0);
}


} // namespace Phone
} // namespace Droid
} // namespace Elastos
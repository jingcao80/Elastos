
#include "elastos/droid/server/telecom/QuickResponseUtils.h"
#include "R.h"
#include "elastos/droid/server/telecom/Log.h"
#include <Elastos.Droid.Content.h>

using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

const String QuickResponseUtils::LOG__TAG("QuickResponseUtils");
const String QuickResponseUtils::SHARED_PREFERENCES_NAME("respond_via_sms_prefs");
const Int32 QuickResponseUtils::NUM_CANNED_RESPONSES = 4;
const String QuickResponseUtils::KEY_CANNED_RESPONSE_PREF_1("canned_response_pref_1");
const String QuickResponseUtils::KEY_CANNED_RESPONSE_PREF_2("canned_response_pref_2");
const String QuickResponseUtils::KEY_CANNED_RESPONSE_PREF_3("canned_response_pref_3");
const String QuickResponseUtils::KEY_CANNED_RESPONSE_PREF_4("canned_response_pref_4");
const String QuickResponseUtils::PACKAGE_NAME_TELEPHONY("com.android.phone");

ECode QuickResponseUtils::MaybeMigrateLegacyQuickResponses(
    /* [in] */ IContext* context)
{
    // The algorithm will go as such:
    // If Telecom QuickResponses exist, we will skip migration because this implies
    // that a user has already specified their desired QuickResponses and have abandoned any
    // older QuickResponses.
    // Then, if Telephony QuickResponses exist, we will move those to Telecom.
    // If neither exist, we'll populate Telecom with the default QuickResponses.
    // This guarantees the caller that QuickResponses exist in SharedPreferences after this
    // function is called.
    Log::D(LOG__TAG, "maybeMigrateLegacyQuickResponses() - Starting");
    AutoPtr<ISharedPreferences> prefs;
    context->GetSharedPreferences(
            SHARED_PREFERENCES_NAME, IContext::MODE_PRIVATE, (ISharedPreferences**)&prefs);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Boolean responsesExist;
    prefs->Contains(KEY_CANNED_RESPONSE_PREF_1, &responsesExist);
    if (responsesExist) {
        // If one QuickResponse exists, they all exist.
        Log::D(LOG__TAG, "maybeMigrateLegacyQuickResponses() - Telecom QuickResponses exist");
        return NOERROR;
    }
    // Grab the all the default QuickResponses from our resources.
    String cannedResponse1;
    res->GetString(R::string::respond_via_sms_canned_response_1, &cannedResponse1);
    String cannedResponse2;
    res->GetString(R::string::respond_via_sms_canned_response_2, &cannedResponse2);
    String cannedResponse3;
    res->GetString(R::string::respond_via_sms_canned_response_3, &cannedResponse3);
    String cannedResponse4;
    res->GetString(R::string::respond_via_sms_canned_response_4, &cannedResponse4);
    Log::D(LOG__TAG, "maybeMigrateLegacyQuickResponses() - No local QuickResponses");
    // We don't have local QuickResponses, let's see if they live in
    // the Telephony package and we'll fall back on using our default values.
    AutoPtr<IContext> telephonyContext;
    // try {
    ECode ec = context->CreatePackageContext(PACKAGE_NAME_TELEPHONY, 0, (IContext**)&telephonyContext);
    // } catch (PackageManager.NameNotFoundException e) {
    if ((ECode)E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION == ec) {
        Log::E(LOG__TAG, "maybeMigrateLegacyQuickResponses() - Can't find Telephony package. %d", ec);
        ec = NOERROR;
    }
    // }
    // Read the old canned responses from the Telephony SharedPreference if possible.
    if (telephonyContext != NULL) {
        // Note that if any one QuickResponse does not exist, we'll use the default
        // value to populate it.
        Log::D(LOG__TAG, "maybeMigrateLegacyQuickResponses() - Using Telephony QuickResponses.");
        AutoPtr<ISharedPreferences> oldPrefs;
        telephonyContext->GetSharedPreferences(
                SHARED_PREFERENCES_NAME, IContext::MODE_PRIVATE, (ISharedPreferences**)&oldPrefs);
        oldPrefs->GetString(KEY_CANNED_RESPONSE_PREF_1, cannedResponse1, &cannedResponse1);
        oldPrefs->GetString(KEY_CANNED_RESPONSE_PREF_2, cannedResponse2, &cannedResponse2);
        oldPrefs->GetString(KEY_CANNED_RESPONSE_PREF_3, cannedResponse3, &cannedResponse3);
        oldPrefs->GetString(KEY_CANNED_RESPONSE_PREF_4, cannedResponse4, &cannedResponse4);
    }
    // Either way, write them back into Telecom SharedPreferences.
    AutoPtr<ISharedPreferencesEditor> editor;
    prefs->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutString(KEY_CANNED_RESPONSE_PREF_1, cannedResponse1);
    editor->PutString(KEY_CANNED_RESPONSE_PREF_2, cannedResponse2);
    editor->PutString(KEY_CANNED_RESPONSE_PREF_3, cannedResponse3);
    editor->PutString(KEY_CANNED_RESPONSE_PREF_4, cannedResponse4);
    Boolean b;
    editor->Commit(&b);
    Log::D(LOG__TAG, "maybeMigrateLegacyQuickResponses() - Done.");
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos


#include "elastos/droid/phone/CProcessOutgoingCallTest.h"
#include "Elastos.Droid.App.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>


using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Phone {

const String CProcessOutgoingCallTest::TAG("ProcessOutgoingCallTest");
const String CProcessOutgoingCallTest::AREACODE("617");

const Boolean CProcessOutgoingCallTest::LOGV = FALSE;

const Boolean CProcessOutgoingCallTest::REDIRECT_411_TO_GOOG411 = TRUE;
const Boolean CProcessOutgoingCallTest::SEVEN_DIGIT_DIALING = TRUE;
const Boolean CProcessOutgoingCallTest::POUND_POUND_SEARCH = TRUE;
const Boolean CProcessOutgoingCallTest::BLOCK_555 = TRUE;

CAR_OBJECT_IMPL(CProcessOutgoingCallTest)

ECode CProcessOutgoingCallTest::constructor()
{
    return BroadcastReceiver::constructor();
}

ECode CProcessOutgoingCallTest::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_NEW_OUTGOING_CALL)) {
        String number;
        intent->GetStringExtra(IIntent::EXTRA_PHONE_NUMBER, &number);

        if (LOGV) {
            StringBuilder sb;
            sb += "Received intent ";
            sb += TO_CSTR(intent);
            sb += " (number = ";
            sb += number;
            sb += ".";
            Logger::V(TAG, "Received intent %s (number = %s.", TO_CSTR(intent), number.string());
        }
        /* Example of how to redirect calls from one number to another. */
        if (REDIRECT_411_TO_GOOG411 && number.Equals(String("411"))) {
            SetResultData(String("18004664411"));
        }

        /* Example of how to modify the phone number in flight. */
        if (SEVEN_DIGIT_DIALING && number.GetLength() == 7) {
            SetResultData(AREACODE + number);
        }

        /* Example of how to route a call to another Application. */
        if (POUND_POUND_SEARCH && number.StartWith("##")) {
            AutoPtr<IIntent> newIntent;
            CIntent::New(IIntent::ACTION_SEARCH, (IIntent**)&newIntent);
            newIntent->PutExtra(ISearchManager::QUERY, number.Substring(2));
            newIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
            context->StartActivity(newIntent);
            SetResultData(String(NULL));
        }

        /* Example of how to deny calls to a particular number.
         * Note that no UI is displayed to the user -- the call simply
         * does not happen.  It is the application's responaibility to
         * explain this to the user. */
        Int32 length = number.GetLength();
        if (BLOCK_555 && length >= 7) {
            String exchange;
            number.Substring(length - 7, length - 4);
            Logger::V(TAG, "exchange = %s", exchange.string());
            if (exchange.Equals(String("555"))) {
                SetResultData(String(NULL));
            }
        }
    }
    return NOERROR;
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos
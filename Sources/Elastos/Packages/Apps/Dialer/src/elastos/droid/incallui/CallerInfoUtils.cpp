
#include "elastos/droid/incallui/CallerInfoUtils.h"
#include "elastos/droid/incallui/CallerInfoAsyncQuery.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Arrays;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

const String CallerInfoUtils::TAG("CallerInfoUtils");
const Int32 CallerInfoUtils::CNAP_SPECIAL_CASE_NO;
const Int32 CallerInfoUtils::QUERY_TOKEN;

AutoPtr<CallerInfo> CallerInfoUtils::GetCallerInfoForCall(
    /* [in] */ IContext* context,
    /* [in] */ Call* call,
    /* [in] */ IOnQueryCompleteListener* listener)
{
    AutoPtr<CallerInfo> info = BuildCallerInfo(context, call);

    // TODO: Have phoneapp send a Uri when it knows the contact that triggered this call.

    if (info->mNumberPresentation == ITelecomManager::PRESENTATION_ALLOWED) {
        // Start the query with the number provided from the call.
        Logger::D(TAG, "==> Actually starting CallerInfoAsyncQuery.startQuery()...");
        CallerInfoAsyncQuery::StartQuery(QUERY_TOKEN, context, info, listener, (ICall*)call);
    }
    return info;
}

AutoPtr<CallerInfo> CallerInfoUtils::BuildCallerInfo(
    /* [in] */ IContext* context,
    /* [in] */ Call* call)
{
    AutoPtr<CallerInfo> info = new CallerInfo();

    // Store CNAP information retrieved from the Connection (we want to do this
    // here regardless of whether the number is empty or not).
    info->mCnapName = call->GetCnapName();
    info->mName = info->mCnapName;
    info->mNumberPresentation = call->GetNumberPresentation();
    info->mNamePresentation = call->GetCnapNamePresentation();

    String number = call->GetNumber();
    if (!TextUtils::IsEmpty(number)) {
        AutoPtr<ArrayOf<String> > numbers;
        StringUtils::Split(number, String("&"), (ArrayOf<String>**)&numbers);
        number = (*numbers)[0];
        if (numbers->GetLength() > 1) {
            info->mForwardingNumber = (*numbers)[1];
        }

        number = ModifyForSpecialCnapCases(context, info, number, info->mNumberPresentation);
        info->mPhoneNumber = number;
    }

    // Because the InCallUI is immediately launched before the call is connected, occasionally
    // a voicemail call will be passed to InCallUI as a "voicemail:" URI without a number.
    // This call should still be handled as a voicemail call.
    String scheme;
    if (call->GetHandle() != NULL &&
            (call->GetHandle()->GetScheme(&scheme), IPhoneAccount::SCHEME_VOICEMAIL.Equals(scheme))) {
        info->MarkAsVoiceMail(context);
    }

    return info;
}

String CallerInfoUtils::ModifyForSpecialCnapCases(
    /* [in] */ IContext* context,
    /* [in] */ CallerInfo* ci,
    /* [in] */ const String& _number,
    /* [in] */ Int32 presentation)
{
    // Obviously we return number if ci == null, but still return number if
    // number == null, because in these cases the correct string will still be
    // displayed/logged after this function returns based on the presentation value.
    String number = _number;
    if (ci == NULL || number.IsNull()) return number;

    Logger::D(TAG, "modifyForSpecialCnapCases: initially, number=%s, presentation=%d ci %s"
            , number.string(), presentation, TO_CSTR(ci));

    // "ABSENT NUMBER" is a possible value we could get from the network as the
    // phone number, so if this happens, change it to "Unknown" in the CallerInfo
    // and fix the presentation to be the same.
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ArrayOf<String> > absentNumberValues;
    res->GetStringArray(Elastos::Droid::Dialer::R::array::absent_num, (ArrayOf<String>**)&absentNumberValues);
    AutoPtr<IList> list;
    Arrays::AsList(absentNumberValues, (IList**)&list);
    AutoPtr<ICharSequence> numberCs;
    CString::New(number, (ICharSequence**)&numberCs);
    Boolean contains;
    if ((list->Contains(numberCs, &contains), contains)
            && presentation == ITelecomManager::PRESENTATION_ALLOWED) {
        context->GetString(Elastos::Droid::Dialer::R::string::unknown, &number);
        ci->mNumberPresentation = ITelecomManager::PRESENTATION_UNKNOWN;
    }

    // Check for other special "corner cases" for CNAP and fix them similarly. Corner
    // cases only apply if we received an allowed presentation from the network, so check
    // if we think we have an allowed presentation, or if the CallerInfo presentation doesn't
    // match the presentation passed in for verification (meaning we changed it previously
    // because it's a corner case and we're being called from a different entry point).
    if (ci->mNumberPresentation == ITelecomManager::PRESENTATION_ALLOWED
            || (ci->mNumberPresentation != presentation
                    && presentation == ITelecomManager::PRESENTATION_ALLOWED)) {
        // For all special strings, change number & numberPrentation.
        if (IsCnapSpecialCaseRestricted(number)) {
            context->GetString(Elastos::Droid::Dialer::R::string::private_num, &number);
            ci->mNumberPresentation = ITelecomManager::PRESENTATION_RESTRICTED;
        }
        else if (IsCnapSpecialCaseUnknown(number)) {
            context->GetString(Elastos::Droid::Dialer::R::string::unknown, &number);
            ci->mNumberPresentation = ITelecomManager::PRESENTATION_UNKNOWN;
        }
        Logger::D(TAG, "SpecialCnap: number=%s; presentation now=%d", number.string(), ci->mNumberPresentation);
    }
    Logger::D(TAG, "modifyForSpecialCnapCases: returning number string=%s", number.string());
    return number;
}

Boolean CallerInfoUtils::IsCnapSpecialCaseRestricted(
    /* [in] */ const String& n)
{
    return n.Equals("PRIVATE") || n.Equals("P") || n.Equals("RES");
}

Boolean CallerInfoUtils::IsCnapSpecialCaseUnknown(
    /* [in] */ const String& n)
{
    return n.Equals("UNAVAILABLE") || n.Equals("UNKNOWN") || n.Equals("UNA") || n.Equals("U");
}

void CallerInfoUtils::SendViewNotification(
    /* [in] */ IContext* context,
    /* [in] */ IUri* contactUri)
{
    assert(0);
    // AutoPtr<ContactLoader> loader = new ContactLoader(context, contactUri,
    //         true /* postViewNotification */);
    // loader.registerListener(0, new OnLoadCompleteListener<Contact>() {
    //     @Override
    //     public void onLoadComplete(
    //             Loader<Contact> loader, Contact contact) {
    //         try {
    //             loader.reset();
    //         } catch (RuntimeException e) {
    //             Log.e(TAG, "Error resetting loader", e);
    //         }
    //     }
    // });
    // loader.startLoading();
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

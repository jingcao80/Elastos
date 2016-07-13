
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Text.h"
#include "elastos/droid/contacts/common/GeoUtil.h"
#include "elastos/droid/contacts/common/util/PhoneNumberFormatter.h"

using Elastos::Droid::Telephony::IPhoneNumberFormattingTextWatcher;
using Elastos::Droid::Telephony::CPhoneNumberFormattingTextWatcher;
using Elastos::Droid::Text::ITextWatcher;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

ECode PhoneNumberFormatter::TextWatcherLoadAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // TODO:
    // AutoPtr<IPhoneNumberFormattingTextWatcher> watcher;
    // CPhoneNumberFormattingTextWatcher::New(mCountryCode, (IPhoneNumberFormattingTextWatcher**)&watcher);
    // *result = watcher;
    // REFCOUNT_ADD(*result);
    *result = NULL;
    return NOERROR;
}

ECode PhoneNumberFormatter::TextWatcherLoadAsyncTask::OnPostExecute(
    /* [in] */ IInterface* watcher)
{
    if (watcher == NULL || IsCancelled()) {
        return NOERROR; // May happen if we cancel the task.
    }
    // Setting a text changed listener is safe even after the view is detached.
    mTextView->AddTextChangedListener(ITextWatcher::Probe(watcher));

    // Note changes the user made before onPostExecute() will not be formatted, but
    // once they type the next letter we format the entire text, so it's not a big deal.
    // (And loading PhoneNumberFormattingTextWatcher is usually fast enough.)
    // We could use watcher.afterTextChanged(mTextView.getEditableText()) to force format
    // the existing content here, but that could cause unwanted results.
    // (e.g. the contact editor thinks the user changed the content, and would save
    // when closed even when the user didn't make other changes.)
    return NOERROR;
}


void PhoneNumberFormatter::SetPhoneNumberFormattingTextWatcher(
    /* [in] */ IContext* context,
    /* [in] */ ITextView* textView)
{
    AutoPtr<TextWatcherLoadAsyncTask> task =
            new TextWatcherLoadAsyncTask(GeoUtil::GetCurrentCountryIso(context), textView);
    task->ExecuteOnExecutor(AsyncTask::THREAD_POOL_EXECUTOR, NULL);
}

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

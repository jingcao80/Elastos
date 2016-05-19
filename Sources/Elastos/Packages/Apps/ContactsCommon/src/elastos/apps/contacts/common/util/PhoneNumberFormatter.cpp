
#include "PhoneNumberFormatter.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

//=================================================================
// PhoneNumberFormatter::TextWatcherLoadAsyncTask
//=================================================================
PhoneNumberFormatter::TextWatcherLoadAsyncTask::TextWatcherLoadAsyncTask(
    /* [in] */ const String& countryCode,
    /* [in] */ ITextView* textView)
    : mTextView(textView)
{
    mCountryCode = countryCode;
}

ECode PhoneNumberFormatter::TextWatcherLoadAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALUE_NOT_NULL(result);

    AutoPtr<IPhoneNumberFormattingTextWatcher> watcher;
    CPhoneNumberFormattingTextWatcher::New(
            mCountryCode, (IPhoneNumberFormattingTextWatcher**)&watcher);
    *result = watcher;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode PhoneNumberFormatter::TextWatcherLoadAsyncTask::OnPostExecute(
    /* [in] */ IInterface* watcher)
{
    if (watcher == NULL || IsCancelled()) {
        return; // May happen if we cancel the task.
    }
    // Setting a text changed listener is safe even after the view is detached.
    mTextView->AddTextChangedListener(watcher);

    // Note changes the user made before onPostExecute() will not be formatted, but
    // once they type the next letter we format the entire text, so it's not a big deal.
    // (And loading PhoneNumberFormattingTextWatcher is usually fast enough.)
    // We could use watcher.afterTextChanged(mTextView.getEditableText()) to force format
    // the existing content here, but that could cause unwanted results.
    // (e.g. the contact editor thinks the user changed the content, and would save
    // when closed even when the user didn't make other changes.)
}

//=================================================================
// PhoneNumberFormatter
//=================================================================
void PhoneNumberFormatter::SetPhoneNumberFormattingTextWatcher(
    /* [in] */ IContext* context,
    /* [in] */ ITextView* textView)
{
     AutoPtr<TextWatcherLoadAsyncTask> task = TextWatcherLoadAsyncTask(
                GeoUtil::GetCurrentCountryIso(context), textView);
     task->ExecuteOnExecutor(IAsyncTask::THREAD_POOL_EXECUTOR, NULL);
}

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos

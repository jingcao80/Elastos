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

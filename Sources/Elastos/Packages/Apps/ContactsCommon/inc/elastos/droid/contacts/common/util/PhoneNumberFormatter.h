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

#ifndef __ELASTOS_APPS_CONTACTS_COMMON_UTIL_PHONENUMBERFORMATTER_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_UTIL_PHONENUMBERFORMATTER_H__

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

class PhoneNumberFormatter
    : public Object
{
private:
    /**
     * Load {@link TextWatcherLoadAsyncTask} in a worker thread and set it to a {@link TextView}.
     */
    class TextWatcherLoadAsyncTask
        : public AsyncTask
    {
    public:
        TextWatcherLoadAsyncTask(
            /* [in] */ const String& countryCode,
            /* [in] */ ITextView* textView);

    protected:
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        CARAPI OnPostExecute(
            /* [in] */ IInterface* watcher);

    private:
        String mCountryCode;
        TextView mTextView;
    };

public:
    /**
     * Delay-set {@link PhoneNumberFormattingTextWatcher} to a {@link TextView}.
     */
    static CARAPI_(void) SetPhoneNumberFormattingTextWatcher(
        /* [in] */ IContext* context,
        /* [in] */ ITextView* textView);

private:
    PhoneNumberFormatter();

};

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_UTIL_PHONENUMBERFORMATTER_H__

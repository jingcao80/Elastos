
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_PHONENUMBERFORMATTER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_PHONENUMBERFORMATTER_H__

#include "Elastos.Droid.Widget.h"
#include <elastos/droid/os/AsyncTask.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

class PhoneNumberFormatter
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
            /* [in] */ ITextView* textView)
            : mCountryCode(countryCode)
            , mTextView(textView)
        {}

    protected:
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    private:
        String mCountryCode;
        AutoPtr<ITextView> mTextView;
    };

public:
    /**
     * Delay-set {@link PhoneNumberFormattingTextWatcher} to a {@link TextView}.
     */
    static CARAPI_(void) SetPhoneNumberFormattingTextWatcher(
        /* [in] */ IContext* context,
        /* [in] */ ITextView* textView);

private:
    PhoneNumberFormatter() {}
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_PHONENUMBERFORMATTER_H__

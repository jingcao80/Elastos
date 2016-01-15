
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/UserDictionary.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;

namespace Elastos {
namespace Droid {
namespace Provider {
//============================================================================
//              Word
//============================================================================
static AutoPtr<IUri> initWordCONTENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::Parse(String("content://") + IUserDictionary::AUTHORITY + "/words", (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> UserDictionary::Words::CONTENT_URI = initWordCONTENT_URI();

ECode UserDictionary::Words::AddWord(
    /* [in] */ IContext* context,
    /* [in] */ const String& word,
    /* [in] */ Int32 frequency,
    /* [in] */ Int32 localeType)
{
    if (localeType != IUserDictionaryWords::LOCALE_TYPE_ALL &&
        localeType != IUserDictionaryWords::LOCALE_TYPE_CURRENT) {
        return NOERROR;
    }

    AutoPtr<ILocale> locale;
    if (localeType == IUserDictionaryWords::LOCALE_TYPE_CURRENT) {
        AutoPtr<ILocaleHelper> localeHelper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
        localeHelper->GetDefault((ILocale**)&locale);
    } else {
        locale = NULL;
    }

    return AddWord(context, word, frequency, String(NULL), locale.Get());
}

ECode UserDictionary::Words::AddWord(
    /* [in] */ IContext* context,
    /* [in] */ const String& word,
    /* [in] */ Int32 frequency,
    /* [in] */ const String& shortcut,
    /* [in] */ ILocale* locale)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);

    if (TextUtils::IsEmpty(word)) {
        return NOERROR;
    }

    if (frequency < UserDictionary::FREQUENCY_MIN) frequency = UserDictionary::FREQUENCY_MIN;
    if (frequency > UserDictionary::FREQUENCY_MAX) frequency = UserDictionary::FREQUENCY_MAX;

    Int32 COLUMN_COUNT = 5;
    AutoPtr<IContentValues> values;
    CContentValues::New(COLUMN_COUNT, (IContentValues**)&values);

    values->Put(IUserDictionaryWords::WORD, word);
    values->Put(IUserDictionaryWords::FREQUENCY, frequency);
    String lcstr;
    if (NULL != locale) {
        locale->ToString(&lcstr);
    } else {
        lcstr = String(NULL);
    }
    values->Put(IUserDictionaryWords::LOCALE, lcstr);
    values->Put(IUserDictionaryWords::APP_ID, 0); // TODO: Get App UID
    values->Put(IUserDictionaryWords::SHORTCUT, shortcut);

    AutoPtr<IUri> result;
    return resolver->Insert(UserDictionary::Words::CONTENT_URI.Get(), values.Get(), (IUri**)&result);
    // It's ok if the insert doesn't succeed because the word
    // already exists.
}

const Int32 UserDictionary::FREQUENCY_MIN = 0;
const Int32 UserDictionary::FREQUENCY_MAX = 255;

static AutoPtr<IUri> initUserDictionaryCONTENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::Parse(String("content://") + IUserDictionary::AUTHORITY, (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> UserDictionary::CONTENT_URI = initUserDictionaryCONTENT_URI();

} // namespace Provider
} // namespace Droid
} // namespace Elastos


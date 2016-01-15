
#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/CUserDictionaryWords.h"
#include "elastos/droid/provider/UserDictionary.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CUserDictionaryWords)

CAR_INTERFACE_IMPL(CUserDictionaryWords, Singleton, IUserDictionaryWords)

ECode CUserDictionaryWords::AddWord(
    /* [in] */ IContext* context,
    /* [in] */ const String& word,
    /* [in] */ Int32 frequency,
    /* [in] */ Int32 localeType)
{
    return UserDictionary::Words::AddWord(context, word, frequency, localeType);
}

ECode CUserDictionaryWords::AddWord(
    /* [in] */ IContext* context,
    /* [in] */ const String& word,
    /* [in] */ Int32 frequency,
    /* [in] */ const String& shortcut,
    /* [in] */ ILocale* locale)
{
    return UserDictionary::Words::AddWord(context, word, frequency, shortcut, locale);
}

ECode CUserDictionaryWords::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = UserDictionary::Words::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos

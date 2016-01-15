
#ifndef __ELASTOS_UTILITY_REGEX_REGEX_CPATTERNHELPER_H__
#define __ELASTOS_UTILITY_REGEX_REGEX_CPATTERNHELPER_H__

#include "_Elastos_Utility_Regex_CPatternHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Utility {
namespace Regex {

CarClass(CPatternHelper)
    , public Singleton
    , public IPatternHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Equivalent to {@code Pattern.compile(pattern, 0)}.
     */
    CARAPI Compile(
        /* [in] */ const String& pattern,
        /* [out] */ IPattern** result);

    /**
     * Returns a compiled form of the given {@code regularExpression}, as modified by the
     * given {@code flags}. See the <a href="#flags">flags overview</a> for more on flags.
     *
     * @throws PatternSyntaxException if the regular expression is syntactically incorrect.
     *
     * @see #CANON_EQ
     * @see #CASE_INSENSITIVE
     * @see #COMMENTS
     * @see #DOTALL
     * @see #LITERAL
     * @see #MULTILINE
     * @see #UNICODE_CASE
     * @see #UNIX_LINES
     */
    CARAPI Compile(
        /* [in] */ const String& regularExpression,
        /* [in] */ Int32 flags,
        /* [out] */ IPattern** result);

    /**
     * Tests whether the given {@code regularExpression} matches the given {@code input}.
     * Equivalent to {@code Pattern.compile(regularExpression).matcher(input).matches()}.
     * If the same regular expression is to be used for multiple operations, it may be more
     * efficient to reuse a compiled {@code Pattern}.
     *
     * @see Pattern#compile(java.lang.String, int)
     * @see Matcher#matches()
     */
    CARAPI Matches(
        /* [in] */ const String& regularExpression,
        /* [in] */ const String& input,
        /* [out] */ Boolean* isMatche);

    CARAPI Matches(
        /* [in] */ const String& regularExpression,
        /* [in] */ ICharSequence* input,
        /* [out] */ Boolean* isMatche);

    /**
     * Quotes the given {@code string} using "\Q" and "\E", so that all
     * meta-characters lose their special meaning. This method correctly
     * escapes embedded instances of "\Q" or "\E". If the entire result
     * is to be passed verbatim to {@link #compile}, it's usually clearer
     * to use the {@link #LITERAL} flag instead.
     */
    CARAPI Quote(
        /* [in] */ const String& string,
        /* [out] */ String* result);
};

} // namespace Regex
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_REGEX_REGEX_CPATTERNHELPER_H__

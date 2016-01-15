
#ifndef __ELASTOS_UTILITY_REGEX_REGEX_MATCHRESULTIMPL_H__
#define __ELASTOS_UTILITY_REGEX_REGEX_MATCHRESULTIMPL_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"

namespace Elastos {
namespace Utility {
namespace Regex {

/**
 * Holds the results of a successful match of a regular expression against a
 * given string. Only used internally, thus sparsely documented (though the
 * defining public interface has full documentation).
 *
 * @see java.util.regex.MatchResult
 */
class MatchResultImpl
    : public Object
    , public IMatchResult
{
public:
    CAR_INTERFACE_DECL()

    MatchResultImpl(
        /* [in] */ const String& text,
        /* [in] */ const ArrayOf<Int32>& offsets);

    CARAPI End(
        /* [out] */ Int32* index);

    CARAPI End(
        /* [in] */ Int32 group,
        /* [out] */ Int32* index);

    CARAPI Group(
        /* [out] */ String* text);

    CARAPI Group(
        /* [in] */ Int32 group,
        /* [out] */ String* text);

    CARAPI GroupCount(
        /* [out] */ Int32* count);

    CARAPI Start(
        /* [in] */ Int32 group,
        /* [out] */ Int32* index);

    CARAPI Start(
        /* [out] */ Int32* index);

private:
    /**
     * Holds the original input text.
     */
    String mText;

    /**
     * Holds the offsets of the groups in the input text. The first two
     * elements specifiy start and end of the zero group, the next two specify
     * group 1, and so on.
     */
    AutoPtr<ArrayOf<Int32> > mOffsets;
};

} // namespace Regex
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_REGEX_REGEX_MATCHRESULTIMPL_H__

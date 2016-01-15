
#ifndef __ELASTOS_DROID_CONTENT_CURIMATCHER_H__
#define __ELASTOS_DROID_CONTENT_CURIMATCHER_H__

#include "_Elastos_Droid_Content_CUriMatcher.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CUriMatcher)
    , public Object
    , public IUriMatcher
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CUriMatcher();

    virtual ~CUriMatcher();

    /**
     * Creates the root node of the URI tree.
     *
     * @param code the code to match for the root URI
     */
    CARAPI constructor(
        /* [in] */ Int32 code);

    /**
     * Add a URI to match, and the code to return when this URI is
     * matched. URI nodes may be exact match string, the token "*"
     * that matches any text, or the token "#" that matches only
     * numbers.
     *
     * @param authority the authority to match
     * @param path the path to match. * may be used as a wild card for
     * any text, and # may be used as a wild card for numbers.
     * @param code the code that is returned when a URI is matched
     * against the given components. Must be positive.
     */
    CARAPI AddURI(
        /* [in] */ const String& authority,
        /* [in] */ const String& path,
        /* [in] */ Int32 code);

    /**
     * Try to match against the path in a url.
     *
     * @param uri       The url whose path we will match against.
     *
     * @return  The code for the matched node (added using addURI),
     * or -1 if there is no matched node.
     */
    CARAPI Match(
        /* [in] */ IUri* uri,
        /* [out] */ Int32* matchCode);

public:
    static AutoPtr<IPattern> GetSplitPattern();

private:
    static AutoPtr<IPattern> PATH_SPLIT_PATTERN;

    static const Int32 EXACT = 0;
    static const Int32 NUMBER = 1;
    static const Int32 TEXT = 2;

private:
    Int32 mCode;
    Int32 mWhich;
    String mText;
    List<AutoPtr<CUriMatcher> > mChildren;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CURIMATCHER_H__

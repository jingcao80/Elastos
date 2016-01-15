#ifndef __ELASTOS_DROID_OS_PATTERNMATCHER_H__
#define __ELASTOS_DROID_OS_PATTERNMATCHER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {

class PatternMatcher
    : public Object
    , public IPatternMatcher
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    PatternMatcher();

    virtual ~PatternMatcher();

    CARAPI constructor();

    CARAPI constructor(
    /* [in] */ const String& pattern,
    /* [in] */ Int32 type);

    CARAPI GetPath(
        /* [out] */ String* path);

    CARAPI GetType(
        /* [out] */ Int32* type);

    CARAPI Match(
        /* [in] */ const String& str,
        /* [out] */ Boolean* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    static CARAPI_(Boolean) MatchPattern(
        /* [in] */ const String& pattern,
        /* [in] */ const String& match,
        /* [in] */ Int32 type);

private:
    String mPattern;
    Int32 mType;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_PATTERNMATCHER_H__


#ifndef __ELASTOS_DROID_UTILITY_STATESET_H__
#define __ELASTOS_DROID_UTILITY_STATESET_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Utility {

class StateSet
{
public:
    static CARAPI_(Boolean) IsWildCard(
        /* [in] */ const ArrayOf<Int32>* stateSetOrSpec);

    static CARAPI_(Boolean) StateSetMatches(
        /* [in] */ const ArrayOf<Int32>* stateSpec,
        /* [in] */ const ArrayOf<Int32>* stateSet);

    static CARAPI_(Boolean) StateSetMatches(
        /* [in] */ const ArrayOf<Int32>* stateSpec,
        /* [in] */ Int32 state);

    static CARAPI_(AutoPtr< ArrayOf<Int32> >) TrimStateSet(
        /* [in] */ ArrayOf<Int32>* states,
        /* [in] */ Int32 newSize);

    //static String dump(int[] states);

public:
    static const AutoPtr< ArrayOf<Int32> > WILD_CARD;
    static const AutoPtr< ArrayOf<Int32> > NOTHING;

private:
    StateSet();
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_UTILITY_STATESET_H__

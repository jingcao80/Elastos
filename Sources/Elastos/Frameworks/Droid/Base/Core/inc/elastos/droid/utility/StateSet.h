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

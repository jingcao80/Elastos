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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_ATRESOPNSEPARSER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_ATRESOPNSEPARSER_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::Object;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class Operators: public Object
{
public:
    static CARAPI_(String) OperatorReplace(
        /* [in] */ const String& response);

    // unoptimized version of operatorreplace for responseOperatorInfos
    // this will provide a little more flexiblilty  in a loop like sisuation
    // same numbers of checks like before
    // this is for the search network functionality
    CARAPI_(String) UnOptimizedOperatorReplace(
        /* [in] */ const String& response);

private:
    // Initialize list of Operator codes
    // this will be taken care of when garbage collection starts.
    AutoPtr<HashMap<String, String> > InitList();

private:
    //this will stay persistant in memory when called
    static String sStored;
    static String sStoredOperators;

    // this will not stay persistant in memory, this will be taken care of
    // iin garbage collection routiene.
    AutoPtr<HashMap<String, String> > mUnOptOperators;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_ATRESOPNSEPARSER_H__

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

#ifndef __ELASTOS_UTILITY_REGEX_REGEX_CMATCHERHELPER_H__
#define __ELASTOS_UTILITY_REGEX_REGEX_CMATCHERHELPER_H__

#include "_Elastos_Utility_Regex_CMatcherHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Utility {
namespace Regex {

/**
 * The result of applying a {@code Pattern} to a given input. See {@link Pattern} for
 * example uses.
 */
CarClass(CMatcherHelper)
    , public Singleton
    , public IMatcherHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns a replacement string for the given one that has all backslashes
     * and dollar signs escaped.
     *
     * @param s
     *            the input string.
     * @return the input string, with all backslashes and dollar signs having
     *         been escaped.
     */
    CARAPI QuoteReplacement(
        /* [in] */ const String& s,
        /* [out] */ String* input);
};

} // namespace Regex
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_REGEX_REGEX_CMATCHERHELPER_H__

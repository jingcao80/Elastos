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

#ifndef __ORG_APACHE_HTTP_UTILITY_LANGUTILS_H_
#define __ORG_APACHE_HTTP_UTILITY_LANGUTILS_H_

#include "elastos/core/Object.h"

using Elastos::Core::IThrowable;

namespace Org {
namespace Apache {
namespace Http {
namespace Utility {

/**
 * A set of utility methods to help produce consistent
 * {@link Object#equals equals} and {@link Object#hashCode hashCode} methods.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
class LangUtils
{
public:
    static CARAPI_(Int32) HashCode(
        /* [in] */ Int32 seed,
        /* [in] */ Int32 hashcode);

    static CARAPI_(Int32) HashCode(
        /* [in] */ Int32 seed,
        /* [in] */ Boolean b);

    static CARAPI_(Int32) HashCode(
        /* [in] */ Int32 seed,
        /* [in] */ IObject* obj);

    static CARAPI_(Boolean) Equals(
        /* [in] */ IObject* obj1,
        /* [in] */ IObject* obj2);

    static CARAPI_(Boolean) Equals(
        /* [in] */ ArrayOf<IObject*>* a1,
        /* [in] */ ArrayOf<IObject*>* a2);

private:
    LangUtils() {}

public:
    static const Int32 HASH_SEED = 17;
    static const Int32 HASH_OFFSET = 37;
};

} // namespace Utility
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_UTILITY_LANGUTILS_H_

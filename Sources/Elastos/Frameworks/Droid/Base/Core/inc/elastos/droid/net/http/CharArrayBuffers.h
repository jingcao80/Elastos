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

#ifndef __ELASTOS_DROID_NET_HTTP_CHARARRAYBUFFERS_H__
#define __ELASTOS_DROID_NET_HTTP_CHARARRAYBUFFERS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * Utility methods for working on CharArrayBuffers.
 *
 * {@hide}
 */
class CharArrayBuffers
    : public Object
{
public:
    /**
     * Returns true if the buffer contains the given string. Ignores leading
     * whitespace and case.
     *
     * @param buffer to search
     * @param beginIndex index at which we should start
     * @param str to search for
     */
    static CARAPI ContainsIgnoreCaseTrimmed(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ Int32 beginIndex,
        /* [in] */ const String& str,
        /* [out] */ Boolean* result);

    /**
     * Returns index of first occurence ch. Lower cases characters leading up
     * to first occurrence of ch.
     */
    static CARAPI SetLowercaseIndexOf(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ Int32 ch,
        /* [out] */ Int32* result);

    static const Char32 uppercaseAddon;

private:
    static CARAPI_(Char32) ToLower(
        /* [in] */ Char32 c);
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CHARARRAYBUFFERS_H__

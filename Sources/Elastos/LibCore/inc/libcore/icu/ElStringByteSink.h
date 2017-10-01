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

#ifndef __LIBCORE_ICU_ELSTRINGBYTESINK_H__
#define __LIBCORE_ICU_ELSTRINGBYTESINK_H__

#include <elastos.h>
#include <unicode/bytestream.h>

using Elastos::String;
using Elastos::Int32;

namespace Libcore {
namespace ICU {

/**
 * Implementation of ByteSink that writes to a "string".
 * The StringClass is usually instantiated with a std::string.
 * @stable ICU 4.2
 */
class ElStringByteSink : public U_ICU_NAMESPACE::ByteSink
{
public:
    /**
     * Constructs a ByteSink that will append bytes to the dest string.
     * @param dest pointer to string object to append to
     * @stable ICU 4.2
     */
    ElStringByteSink(
        /* [in] */ String* dest)
        : mDest(dest)
    { }

    /**
     * Append "bytes[0,n-1]" to this.
     * @param data the pointer to the bytes
     * @param n the number of bytes; must be non-negative
     * @stable ICU 4.2
     */
    virtual void Append(
        /* [in] */ const char* data,
        /* [in] */ int32_t n)
    {
        assert(mDest != NULL);

        Int32 len = strlen(data);
        len = MIN(len, n);
        if (len > 0) {
            char* sub = new char[len + 1];
            memcpy(sub, data, len);
            sub[len] = '\0';
            (*mDest) += sub;
            delete[] sub;
        }

    }

private:
    ElStringByteSink(); ///< default constructor not implemented

    ElStringByteSink(
        /* [in] */ const ElStringByteSink &); ///< copy constructor not implemented

    ElStringByteSink &operator = (
        /* [in] */ const ElStringByteSink &); ///< assignment operator not implemented

private:
    String* mDest;
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_ELSTRINGBYTESINK_H__

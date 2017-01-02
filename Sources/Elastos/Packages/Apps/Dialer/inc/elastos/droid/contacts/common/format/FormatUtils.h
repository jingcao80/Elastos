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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_FORMAT_FORMATUTILS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_FORMAT_FORMATUTILS_H__

#include "Elastos.Droid.Database.h"
#include "_Elastos.Droid.Dialer.h"

using Elastos::Droid::Database::ICharArrayBuffer;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Format {

/**
 * Assorted utility methods related to text formatting in Contacts.
 */
class FormatUtils
{
public:
    /**
     * Finds the earliest point in buffer1 at which the first part of buffer2 matches.  For example,
     * overlapPoint("abcd", "cdef") == 2.
     */
    static CARAPI_(Int32) OverlapPoint(
        /* [in] */ ICharArrayBuffer* buffer1,
        /* [in] */ ICharArrayBuffer* buffer2);

    /**
     * Finds the earliest point in string1 at which the first part of string2 matches.  For example,
     * overlapPoint("abcd", "cdef") == 2.
     */
    // @VisibleForTesting
    static CARAPI_(Int32) OverlapPoint(
        /* [in] */ const String& string1,
        /* [in] */ const String& string2);

    /**
     * Finds the earliest point in array1 at which the first part of array2 matches.  For example,
     * overlapPoint("abcd", "cdef") == 2.
     */
    static CARAPI_(Int32) OverlapPoint(
        /* [in] */ ArrayOf<Char32>* array1,
        /* [in] */ ArrayOf<Char32>* array2);

    /**
     * Applies the given style to a range of the input CharSequence.
     * @param style The style to apply (see the style constants in {@link Typeface}).
     * @param input The CharSequence to style.
     * @param start Starting index of the range to style (will be clamped to be a minimum of 0).
     * @param end Ending index of the range to style (will be clamped to a maximum of the input
     *     length).
     * @param flags Bitmask for configuring behavior of the span.  See {@link android.text.Spanned}.
     * @return The styled CharSequence.
     */
    static CARAPI_(AutoPtr<ICharSequence>) ApplyStyleToSpan(
        /* [in] */ Int32 style,
        /* [in] */ ICharSequence* input,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 flags);

    // @VisibleForTesting
    static CARAPI_(void) CopyToCharArrayBuffer(
        /* [in] */ const String& text,
        /* [in] */ ICharArrayBuffer* buffer);

    /** Returns a String that represents the content of the given {@link CharArrayBuffer}. */
    // @VisibleForTesting
    static CARAPI_(String) CharArrayBufferToString(
        /* [in] */ ICharArrayBuffer* buffer);

    /**
     * Finds the index of the first word that starts with the given prefix.
     * <p>
     * If not found, returns -1.
     *
     * @param text the text in which to search for the prefix
     * @param prefix the text to find, in upper case letters
     */
    static CARAPI_(Int32) IndexOfWordPrefix(
        /* [in] */ ICharSequence* text,
        /* [in] */ const String& prefix);
};

} // Format
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_FORMAT_FORMATUTILS_H__

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

#ifndef __ELASTOS_DROID_TEXT_UTILITY_CRFC822TOKENIZER_H__
#define __ELASTOS_DROID_TEXT_UTILITY_CRFC822TOKENIZER_H__

#include "_Elastos_Droid_Text_Utility_CRfc822Tokenizer.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Widget::ITokenizer;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Utility {

CarClass(CRfc822Tokenizer)
    , public Object
    , public IRfc822Tokenizer
    , public ITokenizer
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * This constructor will try to take a string like
     * "Foo Bar (something) &lt;foo\@google.com&gt;,
     * blah\@google.com (something)"
     * and convert it into one or more Rfc822Tokens, output into the supplied
     * collection.
     *
     * It does *not* decode MIME encoded-words; charset conversion
     * must already have taken place if necessary.
     * It will try to be tolerant of broken syntax instead of
     * returning an error.
     *
     */
    static CARAPI Tokenize(
        /* [in] */ ICharSequence* text,
        /* [in, out] */ ICollection* out);

    /**
     * This method will try to take a string like
     * "Foo Bar (something) &lt;foo\@google.com&gt;,
     * blah\@google.com (something)"
     * and convert it into one or more Rfc822Tokens.
     * It does *not* decode MIME encoded-words; charset conversion
     * must already have taken place if necessary.
     * It will try to be tolerant of broken syntax instead of
     * returning an error.
     */
    static CARAPI Tokenize(
        /* [in] */ ICharSequence* text,
        /* [out] */ ArrayOf<IRfc822Token*>** ret);

    CARAPI FindTokenStart(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 cursor,
        /* [out] */ Int32* ret);

    CARAPI FindTokenEnd(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 cursor,
        /* [out] */ Int32* ret);

    /**
     * Terminates the specified address with a comma and space.
     * This assumes that the specified text already has valid syntax.
     * The Adapter subclass's convertToString() method must make that
     * guarantee.
     */
    CARAPI TerminateToken(
        /* [in] */ ICharSequence* text,
        /* [out] */ ICharSequence** ret);

private:
    static CARAPI Crunch(
        /* [in] */ StringBuilder& sb);
};

} //namespace Utility
} //namespace Text
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_TEXT_UTILITY_CRFC822TOKENIZER_H__

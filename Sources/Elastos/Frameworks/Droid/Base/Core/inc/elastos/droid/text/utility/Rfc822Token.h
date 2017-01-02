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

#ifndef __ELASTOS_DROID_TEXT_UTILITY_Rfc822Token_H__
#define __ELASTOS_DROID_TEXT_UTILITY_Rfc822Token_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Text {
namespace Utility {

/**
 * This class stores an RFC 822-like name, address, and comment,
 * and provides methods to convert them to quoted strings.
 */
class Rfc822Token
    : public Object
    , public IRfc822Token
{
public:
    CAR_INTERFACE_DECL()

    Rfc822Token();

    /**
     * Creates a new Rfc822Token with the specified name, address,
     * and comment.
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& address,
        /* [in] */ const String& comment);

    /**
     * Returns the name part.
     */
    CARAPI GetName(
        /* [out] */ String* str);

    /**
     * Returns the address part.
     */
    CARAPI GetAddress(
        /* [out] */ String* str);

    /**
     * Returns the comment part.
     */
    CARAPI GetComment(
        /* [out] */ String* str);

    /**
     * Changes the name to the specified name.
     */
    CARAPI SetName(
        /* [in] */ const String& name);

    /**
     * Changes the address to the specified address.
     */
    CARAPI SetAddress(
        /* [in] */ const String& address);

    /**
     * Changes the comment to the specified comment.
     */
    CARAPI SetComment(
        /* [in] */ const String& comment);

    /**
     * Returns the name (with quoting added if necessary),
     * the comment (in parentheses), and the address (in angle brackets).
     * This should be suitable for inclusion in an RFC 822 address list.
     */
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Returns the name, conservatively quoting it if there are any
     * characters that are likely to cause trouble outside of a
     * quoted string, or returning it literally if it seems safe.
     */
    static CARAPI QuoteNameIfNecessary(
        /* [in] */ const String& name,
        /* [out] */ String* str);

    /**
     * Returns the name, with internal backslashes and quotation marks
     * preceded by backslashes.  The outer quote marks themselves are not
     * added by this method.
     */
    static CARAPI QuoteName(
        /* [in] */ const String& name,
        /* [out] */ String* str);

    /**
     * Returns the comment, with internal backslashes and parentheses
     * preceded by backslashes.  The outer parentheses themselves are
     * not added by this method.
     */
    static CARAPI QuoteComment(
        /* [in] */ const String& comment,
        /* [out] */ String* str);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* equals);

private:
    static CARAPI_(Boolean) StringEquals(
        /* [in] */ const String& a,
        /* [in] */ const String& b);

protected:
    String mName;
    String mAddress;
    String mComment;
};

} // namespace Utility
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_UTILITY_Rfc822Token_H__

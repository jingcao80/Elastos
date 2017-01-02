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

#ifndef __ELASTOS_UTILITY_REGEX_REGEX_MATCHRESULTIMPL_H__
#define __ELASTOS_UTILITY_REGEX_REGEX_MATCHRESULTIMPL_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Utility {
namespace Regex {

/**
 * Holds the results of a successful match of a regular expression against a
 * given string. Only used internally, thus sparsely documented (though the
 * defining public interface has full documentation).
 *
 * @see java.util.regex.MatchResult
 */
class MatchResultImpl
    : public Object
    , public IMatchResult
{
public:
    CAR_INTERFACE_DECL()

    MatchResultImpl(
        /* [in] */ const String& text,
        /* [in] */ const ArrayOf<Int32>& offsets);

    CARAPI End(
        /* [out] */ Int32* index);

    CARAPI End(
        /* [in] */ Int32 group,
        /* [out] */ Int32* index);

    CARAPI Group(
        /* [out] */ String* text);

    CARAPI Group(
        /* [in] */ Int32 group,
        /* [out] */ String* text);

    CARAPI GroupCount(
        /* [out] */ Int32* count);

    CARAPI Start(
        /* [in] */ Int32 group,
        /* [out] */ Int32* index);

    CARAPI Start(
        /* [out] */ Int32* index);

private:
    /**
     * Holds the original input text.
     */
    String mText;

    /**
     * Holds the offsets of the groups in the input text. The first two
     * elements specifiy start and end of the zero group, the next two specify
     * group 1, and so on.
     */
    AutoPtr<ArrayOf<Int32> > mOffsets;
};

} // namespace Regex
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_REGEX_REGEX_MATCHRESULTIMPL_H__

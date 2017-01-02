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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CINDENTINGPRINTWRITER_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CINDENTINGPRINTWRITER_H__

#include "_Elastos_Droid_Internal_Utility_CIndentingPrintWriter.h"
#include <elastos/io/PrintWriter.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::IO::IWriter;
using Elastos::IO::PrintWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CIndentingPrintWriter)
    , public PrintWriter
    , public IIndentingPrintWriter
{
public:
    CIndentingPrintWriter();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IWriter* writer,
        /* [in] */ const String& indent);

    CARAPI constructor(
        /* [in] */ IWriter* writer,
        /* [in] */ const String& indent,
        /* [in] */ Int32 wrapLength);

    CARAPI IncreaseIndent();

    CARAPI DecreaseIndent();

    CARAPI PrintPair(
        /* [in] */ const String& key,
        /* [in] */ IInterface* value);

    CARAPI PrintHexPair(
        /* [in] */ const String& key,
        /* [in] */ Int32 value);

    CARAPI Write(
        /* [in] */ ArrayOf<Char32>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

private:
    CARAPI MaybeWriteIndent();

private:
    String mSingleIndent;
    Int32 mWrapLength;

    /** Mutable version of current indent */
    StringBuilder mIndentBuilder;
    /** Cache of current {@link #mIndentBuilder} value */
    AutoPtr<ArrayOf<Char32> > mCurrentIndent;
    /** Length of current line being built, excluding any indent */
    Int32 mCurrentLength;
    /**
     * Flag indicating if we're currently sitting on an empty line, and that
     * next write should be prefixed with the current indent.
     */
    Boolean mEmptyLine;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CINDENTINGPRINTWRITER_H__

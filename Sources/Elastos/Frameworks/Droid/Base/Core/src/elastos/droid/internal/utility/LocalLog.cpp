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

#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/internal/utility/LocalLog.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(LocalLog, Object, ILocalLog)

LocalLog::LocalLog()
    : mMaxLines(20)
{
}

ECode LocalLog::constructor(
    /* [in] */ const String& tag)
{
    mTag = tag;
    return NOERROR;
}

ECode LocalLog::W(
    /* [in] */ const String& msg)
{
    AutoLock lock(mLinesLock);
    Slogger::W(mTag, msg);
    if (mLines.GetSize() >= mMaxLines) {
        mLines.PopFront();
    }
    mLines.PushBack(msg);
    return NOERROR;
}

ECode LocalLog::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& header,
    /* [in] */ const String& prefix,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(mLinesLock);
    if (mLines.GetSize() <= 0) {
        *result = FALSE;
        return NOERROR;
    }
    if (header != NULL) {
        pw->Println(header);
    }
    List<String>::Iterator iter = mLines.Begin();
    for (; iter != mLines.End(); ++iter) {
        if (prefix != NULL) {
            pw->Print(prefix);
        }
        pw->Println(*iter);
    }
    *result = TRUE;
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

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

#include "CDropBox.h"
#include "cutils/log.h"
#include "CBase64.h"

namespace Libcore {
namespace IO {

CAR_SINGLETON_IMPL(CDropBox)
CAR_INTERFACE_IMPL(CDropBox, Singleton, IDropBox)
CAR_INTERFACE_IMPL(CDropBox::DefaultReporter, Object, IReporter)

AutoPtr<IReporter> CDropBox::REPORTER = new DefaultReporter();

ECode CDropBox::DefaultReporter::AddData(
    /* [in] */ const String& tag,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 flags)
{
    AutoPtr<IBase64> base64;
    CBase64::AcquireSingleton((IBase64**)&base64);
    String enStr;
    base64->Encode(data, &enStr);
    ALOGD("%s:%s", tag.string(), enStr.string());
    return NOERROR;
}

ECode CDropBox::DefaultReporter::AddText(
    /* [in] */ const String& tag,
    /* [in] */ const String& data)
{
    ALOGD("%s:%s", tag.string(), data.string());
    return NOERROR;
}

ECode CDropBox::SetReporter(
    /* [in] */  IReporter* reporter)
{
    if (reporter == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    REPORTER = reporter;
    return NOERROR;
}

/**
 * Returns non-null Reporter.
 */
ECode CDropBox::GetReporter(
    /* [out] */  IReporter** reporter)
{
    *reporter = REPORTER;
    REFCOUNT_ADD(*reporter)
    return NOERROR;
}

ECode CDropBox::AddData(
    /* [in] */  const String& tag,
    /* [in] */  ArrayOf<Byte>* data,
    /* [in] */  Int32 flags)
{
    REPORTER->AddData(tag, data, flags);
    return NOERROR;
}

ECode CDropBox::AddText(
    /* [in] */  const String& tag,
    /* [in] */  const String& data)
{
    REPORTER->AddText(tag, data);
    return NOERROR;
}

} // namespace IO
} // namespace Libcore
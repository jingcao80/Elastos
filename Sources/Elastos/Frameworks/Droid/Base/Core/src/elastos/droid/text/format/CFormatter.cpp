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

#include "elastos/droid/text/format/CFormatter.h"
#include "elastos/droid/text/format/Formatter.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

CAR_INTERFACE_IMPL(CFormatter, Singleton, IFormatter)

CAR_SINGLETON_IMPL(CFormatter)

ECode CFormatter::FormatFileSize(
    /* [in] */ IContext* context,
    /* [in] */ Int64 number,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = Formatter::FormatFileSize(context, number);
    return NOERROR;
}

ECode CFormatter::FormatIpAddress(
    /* [in] */ Int32 ipv4Address,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = Formatter::FormatIpAddress(ipv4Address);
    return NOERROR;
}

ECode CFormatter::FormatShortFileSize(
    /* [in] */ IContext* context,
    /* [in] */ Int64 number,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = Formatter::FormatShortFileSize(context, number);
    return NOERROR;
}

ECode CFormatter::FormatShortElapsedTime(
     /* [in] */ IContext* context,
     /* [in] */ Int64 millis,
     /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = Formatter::FormatShortElapsedTime(context, millis);
    return NOERROR;
}

} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos

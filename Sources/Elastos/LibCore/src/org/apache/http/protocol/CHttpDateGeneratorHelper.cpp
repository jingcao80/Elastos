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

#include "Elastos.CoreLibrary.Text.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/protocol/CHttpDateGeneratorHelper.h"
#include "org/apache/http/protocol/HttpDateGenerator.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL(CHttpDateGeneratorHelper, Object, IHttpDateGeneratorHelper)

CAR_SINGLETON_IMPL(CHttpDateGeneratorHelper)

ECode CHttpDateGeneratorHelper::GetGMT(
    /* [out] */ ITimeZone** timeZone)
{
    VALIDATE_NOT_NULL(timeZone)
    *timeZone = HttpDateGenerator::GMT;
    REFCOUNT_ADD(*timeZone)
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

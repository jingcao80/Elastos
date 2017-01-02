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

#include "elastos/droid/speech/srec/CRecognizerHelper.h"
#include "elastos/droid/speech/srec/Recognizer.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

CAR_SINGLETON_IMPL(CRecognizerHelper)

CAR_INTERFACE_IMPL(CRecognizerHelper, Singleton, IRecognizerHelper)

ECode CRecognizerHelper::GetConfigDir(
    /* [in] */ ILocale* locale,
    /* [out] */ String* ret)
{
    return Recognizer::GetConfigDir(locale, ret);
}

ECode CRecognizerHelper::EventToString(
    /* [in] */ Int32 event,
    /* [out] */ String* ret)
{
    return Recognizer::EventToString(event, ret);
}

} // namespace Srec
} // namespace Speech
} // namespace Droid
} // namespace Elastos

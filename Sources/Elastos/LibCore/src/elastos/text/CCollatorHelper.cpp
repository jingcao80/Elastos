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

#include "CCollatorHelper.h"
#include "Collator.h"

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CCollatorHelper, Singleton, ICollatorHelper)

CAR_SINGLETON_IMPL(CCollatorHelper)

ECode CCollatorHelper::GetAvailableLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    return Collator::GetAvailableLocales(locales);
}

ECode CCollatorHelper::GetInstance(
    /* [out] */ ICollator** instance)
{
    return Collator::GetInstance(instance);
}

ECode CCollatorHelper::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ ICollator** instance)
{
    return Collator::GetInstance(locale,instance);
}

} // namespace Text
} // namespace Elastos
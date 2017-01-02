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

#include "elastos/droid/dialer/CDialtactsActivityHelper.h"
#include "elastos/droid/dialer/DialtactsActivity.h"

namespace Elastos {
namespace Droid {
namespace Dialer {

CAR_INTERFACE_IMPL(CDialtactsActivityHelper, Singleton, IDialtactsActivityHelper);

CAR_SINGLETON_IMPL(CDialtactsActivityHelper);

ECode CDialtactsActivityHelper::GetCallSettingsIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    AutoPtr<IIntent> obj = DialtactsActivity::GetCallSettingsIntent();
    *intent = obj;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CDialtactsActivityHelper::GetAddNumberToContactIntent(
    /* [in] */ICharSequence* text,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    AutoPtr<IIntent> obj = DialtactsActivity::GetAddNumberToContactIntent(text);
    *intent = obj;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

} // Dialer
} // Droid
} // Elastos

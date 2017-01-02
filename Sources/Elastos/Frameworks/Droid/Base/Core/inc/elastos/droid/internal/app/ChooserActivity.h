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

#ifndef __ELASTOS_DROID_INTERNAL_APP_CHOOSERACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_CHOOSERACTIVITY_H__

#include "elastos/droid/internal/app/ResolverActivity.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class ChooserActivity
    : public ResolverActivity
    , public IChooserActivity
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetReplacementIntent(
        /* [in] */ const String& packageName,
        /* [in] */ IIntent* defIntent,
        /* [out] */ IIntent** outIntent);

protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI_(void) ModifyTargetIntent(
        /* [in] */ IIntent* in);

private:
    AutoPtr<IBundle> mReplacementExtras;
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CHOOSERACTIVITY_H__

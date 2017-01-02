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

#ifndef __ELASTOS_DROID_INTERNAL_VIEW_ACTIONBARPOLICY_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_ACTIONBARPOLICY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

class ActionBarPolicy
    : public Object
    , public IActionBarPolicy
{
public:
    static CARAPI_(AutoPtr<IActionBarPolicy>) Get(
        /* [in] */ IContext* context);

    CAR_INTERFACE_DECL()

    CARAPI GetMaxActionButtons(
        /* [out] */ Int32* rst);

    CARAPI ShowsOverflowMenuButton(
        /* [out] */ Boolean* rst);

    CARAPI GetEmbeddedMenuWidthLimit(
        /* [out] */ Int32* rst);

    CARAPI HasEmbeddedTabs(
        /* [out] */ Boolean* rst);

    CARAPI GetTabContainerHeight(
        /* [out] */ Int32* rst);

    CARAPI EnableHomeButtonByDefault(
        /* [out] */ Boolean* rst);

    CARAPI GetStackedTabMaxWidth(
        /* [out] */ Int32* rst);

private:
    ActionBarPolicy(
        /* [in] */ IContext* ctx);

private:
    IContext* mContext;
};

} // namespace View
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_ACTIONBARPOLICY_H__

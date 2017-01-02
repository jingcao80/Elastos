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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CRenderNodeHelper.h"
#include "elastos/droid/view/RenderNode.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CRenderNodeHelper, Singleton, IRenderNodeHelper)

CAR_SINGLETON_IMPL(CRenderNodeHelper)

ECode CRenderNodeHelper::constructor()
{
    return NOERROR;
}

ECode CRenderNodeHelper::Create(
    /* [in] */ const String& name,
    /* [in] */ IView* owningView,
    /* [out] */ IRenderNode** node)
{
    VALIDATE_NOT_NULL(node)
    AutoPtr<IRenderNode> tmp = RenderNode::Create(name, owningView);
    *node = tmp;
    REFCOUNT_ADD(*node)
    return NOERROR;
}

ECode CRenderNodeHelper::Adopt(
    /* [in] */ Handle64 nativePtr,
    /* [out] */ IRenderNode** node)
{
    VALIDATE_NOT_NULL(node)
    AutoPtr<IRenderNode> tmp = RenderNode::Adopt(nativePtr);
    *node = tmp;
    REFCOUNT_ADD(*node)
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos

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

#ifndef  __ELASTOS_DROID_LAUNCHER2_CINFODROPTARGET_H__
#define  __ELASTOS_DROID_LAUNCHER2_CINFODROPTARGET_H__

#include "_Elastos_Droid_Launcher2_CInfoDropTarget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/ButtonDropTarget.h"
#include "elastos/droid/launcher2/DropTarget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Graphics::Drawable::ITransitionDrawable;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CInfoDropTarget)
    , public ButtonDropTarget
    , public IInfoDropTarget
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    //@Override
    CARAPI AcceptDrop(
        /* [in] */ IDropTargetDragObject* d,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnDragStart(
        /* [in] */ IDragSource* source,
        /* [in] */ IInterface* info,
        /* [in] */ Int32 dragAction);

    //@Override
    CARAPI OnDragEnd();

    //@Override
    CARAPI OnDragEnter(
        /* [in] */ IDropTargetDragObject* d);

    //@Override
    CARAPI OnDragExit(
        /* [in] */ IDropTargetDragObject* d);

protected:
    //@Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(Boolean) IsFromAllApps(
        /* [in] */ IDragSource* source);

private:
    AutoPtr<IColorStateList> mOriginalTextColor;
    AutoPtr<ITransitionDrawable> mDrawable;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CINFODROPTARGET_H__
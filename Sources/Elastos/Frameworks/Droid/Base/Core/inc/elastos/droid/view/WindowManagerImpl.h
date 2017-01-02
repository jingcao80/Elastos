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

#ifndef __ELASTOS_DROID_VIEW_WINDOWMANAGERIMPL_H__
#define __ELASTOS_DROID_VIEW_WINDOWMANAGERIMPL_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

class WindowManagerImpl
    : public Object
    , public IWindowManager
    , public IViewManager
{
public:
    CAR_INTERFACE_DECL()

    WindowManagerImpl(
        /* [in] */ IDisplay* display);

private:
    WindowManagerImpl(
        /* [in] */ IDisplay* display,
        /* [in] */ IWindow* parentWindow);

public:
    CARAPI_(AutoPtr<WindowManagerImpl>) CreateLocalWindowManager(
        /* [in] */ IWindow* parentWindow);

    CARAPI_(AutoPtr<WindowManagerImpl>) CreatePresentationWindowManager(
        /* [in] */ IDisplay* display);

    CARAPI AddView(
        /* [in] */ IView* view,
        /* [in] */ IViewGroupLayoutParams* params);

    CARAPI UpdateViewLayout(
        /* [in] */ IView* view,
        /* [in] */ IViewGroupLayoutParams* params);

    CARAPI RemoveView(
        /* [in] */ IView* view);

    CARAPI RemoveViewImmediate(
        /* [in] */ IView* view);

    CARAPI GetDefaultDisplay(
        /* [out] */ IDisplay** display);

private:
    AutoPtr<IWindowManagerGlobal> mGlobal;
    AutoPtr<IDisplay> mDisplay;
    IWindow* mParentWindow;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_WINDOWMANAGERIMPL_H__

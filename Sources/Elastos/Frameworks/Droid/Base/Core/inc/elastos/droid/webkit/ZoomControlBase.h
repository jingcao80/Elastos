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

#ifndef __ELASTOS_DROID_WEBKIT_ZOOMCONTROLBASE_H__
#define __ELASTOS_DROID_WEBKIT_ZOOMCONTROLBASE_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

interface IZoomControlBase : public IInterface
{
public:
    /**
     * Causes the on-screen zoom control to be made visible
     */
    virtual CARAPI Show() = 0;

    /**
     * Causes the on-screen zoom control to disappear
     */
    virtual CARAPI Hide() = 0;

    /**
     * Enables the control to update its state if necessary in response to a
     * change in the pages zoom level. For example, if the max zoom level is
     * reached then the control can disable the button for zooming in.
     */
    virtual CARAPI Update() = 0;

    /**
     * Checks to see if the control is currently visible to the user.
     */
    virtual CARAPI IsVisible(
        /* [out] */ Boolean* isVisible) = 0;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_ZOOMCONTROLBASE_H__

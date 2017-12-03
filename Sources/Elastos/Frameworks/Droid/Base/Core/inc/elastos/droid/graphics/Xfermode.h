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

#ifndef __ELASTOS_DROID_GRAPHICS_XFERMODE_H__
#define __ELASTOS_DROID_GRAPHICS_XFERMODE_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * Xfermode is the base class for objects that are called to implement custom
 * "transfer-modes" in the drawing pipeline. The static function Create(Modes)
 * can be called to return an instance of any of the predefined subclasses as
 * specified in the Modes enum. When an Xfermode is assigned to an Paint, then
 * objects drawn with that paint have the xfermode applied.
 */
class Xfermode
    : public Object
    , public IXfermode
{
public:
    CAR_INTERFACE_DECL()

    Xfermode();

public:
    static const Int32 DEFAULT;

    Int32 mPorterDuffMode;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_XFERMODE_H__

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

#ifndef __ELASTOS_DROID_WIDGET_CGLES31_H__
#define __ELASTOS_DROID_WIDGET_CGLES31_H__

#include "_Elastos_Droid_Opengl_CGLES31.h"
#include "elastos/droid/opengl/GLES31.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLES31)
    , public GLES31
{
public:
    CAR_SINGLETON_DECL()
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CGLES31_H__


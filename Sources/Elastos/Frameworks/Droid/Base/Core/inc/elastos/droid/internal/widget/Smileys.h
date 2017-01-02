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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_SMILEYS_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_SMILEYS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Internal::Widget::ISmileys;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class Smileys
    : public Object
    , public ISmileys
{
public:
    CAR_INTERFACE_DECL()

    Smileys();

    static CARAPI GetSmileyResource(
        /* [in] */ Int32 which,
        /* [out] */ Int32* result);

private:
    static CARAPI_(AutoPtr< ArrayOf<Int32> >) InnerInitIconds();

private:
    static const AutoPtr< ArrayOf<Int32> > sIconIds;
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_SMILEYS_H__

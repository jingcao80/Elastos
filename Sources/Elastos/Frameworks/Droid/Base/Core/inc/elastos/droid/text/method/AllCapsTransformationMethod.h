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

#ifndef __ELASTOS_DROID_TEXT_METHOD_ALLCAPSTRANSFORMATIONMETHOD_H__
#define __ELASTOS_DROID_TEXT_METHOD_ALLCAPSTRANSFORMATIONMETHOD_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::ILocale;
using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * Transforms source text into an ALL CAPS string, locale-aware.
 *
 * @hide
 */
class AllCapsTransformationMethod
    : public Object
    , public IAllCapsTransformationMethod
    , public ITransformationMethod2
    , public ITransformationMethod
{
public:
    CAR_INTERFACE_DECL()

    AllCapsTransformationMethod();

    virtual ~AllCapsTransformationMethod();

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI GetTransformation(
        /* [in] */ ICharSequence* source,
        /* [in] */ IView* view,
        /* [out] */ ICharSequence** ret);

    //@Override
    CARAPI OnFocusChanged(
        /* [in] */ IView* view,
        /* [in] */ ICharSequence* sourceText,
        /* [in] */ Boolean focused,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    //@Override
    CARAPI SetLengthChangesAllowed(
        /* [in] */ Boolean allowLengthChanges);

private:
    static const String TAG;

    Boolean mEnabled;
    AutoPtr<ILocale> mLocale;
};


} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_ALLCAPSTRANSFORMATIONMETHOD_H__

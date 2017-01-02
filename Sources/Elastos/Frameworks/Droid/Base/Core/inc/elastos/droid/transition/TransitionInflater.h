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

#ifndef __ELASTOS_DROID_TRANSITION_TRANSITIONINFLATER_H__
#define __ELASTOS_DROID_TRANSITION_TRANSITIONINFLATER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Transition.h"
#include <elastos/core/Object.h>

using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::IArrayMap;


namespace Elastos {
namespace Droid {
namespace Transition {

class TransitionInflater
    : public Object
    , public ITransitionInflater
{
public:
    CAR_INTERFACE_DECL()

    TransitionInflater();

    /**
     * Obtains the TransitionInflater from the given context.
     */
    static CARAPI_(AutoPtr<ITransitionInflater>) From(
        /* [in] */ IContext* context);

    CARAPI InflateTransition(
        /* [in] */ Int32 resource,
        /* [out] */ ITransition** result);

    CARAPI InflateTransitionManager(
        /* [in] */ Int32 resource,
        /* [in] */ IViewGroup* sceneRoot,
        /* [out] */ ITransitionManager** result);

private:
    CARAPI constructor(
        /* [in] */ IContext* context);

    //
    // Transition loading
    //
    CARAPI_(AutoPtr<ITransition>) CreateTransitionFromXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ ITransition* parent);

    // AutoPtr<IInterface> CreateCustom(
    //     /* [in] */ IAttributeSet* attrs,
    //     /* [in] */ Class expectedType,
    //     /* [in] */ const String& tag);

    CARAPI_(void) GetTargetIds(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ ITransition* transition);

    //
    // TransitionManager loading
    //

    CARAPI_(AutoPtr<ITransitionManager>) CreateTransitionManagerFromXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IViewGroup* sceneRoot);

    CARAPI_(void) LoadTransition(
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransitionManager* transitionManager);

private:
//    static final Class<?>[] sConstructorSignature;

    static AutoPtr<IArrayMap> sConstructors;

    AutoPtr<IContext> mContext;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_TRANSITIONINFLATER_H__

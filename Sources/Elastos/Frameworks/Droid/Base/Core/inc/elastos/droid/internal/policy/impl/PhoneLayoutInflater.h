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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONELAYOUTINFLATER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONELAYOUTINFLATER_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/LayoutInflater.h"


using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::ILayoutInflaterFactory;
using Elastos::Droid::View::ILayoutInflaterFactory2;
using Elastos::Droid::View::ILayoutInflaterFilter;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

class PhoneLayoutInflater
    : public LayoutInflater
    , public IPhoneLayoutInflater
{
public:
    CAR_INTERFACE_DECL()

    PhoneLayoutInflater();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI CloneInContext(
        /* [in] */ IContext* newContext,
        /* [out] */ ILayoutInflater** newInflater) = 0;

protected:
    /** Override onCreateView to instantiate names that correspond to the
        widgets known to the Widget factory. If we don't find a match,
        call through to our super class.
    */
    CARAPI OnCreateView(
        /* [in] */ const String& name,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IView** view);

private:
    static const AutoPtr< ArrayOf<String> > sClassPrefixList;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONELAYOUTINFLATER_H__

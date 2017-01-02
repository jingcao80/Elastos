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

#ifndef __Elastos_DevSamples_FragmentDemo_CFragmentOne_H__
#define __Elastos_DevSamples_FragmentDemo_CFragmentOne_H__

#include <elastos/droid/app/Fragment.h>
#include "_Elastos_DevSamples_FragmentDemo_CFragmentOne.h"

using Elastos::Droid::App::Fragment;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace DevSamples {
namespace FragmentDemo {

CarClass(CFragmentOne)
    , public Fragment
{
public:
    class MyListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ CFragmentOne* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CFragmentOne* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CFragmentOne();

    ~CFragmentOne();

    CARAPI constructor();

    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);
};

} // namespace FragmentDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_FragmentDemo_CFragmentOne_H__

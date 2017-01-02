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

#ifndef __ELASTOS_DROID_SETTINGS_CCRYPTKEEPERCONFIRM_H__
#define __ELASTOS_DROID_SETTINGS_CCRYPTKEEPERCONFIRM_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Settings_CCryptKeeperConfirm.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/app/Fragment.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::Fragment;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;
using Elastos::Core::Runnable;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CCryptKeeperConfirm)
    , public Fragment
{
public:
    class Blank
        : public Activity
    {
    private:
        class MyRunnable
            : public Runnable
        {
        public:
            TO_STRING_IMPL("CCryptKeeperConfirm::Blank::MyRunnable")

            MyRunnable(
                /* [in] */ Blank* host);

            CARAPI Run();

        private:
            Blank* mHost;
        };

    public:
        TO_STRING_IMPL("CCryptKeeperConfirm::Blank");

        Blank();

        virtual ~Blank();

        CARAPI constructor();

        //@Override
        CARAPI OnCreate(
            /* [in] */ IBundle* savedInstanceState);

    private:
        AutoPtr<IHandler> mHandler;
    };

private:
    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CCryptKeeperConfirm::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ CCryptKeeperConfirm* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CCryptKeeperConfirm* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CCryptKeeperConfirm();

    ~CCryptKeeperConfirm();

    CARAPI constructor();

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

private:
    CARAPI_(void) EstablishFinalConfirmationState();

private:
    static const String TAG;

    AutoPtr<IView> mContentView;
    AutoPtr<IButton> mFinalButton;
    AutoPtr<IViewOnClickListener> mFinalClickListener;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCRYPTKEEPERCONFIRM_H__
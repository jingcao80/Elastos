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

#ifndef __ELASTOS_DROID_INTERNAL_APP_MEDIAROUTEDIALOGPRESENTER_H__
#define __ELASTOS_DROID_INTERNAL_APP_MEDIAROUTEDIALOGPRESENTER_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class MediaRouteDialogPresenter
{
public:
    static CARAPI_(AutoPtr<IDialogFragment>) ShowDialogFragment(
        /* [in] */ IActivity* activity,
        /* [in] */ Int32 routeTypes,
        /* [in] */ IViewOnClickListener* extendedSettingsClickListener);

    static CARAPI_(AutoPtr<IDialog>) CreateDialog(
        /* [in] */ IContext* context,
        /* [in] */ Int32 routeTypes,
        /* [in] */ IViewOnClickListener* extendedSettingsClickListener);

private:
    MediaRouteDialogPresenter();

private:
    static const String TAG;
    static const String CHOOSER_FRAGMENT_TAG ;
    static const String CONTROLLER_FRAGMENT_TAG;
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_MEDIAROUTEDIALOGPRESENTER_H__

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

#ifndef __ELASTOS_DROID_INTERNAL_APP_MEDIAROUTECONTROLLERDIALOGFRAGMENT_H__
#define __ELASTOS_DROID_INTERNAL_APP_MEDIAROUTECONTROLLERDIALOGFRAGMENT_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/app/DialogFragment.h"

using Elastos::Droid::App::DialogFragment;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class MediaRouteControllerDialogFragment
    : public DialogFragment
    , public IMediaRouteControllerDialogFragment
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a media route controller dialog fragment.
     * <p>
     * All subclasses of this class must also possess a default constructor.
     * </p>
     */
    CARAPI constructor();

    /**
     * Called when the controller dialog is being created.
     * <p>
     * Subclasses may override this method to customize the dialog.
     * </p>
     */
    CARAPI OnCreateControllerDialog(
        /* [in] */ IContext* context,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IMediaRouteControllerDialog** dialog);

    // @Override
    CARAPI OnCreateDialog(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IDialog** dialog);
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_MEDIAROUTECONTROLLERDIALOGFRAGMENT_H__

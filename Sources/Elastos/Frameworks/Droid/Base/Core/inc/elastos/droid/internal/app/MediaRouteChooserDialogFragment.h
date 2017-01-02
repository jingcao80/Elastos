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

#ifndef __ELASTOS_DROID_INTERNAL_APP_MEDIAROUTECHOOSERDIALOGFRAGMENT_H__
#define __ELASTOS_DROID_INTERNAL_APP_MEDIAROUTECHOOSERDIALOGFRAGMENT_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/app/DialogFragment.h"

using Elastos::Droid::App::DialogFragment;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class MediaRouteChooserDialogFragment
    : public DialogFragment
    , public IMediaRouteChooserDialogFragment
{
public:
    /**
     * Creates a media route chooser dialog fragment.
     * <p>
     * All subclasses of this class must also possess a default constructor.
     * </p>
     */
    MediaRouteChooserDialogFragment();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI GetRouteTypes(
        /* [out] */ Int32* types);

    CARAPI SetRouteTypes(
        /* [in] */ Int32 types);

    CARAPI SetExtendedSettingsClickListener(
        /* [in] */ IViewOnClickListener* listener);

    /**
     * Called when the chooser dialog is being created.
     * <p>
     * Subclasses may override this method to customize the dialog.
     * </p>
     */
    CARAPI OnCreateChooserDialog(
        /* [in] */ IContext* context,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IMediaRouteChooserDialog** dialog);

    // @Override
    CARAPI OnCreateDialog(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IDialog** dialog);

private:
    const String ARGUMENT_ROUTE_TYPES;
    AutoPtr<IViewOnClickListener> mExtendedSettingsClickListener;
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_MEDIAROUTECHOOSERDIALOGFRAGMENT_H__

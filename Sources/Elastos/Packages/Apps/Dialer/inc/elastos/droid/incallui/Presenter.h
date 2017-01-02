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

#ifndef __ELASTOS_DROID_INCALLUI_PRESENTER_H__
#define __ELASTOS_DROID_INCALLUI_PRESENTER_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class Presenter
    : public Object
    , public IPresenter
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Called after the UI view has been created.  That is when fragment.onViewCreated() is called.
     *
     * @param ui The Ui implementation that is now ready to be used.
     */
    CARAPI OnUiReady(
        /* [in] */ IUi* ui);

    /**
     * Called when the UI view is destroyed in Fragment.onDestroyView().
     */
    CARAPI OnUiDestroy(
        /* [in] */ IUi* ui);

    /**
     * To be overriden by Presenter implementations.  Called when the fragment is being
     * destroyed but before ui is set to null.
     */
    CARAPI OnUiUnready(
        /* [in] */ IUi* ui);

    CARAPI GetUi(
        /* [out] */ IUi** ui);

protected:
    Presenter() {}

private:
    AutoPtr<IUi> mUi;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_PRESENTER_H__

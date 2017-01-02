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

#ifndef __ELASTOS_DROID_TELECOM_STATUSHINTS_H__
#define __ELASTOS_DROID_TELECOM_STATUSHINTS_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telecom.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * Contains status label and icon displayed in the in-call UI.
 * @hide
 */
class StatusHints
    : public Object
    , public IStatusHints
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IComponentName* packageName,
        /* [in] */ ICharSequence* label,
        /* [in] */ Int32 iconResId,
        /* [in] */ IBundle* extras);

    /**
     * @return A package used to load the icon.
     */
    CARAPI GetPackageName(
        /* [out] */ IComponentName** result);

    /**
     * @return The label displayed in the in-call UI.
     */
    CARAPI GetLabel(
        /* [out] */ ICharSequence** result);

    /**
     * The icon resource ID for the icon to show.
     *
     * @return A resource ID.
     */
    CARAPI GetIconResId(
        /* [out] */ Int32* result);

    /**
     * @return An icon displayed in the in-call UI.
     */
    CARAPI GetIcon(
        /* [in] */ IContext* context,
        /* [out] */ IDrawable** result);

    /**
     * @return Extra data used to display status.
     */
    CARAPI GetExtras(
        /* [out] */ IBundle** result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

private:
    AutoPtr<IDrawable> GetIcon(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resId);

private:
    AutoPtr<IComponentName> mPackageName;
    AutoPtr<ICharSequence> mLabel;
    Int32 mIconResId;
    AutoPtr<IBundle> mExtras;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_STATUSHINTS_H__
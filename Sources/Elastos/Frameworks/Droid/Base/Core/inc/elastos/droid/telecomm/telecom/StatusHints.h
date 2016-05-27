#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_STATUSHINTS_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_STATUSHINTS_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telecomm.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecomm {
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
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_STATUSHINTS_H__
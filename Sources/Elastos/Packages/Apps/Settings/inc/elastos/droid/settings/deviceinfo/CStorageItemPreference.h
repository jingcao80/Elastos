
#ifndef __ELASTOS_DROID_SETTINGS_DEVICEINFO_CSTORAGEITEMPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_DEVICEINFO_CSTORAGEITEMPREFERENCE_H__

#include "_Elastos_Droid_Settings_Deviceinfo_CStorageItemPreference.h"
#include "elastos/droid/preference/Preference.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IShapeDrawable;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

CarClass(CStorageItemPreference)
    , public Elastos::Droid::Preference::Preference
{
public:
    TO_STRING_IMPL("CStorageItemPreference")

    CAR_OBJECT_DECL()

    CStorageItemPreference();

    ~CStorageItemPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 titleRes,
        /* [in] */ Int32 colorRes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICharSequence* title,
        /* [in] */ Int32 colorRes,
        /* [in] */ Int32 userHandle);

private:
    static CARAPI_(AutoPtr<IShapeDrawable>) CreateRectShape(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 color);
public:
    Int32 mColor;
    Int32 mUserHandle;
};

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DEVICEINFO_CSTORAGEITEMPREFERENCE_H__
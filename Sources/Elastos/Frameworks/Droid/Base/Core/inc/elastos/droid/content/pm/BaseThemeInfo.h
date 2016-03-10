
#ifndef __ELASTOS_DROID_CONTENT_PM_BASETHEMEINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_BASETHEMEINFO_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

class BaseThemeInfo
    : public Object
    , public IBaseThemeInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    virtual ~BaseThemeInfo() {}

    CARAPI constructor();

    CARAPI GetResolvedString(
        /* [in] */ IResources* res,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 index,
        /* [out] */ String* str);

    CARAPI GetThemeId(
        /* [out] */ String* themeId);

    CARAPI SetThemeId(
        /* [in] */ const String& themeId);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI GetAuthor(
        /* [out] */ String* author);

    CARAPI SetAuthor(
        /* [in] */ const String& author);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /*
     * Flatten this object in to a Parcel.
     *
     * @param dest The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be written.
     * May be 0 or {@link #PARCELABLE_WRITE_RETURN_VALUE}.
     *
     * @see android.os.Parcelable#writeToParcel(android.os.Parcel, int)
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    /**
     * The theme id, which does not change when the theme is modified.
     * Specifies an Android UI Style using style name.
     *
     * @see themeId attribute
     *
     */
    String mThemeId;

    /**
     * The name of the theme (as displayed by UI).
     *
     * @see name attribute
     *
     */
    String mName;

    /**
     * The author name of the theme package.
     *
     * @see author attribute
     *
     */
    String mAuthor;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_BASETHEMEINFO_H__

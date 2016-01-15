
#ifndef  __ELASTOS_DROID_HARDWARE_CLISTUTILS_H__
#define  __ELASTOS_DROID_HARDWARE_CLISTUTILS_H__

#include "_Elastos_Droid_Hardware_Camera2_Utils_CListUtils.h"
#include "elastos/droid/hardware/camera2/utils/ListUtils.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CarClass(CListUtils)
    , public Singleton
    , public IListUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /** Return {@code} true if the {@code list} contains the {@code needle}. */
    CARAPI ListContains(
        /* [in] */ IList* list,
        /* [in] */ IInterface* needle,
        /* [out] */ Boolean* value);

    /**
     * Return {@code true} if the {@code list} is only a single element equal to
     * {@code single}.
     */
    CARAPI ListElementsEqualTo(
        /* [in] */ IList* list,
        /* [in] */ IInterface* single,
        /* [out] */ Boolean* value);

    /**
     * Return a human-readable representation of a list (non-recursively).
     */
    CARAPI ListToString(
        /* [in] */ IList* list,
        /* [out] */ String* str);

    /**
     * Return the first item from {@code choices} that is contained in the {@code list}.
     *
     * <p>Choices with an index closer to 0 get higher priority. If none of the {@code choices}
     * are in the {@code list}, then {@code null} is returned.
     *
     * @param list a list of objects which may or may not contain one or more of the choices
     * @param choices an array of objects which should be used to select an item from
     *
     * @return the first item from {@code choices} contained in {@code list}, otherwise {@code null}
     */
    CARAPI ListSelectFirstFrom(
        /* [in] */ IList* list,
        /* [in] */ ArrayOf<IInterface*>* choices,
        /* [out] */ IInterface** outface);
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_HARDWARE_CLISTUTILS_H__

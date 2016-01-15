
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_LISTUTILS_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_LISTUTILS_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class ListUtils
{
public:
    /** Return {@code} true if the {@code list} contains the {@code needle}. */
    static CARAPI ListContains(
        /* [in] */ IList* list,
        /* [in] */ IInterface* needle,
        /* [out] */ Boolean* value);

    /**
     * Return {@code true} if the {@code list} is only a single element equal to
     * {@code single}.
     */
    static CARAPI ListElementsEqualTo(
        /* [in] */ IList* list,
        /* [in] */ IInterface* single,
        /* [out] */ Boolean* value);

    /**
     * Return a human-readable representation of a list (non-recursively).
     */
    static CARAPI ListToString(
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
    static CARAPI ListSelectFirstFrom(
        /* [in] */ IList* list,
        /* [in] */ ArrayOf<IInterface*>* choices,
        /* [out] */ IInterface** outface);

private:
    ListUtils() {}

    ~ListUtils() {}
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_LISTUTILS_H__

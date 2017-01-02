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

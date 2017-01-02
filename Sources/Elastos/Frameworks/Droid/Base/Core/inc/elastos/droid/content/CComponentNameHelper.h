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

#ifndef __ELASTOS_DROID_CONTENT_CCOMPONENTNAMEHELPER_H__
#define __ELASTOS_DROID_CONTENT_CCOMPONENTNAMEHELPER_H__

#include "_Elastos_Droid_Content_CComponentNameHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::Core::IStringBuilder;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CComponentNameHelper)
    , public Singleton
    , public IComponentNameHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Recover a ComponentName from a String that was previously created with
     * {@link #flattenToString()}.  It splits the string at the first '/',
     * taking the part before as the package name and the part after as the
     * class name.  As a special convenience (to use, for example, when
     * parsing component names on the command line), if the '/' is immediately
     * followed by a '.' then the final class name will be the concatenation
     * of the package name with the string following the '/'.  Thus
     * "com.foo/.Blah" becomes package="com.foo" class="com.foo.Blah".
     *
     * @param str The String that was returned by flattenToString().
     * @return Returns a new ComponentName containing the package and class
     * names that were encoded in <var>str</var>
     *
     * @see #flattenToString()
     */
    CARAPI UnflattenFromString(
        /* [in] */ const String& str,
        /* [out] */ IComponentName** componentName);

    /**
     * Write a ComponentName to a Parcel, handling null pointers.  Must be
     * read with {@link #readFromParcel(Parcel)}.
     *
     * @param c The ComponentName to be written.
     * @param out The Parcel in which the ComponentName will be placed.
     *
     * @see #readFromParcel(Parcel)
     */
    CARAPI WriteToParcel(
        /* [in] */ IComponentName* cn,
        /* [in] */ IParcel* dest);

    /**
     * Read a ComponentName from a Parcel that was previously written
     * with {@link #writeToParcel(ComponentName, Parcel)}, returning either
     * a null or new object as appropriate.
     *
     * @param in The Parcel from which to read the ComponentName
     * @return Returns a new ComponentName matching the previously written
     * object, or null if a null had been written.
     *
     * @see #writeToParcel(ComponentName, Parcel)
     */
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source,
        /* [out] */ IComponentName** cn);

    CARAPI AppendShortClassName(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ const String& packageName,
        /* [in] */ const String& className);

    CARAPI PrintShortClassName(
        /* [in] */ IPrintWriter* sb,
        /* [in] */ const String& packageName,
        /* [in] */ const String& className);

    CARAPI AppendShortString(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ const String& packageName,
        /* [in] */ const String& className);

    CARAPI PrintShortString(
        /* [in] */ IPrintWriter* sb,
        /* [in] */ const String& packageName,
        /* [in] */ const String& className);
};

} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_CCOMPONENTNAMEHELPER_H__

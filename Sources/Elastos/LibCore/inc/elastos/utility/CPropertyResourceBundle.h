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

#ifndef __ELASTOS_UTILITY_CPROPERTY_RESOURCE_BUNDLE_H__
#define __ELASTOS_UTILITY_CPROPERTY_RESOURCE_BUNDLE_H__

#include "_Elastos_Utility_CPropertyResourceBundle.h"
#include "ResourceBundle.h"

using Elastos::IO::IInputStream;
using Elastos::IO::IReader;

namespace Elastos {
namespace Utility {

CarClass(CPropertyResourceBundle)
    , public ResourceBundle
    , public IPropertyResourceBundle
{
private:
    class _Enumeration
        : public IEnumeration
        , public Object
    {
    public:
        CAR_INTERFACE_DECL()

        _Enumeration(
            /* [in] */ CPropertyResourceBundle* host);

        /**
         * Returns whether this {@code Enumeration} has more elements.
         *
         * @return {@code true} if there are more elements, {@code false} otherwise.
         * @see #nextElement
         */
        CARAPI HasMoreElements(
            /* [out] */ Boolean * value);

        /**
         * Returns the next element in this {@code Enumeration}.
         *
         * @return the next element..
         * @throws NoSuchElementException
         *             if there are no more elements.
         * @see #hasMoreElements
         */
        CARAPI GetNextElement(
            /* [out] */ IInterface ** inter);

    private:
        CARAPI_(Boolean) FindNext();

    private:
        AutoPtr<IEnumeration> mLocal; // = getLocalKeys();

        AutoPtr<IEnumeration> mPEnum; // = parent.getKeys();

        String mNextElement;

        AutoPtr<CPropertyResourceBundle> mHost;
    };
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Constructs a new instance of {@code PropertyResourceBundle} and loads the
     * properties file from the specified {@code InputStream}.
     *
     * @param stream
     *            the {@code InputStream}.
     * @throws IOException
     *             if an error occurs during a read operation on the
     *             {@code InputStream}.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* stream);

    /**
     * Constructs a new resource bundle with properties read from {@code reader}.
     *
     * @param reader the {@code Reader}
     * @throws IOException
     * @since 1.6
     */
    CARAPI constructor(
        /* [in] */ IReader* reader);

    /**
     * Returns the names of the resources contained in this {@code ResourceBundle}.
     *
     * @return an {@code Enumeration} of the resource names.
     */
    CARAPI GetKeys(
        /* [out] */ IEnumeration** enu);

    CARAPI HandleGetObject(
        /* [in] */ const String& key,
        /* [out] */ IInterface** outface);

private:
    AutoPtr<IEnumeration> GetLocalKeys();

private:
    AutoPtr<IProperties> mResources;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CPROPERTY_RESOURCE_BUNDLE_H__
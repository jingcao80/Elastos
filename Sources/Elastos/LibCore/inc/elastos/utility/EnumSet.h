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

#ifndef __ELASTOS_UTILITY_ENUMSET_H__
#define __ELASTOS_UTILITY_ENUMSET_H__

#include "AbstractSet.h"

using Elastos::IO::ISerializable;
using Elastos::Core::ICloneable;

namespace Elastos {
namespace Utility {

class EnumSet
    : public AbstractSet
    , public IEnumSet
    , public ICloneable
    , public ISerializable
{
private:
    class SerializationProxy
        : public Object
        , public ISerializable
    {
    public:
        CAR_INTERFACE_DECL()

        SerializationProxy();

    public:
        static const Int64 mSerialVersionUID;// = 362491234563181265L;
        InterfaceID mElementType;
        ArrayOf<IInterface *> * mElements;

    private:
        CARAPI_(IInterface*) ReadResolve();
    };

protected:
    EnumSet(
        /* [in] */ const InterfaceID& cls);

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const InterfaceID& cls);

    static CARAPI NoneOf(
        /* [in] */ const InterfaceID& type,
        /* [out] */ IEnumSet** res);

    static CARAPI_(AutoPtr<IEnumSet>) AllOf(
        /* [in] */ const InterfaceID& type);

    static CARAPI_(AutoPtr<IEnumSet>) CopyOf(
        /* [in] */ IEnumSet* s);

    static CARAPI CopyOf(
        /* [in] */ ICollection* c,
        /* [out] */ IEnumSet** set);

    static CARAPI_(AutoPtr<IEnumSet>) ComplementOf(
        /* [in] */ IEnumSet* s);

    static CARAPI_(AutoPtr<IEnumSet>) Of(
        /* [in] */ IInterface* i);

    static CARAPI_(AutoPtr<IEnumSet>) Of(
        /* [in] */ IInterface* i1,
        /* [in] */ IInterface* i2);

    static CARAPI_(AutoPtr<IEnumSet>) Of(
        /* [in] */ IInterface* i1,
        /* [in] */ IInterface* i2,
        /* [in] */ IInterface* i3);

    static CARAPI_(AutoPtr<IEnumSet>) Of(
        /* [in] */ IInterface* i1,
        /* [in] */ IInterface* i2,
        /* [in] */ IInterface* i3,
        /* [in] */ IInterface* i4);

    static CARAPI_(AutoPtr<IEnumSet>) Of(
        /* [in] */ IInterface* i1,
        /* [in] */ IInterface* i2,
        /* [in] */ IInterface* i3,
        /* [in] */ IInterface* i4,
        /* [in] */ IInterface* i5);

    static CARAPI_(AutoPtr<IEnumSet>) Of(
        /* [in] */ ArrayOf<IInterface*>* array);

    static CARAPI Range(
        /* [in] */ IInterface* start,
        /* [in] */ IInterface* end,
        /* [out] */ IEnumSet** set);

    virtual CARAPI Complement() = 0;

    virtual CARAPI SetRange(
        /* [in] */ IInterface* start,
        /* [in] */ IInterface* end) = 0;

    virtual CARAPI_(Boolean) IsValidType(
        /* [in] */ const InterfaceID& cls);

    virtual CARAPI_(AutoPtr<IInterface>) WriteReplace();

    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    CARAPI Add(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI Add(
        /* [in] */ IInterface* object);

    CARAPI AddAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI AddAll(
        /* [in] */ ICollection* collection);

    CARAPI Clear();

    CARAPI Contains(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI ContainsAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI Remove(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* modified);

    CARAPI Remove(
        /* [in] */ IInterface* object);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI RemoveAll(
        /* [in] */ ICollection* collection);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* modified);

    CARAPI RetainAll(
        /* [in] */ ICollection* collection);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<IInterface*>** array);

    CARAPI ToArray(
        /* [in] */ ArrayOf<IInterface*>* inArray,
        /* [out, callee] */ ArrayOf<IInterface*>** outArray);
private:
    static const Int64 mSerialVersionUID;//=1009687484059888093L

public:
    static InterfaceID mElementClass;
};


} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ENUMSET_H__
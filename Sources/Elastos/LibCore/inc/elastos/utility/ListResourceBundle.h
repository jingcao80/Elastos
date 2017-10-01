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

#ifndef __ELASTOS_UTILITY_LISTRESOURCEBUNDLE_H__
#define __ELASTOS_UTILITY_LISTRESOURCEBUNDLE_H__

#include "ResourceBundle.h"

namespace Elastos{
namespace Utility{

class ListResourceBundle
    : public ResourceBundle
    , public IListResourceBundle
{
public:
    class _FirstEnumeration
        : public Object
        , public IEnumeration
    {
    public:
        CAR_INTERFACE_DECL()

        _FirstEnumeration(
            /* [in] */ ListResourceBundle* host);

        CARAPI HasMoreElements(
            /* [out] */ Boolean* res);

        CARAPI GetNextElement(
            /* [out] */ IInterface** res);

    private:
        CARAPI_(Boolean) FindNext();

    public:
        AutoPtr<ListResourceBundle> mHost;
        AutoPtr<IIterator> mLocal;
        AutoPtr<IEnumeration> mEnum;
        String mNextElement;
    };

    class _SecondEnumeration
        : public Object
        , public IEnumeration
    {
    public:
        CAR_INTERFACE_DECL()

        _SecondEnumeration(
            /* [in] */ ListResourceBundle* host);

        CARAPI HasMoreElements(
            /* [out] */ Boolean* res);

        CARAPI GetNextElement(
            /* [out] */ IInterface** res);

    private:
        AutoPtr<ListResourceBundle> mHost;
        AutoPtr<IIterator> mIt;
    };

public:
    CAR_INTERFACE_DECL()

    ListResourceBundle();

    CARAPI GetKeys(
        /* [out] */ IEnumeration** enu);

    CARAPI HandleGetObject(
        /* [in] */ const String& key,
        /* [out] */ IInterface** outface);

protected:
    CARAPI HandleKeySet(
        /* [out] */ ISet** outset);

    virtual CARAPI_(AutoPtr< ArrayOf< AutoPtr< ArrayOf<IInterface*> > > >) GetContents() = 0;

private:
    CARAPI InitializeTable();

public:
    AutoPtr<IHashMap> mTable;
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_LISTRESOURCEBUNDLE_H__
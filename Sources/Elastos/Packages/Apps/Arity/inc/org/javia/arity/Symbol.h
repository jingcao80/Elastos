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

#ifndef _ORG_JAVIA_ARITY_SYMBOL_H__
#define _ORG_JAVIA_ARITY_SYMBOL_H__

#include "_Org.Javia.Arity.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

namespace Org {
namespace Javia {
namespace Arity {

class Symbol
    : public Object
    , public ISymbol
{
public:
    Symbol(
        /* [in] */ const String& name,
        /* [in] */ IFunction* fun);

    Symbol(
        /* [in] */ const String& name,
        /* [in] */ Double re,
        /* [in] */ Boolean isConst);

    Symbol(
        /* [in] */ const String& name,
        /* [in] */ Double re,
        /* [in] */ Double im,
        /* [in] */ Boolean isConst);

    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<ISymbol>) MakeArg(
        /* [in] */ const String& name,
        /* [in] */ Int32 order);

    static CARAPI_(AutoPtr<ISymbol>) MakeVmOp(
        /* [in] */ const String& name,
        /* [in] */ Int32 op);

    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetArity(
        /* [out] */ Int32* arity);

    static CARAPI_(AutoPtr<ISymbol>) NewEmpty(
        /* [in] */ ISymbol* s);

    CARAPI_(Boolean) IsEmpty();

    CARAPI_(void) SetKey(
        /* [in] */ const String& name,
        /* [in] */ Int32 arity);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

private:
    Symbol(
        /* [in] */ const String& name,
        /* [in] */ Int32 arity,
        /* [in] */ Byte op,
        /* [in] */ Boolean isConst,
        /* [in] */ Int32 dummy);

public:
    static const Int32 CONST_ARITY = -3;
    Byte mOp;
    AutoPtr<IFunction> mFun;
    Double mValueRe, mValueIm;
    Boolean mIsConst;

private:
    String mName;
    Int32 mArity;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_SYMBOL_H__
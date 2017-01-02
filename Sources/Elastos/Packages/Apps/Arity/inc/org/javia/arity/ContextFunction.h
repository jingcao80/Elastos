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

#ifndef _ORG_JAVIA_ARITY_CONTEXTFUNCITON_H__
#define _ORG_JAVIA_ARITY_CONTEXTFUNCITON_H__

#include "org/javia/arity/Function.h"

namespace Org {
namespace Javia {
namespace Arity {

class ContextFunction
    : public Function
    , public IContextFunction
{
public:
    virtual ~ContextFunction() {}

    CAR_INTERFACE_DECL()

    using IContextFunction::Eval;

    //@Override
    CARAPI Eval(
        /* [out] */ Double* result);

    //@Override
    CARAPI Eval(
        /* [in] */ Double x,
        /* [out] */ Double* result);

    //@Override
    CARAPI Eval(
        /* [in] */ Double x,
        /* [in] */ Double y,
        /* [out] */ Double* result);

    //@Override
    CARAPI Eval(
        /* [in] */ ArrayOf<Double>* args,
        /* [out] */ Double* result);

    CARAPI Eval(
        /* [in] */ Double x,
        /* [in] */ IEvalContext* context,
        /* [out] */ Double* result);

    CARAPI Eval(
        /* [in] */ Double x,
        /* [in] */ Double y,
        /* [in] */ IEvalContext* context,
        /* [out] */ Double* result);

    //@Override
    CARAPI EvalComplex(
        /* [out] */ IComplex** complex);

    //@Override
    CARAPI Eval(
        /* [in] */ IComplex* x,
        /* [out] */ IComplex** complex);

    //@Override
    CARAPI Eval(
        /* [in] */ IComplex* x,
        /* [in] */ IComplex* y,
        /* [out] */ IComplex** complex);

    //@Override
    CARAPI Eval(
        /* [in] */ ArrayOf<IComplex*>* args,
        /* [out] */ IComplex** complex);

    CARAPI Eval(
        /* [in] */ IComplex* x,
        /* [in] */ IEvalContext* context,
        /* [out] */ IComplex** complex);

    CARAPI Eval(
        /* [in] */ IComplex* x,
        /* [in] */ IComplex* y,
        /* [in] */ IEvalContext* context,
        /* [out] */ IComplex** complex);

protected:
    CARAPI_(AutoPtr<ArrayOf<IComplex*> >) ToComplex(
        /* [in] */ ArrayOf<Double>* args,
        /* [in] */ IEvalContext* context);

private:
    static AutoPtr<IEvalContext> sContext;
    static const AutoPtr<ArrayOf<Double> > NO_ARGS;
    static const AutoPtr<ArrayOf<IComplex*> > NO_ARGS_COMPLEX;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_CONTEXTFUNCITON_H__
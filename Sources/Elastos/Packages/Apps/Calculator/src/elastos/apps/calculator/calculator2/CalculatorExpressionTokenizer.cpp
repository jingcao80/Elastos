#include "elastos/apps/calculator/calculator2/CalculatorExpressionTokenizer.h"
#include "R.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Content.h>
#include <elastos/core/StringUtils.h>

using Elastos::Utility::CHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Core::StringUtils;

namespace Elastos{
namespace Apps {
namespace Calculator {
namespace Calculator2 {

CalculatorExpressionTokenizer::CalculatorExpressionTokenizer()
{}

CalculatorExpressionTokenizer::~CalculatorExpressionTokenizer()
{}

CAR_INTERFACE_IMPL(CalculatorExpressionTokenizer, Object, ICalculatorExpressionTokenizer)

ECode CalculatorExpressionTokenizer::constructor(
    /* [in] */ IContext* context)
{
    CHashMap::New((IHashMap**)&mReplacementMap);

    String str;

    mReplacementMap->Put(StringUtils::ParseCharSequence(String(".")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::dec_point, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("0")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::digit_0, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("1")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::digit_1, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("2")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::digit_2, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("3")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::digit_3, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("4")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::digit_4, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("5")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::digit_5, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("6")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::digit_6, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("7")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::digit_7, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("8")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::digit_8, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("9")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::digit_9, &str), str)).Get());

    mReplacementMap->Put(StringUtils::ParseCharSequence(String("/")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::op_div, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("*")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::op_mul, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("-")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::op_sub, &str), str)).Get());

    mReplacementMap->Put(StringUtils::ParseCharSequence(String("cos")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::fun_cos, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("ln")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::fun_ln, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("log")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::fun_log, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("sin")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::fun_sin, &str), str)).Get());
    mReplacementMap->Put(StringUtils::ParseCharSequence(String("tan")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::fun_tan, &str), str)).Get());

    mReplacementMap->Put(StringUtils::ParseCharSequence(String("Infinity")).Get(), StringUtils::ParseCharSequence((context->GetString(R::string::inf, &str), str)).Get());
}

ECode CalculatorExpressionTokenizer::GetNormalizedExpression(
    /* [in] */ const String& expr,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ISet> entrySet;
    mReplacementMap->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    AutoPtr<IMapEntry> entry;
    AutoPtr<IInterface> inext;
    AutoPtr<IInterface> ivalue;
    AutoPtr<IInterface> ikey;
    AutoPtr<ICharSequence> cs;
    String value;
    String key;
    String expr_ = expr;
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        entry = NULL;
        inext = NULL;
        it->GetNext((IInterface**)&inext);
        entry = IMapEntry::Probe(inext);
        ivalue = NULL;
        entry->GetValue((IInterface**)&ivalue);
        cs = NULL;
        cs = ICharSequence::Probe(ivalue);
        cs->ToString(&value);
        ikey = NULL;
        entry->GetKey((IInterface**)&ikey);
        cs = NULL;
        cs = ICharSequence::Probe(ikey);
        cs->ToString(&key);
        StringUtils::Replace(expr_, value, key, &expr_);
    }
    *result = expr_;
    return NOERROR;
}

ECode CalculatorExpressionTokenizer::GetLocalizedExpression(
    /* [in] */ const String& expr,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ISet> entrySet;
    mReplacementMap->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    AutoPtr<IMapEntry> entry;
    AutoPtr<IInterface> inext;
    AutoPtr<IInterface> ivalue;
    AutoPtr<IInterface> ikey;
    AutoPtr<ICharSequence> cs;
    String value;
    String key;
    String expr_ = expr;
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        entry = NULL;
        inext = NULL;
        it->GetNext((IInterface**)&inext);
        entry = IMapEntry::Probe(inext);
        ivalue = NULL;
        entry->GetValue((IInterface**)&ivalue);
        cs = NULL;
        cs = ICharSequence::Probe(ivalue);
        cs->ToString(&value);
        ikey = NULL;
        entry->GetKey((IInterface**)&ikey);
        cs = NULL;
        cs = ICharSequence::Probe(ikey);
        cs->ToString(&key);
        StringUtils::Replace(expr_, key, value, &expr_);
    }
    *result = expr_;
    return NOERROR;

}

} // namespace Calculator2
} // namespace Calculator
} // namespace Apps
} // namespace Elastos
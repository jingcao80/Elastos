
#include "CNativeCollationHelper.h"
#include "NativeCollation.h"

namespace Libcore {
namespace ICU {

CAR_SINGLETON_IMPL(CNativeCollationHelper)

CAR_INTERFACE_IMPL(CNativeCollationHelper, Singleton, INativeCollationHelper)

ECode CNativeCollationHelper::CloseCollator(
    /* [in] */ Int64 address)
{
    NativeCollation::CloseCollator(address);
    return NOERROR;
}

ECode CNativeCollationHelper::Compare(
    /* [in] */ Int64 address,
    /* [in] */ const String& source,
    /* [in] */ const String& target ,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)

    *value = NativeCollation::Compare(address,source,target);
    return NOERROR;
}

ECode CNativeCollationHelper::GetAttribute(
    /* [in] */ Int64 address,
    /* [in] */ Int32 type,
    /* [out] */ Int32 * value)
{
    return NativeCollation::GetAttribute(address,type,value);
}

ECode CNativeCollationHelper::GetCollationElementIterator(
    /* [in] */ Int64 address,
    /* [in] */ const String& source,
    /* [out] */ Int64 * value)
{
    return NativeCollation::GetCollationElementIterator(address,source,value);
}

ECode CNativeCollationHelper::GetRules(
    /* [in] */ Int64 address,
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str)

    *str = NativeCollation::GetRules(address);
    return NOERROR;
}

ECode CNativeCollationHelper::GetSortKey(
    /* [in] */ Int64 address,
    /* [in] */ const String& source,
    /* [out, callee] */ ArrayOf<Byte> ** outarray)
{
    VALIDATE_NOT_NULL(outarray)

    *outarray = NativeCollation::GetSortKey(address, source);
    return NOERROR;
}

ECode CNativeCollationHelper::OpenCollator(
    /* [in] */ ILocale* locale,
    /* [out] */ Int64 * value)
{
    return NativeCollation::OpenCollator(locale,value);
}

ECode CNativeCollationHelper::OpenCollatorFromRules(
    /* [in] */ const String& rules,
    /* [in] */ Int32 normalizationMode,
    /* [in] */ Int32 collationStrength,
    /* [out] */ Int64 * value)
{
    return NativeCollation::OpenCollatorFromRules(rules,normalizationMode,collationStrength,value);
}

ECode CNativeCollationHelper::SafeClone(
    /* [in] */ Int64 address,
    /* [out] */ Int64 * value)
{
    return NativeCollation::SafeClone(address,value);
}

ECode CNativeCollationHelper::SetAttribute(
    /* [in] */ Int64 address,
    /* [in] */ Int32 type,
    /* [in] */ Int32 value)
{
    return NativeCollation::SetAttribute(address,type,value);
}

ECode CNativeCollationHelper::CloseElements(
    /* [in] */ Int64 address)
{
    NativeCollation::CloseElements(address);
    return NOERROR;
}

ECode CNativeCollationHelper::GetMaxExpansion(
    /* [in] */ Int64 address,
    /* [in] */ Int32 order,
    /* [out] */ Int32 * value)
{
    *value = NativeCollation::GetMaxExpansion(address,order);
    return NOERROR;
}

ECode CNativeCollationHelper::GetOffset(
    /* [in] */ Int64 address,
    /* [out] */ Int32 * value)
{
    *value = NativeCollation::GetOffset(address);
    return NOERROR;
}

ECode CNativeCollationHelper::GetNext(
    /* [in] */ Int64 address,
    /* [out] */ Int32 * value)
{
    return NativeCollation::GetNext(address,value);
}

ECode CNativeCollationHelper::GetPrevious(
    /* [in] */ Int64 address,
    /* [out] */ Int32 * value)
{
    return NativeCollation::GetPrevious(address,value);
}

ECode CNativeCollationHelper::Reset(
    /* [in] */ Int64 address)
{
    NativeCollation::Reset(address);
    return NOERROR;
}

ECode CNativeCollationHelper::SetOffset(
    /* [in] */ Int64 address,
    /* [in] */ Int32 offset)
{
    return NativeCollation::SetOffset(address,offset);
}

ECode CNativeCollationHelper::SetText(
    /* [in] */ Int64 address,
    /* [in] */ const String& source)
{
    return NativeCollation::SetText(address,source);
}

} // namespace ICU
} // namespace Libcore

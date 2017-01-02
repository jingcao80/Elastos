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

#include "CDateFormatHelper.h"
#include "DateFormat.h"

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CDateFormatHelper, Singleton, IDateFormatHelper)

CAR_SINGLETON_IMPL(CDateFormatHelper)

ECode CDateFormatHelper::GetAvailableLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    return DateFormat::GetAvailableLocales(locales);
}

ECode CDateFormatHelper::GetDateInstance(
    /* [out] */ IDateFormat** instance)
{
    return DateFormat::GetDateInstance(instance);
}

ECode CDateFormatHelper::GetDateInstance(
    /* [in] */ Int32 style,
    /* [out] */ IDateFormat** instance)
{
    return DateFormat::GetDateInstance(style,instance);
}

ECode CDateFormatHelper::GetDateInstance(
    /* [in] */ Int32 style,
    /* [in] */ ILocale* locale,
    /* [out] */ IDateFormat** instance)
{
    return DateFormat::GetDateInstance(style,locale,instance);
}

ECode CDateFormatHelper::GetDateTimeInstance(
    /* [out] */ IDateFormat** instance)
{
    return DateFormat::GetDateTimeInstance(instance);
}

ECode CDateFormatHelper::GetDateTimeInstance(
    /* [in] */ Int32 dateStyle,
    /* [in] */ Int32 timeStyle,
    /* [out] */ IDateFormat** instance)
{
    return DateFormat::GetDateTimeInstance(dateStyle,timeStyle,instance);
}

ECode CDateFormatHelper::GetDateTimeInstance(
    /* [in] */ Int32 dateStyle,
    /* [in] */ Int32 timeStyle,
    /* [in] */ ILocale* locale,
    /* [out] */ IDateFormat** instance)
{
    return DateFormat::GetDateTimeInstance(dateStyle,timeStyle,locale,instance);
}

ECode CDateFormatHelper::Set24HourTimePref(
    /* [in] */ Boolean bval)
{
    return DateFormat::Set24HourTimePref(bval);
}

ECode CDateFormatHelper::GetInstance(
    /* [out] */ IDateFormat** instance)
{
    return DateFormat::GetInstance(instance);
}

ECode CDateFormatHelper::GetTimeInstance(
    /* [out] */ IDateFormat** instance)
{
    return DateFormat::GetTimeInstance(instance);
}

ECode CDateFormatHelper::GetTimeInstance(
    /* [in] */ Int32 style,
    /* [out] */ IDateFormat** instance)
{
    return DateFormat::GetTimeInstance(style,instance);
}

ECode CDateFormatHelper::GetTimeInstance(
    /* [in] */ Int32 style,
    /* [in] */ ILocale* locale,
    /* [out] */ IDateFormat** instance)
{
    return DateFormat::GetTimeInstance(style,locale,instance);
}

} // namespace Text
} // namespace Elastos

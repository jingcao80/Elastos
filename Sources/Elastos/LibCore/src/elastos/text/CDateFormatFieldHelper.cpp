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

#include "CDateFormatFieldHelper.h"
#include "DateFormat.h"

namespace Elastos {
namespace Text {

CAR_SINGLETON_IMPL(CDateFormatFieldHelper)

CAR_INTERFACE_IMPL(CDateFormatFieldHelper, Singleton, IDateFormatFieldHelper)

ECode CDateFormatFieldHelper::GetDateFormatFieldByName(
    /* [in] */ const String& name,
    /* [out] */ IDateFormatField** field)
{
    return NOERROR;
}

ECode CDateFormatFieldHelper::OfCalendarField(
    /* [in] */ Int32 calendarField,
    /* [out] */ IDateFormatField** field)
{
    return DateFormat::Field::OfCalendarField(calendarField,field);
}


} // namespace Text
} // namespace Elastos
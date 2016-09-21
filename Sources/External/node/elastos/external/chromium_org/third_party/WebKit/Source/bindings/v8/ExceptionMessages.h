/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ExceptionMessages_h
#define ExceptionMessages_h

#include "wtf/MathExtras.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/WTFString.h"

namespace WebCore {

class Decimal;

class ExceptionMessages {
public:
    enum BoundType {
        InclusiveBound,
        ExclusiveBound,
    };

    static WTF::String argumentNullOrIncorrectType(int argumentIndex, const WTF::String& expectedType);
    static WTF::String constructorNotCallableAsFunction(const char* type);

    static WTF::String failedToConstruct(const char* type, const WTF::String& detail);
    static WTF::String failedToEnumerate(const char* type, const WTF::String& detail);
    static WTF::String failedToExecute(const char* method, const char* type, const WTF::String& detail);
    static WTF::String failedToGet(const char* property, const char* type, const WTF::String& detail);
    static WTF::String failedToSet(const char* property, const char* type, const WTF::String& detail);
    static WTF::String failedToDelete(const char* property, const char* type, const WTF::String& detail);
    static WTF::String failedToGetIndexed(const char* type, const WTF::String& detail);
    static WTF::String failedToSetIndexed(const char* type, const WTF::String& detail);
    static WTF::String failedToDeleteIndexed(const char* type, const WTF::String& detail);

    template <typename NumType>
    static WTF::String formatNumber(NumType number)
    {
        return formatFiniteNumber(number);
    }

    static WTF::String incorrectPropertyType(const WTF::String& property, const WTF::String& detail);

    template <typename NumberType>
    static WTF::String indexExceedsMaximumBound(const char* name, NumberType given, NumberType bound)
    {
        bool eq = given == bound;
        StringBuilder result;
        result.append("The ");
        result.append(name);
        result.append(" provided (");
        result.append(formatNumber(given));
        result.append(") is greater than ");
        result.append(eq ? "or equal to " : "");
        result.append("the maximum bound (");
        result.append(formatNumber(bound));
        result.append(").");
        return result.toString();
    }

    template <typename NumberType>
    static WTF::String indexExceedsMinimumBound(const char* name, NumberType given, NumberType bound)
    {
        bool eq = given == bound;
        StringBuilder result;
        result.append("The ");
        result.append(name);
        result.append(" provided (");
        result.append(formatNumber(given));
        result.append(") is less than ");
        result.append(eq ? "or equal to " : "");
        result.append("the minimum bound (");
        result.append(formatNumber(bound));
        result.append(").");
        return result.toString();
    }

    template <typename NumberType>
    static WTF::String indexOutsideRange(const char* name, NumberType given, NumberType lowerBound, BoundType lowerType, NumberType upperBound, BoundType upperType)
    {
        StringBuilder result;
        result.append("The ");
        result.append(name);
        result.append(" provided (");
        result.append(formatNumber(given));
        result.append(") is outside the range ");
        result.append(lowerType == ExclusiveBound ? '(' : '[');
        result.append(formatNumber(lowerBound));
        result.append(", ");
        result.append(formatNumber(upperBound));
        result.append(upperType == ExclusiveBound ? ')' : ']');
        result.append('.');
        return result.toString();
    }

    static WTF::String invalidArity(const char* expected, unsigned provided);

    // If  > 0, the argument index that failed type check (1-indexed.)
    // If == 0, a (non-argument) value (e.g., a setter) failed the same check.
    static WTF::String notAnArrayTypeArgumentOrValue(int argumentIndex);
    static WTF::String notASequenceTypeProperty(const WTF::String& propertyName);
    static WTF::String notAFiniteNumber(double value, const char* name = "value provided");
    static WTF::String notAFiniteNumber(const Decimal& value, const char* name = "value provided");

    static WTF::String notEnoughArguments(unsigned expected, unsigned providedleastNumMandatoryParams);

    static WTF::String readOnly(const char* detail = 0);

private:
    template <typename NumType>
    static WTF::String formatFiniteNumber(NumType number)
    {
        if (number > 1e20 || number < -1e20)
            return WTF::String::format("%e", 1.0*number);
        return WTF::String::number(number);
    }

    template <typename NumType>
    static WTF::String formatPotentiallyNonFiniteNumber(NumType number)
    {
        if (std::isnan(number))
            return "NaN";
        if (std::isinf(number))
            return number > 0 ? "Infinity" : "-Infinity";
        if (number > 1e20 || number < -1e20)
            return WTF::String::format("%e", number);
        return WTF::String::number(number);
    }

    static WTF::String ordinalNumber(int number);
};

template <> WTF::String ExceptionMessages::formatNumber<float>(float number);
template <> WTF::String ExceptionMessages::formatNumber<double>(double number);

} // namespace WebCore

#endif // ExceptionMessages_h

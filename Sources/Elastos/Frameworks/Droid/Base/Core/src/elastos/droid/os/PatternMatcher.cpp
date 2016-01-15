
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/os/PatternMatcher.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL_2(PatternMatcher, Object, IPatternMatcher, IParcelable)

PatternMatcher::PatternMatcher()
    : mType(-1)
{
}

PatternMatcher::~PatternMatcher()
{
}

ECode PatternMatcher::constructor()
{
    return NOERROR;
}

ECode PatternMatcher::constructor(
    /* [in] */ const String& pattern,
    /* [in] */ Int32 type)
{
    mPattern = pattern;
    mType = type;
    return NOERROR;
}

ECode PatternMatcher::GetPath(
    /* [in] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    *path = mPattern;
    return NOERROR;
}

ECode PatternMatcher::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);

    *type = mType;
    return NOERROR;
}

ECode PatternMatcher::Match(
    /* [in] */ const String& str,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = MatchPattern(mPattern, str, mType);
    return NOERROR;
}

ECode PatternMatcher::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("? ");
    switch (mType) {
        case IPatternMatcher::PATTERN_LITERAL:
            *str = String("LITERAL: ");
            break;
        case IPatternMatcher::PATTERN_PREFIX:
            *str = String("PREFIX: ");
            break;
        case IPatternMatcher::PATTERN_SIMPLE_GLOB:
            *str = String("GLOB: ");
            break;
    }

    StringBuilder sb("PatternMatcher{");
    sb += mType;
    sb += mPattern;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

ECode PatternMatcher::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);
    source->ReadString(&mPattern);
    source->ReadInt32(&mType);
    return NOERROR;
}

ECode PatternMatcher::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    dest->WriteString(mPattern);
    dest->WriteInt32(mType);
    return NOERROR;
}

Boolean PatternMatcher::MatchPattern(
    /* [in] */ const String& pattern,
    /* [in] */ const String& match,
    /* [in] */ Int32 type)
{
    if (match.IsNull() || pattern.IsNull()) {
        return FALSE;
    }

    if (type == PATTERN_LITERAL) {
        return pattern.Equals(match);
    }

    if (type == PATTERN_PREFIX) {
        return match.StartWith(pattern);
    }
    else if (type != PATTERN_SIMPLE_GLOB) {
        return FALSE;
    }

    AutoPtr<ArrayOf<Char32> > patternChars = pattern.GetChars();
    AutoPtr<ArrayOf<Char32> > matchChars = pattern.GetChars();
    const Int32 NP = patternChars->GetLength();
    const Int32 NM = matchChars->GetLength();
    if (NP <= 0) {
        return NM <= 0;
    }

    Int32 ip = 0, im = 0;
    char nextChar = (*patternChars)[0];
    while ((ip < NP) && (im < NM)) {
        char c = nextChar;
        ip++;
        nextChar = ip < NP ? (*patternChars)[ip] : 0;
        const Boolean escaped = (c == '\\');
        if (escaped) {
            c = nextChar;
            ip++;
            nextChar = ip < NP ? (*patternChars)[ip] : 0;
        }
        if (nextChar == '*') {
            if (!escaped && c == '.') {
            if (ip >= (NP-1)) {
                    // at the end with a pattern match, so
                    // all is good without checking!
                    return TRUE;
                }
                ip++;
                nextChar = (*patternChars)[ip];
                // Consume everything until the next character in the
                // pattern is found.
                if (nextChar == '\\') {
                    ip++;
                    nextChar = ip < NP ? (*patternChars)[ip] : 0;
                }

                do {
                    if ((*matchChars)[im] == nextChar) {
                        break;
                    }
                    im++;
                } while (im < NM);

                if (im == NM) {
                    // Whoops, the next character in the pattern didn't
                    // exist in the match.
                    return FALSE;
                }
                ip++;
                nextChar = ip < NP ? (*patternChars)[ip] : 0;
                im++;
            }
            else {
                // Consume only characters matching the one before '*'.
                do {
                    if ((*matchChars)[im] != c) {
                        break;
                    }
                    im++;
                } while (im < NM);
                ip++;
                nextChar = ip < NP ? (*patternChars)[ip] : 0;
            }
        }
        else {
            if (c != '.' && (*matchChars)[im] != c) {
                return FALSE;
            }
            im++;
        }
    }

    if (ip >= NP && im >= NM) {
        // Reached the end of both strings, all is good!
        return TRUE;
    }

    // One last check: we may have finished the match string, but still
    // have a '.*' at the end of the pattern, which should still count
    // as a match.
    if (ip == NP-2 && (*patternChars)[ip] == '.'
        && (*patternChars)[ip+1] == '*') {
        return TRUE;
    }

    return FALSE;
}
} // namespace Os
} // namespace Droid
} // namespace Elastos

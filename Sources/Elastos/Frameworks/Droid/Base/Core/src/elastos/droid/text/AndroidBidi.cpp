#include "elastos/droid/text/AndroidBidi.h"
#include "elastos/droid/text/CLayoutDirections.h"
#include "elastos/droid/text/Layout.h"
#include "unicode/ubidi.h"

namespace Elastos {
namespace Droid {
namespace Text {

Int32 AndroidBidi::Bidi(
    /* [in] */ Int32 dir,
    /* [in] */ ArrayOf<Char32>* chs,
    /* [in] */ ArrayOf<Byte>* chInfo,
    /* [in] */ Int32 n,
    /* [in] */ Boolean haveInfo)
{
    if (chs == NULL || chInfo == NULL) {
        //Java:    throw new NullPointerException();
        return 0;// E_NULL_POINTER_EXCEPTION;
    }

    if (n < 0 || chs->GetLength() < n || chInfo->GetLength() < n) {
        //Java:    throw new IndexOutOfBoundsException();
        return 0;// E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    switch(dir) {
        case ILayout::DIR_REQUEST_LTR: {
            dir = 0;
            break;
        }
        case ILayout::DIR_REQUEST_RTL: {
            dir = 1;
            break;
        }
        case ILayout::DIR_REQUEST_DEFAULT_LTR: {
            dir = -2;
            break;
        }
        case ILayout::DIR_REQUEST_DEFAULT_RTL: {
            dir = -1;
            break;
        }
        default: {
            dir = 0;
            break;
        }
    }

    Int32 result = RunBidi(dir, chs, chInfo, n, haveInfo);
    result = (result & 0x1) == 0 ? ILayout::DIR_LEFT_TO_RIGHT : ILayout::DIR_RIGHT_TO_LEFT;
    return result;
}

AutoPtr<ILayoutDirections> AndroidBidi::Directions(
    /* [in] */ Int32 dir,
    /* [in] */ ArrayOf<Byte>* levels,
    /* [in] */ Int32 lstart,
    /* [in] */ ArrayOf<Char32>* chars,
    /* [in] */ Int32 cstart,
    /* [in] */ Int32 len)
{
    if (len == 0) {
        return Layout::DIRS_ALL_LEFT_TO_RIGHT;
    }

    Int32 baseLevel = (dir == ILayout::DIR_LEFT_TO_RIGHT) ? 0 : 1;
    Int32 curLevel = (Int32)((*levels)[lstart]);
    Int32 minLevel = curLevel;
    Int32 runCount = 1;
    for (Int32 i = lstart + 1, e = lstart + len; i < e; ++i) {
        Int32 level = (Int32)((*levels)[i]);
        if (level != curLevel) {
            curLevel = level;
            ++runCount;
        }
    }

    // add final run for trailing counter-directional whitespace
    Int32 visLen = len;
    if ((curLevel & 1) != (baseLevel & 1)) {
        // look for visible end
        while (--visLen >= 0) {
            Char32 ch = (*chars)[cstart + visLen];

            if (ch == '\n') {
                --visLen;
                break;
            }

            if (ch != ' ' && ch != '\t') {
                break;
            }
        }
        ++visLen;
        if (visLen != len) {
            ++runCount;
        }
    }

    if (runCount == 1 && minLevel == baseLevel) {
        AutoPtr<ILayoutDirections> dir;
        // we're done, only one run on this line
        if ((minLevel & 1) != 0) {
            return Layout::DIRS_ALL_RIGHT_TO_LEFT;
        }
        return Layout::DIRS_ALL_LEFT_TO_RIGHT;
    }

    AutoPtr< ArrayOf<Int32> > ld = ArrayOf<Int32>::Alloc(runCount * 2);
    Int32 maxLevel = minLevel;
    Int32 levelBits = minLevel << ILayout::RUN_LEVEL_SHIFT;
    {
        // Start of first pair is always 0, we write
        // length then start at each new run, and the
        // last run length after we're done.
        Int32 n = 1;
        Int32 prev = lstart;
        curLevel = minLevel;
        for (Int32 i = lstart, e = lstart + visLen; i < e; ++i) {
            Int32 level = (Int32)((*levels)[i]);
            if (level != curLevel) {
                curLevel = level;
                if (level > maxLevel) {
                    maxLevel = level;
                }
                else if (level < minLevel) {
                    minLevel = level;
                }
                // XXX ignore run length limit of 2^RUN_LEVEL_SHIFT
                (*ld)[n++] = (i - prev) | levelBits;
                (*ld)[n++] = i - lstart;
                levelBits = curLevel << ILayout::RUN_LEVEL_SHIFT;
                prev = i;
            }
        }
        (*ld)[n] = (lstart + visLen - prev) | levelBits;
        if (visLen < len) {
            (*ld)[++n] = visLen;
            (*ld)[++n] = (len - visLen) | (baseLevel << ILayout::RUN_LEVEL_SHIFT);
        }
    }

    // See if we need to swap any runs.
    // If the min level run direction doesn't match the base
    // direction, we always need to swap (at this point
    // we have more than one run).
    // Otherwise, we don't need to swap the lowest level.
    // Since there are no logically adjacent runs at the same
    // level, if the max level is the same as the (new) min
    // level, we have a series of alternating levels that
    // is already in order, so there's no more to do.
    //
    Boolean swap;
    if ((minLevel & 1) == baseLevel) {
        minLevel += 1;
        swap = maxLevel > minLevel;
    } else {
        swap = runCount > 1;
    }
    if (swap) {
        for (Int32 level = maxLevel - 1; level >= minLevel; --level) {
            for (Int32 i = 0; i < ld->GetLength(); i += 2) {
                if ((*levels)[(*ld)[i]] >= level) {
                    Int32 e = i + 2;
                    while (e < ld->GetLength() && (*levels)[(*ld)[e]] >= level) {
                        e += 2;
                    }
                    for (Int32 low = i, hi = e - 2; low < hi; low += 2, hi -= 2) {
                         Int32 x = (*ld)[low];
                         (*ld)[low] = (*ld)[hi];
                         (*ld)[hi] = x;
                         x = (*ld)[low+1];
                         (*ld)[low+1] = (*ld)[hi+1];
                         (*ld)[hi+1] = x;
                    }
                    i = e + 2;
                }
            }
        }
    }

    AutoPtr<ILayoutDirections> directions;
    CLayoutDirections::New(ld, (ILayoutDirections**)&directions);
    return directions;
}

Int32 AndroidBidi::RunBidi(
    /* [in] */ Int32 dir,
    /* [in] */ ArrayOf<Char32>* chs,
    /* [in] */ ArrayOf<Byte>* info,
    /* [in] */ Int32 n,
    /* [in] */ Boolean haveInfo)
{
    // Parameters are checked on java side
    // Failures from GetXXXArrayElements indicate a serious out-of-memory condition
    // that we don't bother to report, we're probably dead anyway.
    Int32 result = 0;
    if (chs != NULL && info != NULL) {
        UErrorCode status = U_ZERO_ERROR;
        UBiDi* bidi = ubidi_openSized(n, 0, &status);
        ubidi_setPara(bidi, (const UChar*)chs->GetPayload(), n, dir, NULL, &status);
        if (U_SUCCESS(status)) {
            for (Int32 i = 0; i < n; ++i) {
              (*info)[i] = ubidi_getLevelAt(bidi, i);
            }
            result = ubidi_getParaLevel(bidi);
        }
        else {
            // jniThrowException(env, "java/lang/RuntimeException", NULL);
            return 0;
        }
        ubidi_close(bidi);
    }
    return result;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos

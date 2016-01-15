#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/text/CBidiFormatter.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Text {

static String StringFromChar32(
    /* [in] */ Char32 ch)
{
    StringBuilder sb(4);
    sb.AppendChar(ch);
    return sb.ToString();
}

// private static TextDirectionHeuristic DEFAULT_TEXT_DIRECTION_HEURISTIC = FIRSTSTRONG_LTR;

/**
 * Unicode "Left-To-Right Embedding" (LRE) character.
 */
const Char32 CBidiFormatter::LRE = '\u202A';

/**
 * Unicode "Right-To-Left Embedding" (RLE) character.
 */
const Char32 CBidiFormatter::RLE = '\u202B';

/**
 * Unicode "Pop Directional Formatting" (PDF) character.
 */
const Char32 CBidiFormatter::PDF = '\u202C';

/**
 *  Unicode "Left-To-Right Mark" (LRM) character.
 */
const Char32 CBidiFormatter::LRM = '\u200E';

/*
 * Unicode "Right-To-Left Mark" (RLM) character.
 */
const Char32 CBidiFormatter::RLM = '\u200F';


const String CBidiFormatter::LRM_STRING = StringFromChar32('\u200E');//Character.toString(LRM);
const String CBidiFormatter::RLM_STRING = StringFromChar32('\u200F');//Character.toString(RLM);

const String CBidiFormatter::EMPTY_STRING("");

const Int32 CBidiFormatter::FLAG_STEREO_RESET = 2;
const Int32 CBidiFormatter::DEFAULT_FLAGS = 2;//FLAG_STEREO_RESET;

const AutoPtr<IBidiFormatter> DEFAULT_LTR_INSTANCE;// = new BidiFormatter(
        // false /* LTR context */,
        // DEFAULT_FLAGS,
        // DEFAULT_TEXT_DIRECTION_HEURISTIC);

const AutoPtr<IBidiFormatter> DEFAULT_RTL_INSTANCE;// = new BidiFormatter(
        // true /* RTL context */,
        // DEFAULT_FLAGS,
        // DEFAULT_TEXT_DIRECTION_HEURISTIC);


CAR_INTERFACE_IMPL(CBidiFormatter, Object, IBidiFormatter)

CAR_OBJECT_IMPL(CBidiFormatter)

CBidiFormatter::CBidiFormatter()
    : mIsRtlContext(FALSE)
    , mFlags(0)
{
}

ECode CBidiFormatter::constructor()
{
    return NOERROR;
}

ECode CBidiFormatter::IsRtlContext(
    /* [out] */ Boolean* ret)
{
    return NOERROR;
}

ECode CBidiFormatter::GetStereoReset(
    /* [out] */ Boolean* ret)
{
    return NOERROR;
}

ECode CBidiFormatter::MarkAfter(
    /* [in] */ const String& str,
    /* [in] */ ITextDirectionHeuristic* heuristic,
    /* [out] */ String* ret)
{
    return NOERROR;
}

ECode CBidiFormatter::MarkBefore(
    /* [in] */ const String& str,
    /* [in] */ ITextDirectionHeuristic* heuristic,
    /* [out] */ String* ret)
{
    return NOERROR;
}

ECode CBidiFormatter::IsRtl(
    /* [in] */ const String& str,
    /* [out] */ Boolean* ret)
{
    return NOERROR;
}

ECode CBidiFormatter::UnicodeWrap(
    /* [in] */ const String& str,
    /* [in] */ ITextDirectionHeuristic* heuristic,
    /* [in] */ Boolean isolate,
    /* [out] */ String* ret)
{
    return NOERROR;
}

ECode CBidiFormatter::UnicodeWrap(
    /* [in] */ const String& str,
    /* [in] */ ITextDirectionHeuristic* heuristic,
    /* [out] */ String* ret)
{
    return NOERROR;
}


ECode CBidiFormatter::UnicodeWrap(
    /* [in] */ const String& str,
    /* [in] */ Boolean isolate,
    /* [out] */ String* ret)
{
    return NOERROR;
}

ECode CBidiFormatter::UnicodeWrap(
    /* [in] */ const String& str,
    /* [out] */ String* ret)
{
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
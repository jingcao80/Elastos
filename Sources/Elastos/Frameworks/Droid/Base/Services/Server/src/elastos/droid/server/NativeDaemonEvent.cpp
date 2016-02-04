#include "elastos/droid/server/NativeDaemonEvent.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {

NativeDaemonEvent::NativeDaemonEvent(
    /* [in] */ Int32 cmdNumber,
    /* [in] */ Int32 code,
    /* [in] */ const String& message,
    /* [in] */ const String& rawEvent)
    : mCmdNumber(cmdNumber)
    , mCode(code)
    , mMessage(message)
    , mRawEvent(rawEvent)
{}

Int32 NativeDaemonEvent::GetCmdNumber()
{
    return mCmdNumber;
}

Int32 NativeDaemonEvent::GetCode()
{
    return mCode;
}

String NativeDaemonEvent::GetMessage()
{
    return mMessage;
}

String NativeDaemonEvent::GetRawEvent()
{
    return mRawEvent;
}

String NativeDaemonEvent::ToString()
{
    return mRawEvent;
}

/**
 * Test if event represents a partial response which is continued in
 * additional subsequent events.
 */
Boolean NativeDaemonEvent::IsClassContinue()
{
    return mCode >= 100 && mCode < 200;
}

/**
 * Test if event represents a command success.
 */
Boolean NativeDaemonEvent::IsClassOk()
{
    return mCode >= 200 && mCode < 300;
}

/**
 * Test if event represents a remote native daemon error.
 */
Boolean NativeDaemonEvent::IsClassServerError()
{
    return mCode >= 400 && mCode < 500;
}

/**
 * Test if event represents a command syntax or argument error.
 */
Boolean NativeDaemonEvent::IsClassClientError()
{
    return mCode >= 500 && mCode < 600;
}

/**
 * Test if event represents an unsolicited event from native daemon.
 */
Boolean NativeDaemonEvent::IsClassUnsolicited()
{
    return IsClassUnsolicited(mCode);
}

Boolean NativeDaemonEvent::IsClassUnsolicited(
    /* [in] */ Int32 code)
{
    return code >= 600 && code < 700;
}

/**
 * Verify this event matches the given code.
 *
 * @throws IllegalStateException if {@link #getCode()} doesn't match.
 */
ECode NativeDaemonEvent::CheckCode(
    /* [in] */ Int32 code)
{
    if (mCode != code) {
//        throw new IllegalStateException("Expected " + code + " but was: " + this);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

/**
 * Parse the given raw event into {@link NativeDaemonEvent} instance.
 *
 * @throws IllegalArgumentException when line doesn't match format expected
 *             from native side.
 */
ECode NativeDaemonEvent::ParseRawEvent(
    /* [in] */ const String& rawEvent,
    /* [out] */ NativeDaemonEvent** event)
{
    VALIDATE_NOT_NULL(event);
    *event = NULL;

    AutoPtr< ArrayOf<String> > parsed;
    StringUtils::Split(rawEvent, String(" "), (ArrayOf<String>**)&parsed);
    if (parsed->GetLength() < 2) {
        // throw new IllegalArgumentException("Insufficient arguments");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 skiplength = 0;

    Int32 code;

//    try {
    code = StringUtils::ParseInt32((*parsed)[0]);
    skiplength = (*parsed)[0].GetLength() + 1;
//    } catch (NumberFormatException e) {
//        throw new IllegalArgumentException("problem parsing code", e);
//    }

    Int32 cmdNumber = -1;
    if (IsClassUnsolicited(code) == FALSE) {
        if (parsed->GetLength()< 3) {
//            throw new IllegalArgumentException("Insufficient arguemnts");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
//        try {
        cmdNumber = StringUtils::ParseInt32((*parsed)[1]);
        skiplength += (*parsed)[1].GetLength() + 1;
//        } catch (NumberFormatException e) {
//            throw new IllegalArgumentException("problem parsing cmdNumber", e);
//        }
    }

    String message = rawEvent.Substring(skiplength);

    *event = new NativeDaemonEvent(cmdNumber, code, message, rawEvent);
    REFCOUNT_ADD(*event);
    return NOERROR;
}

/**
 * Filter the given {@link NativeDaemonEvent} list, returning
 * {@link #getMessage()} for any events matching the requested code.
 */
AutoPtr< ArrayOf<String> > NativeDaemonEvent::FilterMessageList(
    /* [in] */ const ArrayOf<NativeDaemonEvent*>& events,
    /* [in] */ Int32 matchCode)
{
    List<String> resultList;
    for (Int32 i = 0; i < events.GetLength(); i++) {
        if (events[i]->GetCode() == matchCode) {
            resultList.PushBack(events[i]->GetMessage());
        }
    }
    AutoPtr< ArrayOf<String> > resultArray = ArrayOf<String>::Alloc(resultList.GetSize());
    List<String>::Iterator iter;
    Int32 i;
    for (iter = resultList.Begin(), i = 0; iter != resultList.End(); ++iter, i++) {
        (*resultArray)[i] = *iter;
    }
    return resultArray;
}

/**
 * Find the Nth field of the event.
 *
 * This ignores and code or cmdNum, the first return value is given for N=0.
 * Also understands "\"quoted\" multiword responses" and tries them as a single field
 */
String NativeDaemonEvent::GetField(
    /* [in] */ Int32 n)
{
    if (mParsed == NULL) {
        mParsed = UnescapeArgs(mRawEvent);
    }
    n += 2; // skip code and command#
    if (n > mParsed->GetLength()) return String(NULL);
    return (*mParsed)[n];
}

AutoPtr< ArrayOf<String> > NativeDaemonEvent::UnescapeArgs(
    /* [in] */ const String& rawEvent)
{
    Boolean DEBUG_ROUTINE = FALSE;
    String LOGTAG("unescapeArgs");
    List<String> parsed;
    Int32 length = rawEvent.GetLength();
    Int32 current = 0;
    Int32 wordEnd = -1;
    Boolean quoted = FALSE;

//    if (DEBUG_ROUTINE) Slog.e(LOGTAG, "parsing '" + rawEvent + "'");
    if (rawEvent.GetChar(current) == '\"') {
        quoted = TRUE;
        current++;
    }
    while (current < length) {
        // find the end of the word
        char terminator = quoted ? '\"' : ' ';
        wordEnd = current;
        while (wordEnd < length && rawEvent.GetChar(wordEnd) != terminator) {
            if (rawEvent.GetChar(wordEnd) == '\\') {
                // skip the escaped char
                ++wordEnd;
            }
            ++wordEnd;
        }
        if (wordEnd > length) wordEnd = length;
        String word = rawEvent.Substring(current, wordEnd);
        current += word.GetLength();
        if (!quoted) {
            word = word.Trim();
        }
        else {
            current++;  // skip the trailing quote
        }
        // unescape stuff within the word
        String w;
        StringUtils::ReplaceAll(word, String("\\\\"), String("\\"), &w);
        StringUtils::ReplaceAll(w, String("\\\""), String("\""), &word);

//        if (DEBUG_ROUTINE) Slog.e(LOGTAG, "found '" + word + "'");
        parsed.PushBack(word);

        // find the beginning of the next word - either of these options
        Int32 nextSpace = rawEvent.IndexOf(' ', current);
        Int32 nextQuote = rawEvent.IndexOf(" \"", current);
        if (DEBUG_ROUTINE) {
//            Slog.e(LOGTAG, "nextSpace=" + nextSpace + ", nextQuote=" + nextQuote);
        }
        if (nextQuote > -1 && nextQuote <= nextSpace) {
            quoted = TRUE;
            current = nextQuote + 2;
        }
        else {
            quoted = FALSE;
            if (nextSpace > -1) {
                current = nextSpace + 1;
            }
        } // else we just start the next word after the current and read til the end
        if (DEBUG_ROUTINE) {
//            Slog.e(LOGTAG, "next loop - current=" + current +
//                    ", length=" + length + ", quoted=" + quoted);
        }
    }
    AutoPtr< ArrayOf<String> > parsedArray = ArrayOf<String>::Alloc(parsed.GetSize());
    List<String>::Iterator it;
    Int32 i;
    for (it = parsed.Begin(), i = 0; it != parsed.End(); ++it, i++) {
        (*parsedArray)[i] = *it;
    }
    return parsedArray;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos

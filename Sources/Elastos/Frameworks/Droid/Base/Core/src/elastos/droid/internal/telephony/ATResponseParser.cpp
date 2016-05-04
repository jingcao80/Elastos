/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony;

/**
 * {@hide}
 */
public class ATResponseParser
{
    /*************************** Instance Variables **************************/

    private String mLine;
    private Int32 mNext = 0;
    private Int32 mTokStart, mTokEnd;

    /***************************** Class Methods *****************************/

    public
    ATResponseParser (String line)
    {
        mLine = line;
    }

    public Boolean
    NextBoolean()
    {
        // "\s*(\d)(,|$)"
        // \d is '0' or '1'

        NextTok();

        If (mTokEnd - mTokStart > 1) {
            throw new ATParseEx();
        }
        Char32 c = mLine->CharAt(mTokStart);

        If (c == '0') return FALSE;
        If (c ==  '1') return TRUE;
        throw new ATParseEx();
    }


    /** positive Int32 only */
    public Int32
    NextInt()
    {
        // "\s*(\d+)(,|$)"
        Int32 ret = 0;

        NextTok();

        For (Int32 i = mTokStart ; i < mTokEnd ; i++) {
            Char32 c = mLine->CharAt(i);

            // Yes, ASCII decimal digits only
            If (c < '0' || c > '9') {
                throw new ATParseEx();
            }

            ret *= 10;
            ret += c - '0';
        }

        return ret;
    }

    public String
    NextString()
    {
        NextTok();

        return mLine->Substring(mTokStart, mTokEnd);
    }

    public Boolean
    HasMore()
    {
        return mNext < mLine->Length();
    }

    private void
    NextTok()
    {
        Int32 len = mLine->Length();

        If (mNext == 0) {
            SkipPrefix();
        }

        If (mNext >= len) {
            throw new ATParseEx();
        }

        try {
            // \s*("([^"]*)"|(.*)\s*)(,|$)

            Char32 c = mLine->CharAt(mNext++);
            Boolean hasQuote = FALSE;

            c = SkipWhiteSpace(c);

            If (c == '"') {
                If (mNext >= len) {
                    throw new ATParseEx();
                }
                c = mLine->CharAt(mNext++);
                mTokStart = mNext - 1;
                While (c != '"' && mNext < len) {
                    c = mLine->CharAt(mNext++);
                }
                If (c != '"') {
                    throw new ATParseEx();
                }
                mTokEnd = mNext - 1;
                If (mNext < len && mLine->CharAt(mNext++) != ',') {
                    throw new ATParseEx();
                }
            } else {
                mTokStart = mNext - 1;
                mTokEnd = mTokStart;
                While (c != ',') {
                    If (!Character->IsWhitespace(c)) {
                        mTokEnd = mNext;
                    }
                    If (mNext == len) {
                        break;
                    }
                    c = mLine->CharAt(mNext++);
                }
            }
        } Catch (StringIndexOutOfBoundsException ex) {
            throw new ATParseEx();
        }
    }


    /** Throws ATParseEx if whitespace extends to the end of string */
    private Char32
    SkipWhiteSpace (Char32 c)
    {
        Int32 len;
        len = mLine->Length();
        While (mNext < len && Character->IsWhitespace(c)) {
            c = mLine->CharAt(mNext++);
        }

        If (Character->IsWhitespace(c)) {
            throw new ATParseEx();
        }
        return c;
    }


    private void
    SkipPrefix()
    {
        // consume "^[^:]:"

        mNext = 0;
        Int32 s = mLine->Length();
        While (mNext < s){
            Char32 c = mLine->CharAt(mNext++);

            If (c == ':') {
                return;
            }
        }

        throw new ATParseEx("missing prefix");
    }

}

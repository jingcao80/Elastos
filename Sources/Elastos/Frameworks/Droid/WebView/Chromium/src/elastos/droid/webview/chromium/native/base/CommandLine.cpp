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

#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/webkit/webview/chromium/native/base/CommandLine.h"
#include "elastos/droid/webkit/webview/chromium/native/base/api/CommandLine_dec.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/core/Character.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/Arrays.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Core::Character;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IReader;
using Elastos::IO::IInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CInputStreamReader;
using Elastos::Utility::Concurrent::Atomic::CAtomicReference;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

//===============================================================
//              JavaCommandLine
//===============================================================

JavaCommandLine::JavaCommandLine(
    /* [in] */ ArrayOf<String>* args)
    : mArgsBegin(1)
{
    CHashMap::New((IHashMap**)&mSwitches);
    CArrayList::New((IArrayList**)&mArgs);

    if (args == NULL || args->GetLength() == 0 || (*args)[0].IsNull()) {
        //mArgs.add("");
        AutoPtr<ICharSequence> arg;
        CString::New(String(""), (ICharSequence**)&arg);
        mArgs->Add(TO_IINTERFACE(arg));
    }
    else {
        //mArgs.add(args[0]);
        AutoPtr<ICharSequence> arg;
        CString::New((*args)[0], (ICharSequence**)&arg);
        mArgs->Add(TO_IINTERFACE(arg));
        AppendSwitchesInternal(args, 1);
    }
    // Invariant: we always have the argv[0] program name element.
    //assert mArgs.size() > 0;
}

/**
 * Returns the switches and arguments passed into the program, with switches and their
 * values coming before all of the arguments.
 */
AutoPtr< ArrayOf<String> > JavaCommandLine::GetCommandLineArguments()
{
    //return mArgs.toArray(new String[mArgs.size()]);
    Int32 size;
    mArgs->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > inArray = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr<ArrayOf<IInterface*> > outArray;
    mArgs->ToArray(inArray, (ArrayOf<IInterface*>**)&outArray);
    Int32 outSize = outArray->GetLength();
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(outSize);
    for(Int32 i = 0; i < outSize; ++i) {
        AutoPtr<IInterface> obj = (*outArray)[i];
        ICharSequence* cs = ICharSequence::Probe(obj);
        String str;
        cs->ToString(&str);
        result->Set(i, str);
    }
    return result;
}

//@Override
Boolean JavaCommandLine::HasSwitch(
    /* [in] */ const String& switchString)
{
    Boolean result;
    AutoPtr<ICharSequence> cs;
    CString::New(switchString, (ICharSequence**)&cs);

    mSwitches->ContainsKey(TO_IINTERFACE(cs), &result);
    return result;
}

//@Override
String JavaCommandLine::GetSwitchValue(
    /* [in] */ const String& switchString)
{
    // This is slightly round about, but needed for consistency with the NativeCommandLine
    // version which does not distinguish empty values from key not present.
    //String value = mSwitches.get(switchString);
    AutoPtr<ICharSequence> cs;
    CString::New(switchString, (ICharSequence**)&cs);

    AutoPtr<IInterface> obj;
    mSwitches->Get(TO_IINTERFACE(cs), (IInterface**)&obj);
    String result;
    ICharSequence* csq = ICharSequence::Probe(obj);
    csq->ToString(&result);

    return result;
}

//@Override
void JavaCommandLine::AppendSwitch(
    /* [in] */ const String& switchString)
{
    AppendSwitchWithValue(switchString, String(NULL));
}

/**
 * Appends a switch to the current list.
 * @param switchString the switch to add.  It should NOT start with '--' !
 * @param value the value for this switch.
 */
//@Override
void JavaCommandLine::AppendSwitchWithValue(
    /* [in] */ const String& switchString,
    /* [in] */ const String& value)
{
    AutoPtr<ICharSequence> key;
    CString::New(switchString, (ICharSequence**)&key);
    AutoPtr<ICharSequence> csvalue;
    CString::New(value, (ICharSequence**)&csvalue);

    mSwitches->Put(TO_IINTERFACE(key), TO_IINTERFACE(csvalue));

    // Append the switch and update the switches/arguments divider mArgsBegin.
    String combinedSwitchString = SWITCH_PREFIX + switchString;
    if (!value.IsNullOrEmpty())
        combinedSwitchString += SWITCH_VALUE_SEPARATOR + value;

    //mArgs.add(mArgsBegin++, combinedSwitchString);
    AutoPtr<ICharSequence> cs;
    CString::New(combinedSwitchString, (ICharSequence**)&cs);
    mArgs->Add(mArgsBegin++, TO_IINTERFACE(cs));
}

//@Override
void JavaCommandLine::AppendSwitchesAndArguments(
    /* [in] */ ArrayOf<String>* array)
{
    AppendSwitchesInternal(array, 0);
}

// Add the specified arguments, but skipping the first |skipCount| elements.
void JavaCommandLine::AppendSwitchesInternal(
    /* [in] */ ArrayOf<String>* array,
    /* [in] */ Int32 skipCount)
{
    Boolean parseSwitches = TRUE;
    Int32 length = array->GetLength();
    for (Int32 i = 0; i < length; ++i) {
        String arg = (*array)[i];
        if (skipCount > 0) {
            --skipCount;
            continue;
        }

        if (arg.Equals(SWITCH_TERMINATOR)) {
            parseSwitches = FALSE;
        }

        if (parseSwitches && arg.StartWith(SWITCH_PREFIX)) {
            //String[] parts = arg.Split(SWITCH_VALUE_SEPARATOR, 2);
            AutoPtr<ArrayOf<String> > parts;
            StringUtils::Split(arg, SWITCH_VALUE_SEPARATOR, 2, (ArrayOf<String>**)&parts);

            String value = parts->GetLength() > 1 ? (*parts)[1] : String(NULL);
            AppendSwitchWithValue((*parts)[0].Substring(SWITCH_PREFIX.GetLength()), value);
        }
        else {
            //mArgs.add(arg);
            AutoPtr<ICharSequence> cs;
            CString::New(arg, (ICharSequence**)&cs);
            mArgs->Add(TO_IINTERFACE(cs));
        }
    }
}

//===============================================================
//                    NativeCommandLine
//===============================================================

//@Override
Boolean NativeCommandLine::HasSwitch(
    /* [in] */ const String& switchString)
{
    return NativeHasSwitch(switchString);
}

//@Override
String NativeCommandLine::GetSwitchValue(
    /* [in] */ const String& switchString)
{
    return NativeGetSwitchValue(switchString);
}

//@Override
void NativeCommandLine::AppendSwitch(
    /* [in] */ const String& switchString)
{
    NativeAppendSwitch(switchString);
}

//@Override
void NativeCommandLine::AppendSwitchWithValue(
    /* [in] */ const String& switchString,
    /* [in] */ const String& value)
{
    NativeAppendSwitchWithValue(switchString, value);
}

//@Override
void NativeCommandLine::AppendSwitchesAndArguments(
    /* [in] */ ArrayOf<String>* array)
{
    NativeAppendSwitchesAndArguments(array);
}

//@Override
Boolean NativeCommandLine::IsNativeImplementation()
{
    return TRUE;
}

//===============================================================
//                         CommandLine
//===============================================================

const String CommandLine::TAG("CommandLine");
const String CommandLine::SWITCH_PREFIX("--");
const String CommandLine::SWITCH_TERMINATOR(SWITCH_PREFIX);
const String CommandLine::SWITCH_VALUE_SEPARATOR("=");
AutoPtr<IAtomicReference>  CommandLine::sCommandLine;

Int32 CommandLine::CommandLineInit()
{
    CAtomicReference::New((IAtomicReference**)&CommandLine::sCommandLine);
    return 0;
}

static Int32 initValue = CommandLine::CommandLineInit();


CommandLine::CommandLine()
{
}

/**
 * Return the value associated with the given switch, or {@code defaultValue} if the switch
 * was not specified.
 * @param switchString The switch key to lookup. It should NOT start with '--' !
 * @param defaultValue The default value to return if the switch isn't set.
 * @return Switch value, or {@code defaultValue} if the switch is not set or set to empty.
 */
String CommandLine::GetSwitchValue(
    /* [in] */ const String& switchString,
    /* [in] */ const String& defaultValue)
{
    String value = GetSwitchValue(switchString);
    return TextUtils::IsEmpty(value) ? defaultValue : value;
}

/**
 * Determine if the command line is bound to the native (JNI) implementation.
 * @return true if the underlying implementation is delegating to the native command line.
 */
Boolean CommandLine::IsNativeImplementation()
{
    return FALSE;
}

/**
 * @returns true if the command line has already been initialized.
 */
Boolean CommandLine::IsInitialized()
{
    AutoPtr<IInterface> obj;
    sCommandLine->Get((IInterface**)&obj);
    return obj != NULL;
}

// Equivalent to CommandLine::ForCurrentProcess in C++.
AutoPtr<CommandLine> CommandLine::GetInstance()
{
    AutoPtr<IInterface> obj;
    sCommandLine->Get((IInterface**)&obj);
    //CommandLine commandLine = sCommandLine.get();
    assert(obj != NULL);
    AutoPtr<CommandLine> commandLine = (CommandLine*)(IObject::Probe(obj));
    return commandLine;
}

/**
 * Initialize the singleton instance, must be called exactly once (either directly or
 * via one of the convenience wrappers below) before using the static singleton instance.
 * @param args command line flags in 'argv' format: args[0] is the program name.
 */
void CommandLine::Init(
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<CommandLine> instance = new JavaCommandLine(args);
    SetInstance(instance);
}

/**
 * Initialize the command line from the command-line file.
 *
 * @param file The fully qualified command line file.
 */
void CommandLine::InitFromFile(
    /* [in] */ const String& file)
{
    // Arbitrary clamp of 8k on the amount of file we read in.
    AutoPtr< ArrayOf<Char32> > buffer = ReadUtf8FileFully(file, 8 * 1024);
    Init(buffer == NULL ? NULL : TokenizeQuotedAruments(buffer));
}

/**
 * Resets both the java proxy and the native command lines. This allows the entire
 * command line initialization to be re-run including the call to onJniLoaded.
 */
void CommandLine::Reset()
{
    SetInstance(NULL);
}

/**
 * Public for testing (TODO: why are the tests in a different package?)
 * Parse command line flags from a flat buffer, supporting double-quote enclosed strings
 * containing whitespace. argv elements are derived by splitting the buffer on whitepace;
 * double quote characters may enclose tokens containing whitespace; a double-quote literal
 * may be escaped with back-slash. (Otherwise backslash is taken as a literal).
 * @param buffer A command line in command line file format as described above.
 * @return the tokenized arguments, suitable for passing to init().
 */
AutoPtr< ArrayOf<String> > CommandLine::TokenizeQuotedAruments(
    /* [in] */ ArrayOf<Char32>* buffer)
{
    //ArrayList<String> args = new ArrayList<String>();
    AutoPtr<IArrayList> args;
    CArrayList::New((IArrayList**)&args);
    AutoPtr<StringBuilder> arg;
    Int8 noQuote = '\0';
    Int8 singleQuote = '\'';
    Int8 doubleQuote = '"';
    Int8 currentQuote = noQuote;
    Int32 bsize = buffer->GetLength();
    for (Int32 i = 0; i < bsize; ++i) {
        Int8 c = (*buffer)[i];
        // Detect start or end of quote block.
        if ((currentQuote == noQuote && (c == singleQuote || c == doubleQuote)) ||
            c == currentQuote) {
            Int32 length;
            if (arg != NULL && (arg->GetLength(&length), length) > 0 && arg->GetCharAt(length - 1) == '\\') {
                // Last char was a backslash; pop it, and treat c as a literal.
                arg->SetCharAt(length - 1, c);
            } else {
                currentQuote = currentQuote == noQuote ? c : noQuote;
            }
        } else if (currentQuote == noQuote && Character::IsWhitespace(c)) {
            if (arg != NULL) {
                String argString = arg->ToString();

                AutoPtr<ICharSequence> cs;
                CString::New(argString, (ICharSequence**)&cs);
                args->Add(TO_IINTERFACE(cs));
                arg = NULL;
            }
        } else {
            if (arg == NULL) arg = new StringBuilder();
            arg->Append(c);
        }
    }
    if (arg != NULL) {
        if (currentQuote != noQuote) {
            Logger::W(TAG, "Unterminated quoted string: ");// + arg);
        }
        //args.add(arg.toString());
        String argString = arg->ToString();
        AutoPtr<ICharSequence> cs;
        CString::New(argString, (ICharSequence**)&cs);
        args->Add(TO_IINTERFACE(cs));
    }
    //return args.toArray(new String[args.size()]);
    Int32 size;
    args->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > inArray = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr<ArrayOf<IInterface*> > outArray;
    args->ToArray(inArray, (ArrayOf<IInterface*>**)&outArray);
    Int32 outSize = outArray->GetLength();
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(outSize);
    for(Int32 i = 0; i < outSize; ++i) {
        AutoPtr<IInterface> obj = (*outArray)[i];
        ICharSequence* cs = ICharSequence::Probe(obj);
        String str;
        cs->ToString(&str);
        result->Set(i, str);
    }
    return result;
}

void CommandLine::EnableNativeProxy()
{
    // Make a best-effort to ensure we make a clean (atomic) switch over from the old to
    // the new command line implementation. If another thread is modifying the command line
    // when this happens, all bets are off. (As per the native CommandLine).
    sCommandLine->Set(TO_IINTERFACE(new NativeCommandLine()));
}

AutoPtr< ArrayOf<String> > CommandLine::GetJavaSwitchesOrNull()
{
    //CommandLine commandLine = sCommandLine.get();
    AutoPtr<IInterface> obj;
    sCommandLine->Get((IInterface**)&obj);
    if (obj != NULL) {
        CommandLine* commandLine = (CommandLine*)(IObject::Probe(obj));
        assert(!commandLine->IsNativeImplementation());
        return ((JavaCommandLine*)commandLine)->GetCommandLineArguments();
    }
    return NULL;
}

void CommandLine::SetInstance(
    /* [in] */ CommandLine* commandLine)
{
    AutoPtr<IInterface> obj;
    sCommandLine->GetAndSet(TO_IINTERFACE(commandLine), (IInterface**)&obj);
    //CommandLine oldCommandLine = sCommandLine.getAndSet(commandLine);
    CommandLine* oldCommandLine = (CommandLine*)(IObject::Probe(obj));
    if (oldCommandLine != NULL && oldCommandLine->IsNativeImplementation()) {
        NativeReset();
    }
}

/**
 * @param fileName the file to read in.
 * @param sizeLimit cap on the file size.
 * @return Array of chars read from the file, or null if the file cannot be read
 *         or if its length exceeds |sizeLimit|.
 */
AutoPtr< ArrayOf<Char32> > CommandLine::ReadUtf8FileFully(
    /* [in] */ const String& fileName,
    /* [in] */ Int32 sizeLimit)
{
    AutoPtr<IReader> reader;
    AutoPtr<IFile> f;
    CFile::New(fileName, (IFile**)&f);
    Int64 fileLength;
    f->GetLength(&fileLength);

    if (fileLength == 0) {
        return NULL;
    }

    if (fileLength > sizeLimit) {
        Logger::W(TAG, "File %s length %lld exceeds limit %d", fileName.string(), fileLength, sizeLimit);
        return NULL;
    }

    //try {
        //char[] buffer = new char[(int) fileLength];
        AutoPtr<ArrayOf<Char32> > buffer = ArrayOf<Char32>::Alloc((Int32)fileLength);
        AutoPtr<IFileInputStream> fis;
        CFileInputStream::New(f, (IFileInputStream**)&fis);
        //reader = new InputStreamReader(new FileInputStream(f), "UTF-8");
        CInputStreamReader::New(IInputStream::Probe(fis), String("UTF-8"), (IReader**)&reader);
        Int32 charsRead;
        ECode ec = reader->Read(buffer, &charsRead);
        // Debug check that we've exhausted the input stream (will fail e.g. if the
        // file grew after we inspected its length).
        //assert(!(reader->IsReady());
        if (reader != NULL) ICloseable::Probe(reader)->Close();
        if (FAILED(ec))
            return NULL;
        AutoPtr<ArrayOf<Char32> > result;
        return charsRead < buffer->GetLength() ? (Arrays::CopyOfRange(buffer, 0, charsRead, (ArrayOf<Char32>**)&result), result) : buffer;
    //} catch (FileNotFoundException e) {
    //    return null;
    //} catch (IOException e) {
    //    return null;
    //} finally {
        //try {
        //} catch (IOException e) {
        //    Log.e(TAG, "Unable to close file reader.", e);
        //}
    //}
}

void CommandLine::NativeReset()
{
    Elastos_CommandLine_nativeReset();
}

Boolean CommandLine::NativeHasSwitch(
    /* [in] */ const String& switchString)
{
    return Elastos_CommandLine_nativeHasSwitch(switchString);
}

String CommandLine::NativeGetSwitchValue(
    /* [in] */ const String& switchString)
{
    return Elastos_CommandLine_nativeGetSwitchValue(switchString);
}

void CommandLine::NativeAppendSwitch(
    /* [in] */ const String& switchString)
{
    Elastos_CommandLine_nativeAppendSwitch(switchString);
}

void CommandLine::NativeAppendSwitchWithValue(
    /* [in] */ const String& switchString,
    /* [in] */ const String& value)
{
    Elastos_CommandLine_nativeAppendSwitchWithValue(switchString, value);
}

void CommandLine::NativeAppendSwitchesAndArguments(
    /* [in] */ ArrayOf<String>* array)
{
    Elastos_CommandLine_nativeAppendSwitchesAndArguments(array);
}

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

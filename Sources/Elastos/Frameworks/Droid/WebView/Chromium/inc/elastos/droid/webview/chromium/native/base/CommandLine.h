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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_COMMANDLINE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_COMMANDLINE_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
// import java.io.FileNotFoundException;
// import java.io.IOException;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::IReader;
using Elastos::Utility::Concurrent::Atomic::IAtomicReference;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * Java mirror of base/command_line.h.
 * Android applications don't have command line arguments. Instead, they're "simulated" by reading a
 * file at a specific location early during startup. Applications each define their own files, e.g.,
 * ContentShellActivity.COMMAND_LINE_FILE or ChromeShellApplication.COMMAND_LINE_FILE.
**/
class CommandLine
    : public Object
{
    friend class JavaCommandLine;
    friend class NativeCommandLine;
public:
    //CAR_INTERFACE_DECL()

    // Public abstract interface, implemented in derived classes.
    // All these methods reflect their native-side counterparts.
    /**
     *  Returns true if this command line contains the given switch.
     *  (Switch names ARE case-sensitive).
     */
    virtual CARAPI_(Boolean) HasSwitch(
        /* [in] */ const String& switchString) = 0;

    /**
     * Return the value associated with the given switch, or null.
     * @param switchString The switch key to lookup. It should NOT start with '--' !
     * @return switch value, or null if the switch is not set or set to empty.
     */
    virtual CARAPI_(String) GetSwitchValue(
        /* [in] */ const String& switchString) = 0;

    /**
     * Return the value associated with the given switch, or {@code defaultValue} if the switch
     * was not specified.
     * @param switchString The switch key to lookup. It should NOT start with '--' !
     * @param defaultValue The default value to return if the switch isn't set.
     * @return Switch value, or {@code defaultValue} if the switch is not set or set to empty.
     */
    virtual CARAPI_(String) GetSwitchValue(
        /* [in] */ const String& switchString,
        /* [in] */ const String& defaultValue);

    /**
     * Append a switch to the command line.  There is no guarantee
     * this action happens before the switch is needed.
     * @param switchString the switch to add.  It should NOT start with '--' !
     */
    virtual CARAPI_(void) AppendSwitch(
        /* [in] */ const String& switchString) = 0;

    /**
     * Append a switch and value to the command line.  There is no
     * guarantee this action happens before the switch is needed.
     * @param switchString the switch to add.  It should NOT start with '--' !
     * @param value the value for this switch.
     * For example, --foo=bar becomes 'foo', 'bar'.
     */
    virtual CARAPI_(void) AppendSwitchWithValue(
        /* [in] */ const String& switchString,
        /* [in] */ const String& value) = 0;

    /**
     * Append switch/value items in "command line" format (excluding argv[0] program name).
     * E.g. { '--gofast', '--username=fred' }
     * @param array an array of switch or switch/value items in command line format.
     *   Unlike the other append routines, these switches SHOULD start with '--' .
     *   Unlike init(), this does not include the program name in array[0].
     */
    virtual CARAPI_(void) AppendSwitchesAndArguments(
        /* [in] */ ArrayOf<String>* array) = 0;

    /**
     * Determine if the command line is bound to the native (JNI) implementation.
     * @return true if the underlying implementation is delegating to the native command line.
     */
    virtual CARAPI_(Boolean) IsNativeImplementation();

    /**
     * @returns true if the command line has already been initialized.
     */
    static CARAPI_(Boolean) IsInitialized();

    // Equivalent to CommandLine::ForCurrentProcess in C++.
    static CARAPI_(AutoPtr<CommandLine>) GetInstance();

    /**
     * Initialize the singleton instance, must be called exactly once (either directly or
     * via one of the convenience wrappers below) before using the static singleton instance.
     * @param args command line flags in 'argv' format: args[0] is the program name.
     */
    static CARAPI_(void) Init(
        /* [in] */ ArrayOf<String>* args);

    /**
     * Initialize the command line from the command-line file.
     *
     * @param file The fully qualified command line file.
     */
    static CARAPI_(void) InitFromFile(
        /* [in] */ const String& file);

    /**
     * Resets both the java proxy and the native command lines. This allows the entire
     * command line initialization to be re-run including the call to onJniLoaded.
     */
    static CARAPI_(void) Reset();

    /**
     * Public for testing (TODO: why are the tests in a different package?)
     * Parse command line flags from a flat buffer, supporting double-quote enclosed strings
     * containing whitespace. argv elements are derived by splitting the buffer on whitepace;
     * double quote characters may enclose tokens containing whitespace; a double-quote literal
     * may be escaped with back-slash. (Otherwise backslash is taken as a literal).
     * @param buffer A command line in command line file format as described above.
     * @return the tokenized arguments, suitable for passing to init().
     */
    static CARAPI_(AutoPtr< ArrayOf<String> >) TokenizeQuotedAruments(
        /* [in] */ ArrayOf<Char32>* buffer);

    static CARAPI_(void) EnableNativeProxy();

    static CARAPI_(AutoPtr< ArrayOf<String> >) GetJavaSwitchesOrNull();

    static CARAPI_(Int32) CommandLineInit();
private:
    CommandLine();

    static CARAPI_(void) SetInstance(
        /* [in] */ CommandLine* commandLine);

    /**
     * @param fileName the file to read in.
     * @param sizeLimit cap on the file size.
     * @return Array of chars read from the file, or null if the file cannot be read
     *         or if its length exceeds |sizeLimit|.
     */
    static CARAPI_(AutoPtr< ArrayOf<Char32> >) ReadUtf8FileFully(
        /* [in] */ const String& fileName,
        /* [in] */ Int32 sizeLimit);

    static CARAPI_(void) NativeReset();

    static CARAPI_(Boolean) NativeHasSwitch(
        /* [in] */ const String& switchString);

    static CARAPI_(String) NativeGetSwitchValue(
        /* [in] */ const String& switchString);

    static CARAPI_(void) NativeAppendSwitch(
        /* [in] */ const String& switchString);

    static CARAPI_(void) NativeAppendSwitchWithValue(
        /* [in] */ const String& switchString,
        /* [in] */ const String& value);

    static CARAPI_(void) NativeAppendSwitchesAndArguments(
        /* [in] */ ArrayOf<String>* array);

private:
    static AutoPtr<IAtomicReference> sCommandLine;
    //     new AtomicReference<CommandLine>(); <CommandLine>

    static const String TAG;
    static const String SWITCH_PREFIX;
    static const String SWITCH_TERMINATOR;
    static const String SWITCH_VALUE_SEPARATOR;
};

class JavaCommandLine : public CommandLine
{
    friend class CommandLine;
public:
    JavaCommandLine(
        /* [in] */ ArrayOf<String>* args);

    //@Override
    CARAPI_(Boolean) HasSwitch(
        /* [in] */ const String& switchString);

    //@Override
    CARAPI_(String) GetSwitchValue(
        /* [in] */ const String& switchString);

    //@Override
    CARAPI_(void) AppendSwitch(
        /* [in] */ const String& switchString);

    /**
     * Appends a switch to the current list.
     * @param switchString the switch to add.  It should NOT start with '--' !
     * @param value the value for this switch.
     */
    //@Override
    CARAPI_(void) AppendSwitchWithValue(
        /* [in] */ const String& switchString,
        /* [in] */ const String& value);

    //@Override
    CARAPI_(void) AppendSwitchesAndArguments(
        /* [in] */ ArrayOf<String>* array);

private:
    /**
     * Returns the switches and arguments passed into the program, with switches and their
     * values coming before all of the arguments.
     */
    CARAPI_(AutoPtr< ArrayOf<String> >) GetCommandLineArguments();

    // Add the specified arguments, but skipping the first |skipCount| elements.
    CARAPI_(void) AppendSwitchesInternal(
        /* [in] */ ArrayOf<String>* array,
        /* [in] */ Int32 skipCount);

private:
    AutoPtr<IHashMap> mSwitches;// <String, String>
    AutoPtr<IArrayList> mArgs;// <String>

    // The arguments begin at index 1, since index 0 contains the executable name.
    Int32 mArgsBegin;
};

class NativeCommandLine : public CommandLine
{
public:
    //@Override
    CARAPI_(Boolean) HasSwitch(
        /* [in] */ const String& switchString);

    //@Override
    CARAPI_(String) GetSwitchValue(
        /* [in] */ const String& switchString);

    //@Override
    CARAPI_(void) AppendSwitch(
        /* [in] */ const String& switchString);

    //@Override
    CARAPI_(void) AppendSwitchWithValue(
        /* [in] */ const String& switchString,
        /* [in] */ const String& value);

    //@Override
    CARAPI_(void) AppendSwitchesAndArguments(
        /* [in] */ ArrayOf<String>* array);

    //@Override
    CARAPI_(Boolean) IsNativeImplementation();
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_COMMANDLINE_H__

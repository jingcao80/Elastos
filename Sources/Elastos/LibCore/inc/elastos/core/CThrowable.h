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

#ifndef __ELASTOS_CORE_CTHROWABLE_H__
#define __ELASTOS_CORE_CTHROWABLE_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Core_CThrowable.h"
#include "Object.h"

using Elastos::IO::IPrintStream;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;

namespace Elastos {
namespace Core {

CarClass(CThrowable)
    , public Object
    , public IThrowable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CThrowable();

    ~CThrowable();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& detailMessage);

    CARAPI constructor(
        /* [in] */ const String& detailMessage,
        /* [in] */ IThrowable * cause);

    CARAPI constructor(
        /* [in] */ IThrowable * cause);

    CARAPI constructor(
        /* [in] */ const String& detailMessage,
        /* [in] */ IThrowable * cause,
        /* [in] */ Boolean enableSuppression,
        /* [in] */ Boolean writableStackTrace);

    /**
     * Records the stack trace from the point where this method has been called
     * to this {@code Throwable}. This method is invoked by the {@code Throwable} constructors.
     *
     * <p>This method is public so that code (such as an RPC system) which catches
     * a {@code Throwable} and then re-throws it can replace the construction-time stack trace
     * with a stack trace from the location where the exception was re-thrown, by <i>calling</i>
     * {@code fillInStackTrace}.
     *
     * <p>This method is non-final so that non-Java language implementations can disable VM stack
     * traces for their language. Filling in the stack trace is relatively expensive.
     * <i>Overriding</i> this method in the root of a language's exception hierarchy allows the
     * language to avoid paying for something it doesn't need.
     *
     * @return this {@code Throwable} instance.
     */
    CARAPI FillInStackTrace();

    /**
     * Returns the extra information message which was provided when this
     * {@code Throwable} was created. Returns {@code null} if no message was
     * provided at creation time.
     *
     * @return this {@code Throwable}'s detail message.
     */
    CARAPI GetMessage(
        /* [out] */ String* str);

    /**
     * Returns the extra information message which was provided when this
     * {@code Throwable} was created. Returns {@code null} if no message was
     * provided at creation time. Subclasses may override this method to return
     * localized text for the message. Android returns the regular detail message.
     *
     * @return this {@code Throwable}'s localized detail message.
     */
    CARAPI GetLocalizedMessage(
        /* [out] */ String* str);

    /**
     * Returns the array of stack trace elements of this {@code Throwable}. Each
     * {@code StackTraceElement} represents an entry in the call stack. The
     * element at position 0 is the top of the stack, that is, the stack frame
     * where this {@code Throwable} is thrown.
     *
     * @return a copy of the array of {@code StackTraceElement}s representing
     *         the call stack. Changes in the array obtained from this call will
     *         not change the call stack stored in this {@code Throwable}.
     * @see #printStackTrace()
     */
    CARAPI GetStackTrace(
        /* [out, callee] */ ArrayOf<IStackTraceElement*>** outarr);

    /**
     * Sets the array of stack trace elements. Each {@code StackTraceElement}
     * represents an entry in the call stack. A copy of the specified array is
     * stored in this {@code Throwable}. will be returned by {@code
     * getStackTrace()} and printed by {@code printStackTrace()}.
     *
     * @param trace
     *            the new array of {@code StackTraceElement}s. A copy of the
     *            array is stored in this {@code Throwable}, so subsequent
     *            changes to {@code trace} will not change the call stack stored
     *            in this {@code Throwable}.
     * @throws NullPointerException
     *             if any element in {@code trace} is {@code null}.
     * @see #printStackTrace()
     */
    CARAPI SetStackTrace(
        /* [in] */ ArrayOf<IStackTraceElement*>* trace);

    /**
     * Writes a printable representation of this {@code Throwable}'s stack trace
     * to the {@code System.err} stream.
     *
     */
    CARAPI PrintStackTrace();

    /**
     * Writes a printable representation of this {@code Throwable}'s stack trace
     * to the specified print stream. If the {@code Throwable} contains a
     * {@link #getCause() cause}, the method will be invoked recursively for
     * the nested {@code Throwable}.
     *
     * @param err
     *            the stream to write the stack trace on.
     */
    CARAPI PrintStackTrace(
        /* [in] */ IPrintStream* err);

    /**
     * Writes a printable representation of this {@code Throwable}'s stack trace
     * to the specified print writer. If the {@code Throwable} contains a
     * {@link #getCause() cause}, the method will be invoked recursively for the
     * nested {@code Throwable}.
     *
     * @param err
     *            the writer to write the stack trace on.
     */
    CARAPI PrintStackTrace(
        /* [in] */ IPrintWriter* err);

    CARAPI PrintStackTrace(
        /* [in] */ IAppendable* err,
        /* [in] */ const String& indent,
        /* [in] */ ArrayOf<IStackTraceElement*>* parentStack);

    /**
     * Initializes the cause of this {@code Throwable}. The cause can only be
     * initialized once.
     *
     * @param throwable
     *            the cause of this {@code Throwable}.
     * @return this {@code Throwable} instance.
     * @throws IllegalArgumentException
     *             if {@code Throwable} is this object.
     * @throws IllegalStateException
     *             if the cause has already been initialized.
     */
    CARAPI InitCause(
        /* [in] */ IThrowable* throwable);

    /**
     * Returns the cause of this {@code Throwable}, or {@code null} if there is
     * no cause.
     *
     * @return Throwable this {@code Throwable}'s cause.
     */
    CARAPI GetCause(
        /* [out] */ IThrowable** outthr);

    /**
     * Adds {@code throwable} to the list of throwables suppressed by this. The
     * throwable will included when this exception's stack trace is printed.
     *
     * @throws IllegalArgumentException if {@code throwable == this}.
     * @throws NullPointerException if {@code throwable == null}.
     * @since 1.7
     * @hide 1.7
     */
    CARAPI AddSuppressed(
        /* [in] */ IThrowable* throwable);

    /**
     * Returns the throwables suppressed by this.
     *
     * @since 1.7
     * @hide 1.7
     */
    CARAPI GetSuppressed(
        /* [out, callee] */ ArrayOf<IThrowable*>** outarr);

    CARAPI ToString(
        /* [out] */ String * info);

private:
    /*
     * Creates a compact, VM-specific collection of goodies, suitable for
     * storing in the "stackState" field, based on the current thread's
     * call stack.
     */
    static AutoPtr<Object> NativeFillInStackTrace();

    /*
     * Creates an array of StackTraceElement objects from the data held
     * in "stackState".
     */
    static AutoPtr<ArrayOf<IStackTraceElement*> > NativeGetStackTrace(
        /* [in] */ Object* stackState);

    static Int32 CountDuplicates(
        /* [in] */ ArrayOf<IStackTraceElement*>* currentStack,
        /* [in] */ ArrayOf<IStackTraceElement*>* parentStack);


    /**
     * Returns an array of StackTraceElement. Each StackTraceElement
     * represents a entry on the stack.
     */
     AutoPtr<ArrayOf<IStackTraceElement*> > GetInternalStackTrace();

private:
    /**
     * The message provided when the exception was created.
     */
    String mDetailMessage;
    String mBacktrace;

    /**
     * The cause of this Throwable. Null when there is no cause.
     */
    // should not use AutoPtr<IThrowable>
    IThrowable* mCause;

    /**
     * Throwables suppressed by this throwable. Null when suppressed exceptions
     * are disabled.
     */
    AutoPtr<IList> mSuppressedExceptions;
    //List<Throwable> suppressedExceptions = Collections.emptyList();

    /**
     * An intermediate representation of the stack trace.  This field may
     * be accessed by the VM; do not rename.
     */
    /*transient volatile*/ AutoPtr<Object> mStackState;

    /**
     * A fully-expanded representation of the stack trace.
     */
    AutoPtr<ArrayOf<IStackTraceElement*> > mStackTrace;
};


} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_CTHROWABLE_H__

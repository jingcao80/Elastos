
#include "Elastos.CoreLibrary.IO.h"
#include "CThrowable.h"
#include "Collections.h"
#include "EmptyArray.h"
#include "CSystem.h"
#include "CoreUtils.h"
#include "CArrayList.h"
#include "Arrays.h"

using Elastos::Utility::Arrays;
using Elastos::Utility::Collections;
using Elastos::Utility::CArrayList;
using Libcore::Utility::EmptyArray;

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL(CThrowable, Object, IThrowable)

CAR_OBJECT_IMPL(CThrowable)

CThrowable::CThrowable()
{
    mSuppressedExceptions = Collections::EMPTY_LIST;
}

CThrowable::~CThrowable()
{
    if (mCause != (IThrowable*)this) {
        mCause->Release();
        mCause = NULL;
    }
}

ECode CThrowable::constructor()
{
    mStackTrace = EmptyArray::STACK_TRACE_ELEMENT;
    FillInStackTrace();
    mCause = (IThrowable*)this;
    return NOERROR;
}

ECode CThrowable::constructor(
    /* [in] */ const String& detailMessage)
{
    mDetailMessage = detailMessage;
    mStackTrace = EmptyArray::STACK_TRACE_ELEMENT;
    mCause = (IThrowable*)this;
    FillInStackTrace();
    return NOERROR;
}

ECode CThrowable::constructor(
    /* [in] */ const String& detailMessage,
    /* [in] */ IThrowable * cause)
{
    mDetailMessage = detailMessage;
    mCause = cause;
    if (mCause != (IThrowable*)this) mCause->AddRef();
    mStackTrace = EmptyArray::STACK_TRACE_ELEMENT;
    FillInStackTrace();
    return NOERROR;
}

ECode CThrowable::constructor(
    /* [in] */ IThrowable * cause)
{
    mDetailMessage = Object::ToString(cause);
    mCause = cause;
    if (mCause != (IThrowable*)this) mCause->AddRef();
    mStackTrace = EmptyArray::STACK_TRACE_ELEMENT;
    FillInStackTrace();
    return NOERROR;
}

ECode CThrowable::constructor(
    /* [in] */ const String& detailMessage,
    /* [in] */ IThrowable * cause,
    /* [in] */ Boolean enableSuppression,
    /* [in] */ Boolean writableStackTrace)
{
    mDetailMessage = detailMessage;
    mCause = cause;
    if (mCause != (IThrowable*)this) mCause->AddRef();
    if (!enableSuppression) {
        mSuppressedExceptions = NULL;
    }
    if (writableStackTrace) {
        mStackTrace = EmptyArray::STACK_TRACE_ELEMENT;
        FillInStackTrace();
    }
    else {
        mStackTrace = NULL;
    }
    return NOERROR;
}

ECode CThrowable::FillInStackTrace()
{
    if (mStackTrace == NULL) {
        return NOERROR; // writableStackTrace was false.
    }
    // Fill in the intermediate representation.
    mStackState = NativeFillInStackTrace();
    // Mark the full representation as in need of update.
    mStackTrace = EmptyArray::STACK_TRACE_ELEMENT;
    return NOERROR;
}

ECode CThrowable::GetMessage(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mDetailMessage;
    return NOERROR;
}

ECode CThrowable::GetLocalizedMessage(
    /* [out] */ String* str)
{
    return GetMessage(str);
}

ECode CThrowable::GetStackTrace(
    /* [out, callee] */ ArrayOf<IStackTraceElement*>** outarr)
{
    VALIDATE_NOT_NULL(outarr)
    AutoPtr<ArrayOf<IStackTraceElement*> > stes = GetInternalStackTrace();
    *outarr = stes;
    REFCOUNT_ADD(*outarr)
    return NOERROR;
}

ECode CThrowable::SetStackTrace(
    /* [in] */ ArrayOf<IStackTraceElement*>* trace)
{
    if (mStackTrace == NULL) {
        return NOERROR; // writableStackTrace was false.
    }
    AutoPtr<ArrayOf<IStackTraceElement *> > newTrace = trace->Clone();
    for (Int32 i = 0; i < newTrace->GetLength(); i++) {
        if ((*newTrace)[i] == NULL) {
            // throw new NullPointerException("trace[" + i + "] == NULL");
            return E_NULL_POINTER_EXCEPTION;
        }
    }
    mStackTrace = newTrace;
    return NOERROR;
}

ECode CThrowable::PrintStackTrace()
{
    AutoPtr<Elastos::Core::ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((Elastos::Core::ISystem**)&system);
    AutoPtr<IPrintStream> err;
    system->GetErr((IPrintStream**)&err);
    return PrintStackTrace(err);
}

Int32 CThrowable::CountDuplicates(
    /* [in] */ ArrayOf<IStackTraceElement*>* currentStack,
    /* [in] */ ArrayOf<IStackTraceElement*>* parentStack)
{
    assert(currentStack);
    assert(parentStack);
    Int32 duplicates = 0;
    Int32 parentIndex = parentStack->GetLength();
    IStackTraceElement* parentFrame;
    for (Int32 i = currentStack->GetLength(); --i >= 0 && --parentIndex >= 0;) {
         parentFrame = (*parentStack)[parentIndex];
        if (Object::Equals(parentFrame, (*currentStack)[i])) {
            duplicates++;
        }
        else {
            break;
        }
    }
    return duplicates;
}

AutoPtr<ArrayOf<IStackTraceElement*> > CThrowable::GetInternalStackTrace()
{
    if (mStackTrace == EmptyArray::STACK_TRACE_ELEMENT) {
        mStackTrace = NativeGetStackTrace(mStackState);
        mStackState = NULL; // Let go of intermediate representation.
        return mStackTrace;
    }
    else if (mStackTrace == NULL) {
        return EmptyArray::STACK_TRACE_ELEMENT;
    }
    else {
      return mStackTrace;
    }
}

ECode CThrowable::PrintStackTrace(
    /* [in] */ IPrintStream* err)
{
    // try {
    return PrintStackTrace(IAppendable::Probe(err), String(""), NULL);
    // } catch (IOException e) {
    //     // Appendable.append throws IOException but PrintStream.append doesn't.
    //     throw new AssertionError();
    // }
}

ECode CThrowable::PrintStackTrace(
    /* [in] */ IPrintWriter* err)
{
    // try {
    return PrintStackTrace(IAppendable::Probe(err), String(""), NULL);
    // } catch (IOException e) {
    //     // Appendable.append throws IOException, but PrintWriter.append doesn't.
    //     throw new AssertionError();
    // }
    return NOERROR;
}

ECode CThrowable::PrintStackTrace(
    /* [in] */ IAppendable* err,
    /* [in] */ const String& indent,
    /* [in] */ ArrayOf<IStackTraceElement*>* parentStack)
{
    assert(err);

    String str;
    ToString(&str);
    err->Append(CoreUtils::Convert(str));
    err->Append(CoreUtils::Convert("\n"));

    AutoPtr<ArrayOf<IStackTraceElement*> > stack = GetInternalStackTrace();
    if (stack != NULL) {
        Int32 duplicates = parentStack != NULL ? CountDuplicates(stack, parentStack) : 0;
        for (Int32 i = 0; i < stack->GetLength() - duplicates; i++) {
            err->Append(CoreUtils::Convert(indent));
            err->Append(CoreUtils::Convert("\tat "));
            err->Append(CoreUtils::Convert(Object::ToString((*stack)[i])));
            err->Append(CoreUtils::Convert("\n"));
        }

        if (duplicates > 0) {
            err->Append(CoreUtils::Convert(indent));
            err->Append(CoreUtils::Convert("\t... "));
            err->Append(CoreUtils::Convert(StringUtils::ToString(duplicates)));
            err->Append(CoreUtils::Convert(" more\n"));
        }
    }

    // Print suppressed exceptions indented one level deeper.
    if (mSuppressedExceptions != NULL) {
        AutoPtr<IIterator> it;
        mSuppressedExceptions->GetIterator((IIterator**)&it);
        Boolean hasNext;
        IThrowable* tr;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            tr = IThrowable::Probe(obj);
            err->Append(CoreUtils::Convert(indent));
            err->Append(CoreUtils::Convert("\tSuppressed: "));
            tr->PrintStackTrace(err, indent + "\t", stack);
        }
    }

    AutoPtr<IThrowable> cause;
    GetCause((IThrowable**)&cause);
    if (cause != NULL) {
        err->Append(CoreUtils::Convert(indent));
        err->Append(CoreUtils::Convert("Caused by: "));
        cause->PrintStackTrace(err, indent, stack);
    }
    return NOERROR;
}

ECode CThrowable::ToString(
    /* [out] */ String * info)
{
    String msg;
    GetLocalizedMessage(&msg);
    String name("CThrowable");
    if (msg.IsNull()) {
        *info = name;
        return NOERROR;
    }
    *info = name + ": " + msg;
    return NOERROR;
}

ECode CThrowable::InitCause(
    /* [in] */ IThrowable* throwable)
{
    if (mCause != (IThrowable*)this) {
        //throw new IllegalStateException("Cause already initialized");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (throwable == (IThrowable*)this) {
        // throw new IllegalArgumentException("throwable == this");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mCause = throwable;
    if (mCause != (IThrowable*)this) mCause->AddRef();
    return NOERROR;
}

ECode CThrowable::GetCause(
    /* [out] */ IThrowable** outthr)
{
    VALIDATE_NOT_NULL(outthr)
    *outthr = NULL;
    if (mCause == (IThrowable*)this) {
        return NOERROR;
    }
    *outthr = mCause;
    REFCOUNT_ADD(*outthr)
    return NOERROR;
}

ECode CThrowable::AddSuppressed(
    /* [in] */ IThrowable* throwable)
{
    if (throwable == (IThrowable*)this) {
        // throw new IllegalArgumentException("throwable == this");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (throwable == NULL) {
        // throw new NullPointerException("throwable == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (mSuppressedExceptions != NULL) {
        Boolean isEmpty;
        mSuppressedExceptions->IsEmpty(&isEmpty);
        // Suppressed exceptions are enabled.
        if (isEmpty) {
            // Ensure we have somewhere to place suppressed exceptions.
            mSuppressedExceptions = NULL;
            CArrayList::New(1, (IList**)&mSuppressedExceptions);
        }
        mSuppressedExceptions->Add(throwable);
    }

    return NOERROR;
}

ECode CThrowable::GetSuppressed(
    /* [out, callee] */ ArrayOf<IThrowable*>** outarr)
{
    VALIDATE_NOT_NULL(outarr)

    Boolean isEmpty = TRUE;
    if (mSuppressedExceptions != NULL) {
        mSuppressedExceptions->IsEmpty(&isEmpty);
    }

    AutoPtr<ArrayOf<IThrowable*> > temp;
    if (isEmpty) {
        temp = EmptyArray::THROWABLE;
    }
    else {
        Arrays::FromList(mSuppressedExceptions, (ArrayOf<IThrowable*>**)&temp);
    }

    *outarr = temp;
    REFCOUNT_ADD(*outarr)
    return NOERROR;
}

AutoPtr<Object> CThrowable::NativeFillInStackTrace()
{
    AutoPtr<Object> result;
    return result;
}

AutoPtr<ArrayOf<IStackTraceElement*> > CThrowable::NativeGetStackTrace(
    /* [in] */ Object* stackState)
{
    AutoPtr<ArrayOf<IStackTraceElement*> > result;
    return result;
}

} // namespace Core
} // namespace Elastos
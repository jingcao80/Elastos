
#include "Elastos.CoreLibrary.Net.h"
#include "ServiceLoader.h"
#include "io/CInputStreamReader.h"
#include "io/CBufferedReader.h"
#include "core/CString.h"
#include "core/Character.h"
#include "utility/CHashSet.h"
#include "utility/Collections.h"
#include "core/Thread.h"
#include "core/CSystem.h"

using Elastos::Core::CString;
using Elastos::Core::Character;
using Elastos::Core::Thread;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::IThread;
using Elastos::Core::ISystem;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Collections;
using Elastos::Net::IURL;
using Elastos::IO::IInputStream;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::IReader;

namespace Elastos {
namespace Utility {

///////////////////////////////////////
//  ServiceLoader::ServiceIterator
///////////////////////////////////////
CAR_INTERFACE_IMPL(ServiceLoader::ServiceIterator, Object, IIterator)

ServiceLoader::ServiceIterator::ServiceIterator(
    /* [in] */ ServiceLoader* sl)
    : mServiceIt(sl->GetService())
    , mClassLoaderIt(sl->GetClassLoader())
    , mServicesIt(sl->GetServices())
    , mIsRead(FALSE)
    , mQueue(NULL)
{
}

ECode ServiceLoader::ServiceIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (!mIsRead) {
        ReadClass();
    }

    Boolean empty;
    *result = mQueue && ((ICollection::Probe(mQueue))->IsEmpty(&empty), empty);
    return NOERROR;
}

ECode ServiceLoader::ServiceIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    Boolean hasNext;
    if (HasNext(&hasNext), hasNext) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }

    AutoPtr<IInterface> temp;
    mQueue->Remove((IInterface**)&temp);

    AutoPtr<ICharSequence> className = ICharSequence::Probe(temp);

    /*try {
        return service.cast(classLoader.loadClass(className).newInstance());
    } catch (Exception e) {
        throw new ServiceConfigurationError("Couldn't instantiate class " + className, e);
    }*/
    return NOERROR;
}

ECode ServiceLoader::ServiceIterator::ReadClass()
{
    Int32 size;
    ICollection::Probe(mServicesIt)->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> element;
        (IList::Probe(mQueue))->Get(i, (IInterface**)&element);
        AutoPtr<IURL> url = IURL::Probe(element);
        if (!url) assert(0 && "TODO");
        AutoPtr<IInputStream> is;
        url->OpenStream((IInputStream**)&is);
        AutoPtr<IInputStreamReader> isr;
        CInputStreamReader::New(is, String("UTF-8"), (IInputStreamReader**)&isr);
        AutoPtr<IBufferedReader> reader;
        CBufferedReader::New(IReader::Probe(isr), (IBufferedReader**)&reader);

        String line;
        while (reader->ReadLine(&line), line.IsNullOrEmpty()) {
            Int32 commentStart = line.IndexOf('#');
            if (commentStart != -1) {
                line = line.Substring(0, commentStart);
            }
            line = line.Trim();
            if (line.IsNullOrEmpty()) {
                continue;
            }
            String classname = line;
            if (FAILED(CheckValidJavaClassName(classname))) {
                String error;
                url->ToString(&error);
                printf("Couldn't read %s", error.string());
                //IoUtils::CloseQuietly(reader);
                return NOERROR;
            }


            AutoPtr<ICharSequence> csq;
            CString::New(classname, (ICharSequence**)&csq);
            Boolean isContains = FALSE;
            if (((ICollection::Probe(mQueue))->Contains(csq, &isContains)), !isContains) {
                (ICollection::Probe(mQueue))->Add(csq);
            }
        }
        mIsRead = TRUE;
        //IoUtils::CloseQuietly(reader);
    }
    return NOERROR;
}

ECode ServiceLoader::ServiceIterator::Remove()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ServiceLoader::ServiceIterator::CheckValidJavaClassName(
    /* [in] */ const String& className)
{
    for (Int32 i = 0; i < className.GetLength(); ++i) {
        Char32 ch = className.GetChar(i);
        if (!Character::IsCarIdentifierPart(ch) && ch != '.') {
            printf("Bad character ' %c ' in class name", ch);
            return NOERROR;
        }
    }
    return NOERROR;
}

///////////////////////////////////////
//          ServiceLoader
///////////////////////////////////////

CAR_INTERFACE_IMPL_2(ServiceLoader, Object, IServiceLoader, IIterable)

ServiceLoader::ServiceLoader(
    /* [in] */ InterfaceID service,
    /* [in] */ IClassLoader* cl)
    : mService(service)
    , mClassLoader(cl)
{
    CHashSet::New((ISet**)&mServices);
    Reload();
}

ECode ServiceLoader::Reload()
{
    InternalLoad();
    return NOERROR;
}

ECode ServiceLoader::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)
    *it = new ServiceIterator(this);
    REFCOUNT_ADD(*it);
    return NOERROR;
}

AutoPtr<IServiceLoader> ServiceLoader::Load(
    /* [in] */ InterfaceID service,
    /* [in] */ IClassLoader* cl)
{
    if (!cl) {
        //cl = ClassLoader.getSystemClassLoader();
    }
    return new ServiceLoader(service, cl);
}

AutoPtr<IServiceLoader> ServiceLoader::Load(
    /* [in] */ InterfaceID service)
{
    AutoPtr<IThread> thread = Thread::GetCurrentThread();
    AutoPtr<IClassLoader> cl;
    thread->GetContextClassLoader((IClassLoader**)&cl);
    return Load(service, cl);
}

AutoPtr<IServiceLoader> ServiceLoader::LoadInstalled(
    /* [in] */ InterfaceID service)
{
    AutoPtr<IClassLoader> cl ;//= ClassLoader.getSystemClassLoader();
    if (cl) {
        while (TRUE /*cl->GetParent() != null*/) {
            //cl = cl->GetParent();
        }
    }
    return Load(service, cl);
}

AutoPtr<IInterface> ServiceLoader::LoadFromSystemProperty(
    /* [in] */ const InterfaceID& service)
{
    AutoPtr<ISystem> st;
    CSystem::AcquireSingleton((ISystem**)&st);
    String className;
    st->GetProperty(className/*service->GetName()*/, &className);
    if (className.IsNullOrEmpty()) {
        //InterfaceID c = ClassLoader.getSystemClassLoader().loadClass(className);
        //return (S) c.newInstance();
    }
    return NULL;
}

ECode ServiceLoader::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)
    *info = String("ServiceLoader for ");// + service.getName();
    return NOERROR;
}

InterfaceID ServiceLoader::GetService()
{
    return mService;
}

AutoPtr<IClassLoader> ServiceLoader::GetClassLoader()
{
    return mClassLoader;
}

AutoPtr<ISet> ServiceLoader::GetServices()
{
    return mServices;
}

ECode ServiceLoader::InternalLoad()
{
    (ICollection::Probe(mServices))->Clear();
    String name = String("META-INF/services/");// + service.getName();
    AutoPtr<IArrayList> list;
    Collections::List(/*classLoader.getResources(name)*/ NULL, (IArrayList**)&list);
    (ICollection::Probe(mServices))->AddAll(ICollection::Probe(list));
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
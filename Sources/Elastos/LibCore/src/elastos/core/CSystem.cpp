
#include "CSystem.h"
#include "StringBuilder.h"
#include "CString.h"
#include "CHashMap.h"
#include "CProperties.h"
#include "ICUUtil.h"
#include "CSelectorProviderHelper.h"
#include "CFileDescriptor.h"
#include "CFileInputStream.h"
#include "CFileOutputStream.h"
#include "CBufferedInputStream.h"
#include "CPrintStream.h"
#include "droid/system/Os.h"

#include <stdlib.h>
#include <cutils/log.h>
#include <unistd.h>
#include <sys/utsname.h>
#include "openssl/opensslv.h"
#include "zlib.h"

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CProperties;
using Elastos::IO::CFileDescriptor;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CPrintStream;
using Elastos::IO::EIID_IInputStream;
using Elastos::IO::EIID_IOutputStream;
using Elastos::IO::EIID_IPrintStream;
using Elastos::IO::Channels::Spi::ISelectorProvider;
using Elastos::IO::Channels::Spi::ISelectorProviderHelper;
using Elastos::IO::Channels::Spi::CSelectorProviderHelper;
using Libcore::ICU::ICUUtil;
using Elastos::Droid::System::Os;
using Elastos::Droid::System::IStructPasswd;

namespace Elastos {
namespace Core {

String CSystem::sLineSeparator;
AutoPtr<IProperties> CSystem::sSystemProperties;
AutoPtr<IInputStream> CSystem::mIn;
AutoPtr<IPrintStream> CSystem::mOut;
AutoPtr<IPrintStream> CSystem::mErr;

static AutoPtr<HashMap<String, String> > InitAbiMap()
{
    AutoPtr<HashMap<String, String> > map = new HashMap<String, String>();
    (*map)[String("armeabi")] = String("arm");
    (*map)[String("armeabi-v7a")] = String("arm");
    (*map)[String("mips")] = String("mips");
    (*map)[String("mips64")] = String("mips64");
    (*map)[String("x86")] = String("x86");
    (*map)[String("x86_64")] = String("x86_64");
    (*map)[String("arm64-v8a")] = String("arm64");
    return map;
}
const AutoPtr<HashMap<String, String> > CSystem::ABI_TO_INSTRUCTION_SET_MAP = InitAbiMap();


CAR_INTERFACE_IMPL(CSystem, Singleton, ISystem)

CAR_SINGLETON_IMPL(CSystem)

ECode CSystem::constructor()
{
    AutoPtr<IInputStream> input;
    CFileInputStream::New(CFileDescriptor::IN, (IInputStream**)&input);
    CBufferedInputStream::New(input, (IInputStream**)&mIn);

    AutoPtr<IOutputStream> output;
    CFileOutputStream::New(CFileDescriptor::OUT, (IOutputStream**)&output);
    CPrintStream::New(output, (IPrintStream**)&mOut);

    AutoPtr<IOutputStream> err;
    CFileOutputStream::New(CFileDescriptor::ERR, (IOutputStream**)&err);
    CPrintStream::New(err, (IPrintStream**)&mErr);

    return NOERROR;
}

ECode CSystem::SetIn(
    /* [in] */ IInputStream* newIn)
{
    mIn = newIn;
    return NOERROR;
}

ECode CSystem::GetIn(
    /* [out] */ IInputStream** input)
{
    VALIDATE_NOT_NULL(input);
    *input = mIn;
    REFCOUNT_ADD(*input);
    return NOERROR;
}

ECode CSystem::SetOut(
    /* [in] */ IPrintStream* newOut)
{
    mOut = newOut;
    return NOERROR;
}

ECode CSystem::GetOut(
    /* [out] */ IPrintStream** output)
{
    VALIDATE_NOT_NULL(output);
    *output = mOut;
    REFCOUNT_ADD(*output);
    return NOERROR;
}

ECode CSystem::SetErr(
    /* [in] */ IPrintStream* newErr)
{
    mErr = newErr;
    return NOERROR;
}

ECode CSystem::GetErr(
    /* [out] */ IPrintStream** err)
{
    VALIDATE_NOT_NULL(err);
    *err = mErr;
    REFCOUNT_ADD(*err);
    return NOERROR;
}

ECode CSystem::GetCurrentTimeMillis(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);

    // we don't support the clocks here.
    struct timeval t;
    t.tv_sec = t.tv_usec = 0;
    gettimeofday(&t, NULL);
    *value = ((Int64)t.tv_sec * 1000 + (Int64)t.tv_usec / 1000.0);
    return NOERROR;
}

ECode CSystem::GetNanoTime(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);

    struct timespec ts;

    ts.tv_sec = ts.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    *value = (Int64)ts.tv_sec*1000000000LL + ts.tv_nsec;
    return NOERROR;
}

ECode CSystem::Exit(
    /* [in] */ Int32 code)
{
    //Runtime.getRuntime().exit(code);
    return NOERROR;
}

ECode CSystem::GC()
{
    //Runtime.getRuntime().gc();
    return NOERROR;
}

ECode CSystem::IdentityHashCode(
    /* [in] */ IInterface* anObject,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = (Int32)anObject;
    return NOERROR;
}

ECode CSystem::GetLineSeparator(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    if (sLineSeparator.IsNull()) {
        GetProperty(String("line.separator"), &sLineSeparator);
    }
    *value = sLineSeparator;
    return NOERROR;
}

ECode CSystem::GetEnv(
    /* [in] */ const String& name,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = GetEnvByName(name);
    return NOERROR;
}

ECode CSystem::GetEnv(
    /* [in] */ const String& name,
    /* [in] */ const String& defaultValue,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    *value = GetEnvByName(name);
    if ((*value).IsNull()) {
        *value = defaultValue;
    }

    return NOERROR;
}

String CSystem::GetEnvByName(
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        ALOGD("Failed to CSystem::GetEnv(), name == null!");
        return String(NULL);
    }

    return String(getenv(name.string()));
}

ECode CSystem::GetEnvs(
    /* [out] */ IMap** map)
{
    VALIDATE_NOT_NULL(map);

    AutoPtr<IMap> hashMap;
    CHashMap::New((IMap**)&hashMap);
    char ** p = environ;
    Int32 index = 0;
    String entry, key, value;
    while (p && *p) {
        entry = String(*p);
        index = entry.IndexOf('=');
        if (index != -1) {
            key = entry.Substring(0, index);
            value = entry.Substring(index + 1);

            AutoPtr<ICharSequence> ko, vo;
            CString::New(key, (ICharSequence**)&ko);
            CString::New(value, (ICharSequence**)&vo);
            hashMap->Put(ko.Get(), vo.Get(), NULL);
        }
        ++p;
    }

    *map = hashMap;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

ECode CSystem::InheritedChannel(
    /* [out] */ IChannel** value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<ISelectorProviderHelper> helper;
    CSelectorProviderHelper::AcquireSingleton((ISelectorProviderHelper**)&helper);
    AutoPtr<ISelectorProvider> provider;
    helper->GetProvider((ISelectorProvider**)&provider);
    provider->InheritedChannel(value);
    return NOERROR;
}

ECode CSystem::GetProperties(
    /* [out] */ IProperties** value)
{
    VALIDATE_NOT_NULL(value);

    if (sSystemProperties == NULL) {
        InitSystemProperties();
    }
    *value = sSystemProperties;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CSystem::InitSystemProperties()
{
    // VMRuntime runtime = VMRuntime.getRuntime();
    AutoPtr<IProperties> p;
    CProperties::New((IProperties**)&p);

    String projectUrl("http://www.android.com/");
    String projectName("The Android Project");

    // p->SetProperty(String("java.boot.class.path"), runtime.bootClassPath(), NULL);
    // p->SetProperty(String("java.class.path"), runtime.classPath(), NULL);

    // None of these four are meaningful on Android, but these keys are guaranteed
    // to be present for System.getProperty. For java.class.version, we use the maximum
    // class file version that dx currently supports.
    p->SetProperty(String("java.class.version"), String("50.0"), NULL);
    p->SetProperty(String("java.compiler"), String(""), NULL);
    p->SetProperty(String("java.ext.dirs"), String(""), NULL);
    p->SetProperty(String("java.version"), String("0"), NULL);

    String tmp;
    GetEnv(String("JAVA_HOME"), String("/system"), &tmp);
    p->SetProperty(String("java.home"), tmp, NULL);

    p->SetProperty(String("java.specification.name"), String("Dalvik Core Library"), NULL);
    p->SetProperty(String("java.specification.vendor"), projectName, NULL);
    p->SetProperty(String("java.specification.version"), String("0.9"), NULL);

    p->SetProperty(String("java.vendor"), projectName, NULL);
    p->SetProperty(String("java.vendor.url"), projectUrl, NULL);
    p->SetProperty(String("java.vm.name"), String("Dalvik"), NULL);
    p->SetProperty(String("java.vm.specification.name"), String("Dalvik Virtual Machine Specification"), NULL);
    p->SetProperty(String("java.vm.specification.vendor"), projectName, NULL);
    p->SetProperty(String("java.vm.specification.version"), String("0.9"), NULL);
    p->SetProperty(String("java.vm.vendor"), projectName, NULL);
    // p->SetProperty(String("java.vm.version"), runtime.vmVersion(), NULL);

    p->SetProperty(String("file.separator"), String("/"), NULL);
    p->SetProperty(String("line.separator"), String("\n"), NULL);
    p->SetProperty(String("path.separator"), String(":"), NULL);

    p->SetProperty(String("java.runtime.name"), String("Android Runtime"), NULL);
    p->SetProperty(String("java.runtime.version"), String("0.9"), NULL);
    p->SetProperty(String("java.vm.vendor.url"), projectUrl, NULL);

    p->SetProperty(String("file.encoding"), String("UTF-8"), NULL);
    p->SetProperty(String("user.language"), String("en"), NULL);
    p->SetProperty(String("user.region"), String("US"), NULL);

    Int32 uid;
    Os::Getuid(&uid);
    AutoPtr<IStructPasswd> pwd;
    Os::Getpwuid(uid, (IStructPasswd**)&pwd);
    if (pwd) {
        String name;
        pwd->GetName(&name);
        p->SetProperty(String("user.name"), name, NULL);
    }

    struct utsname info;
    if (uname(&info) == 0) {
        p->SetProperty(String("os.arch"), String(info.machine), NULL);
        p->SetProperty(String("os.name"), String(info.sysname), NULL);
        p->SetProperty(String("os.version"), String(info.release), NULL);
    }

    String icuVersion = ICUUtil::GetIcuVersion();
    String unicodeVersion = ICUUtil::GetUnicodeVersion();
    String cldrVersion;
    ICUUtil::GetCldrVersion(&cldrVersion);
    // Undocumented Android-only properties.
    p->SetProperty(String("android.icu.library.version"), icuVersion, NULL);
    p->SetProperty(String("android.icu.unicode.version"), unicodeVersion, NULL);
    p->SetProperty(String("android.icu.cldr.version"), cldrVersion, NULL);

    ParsePropertyAssignments(p, SpecialProperties());

    // Override built-in properties with settings from the command line.
    //TODO parsePropertyAssignments(p, runtime.properties());

    CSystem::sSystemProperties = p;
    return NOERROR;
}

ECode CSystem::GetProperty(
    /* [in] */ const String& propertyName,
    /* [out] */ String* value)
{
    return GetProperty(propertyName, String(NULL), value);
}

ECode CSystem::GetProperty(
    /* [in] */ const String& prop,
    /* [in] */ const String& defaultValue,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = String(NULL);

    if (prop.IsNullOrEmpty()) {
        ALOGD("Failed to GetProperty! parameter prop is null or empty!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IProperties> properties;
    GetProperties((IProperties**)&properties);
    properties->GetProperty(prop, defaultValue, value);
    if ((*value).IsNull()) {
        *value = defaultValue;
    }
    return NOERROR;
}

ECode CSystem::SetProperty(
    /* [in] */ const String& prop,
    /* [in] */ const String& value,
    /* [out] */ String* oldValue)
{
    VALIDATE_NOT_NULL(value);
    *oldValue = String(NULL);

    if (prop.IsNullOrEmpty()) {
        ALOGD("Failed to SetProperty! parameter prop is null or empty!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IProperties> properties;
    GetProperties((IProperties**)&properties);
    properties->SetProperty(prop, value, oldValue);
    return NOERROR;
}

ECode CSystem::ClearProperty(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = String(NULL);

    if (key.IsNullOrEmpty()) {
        ALOGD("Failed to ClearProperty! parameter key is null or empty!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IProperties> properties;
    GetProperties((IProperties**)&properties);
    AutoPtr<IInterface> obj;
    AutoPtr<ICharSequence> keyObj;
    CString::New(key, (ICharSequence**)&keyObj);
    IMap::Probe(properties)->Remove(keyObj, (IInterface**)&obj);
    if (obj) {
        ICharSequence* seq = ICharSequence::Probe(obj);
        if (seq) {
            seq->ToString(value);
        }
    }

    return NOERROR;
}

ECode CSystem::Load(
    /* [in] */ const String& pathName)
{
    //Runtime.getRuntime().load(pathName, VMStack.getCallingClassLoader());
    return NOERROR;
}

ECode CSystem::LoadLibrary(
    /* [in] */ const String& libName)
{
    //Runtime.getRuntime().loadLibrary(libName, VMStack.getCallingClassLoader());
    return NOERROR;
}

ECode CSystem::MapLibraryName(
    /* [in] */ const String& userLibName,
    /* [out] */ String* value)
 {
    VALIDATE_NOT_NULL(value);
    *value = String(NULL);
    return NOERROR;
 }

ECode CSystem::RunFinalization()
{
    //Runtime.getRuntime().runFinalization();
    return NOERROR;
}

ECode CSystem::RunFinalizersOnExit(
    /* [in] */ Boolean flag)
{
    //Runtime.runFinalizersOnExit(flag);
    return NOERROR;
}

ECode CSystem::SetProperties(
    /* [in] */ IProperties* properties)
{
    sSystemProperties = properties;
    return NOERROR;
}

AutoPtr<ArrayOf<String> > CSystem::SpecialProperties()
{
    char path[PATH_MAX];
    getcwd(path, sizeof(path));

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(3);
    StringBuilder sb(64);
    sb += "user.dir=";
    sb += path;
    array->Set(0, sb.ToString());

    sb.Reset();
    sb += "android.zlib.version=";
    sb += ZLIB_VERSION;
    array->Set(1, sb.ToString());

    sb.Reset();
    sb += "android.openssl.version=";
    sb += OPENSSL_VERSION_TEXT;
    array->Set(2, sb.ToString());

    return array;
}

ECode CSystem::ParsePropertyAssignments(
    /* [in] */ IProperties* p,
    /* [in] */ ArrayOf<String>* assignments)
{
    if (assignments == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String key, value, assignment;
    for (Int32 i = 0; i < assignments->GetLength(); ++i) {
        assignment = (*assignments)[i];
        Int32 split = assignment.IndexOf('=');
        key = assignment.Substring(0, split);
        value = assignment.Substring(split + 1);
        p->SetProperty(key, value, NULL);
    }
    return NOERROR;
}

// From dalvik/VMRuntime.java
//
ECode CSystem::GetInstructionSet(
    /* [in] */ const String& abi,
    /* [out] */ String* instructionSet)
{
    VALIDATE_NOT_NULL(instructionSet)
    *instructionSet  = NULL;

    HashMap<String, String>::Iterator it = ABI_TO_INSTRUCTION_SET_MAP->Find(abi);
    if (it == ABI_TO_INSTRUCTION_SET_MAP->End()) {
        ALOGE("Unsupported ABI: %s", abi.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *instructionSet = it->mSecond;
    return NOERROR;
}

ECode CSystem::Is64BitInstructionSet(
    /* [in] */ const String& instructionSet,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = instructionSet.Equals("arm64")
        || instructionSet.Equals("x86_64")
        || instructionSet.Equals("mips64");
    return NOERROR;
}

ECode CSystem::Is64BitAbi(
    /* [in] */ const String& abi,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    String set;
    FAIL_RETURN(GetInstructionSet(abi, &set))
    return Is64BitInstructionSet(set, result);
}

ECode CSystem::Is64Bit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (sizeof(void*) == sizeof(uint64_t));
    return NOERROR;
}

} // namespace Core
} // namespace Elastos

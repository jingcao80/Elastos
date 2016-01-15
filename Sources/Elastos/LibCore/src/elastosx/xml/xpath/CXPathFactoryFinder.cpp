
#include "CXPathFactoryFinder.h"
#include "Logger.h"
#include "CProperties.h"
#include "StringBuilder.h"
#include "Thread.h"
#include "CSystem.h"
#include "File.h"
#include "AutoLock.h"
#include "CFile.h"
#include "CFileInputStream.h"
#include "XPathFactory.h"
#include "CBufferedReader.h"
#include "CInputStreamReader.h"
#include "IoUtils.h"

using Elastos::Utility::CProperties;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::StringBuilder;
using Elastos::Core::IThread;
using Elastos::Core::Thread;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::AutoLock;
using Elastos::IO::File;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IBufferedReader;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::ICloseable;
using Libcore::IO::IoUtils;

namespace Elastosx {
namespace Xml {
namespace XPath {

static AutoPtr<IProperties> initCacheProps()
{
    AutoPtr<IProperties> cp;
    CProperties::NewByFriend( (CProperties**)&cp );
    return (IProperties*)cp.Get();
}

Boolean CXPathFactoryFinder::debug = FALSE;
const Int32 CXPathFactoryFinder::DEFAULT_LINE_LENGTH;
Boolean CXPathFactoryFinder::firstTime = TRUE;
AutoPtr<IProperties> CXPathFactoryFinder::cacheProps = initCacheProps();


CAR_INTERFACE_IMPL(CXPathFactoryFinder, Object, IXPathFactoryFinder)

CAR_OBJECT_IMPL(CXPathFactoryFinder)

void CXPathFactoryFinder::DebugPrintln(
    /* [in] */ String msg)
{
    if (debug) {
        Logger::E("JAXP: ", msg.string());
    }
}

ECode CXPathFactoryFinder::constructor(
    /* [in] */ IClassLoader* loader)
{
    classLoader = loader;
    if (debug) {
        DebugDisplayClassLoader();
    }
    return NOERROR;
}

void CXPathFactoryFinder::DebugDisplayClassLoader()
{
    AutoPtr<IThread> ct = Thread::GetCurrentThread();
    AutoPtr<IClassLoader> cl;
    ct->GetContextClassLoader((IClassLoader**)&cl);
    //String str;
    //classLoader->ToString(&str);
    StringBuilder sb("using thread context class loader (");
    //sb += str;
    sb += ") for search";
    if (classLoader == cl) {
        DebugPrintln(sb.ToString());
        return;
    }

/*
    AutoPtr<IClassLoader> cl2 = ClassLoader::GetSystemClassLoader();
    String str2;
    classLoader->ToString(&str2);
    StringBuilder sb2("using system class loader (");
    sb2 += str2;
    sb2 += ") for search";
    if (classLoader==cl2) {
        DebugPrintln(sb2.ToString());
        return;
    }
*/

    //String str3;
    //classLoader->ToString(&str3);
    StringBuilder sb3("using class loader (");
    //sb3 += str3;
    sb3 += ") for search";
    DebugPrintln(sb3.ToString());
}

ECode CXPathFactoryFinder::NewFactory(
    /* [in] */ const String& uri,
    /* [out] */ IXPathFactory** nf)
{
    VALIDATE_NOT_NULL(nf);
    if (uri.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IXPathFactory> f;
    _newFactory(uri, (IXPathFactory**)&f);
    if (debug) {
        if (f != NULL) {
            AutoPtr<IClassInfo> classInfo;
            _CObject_ReflectClassInfo(f->Probe(EIID_IXPathFactory), (IClassInfo**)&classInfo);
            String className("--");
            if (classInfo != NULL) {
                classInfo->GetName(&className);
            }
            StringBuilder sb("factory '");
            sb += className;
            sb += "' was found for ";
            sb += uri;
            DebugPrintln(sb.ToString());

        } else {
            StringBuilder sb("unable to find a factory for ");
            sb += uri;
            DebugPrintln(sb.ToString());
        }
    }
    *nf = f;
    REFCOUNT_ADD(*nf)
    return NOERROR;
}

ECode CXPathFactoryFinder::_newFactory(
    /* [in] */ String uri,
    /* [out] */ IXPathFactory** xp)
{
    AutoPtr<IXPathFactory> xpf;
    StringBuilder sb("XPathFactory:");
    sb += uri;
    String propertyName = sb.ToString();
    // system property look up
    //try {
        if (debug) {
            StringBuilder v("Looking up system property '");
            v += propertyName;
            v += "'";
            DebugPrintln(v.ToString() );
        }
        String r;
        AutoPtr<ISystem> s;
        CSystem::AcquireSingleton((ISystem**)&s);
        s->GetProperty(propertyName, &r);
        if (!(r.IsNull()) && r.GetLength() > 0) {
            if (debug) {
                StringBuilder v("The value is '");
                v += r;
                v += "'";
                DebugPrintln(v.ToString());
            }
            CreateInstance(r, (IXPathFactory**)&xpf);
            return NOERROR;
        } else if (debug) {
            DebugPrintln(String("The property is undefined.") );
        }
    //} catch (Exception e) {
    //    e.printStackTrace();
    //}

    String javah;
    s->GetProperty(String("java.home"), &javah);

    StringBuilder sbv(javah);
    sbv += File::sSeparator;
    sbv += "lib";
    sbv += File::sSeparator;
    sbv += "jaxp.properties";
    String configFile = sbv.ToString();

    String factoryClassName(NULL) ;

    // try to read from $java.home/lib/jaxp.properties
    //try {
        if(firstTime){
            //synchronized(cacheProps){
            AutoLock lock(this);
            if(firstTime){
                AutoPtr<IFile> f;
                CFile::New(configFile, (IFile**)&f);
                firstTime = FALSE;
                Boolean isExists = FALSE;
                f->Exists(&isExists);
                if (isExists) {
                    if (debug) {
                        StringBuilder ss("Read properties file ");
                        //ss += f.ToString();
                        DebugPrintln(ss.ToString());
                    }
                    AutoPtr<IFileInputStream> fis;
                    CFileInputStream::New(f, (IFileInputStream**)&fis);
                    AutoPtr<IInputStream> is = IInputStream::Probe(fis);
                    cacheProps->Load(is);
                }
            }
            //}
        }
        cacheProps->GetProperty(propertyName, &factoryClassName);
        if (debug) {
            StringBuilder sb("found ");
            sb += factoryClassName;
            sb += " in $java.home/jaxp.properties";
            DebugPrintln(sb.ToString());
        }

        if (!(factoryClassName .IsNull()) ) {
            CreateInstance(factoryClassName, (IXPathFactory**)&xpf);
        }
    //} catch (Exception ex) {
    //    if (debug) {
    //        ex.printStackTrace();
    //    }
    //}

    // try META-INF/services files
/**    for (URL resource : createServiceFileIterator()) {
        if (debug) debugPrintln("looking into " + resource);
        try {
            xpf = loadFromServicesFile(uri, resource.toExternalForm(), resource.openStream());
            if(xpf!=null)    return xpf;
        } catch(IOException e) {
            if( debug ) {
                debugPrintln("failed to read "+resource);
                e.printStackTrace();
            }
        }
    }**/

    // platform default
    if(uri.Equals(XPathFactory::DEFAULT_OBJECT_MODEL_URI)) {
        if (debug) {
            DebugPrintln(String("attempting to use the platform default W3C DOM XPath lib"));
        }
        return CreateInstance(String("org.apache.xpath.jaxp.XPathFactoryImpl"),(IXPathFactory**)&xp);
    }

    if (debug) {
        DebugPrintln(String("all things were tried, but none was found. bailing out."));
    }
    *xp = NULL;
    return NOERROR;
}

String CXPathFactoryFinder::Which(
    /* [in] */ IClassInfo* clazz )
{
    String className;
    clazz->GetName(&className);
    //return Which( className, clazz.getClassLoader() );
    return String(NULL);
}

ECode CXPathFactoryFinder::CreateInstance(
    /* [in] */ const String& className,
    /* [out] */ IXPathFactory** instance)
{
    if (debug) {
        StringBuilder sb("instantiating ");
        sb += className;
        DebugPrintln(sb.ToString());
    }
    AutoPtr<IClassInfo> clazz;
    if( classLoader!=NULL ) {
        classLoader->LoadClass(className, (IClassInfo**)&clazz);
    } else {
        //clazz = Class.forName(className);
    }

    if(debug) {
        StringBuilder sb("loaded it from ");
        sb += Which(clazz);
        DebugPrintln(sb.ToString());
    }
    //Object o = clazz.newInstance();

    //if( o instanceof XPathFactory )
   //     return (XPathFactory)o;

    //if (debug) debugPrintln(className+" is not assignable to "+SERVICE_CLASS.getName());

    *instance = NULL;
    return NOERROR;
}

ECode CXPathFactoryFinder::LoadFromServicesFile(
    /* [in] */ String uri,
    /* [in] */ String resourceName,
    /* [in] */ IInputStream* in,
    /* [out] */ IXPathFactory** xpf)
{
    VALIDATE_NOT_NULL(xpf)
    if (debug) {
        StringBuilder sb("Reading ");
        sb += resourceName;
        DebugPrintln(sb.ToString() );
    }

    AutoPtr<IBufferedReader> rd;
    AutoPtr<IInputStreamReader> reader;
    ECode ec1 = CInputStreamReader::New(in, String("UTF-8"), (IInputStreamReader**)&reader);
    AutoPtr<IReader> r = IReader::Probe(reader);
    ECode ec2 = CBufferedReader::New(r,/* DEFAULT_LINE_LENGTH,*/ (IBufferedReader**)&rd);
    if (ec1 == (ECode)E_UNSUPPORTED_ENCODING_EXCEPTION || ec2 == (ECode)E_UNSUPPORTED_ENCODING_EXCEPTION) {
        CInputStreamReader::New(in, (IInputStreamReader**)&reader);
        r = IReader::Probe(reader);
        CBufferedReader::New(r, /*DEFAULT_LINE_LENGTH,*/ (IBufferedReader**)&rd);
    }

    String factoryClassName;
    AutoPtr<IXPathFactory> resultFactory = NULL;
    // See spec for provider-configuration files: http://java.sun.com/j2se/1.5.0/docs/guide/jar/jar.html#Provider%20Configuration%20File
    while (TRUE) {

        ECode ec = rd->ReadLine(&factoryClassName);
        if (ec == (ECode)E_IO_EXCEPTION) {
            break;
        }

        if (!(factoryClassName.IsNull()) ){
            // Ignore comments in the provider-configuration file
            Int32 hashIndex = factoryClassName.IndexOf('#');
            if (hashIndex != -1) {
                factoryClassName = factoryClassName.Substring(0, hashIndex);
            }

            // Ignore leading and trailing whitespace
            factoryClassName = factoryClassName.Trim();

            // If there's no text left or if this was a blank line, go to the next one.
            if (factoryClassName.GetLength() == 0) {
                continue;
            }

            // Found the right XPathFactory if its isObjectModelSupported(String uri) method returns true.
            AutoPtr<IXPathFactory> foundFactory;
            CreateInstance(factoryClassName, (IXPathFactory**)&foundFactory);
            Boolean v = FALSE;
            foundFactory->IsObjectModelSupported(uri, &v);
            if (v) {
                resultFactory = foundFactory;
                break;
            }
        }
        else {
            break;
        }
    }

    AutoPtr<ICloseable> c = ICloseable::Probe(rd);
    IoUtils::CloseQuietly(c);

    *xpf = resultFactory;
    REFCOUNT_ADD(*xpf);
    return NOERROR;
}

//ECode CXPathFactoryFinder::CreateServiceFileIterator(
//    /* [out] */ IIterable** sfi)
/**{
    if (classLoader == null) {
        URL resource = XPathFactoryFinder.class.getClassLoader().getResource(SERVICE_ID);
        return Collections.singleton(resource);
    } else {
        try {
            Enumeration<URL> e = classLoader.getResources(SERVICE_ID);
            if (debug && !e.hasMoreElements()) {
                debugPrintln("no "+SERVICE_ID+" file was found");
            }

            return Collections.list(e);
        } catch (IOException e) {
            if (debug) {
                debugPrintln("failed to enumerate resources "+SERVICE_ID);
                e.printStackTrace();
            }
            return Collections.emptySet();
        }
    }
}**/

String CXPathFactoryFinder::Which(
    /* [in] */ String classname,
    /* [in] */ IClassLoader* loader)
{
    StringBuilder sb(classname.Replace('.', '/'));
    sb += ".class";
    String classnameAsResource = sb.ToString();

    //if (loader==NULL) loader = ClassLoader.getSystemClassLoader();

    //URL it = loader.getResource(classnameAsResource);
    //return it != null ? it.toString() : null;
    return String(NULL);
}

} // namespace XPath
} // namespace Xml
} // namespace Elastos

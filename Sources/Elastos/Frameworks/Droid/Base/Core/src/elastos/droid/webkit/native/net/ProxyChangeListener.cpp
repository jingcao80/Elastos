
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/CIntentFilter.h"
//#include "elastos/droid/net/CProxyProperties.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/webkit/native/net/ProxyChangeListener.h"
#include "elastos/droid/webkit/native/net/api/ProxyChangeListener_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
//using Elastos::Droid::Net::CProxyProperties;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Net::IProxyInfoHelper;
using Elastos::Droid::Net::IProxyProperties;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Net {

using Elastos::Droid::Net::IProxy;

//=====================================================================
//                   ProxyChangeListener::ProxyConfig
//=====================================================================
ProxyChangeListener::ProxyConfig::ProxyConfig(
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
    : mHost(host)
    , mPort(port)
{
    // ==================before translated======================
    // mHost = host;
    // mPort = port;
}

//=====================================================================
//                  ProxyChangeListener::ProxyReceiver
//=====================================================================
ProxyChangeListener::ProxyReceiver::ProxyReceiver(
    /* [in] */ ProxyChangeListener* owner)
    : mOwner(owner)
{
}

ECode ProxyChangeListener::ProxyReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);
    // ==================before translated======================
    // if (intent.getAction().equals(Proxy.PROXY_CHANGE_ACTION)) {
    //     proxySettingsChanged(extractNewProxy(intent));
    // }

    assert(NULL != mOwner);
    assert(0);
    String action;
    intent->GetAction(&action);
    if (action.Equals(IProxy::PROXY_CHANGE_ACTION)) {
        AutoPtr<ProxyChangeListener::ProxyConfig> config = ExtractNewProxy(intent);
        mOwner->ProxySettingsChanged(config);
    }
    return NOERROR;
}

AutoPtr<ProxyChangeListener::ProxyConfig> ProxyChangeListener::ProxyReceiver::ExtractNewProxy(
    /* [in] */ IIntent* intent)
{
    // ==================before translated======================
    // try {
    //     final String GET_HOST_NAME = "getHost";
    //     final String GET_PORT_NAME = "getPort";
    //     String className;
    //     String proxyInfo;
    //     if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.KITKAT) {
    //         className = "android.net.ProxyProperties";
    //         proxyInfo = "proxy";
    //     } else {
    //         className = "android.net.ProxyInfo";
    //         proxyInfo = "android.intent.extra.PROXY_INFO";
    //     }
    //
    //     Object props = intent.getExtras().get(proxyInfo);
    //     if (props == null) {
    //         return null;
    //     }
    //
    //     Class<?> cls = Class.forName(className);
    //     Method getHostMethod = cls.getDeclaredMethod(GET_HOST_NAME);
    //     Method getPortMethod = cls.getDeclaredMethod(GET_PORT_NAME);
    //
    //     String host = (String) getHostMethod.invoke(props);
    //     int port = (Integer) getPortMethod.invoke(props);
    //
    //     return new ProxyConfig(host, port);
    // } catch (ClassNotFoundException ex) {
    //     Log.e(TAG, "Using no proxy configuration due to exception:" + ex);
    //     return null;
    // } catch (NoSuchMethodException ex) {
    //     Log.e(TAG, "Using no proxy configuration due to exception:" + ex);
    //     return null;
    // } catch (IllegalAccessException ex) {
    //     Log.e(TAG, "Using no proxy configuration due to exception:" + ex);
    //     return null;
    // } catch (InvocationTargetException ex) {
    //     Log.e(TAG, "Using no proxy configuration due to exception:" + ex);
    //     return null;
    // } catch (NullPointerException ex) {
    //     Log.e(TAG, "Using no proxy configuration due to exception:" + ex);
    //     return null;
    // }

    assert(NULL != mOwner);
    assert(0);
    //try {
        const String GET_HOST_NAME("getHost");
        const String GET_PORT_NAME("getPort");
        String sClassName;
        String sProxyInfo;
        if (Build::VERSION::SDK_INT <= Build::VERSION_CODES::KITKAT) {
            sClassName = String("android.net.ProxyProperties");
            sProxyInfo = String("proxy");
        }
        else {
            sClassName = String("android.net.ProxyInfo");
            sProxyInfo = String("android.intent.extra.PROXY_INFO");
        }

        AutoPtr<IInterface> props;
        AutoPtr<IBundle> extras;
        intent->GetExtras((IBundle**)&extras);
        extras->Get(sProxyInfo, (IInterface**)&props);
        if (NULL == props) {
            AutoPtr<ProxyChangeListener::ProxyConfig> ret = NULL;
            return ret;
        }

        AutoPtr<IProxyProperties> proxyProperties;
        AutoPtr<IProxyInfo> proxyInfo;
        String host;
        Int32 port;
        if (Build::VERSION::SDK_INT <= Build::VERSION_CODES::KITKAT) {
            //--:CProxyProperties::New((IProxyProperties**)&proxyProperties);
            proxyProperties->GetHost(&host);
            proxyProperties->GetPort(&port);
        }
        else {
            AutoPtr<IProxyInfoHelper> helper;
            //--CProxyInfoHelper::AcquireSingleton((IProxyInfoHelper**)&helper);
            //--helper->GetInstance((IProxyInfo**)&proxyInfo);
            proxyInfo->GetHost(&host);
            proxyInfo->GetPort(&port);
        }
        AutoPtr<ProxyChangeListener::ProxyConfig> result = new ProxyChangeListener::ProxyConfig(host, port);
        return result;
    //} catch (ClassNotFoundException ex) {
    //    Log.e(TAG, "Using no proxy configuration due to exception:" + ex);
    //    return null;
    //} catch (NoSuchMethodException ex) {
    //    Log.e(TAG, "Using no proxy configuration due to exception:" + ex);
    //    return null;
    //} catch (IllegalAccessException ex) {
    //    Log.e(TAG, "Using no proxy configuration due to exception:" + ex);
    //    return null;
    //} catch (InvocationTargetException ex) {
    //    Log.e(TAG, "Using no proxy configuration due to exception:" + ex);
    //    return null;
    //} catch (NullPointerException ex) {
    //    Log.e(TAG, "Using no proxy configuration due to exception:" + ex);
    //    return null;
    //}
}

//=====================================================================
//                         ProxyChangeListener
//=====================================================================
const String ProxyChangeListener::TAG("ProxyChangeListener");
Boolean ProxyChangeListener::sEnabled = true;

ECode ProxyChangeListener::SetEnabled(
    /* [in] */ Boolean enabled)
{
    // ==================before translated======================
    // sEnabled = enabled;

    sEnabled = enabled;
    return NOERROR;
}

ECode ProxyChangeListener::SetDelegateForTesting(
    /* [in] */ Delegate* delegate)
{
    VALIDATE_NOT_NULL(delegate);
    // ==================before translated======================
    // mDelegate = delegate;

    mDelegate = delegate;
    return NOERROR;
}

AutoPtr<ProxyChangeListener> ProxyChangeListener::Create(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // return new ProxyChangeListener(context);

    AutoPtr<ProxyChangeListener> result = new ProxyChangeListener(context);
    return result;
}

String ProxyChangeListener::GetProperty(
    /* [in] */ const String& property)
{
    // ==================before translated======================
    // return System.getProperty(property);

    assert(0);
    String result;
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    sys->GetProperty(property, &result);
    return result;
}

ECode ProxyChangeListener::Start(
    /* [in] */ Int64 nativePtr)
{
    // ==================before translated======================
    // assert mNativePtr == 0;
    // mNativePtr = nativePtr;
    // registerReceiver();

    assert(0 != mNativePtr);
    mNativePtr = nativePtr;
    RegisterReceiver();
    return NOERROR;
}

ECode ProxyChangeListener::Stop()
{
    // ==================before translated======================
    // mNativePtr = 0;
    // unregisterReceiver();

    mNativePtr = 0;
    UnregisterReceiver();
    return NOERROR;
}

ProxyChangeListener::ProxyChangeListener(
    /* [in] */ IContext* context)
    : mContext(context)
{
    // ==================before translated======================
    // mContext = context;
}

ECode ProxyChangeListener::ProxySettingsChanged(
    /* [in] */ ProxyChangeListener::ProxyConfig* cfg)
{
    VALIDATE_NOT_NULL(cfg);
    // ==================before translated======================
    // if (!sEnabled) {
    //     return;
    // }
    // if (mDelegate != null) {
    //     mDelegate.proxySettingsChanged();
    // }
    // if (mNativePtr == 0) {
    //     return;
    // }
    // // Note that this code currently runs on a MESSAGE_LOOP_UI thread, but
    // // the C++ code must run the callbacks on the network thread.
    // if (cfg != null) {
    //     nativeProxySettingsChangedTo(mNativePtr, cfg.mHost, cfg.mPort);
    // } else {
    //     nativeProxySettingsChanged(mNativePtr);
    // }

    assert(0);
    if (!sEnabled) {
        return NOERROR;
    }
    if (mDelegate != NULL) {
        mDelegate->ProxySettingsChanged();
    }
    if (0 == mNativePtr) {
        return NOERROR;
    }
    // Note that this code currently runs on a MESSAGE_LOOP_UI thread, but
    // the C++ code must run the callbacks on the network thread.
    if (cfg != NULL) {
        NativeProxySettingsChangedTo(mNativePtr, cfg->mHost, cfg->mPort);
    }
    else {
        NativeProxySettingsChanged(mNativePtr);
    }
    return NOERROR;
}

ECode ProxyChangeListener::RegisterReceiver()
{
    // ==================before translated======================
    // if (mProxyReceiver != null) {
    //     return;
    // }
    // IntentFilter filter = new IntentFilter();
    // filter.addAction(Proxy.PROXY_CHANGE_ACTION);
    // mProxyReceiver = new ProxyReceiver();
    // mContext.getApplicationContext().registerReceiver(mProxyReceiver, filter);

    assert(0);
    if (mProxyReceiver != NULL) {
        return NOERROR;
    }
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IProxy::PROXY_CHANGE_ACTION);
    mProxyReceiver = new ProxyReceiver(this);
    AutoPtr<IContext> context;
    mContext->GetApplicationContext((IContext**)&context);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mProxyReceiver, filter, (IIntent**)&intent);
    return NOERROR;
}

ECode ProxyChangeListener::UnregisterReceiver()
{
    // ==================before translated======================
    // if (mProxyReceiver == null) {
    //     return;
    // }
    // mContext.unregisterReceiver(mProxyReceiver);
    // mProxyReceiver = null;

    if (mProxyReceiver == NULL) {
        return NOERROR;
    }
    mContext->UnregisterReceiver(mProxyReceiver);
    mProxyReceiver = NULL;
    return NOERROR;
}

ECode ProxyChangeListener::NativeProxySettingsChangedTo(
    /* [in] */ Int64 nativePtr,
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
{
    Elastos_ProxyChangeListener_nativeProxySettingsChangedTo(THIS_PROBE(IInterface), (Handle32)nativePtr, host, port);
    return NOERROR;
}

ECode ProxyChangeListener::NativeProxySettingsChanged(
    /* [in] */ Int64 nativePtr)
{
    Elastos_ProxyChangeListener_nativeProxySettingsChanged(THIS_PROBE(IInterface), (Handle32)nativePtr);
    return NOERROR;
}

AutoPtr<IInterface> ProxyChangeListener::Create(
    /* [in] */ IInterface* context)
{
    IContext* c = IContext::Probe(context);
    return TO_IINTERFACE(Create(c));
}

void ProxyChangeListener::Start(
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 nativePtr)
{
    ProxyChangeListener* mObj = (ProxyChangeListener*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ProxyChangeListener", "ProxyChangeListener::Start, mObj is NULL");
        return;
    }
    mObj->Start(nativePtr);
}

void ProxyChangeListener::Stop(
    /* [in] */ IInterface* obj)
{
    ProxyChangeListener* mObj = (ProxyChangeListener*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ProxyChangeListener", "ProxyChangeListener::Stop, mObj is NULL");
        return;
    }
    mObj->Stop();
}

} // namespace Net
} // namespace Webkit
} // namespace Droid
} // namespace Elastos




#include "CBrowserPlugin.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BrowserPlugin {

PInterface CBrowserPlugin::Probe(
    /* [in] */ REIID riid)
{
    //if (riid == EIID_IInterface) {
    //    return (IInterface*)(IBaseInputConnection *)this;
    //}
    return NULL;
}

UInt32 CBrowserPlugin::AddRef()
{
    return Elastos::Droid::App::Service::AddRef();
}

UInt32 CBrowserPlugin::Release()
{
    return Elastos::Droid::App::Service::Release();
}

ECode CBrowserPlugin::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    //VALIDATE_NOT_NULL(iid);
    //if (object == (IInterface*)(IBaseInputConnection *)this) {
    //    *iid = EIID_IBaseInputConnection;
    //}
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
    //return NOERROR;
}

ECode CBrowserPlugin::OnBind(IIntent* intent, IBinder** binder)
{
    return NOERROR;
}

}
}
}
}



#include <node.h>

#include <nan.h>

#include "macros.h"

#include "nan-ext.h"

#include "car-object.h"
#include "car-object-adapter.h"
#include "error.h"
#include "parse-uri.h"
#include "require.h"



using namespace node;

using namespace Nan;

using namespace v8;

CAR_BRIDGE_NAMESPACE_BEGIN

static NAN_METHOD(Require)
{
    try {
        ::Nan::HandleScope scope;

        Local<Value> arg0;

        if (info.Length() < 1)
            throw Error(Error::INVALID_ARGUMENT, "");

        arg0 = info[0];
        if (!arg0->IsString())
            throw Error(Error::INVALID_ARGUMENT, "");

        ParseURI parseURI(*Utf8String(arg0));

        NAN_METHOD_RETURN_VALUE(
                Require(parseURI.ecoPath(),
                    parseURI.major(), parseURI.minor(), parseURI.build(), parseURI.revision(),
                    parseURI.nEntryIds(), parseURI.entryIds())
                );
    } catch (Error const &error) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(error));
    } catch (...) {
        ::Nan::HandleScope scope;

        ThrowError(ToValue(Error(Error::FAILED, "")));
    }
}

static NAN_MODULE_INIT(Initialize)
{
    CARObject::Initialize();

    CARObjectAdapter::Initialize();

    Export(target, "version", New("0.0.0").ToLocalChecked());

    Export(target, "require", Require);
}

NODE_MODULE(CARBridge, Initialize);

CAR_BRIDGE_NAMESPACE_END


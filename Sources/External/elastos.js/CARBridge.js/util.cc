
#include <cstddef>

#include <memory>
#include <new>
#include <string>
#include <unordered_map>

#include <node.h>

#include <nan.h>

#include <elastos.h>

#include "macros.h"

#include "nan-ext.h"

#include "error.h"
#include "js-2-car.h"
#include "weak-external-base.h"



using namespace std;

using namespace node;

using namespace Nan;

using namespace v8;

_ELASTOS_NAMESPACE_USING

CAR_BRIDGE_NAMESPACE_BEGIN

struct _Aspects: WeakExternalBase {
    struct Hash: __hash_base<size_t, ClassID> {
        size_t operator()(ClassID const &id) const
        {
            size_t value;

            value = hash<UInt32>()(id.mClsid.mData1);

            value <<= 1;
            value ^= hash<UInt16>()(id.mClsid.mData2);

            value <<= 1;
            value ^= hash<UInt16>()(id.mClsid.mData3);

            for (size_t i = 0; i < 8; ++i) {
                value <<= 1;
                value ^= hash<UInt8>()(id.mClsid.mData4[i]);
            }

            value <<= 1;
            value ^= hash<string>()(id.mUunm);

            value <<= 1;
            value ^= hash<UInt32>()(id.mCarcode);

            return value;
        }
    };

    unordered_map<ClassID, AutoPtr<IAspect>, Hash> mapIdToAspect;

private:
    ~_Aspects() = default;
};

IInterface *Probe(Local<Object> object, EIID const &iid)
{
    if (IsCARObject(object)) {
        IInterface *carObject;

        carObject = AsCARObject(object);

        return carObject->Probe(iid);
    } else {
        struct _Aspects *aspects;

        if (!Has(object, New(".__attachedAspects__").ToLocalChecked()).FromJust())
            return 0;

        aspects =
            (struct _Aspects *)
            Get(object, New(".__attachedAspects__").ToLocalChecked()).ToLocalChecked().As<External>()->Value();

        for (auto it = aspects->mapIdToAspect.begin(), end = aspects->mapIdToAspect.end(); it != end; ++it) {
            IInterface *interface_;

            interface_ = it->second->Probe(iid);
            if (interface_ != 0)
                return interface_;
        }

        return 0;
    }
}

void AttachAspect(Local<Object> object, ClassID const &aspectId)
{
    ECode ec;

    AutoPtr<IAspect> aspect;
    IAspect *_aspect;

    ec = _CObject_CreateInstance(aspectId, RGM_SAME_DOMAIN, EIID_IAspect, reinterpret_cast<IInterface **>(&_aspect));
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    aspect = _aspect, _aspect->Release();

    if (IsCARObject(object)) {
        IInterface *carObject;

        IObject *_carObject;

        carObject = AsCARObject(object);

        _carObject = (IObject *)carObject->Probe(EIID_IObject);
        if (_carObject == 0)
            throw Error(Error::INVALID_ARGUMENT, "");

        ec = aspect->AspectAggregate(AggrType_Aggregate, _carObject);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");
    } else {
        struct _Aspects *aspects;

        if (!Has(object, New(".__attachedAspects__").ToLocalChecked()).FromJust()) {
            unique_ptr<struct _Aspects, _Aspects::Deleter> _aspects(new(nothrow) struct _Aspects);
            if (_aspects == nullptr)
                throw Error(Error::NO_MEMORY, "");

            DefineOwnProperty(object,
                    New(".__attachedAspects__").ToLocalChecked(),
                    _aspects->self(),
                    static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

            aspects = _aspects.release();
        } else
            aspects =
                (struct _Aspects *)
                Get(object, New(".__attachedAspects__").ToLocalChecked()).ToLocalChecked().As<External>()->Value();

        aspects->mapIdToAspect[aspectId] = aspect;
    }
}

void DetachAspect(Local<Object> object, ClassID const &aspectId)
{
    if (IsCARObject(object)) {
        IInterface *carObject;

        IObject *_carObject;

        ECode ec;

        carObject = AsCARObject(object);

        _carObject = (IObject *)carObject->Probe(EIID_IObject);
        if (_carObject == 0)
            throw Error(Error::INVALID_ARGUMENT, "");

        ec = _carObject->Aggregate(AggrType_Unaggregate,
                const_cast<IInterface *>(reinterpret_cast<IInterface const *>(&aspectId)));
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");
    } else {
        struct _Aspects *aspects;

        if (!Has(object, New(".__attachedAspects__").ToLocalChecked()).FromJust())
            return;

        aspects =
            (struct _Aspects *)
            Get(object, New(".__attachedAspects__").ToLocalChecked()).ToLocalChecked().As<External>()->Value();

        aspects->mapIdToAspect.erase(aspectId);
    }
}

CAR_BRIDGE_NAMESPACE_END


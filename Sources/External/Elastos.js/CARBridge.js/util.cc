
#include <node.h>

#include <nan.h>

#include <elastos.h>

#include "macros.h"

#include "map.h"



using namespace node;

using namespace Nan;

using namespace v8;

_ELASTOS_NAMESPACE_USING

CAR_BRIDGE_NAMESPACE_BEGIN

struct _Aspects: WeakExternalBase {
    Map<ClassID, AutoPtr<IAspect>> mapIdToAspect;

private:
    ~_Aspects() = default;
};

IInterface *Probe(Local<Object> object, EIID const &iid)
{
    if (IsCARObject(object)) {
        AutoPtr<IInterface> carObject;
        IInterface *_carObject;

        _carObject = ToCARObject(object);
        carObject = _carObject, _carObject->Release();

        return carObject->Probe(iid);
    } else {
        struct _Aspects *aspects;

        if (!Has(object, New(".__attachedAspects__").ToLocalChecked()).FromJust())
            return 0;

        aspects =
            (struct _Aspects *)
            Get(object, New(".__attachedAspects__").ToLocalChecked()).ToLocalChecked().As<External>()->Value();

        for (auto it = aspects->mapIdToAspect.Begin(), end = aspects->mapIdToAspect.End(); it != end; ++it) {
            IInterface *interface_;

            interface_ = it->Second->Probe(iid);
            if (interface_ != 0)
                return interface_;
        }

        return 0;
    }
}

void AttachAspect(Local<Object> object, ClassID const &aspectId)
{
    AutoPtr<IAspect> aspect;
    IAspect *_aspect;

    ECode ec;

    ec = _CObject_CreateInstance(aspectId, RGM_SAME_DOMAIN, EIID_IAspect, &_aspect);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    aspect = _aspect, _aspect->Release();

    if (IsCARObject(object)) {
        AutoPtr<IInterface> carObject;
        IInterface *_carObject;

        IObject *__carObject;

        _carObject = ToCARObject(object);
        carObject = _carObject, _carObject->Release();

        __carObject = (IObject *)carObject->Probe(EIID_IObject);
        if (__carObject == 0)
            throw Error(Error::INVALID_ARGUMENT, "");

        ec = aspect->AspectAggregate(AggrType_Aggregate, __carObject);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");
    } else {
        struct _Aspects *aspects;

        if (!Has(object, New(".__attachedAspects__").ToLocalChecked()).FromJust()) {
            UniquePtr<struct _Aspects, &_Aspects::Delete<struct _Aspects>> _aspects;

            _aspects = new(NO_THROW) struct _Aspects;
            if (_aspects == nullptr)
                throw Error(Error::NO_MEMORY, "");

            DefineOwnProperty(object,
                    New(".__attachedAspects__").ToLocalChecked(),
                    _aspects->self(),
                    static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

            aspects = _aspects.Release();
        } else
            aspects =
                (struct _Aspects *)
                Get(object, New(".__attachedAspects__").ToLocalChecked()).ToLocalChecked().As<External>()->Value();

        aspects->mapIdToAspect[aspectId] = aspect;
    }
}

void DetachAspect(Local<Object> object, ClassID const &aspectId)
{
    ECode ec;

    if (IsCARObject(object)) {
        AutoPtr<IInterface> carObject;
        IInterface *_carObject;

        IObject *__carObject;

        _carObject = ToCARObject(object);
        carObject = _carObject, _carObject->Release();

        __carObject = (IObject *)carObject->Probe(EIID_IObject);
        if (__carObject == 0)
            throw Error(Error::INVALID_ARGUMENT, "");

        ec = __carObject->Aggregate(AggrType_Unaggregate, aspectId);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");
    } else {
        struct _Aspects *aspects;

        if (!Has(object, New(".__attachedAspects__").ToLocalChecked()).FromJust())
            return;

        aspects =
            (struct _Aspects *)
            Get(object, New(".__attachedAspects__").ToLocalChecked()).ToLocalChecked().As<External>()->Value();

        aspects->mapIdToAspect.Erase(aspectId);
    }
}

CAR_BRIDGE_NAMESPACE_END


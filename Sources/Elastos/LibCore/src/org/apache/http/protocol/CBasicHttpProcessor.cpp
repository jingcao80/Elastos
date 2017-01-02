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

#include "org/apache/http/protocol/CBasicHttpProcessor.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL_6(CBasicHttpProcessor, Object, IBasicHttpProcessor, IHttpRequestInterceptor
        , IHttpResponseInterceptor, IHttpRequestInterceptorList, IHttpResponseInterceptorList, ICloneable)

CAR_OBJECT_IMPL(CBasicHttpProcessor)

ECode CBasicHttpProcessor::AddRequestInterceptor(
    /* [in] */ IHttpRequestInterceptor* itcp)
{
    if (itcp == NULL) {
        return NOERROR;
    }
    if (mRequestInterceptors == NULL) {
        AutoPtr<IArrayList> al;
        CArrayList::New((IArrayList**)&al);
        mRequestInterceptors = IList::Probe(al);
    }
    mRequestInterceptors->Add(itcp);
    return NOERROR;
}

ECode CBasicHttpProcessor::AddRequestInterceptor(
    /* [in] */ IHttpRequestInterceptor* itcp,
    /* [in] */ Int32 index)
{
    if (index < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (itcp == NULL) {
        return NOERROR;
    }

    if (mRequestInterceptors == NULL) {
        if (index > 0) {
            return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        AutoPtr<IArrayList> al;
        CArrayList::New((IArrayList**)&al);
        mRequestInterceptors = IList::Probe(al);
    }
    mRequestInterceptors->Add(index, itcp);
    return NOERROR;
}

ECode CBasicHttpProcessor::AddResponseInterceptor(
    /* [in] */ IHttpResponseInterceptor* itcp,
    /* [in] */ Int32 index)
{
    if (index < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (itcp == NULL) {
        return NOERROR;
    }

    if (mResponseInterceptors == NULL) {
        if (index > 0) {
            return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        AutoPtr<IArrayList> al;
        CArrayList::New((IArrayList**)&al);
        mResponseInterceptors = IList::Probe(al);
    }
    mResponseInterceptors->Add(index, itcp);
    return NOERROR;
}

ECode CBasicHttpProcessor::RemoveRequestInterceptorByClass(
    /* [in] */ IInterface* clazz /*Class clazz*/)
{
    if (mRequestInterceptors == NULL) {
        return NOERROR;
    }
    AutoPtr<IIterator> it;
    mRequestInterceptors->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        Boolean equals;
        if (IObject::Probe(value)->Equals(clazz, &equals), equals) {
            it->Remove();
        }
    }
    return NOERROR;
}

ECode CBasicHttpProcessor::RemoveResponseInterceptorByClass(
    /* [in] */ IInterface* clazz /*Class clazz*/)
{
    if (mResponseInterceptors == NULL) {
        return NOERROR;
    }
    AutoPtr<IIterator> it;
    mResponseInterceptors->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        Boolean equals;
        if (IObject::Probe(value)->Equals(clazz, &equals), equals) {
            it->Remove();
        }
    }
    return NOERROR;
}

ECode CBasicHttpProcessor::AddInterceptor(
    /* [in] */ IHttpRequestInterceptor* interceptor)
{
    return AddRequestInterceptor(interceptor);
}

ECode CBasicHttpProcessor::AddInterceptor(
    /* [in] */ IHttpRequestInterceptor* interceptor,
    /* [in] */ Int32 index)
{
    return AddRequestInterceptor(interceptor, index);
}

ECode CBasicHttpProcessor::GetRequestInterceptorCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;
    if (mRequestInterceptors != NULL) {
        return mRequestInterceptors->GetSize(count);
    }
    return NOERROR;
}

ECode CBasicHttpProcessor::GetRequestInterceptor(
    /* [in] */ Int32 index,
    /* [out] */ IHttpRequestInterceptor** interceptor)
{
    VALIDATE_NOT_NULL(interceptor)
    Int32 size;
    if ((mRequestInterceptors == NULL) ||
            (index < 0) || (mRequestInterceptors->GetSize(&size), index >= size)) {
        *interceptor = NULL;
        return NOERROR;
    }

    AutoPtr<IInterface> value;
    mRequestInterceptors->Get(index, (IInterface**)&value);
    *interceptor = IHttpRequestInterceptor::Probe(value);
    REFCOUNT_ADD(*interceptor)
    return NOERROR;
}

ECode CBasicHttpProcessor::ClearRequestInterceptors()
{
    mRequestInterceptors = NULL;
    return NOERROR;
}

ECode CBasicHttpProcessor::AddResponseInterceptor(
    /* [in] */ IHttpResponseInterceptor* itcp)
{
    if (itcp == NULL) {
        return NOERROR;
    }
    if (mResponseInterceptors == NULL) {
        AutoPtr<IArrayList> al;
        CArrayList::New((IArrayList**)&al);
        mResponseInterceptors = IList::Probe(al);
    }
    mResponseInterceptors->Add(itcp);
    return NOERROR;
}

ECode CBasicHttpProcessor::AddInterceptor(
    /* [in] */ IHttpResponseInterceptor* interceptor)
{
    return AddResponseInterceptor(interceptor);
}

ECode CBasicHttpProcessor::AddInterceptor(
    /* [in] */ IHttpResponseInterceptor* interceptor,
    /* [in] */ Int32 index)
{
    return AddResponseInterceptor(interceptor, index);
}

ECode CBasicHttpProcessor::GetResponseInterceptorCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    if (mResponseInterceptors != NULL) {
        return mResponseInterceptors->GetSize(count);
    }
    *count = 0;
    return NOERROR;
}

ECode CBasicHttpProcessor::GetResponseInterceptor(
    /* [in] */ Int32 index,
    /* [out] */ IHttpResponseInterceptor** interceptor)
{
    VALIDATE_NOT_NULL(interceptor)
    Int32 size;
    if ((mResponseInterceptors == NULL) ||
            (index < 0) || (mResponseInterceptors->GetSize(&size), index >= size)) {
        *interceptor = NULL;
        return NOERROR;
    }

    AutoPtr<IInterface> value;
    mResponseInterceptors->Get(index, (IInterface**)&value);
    *interceptor = IHttpResponseInterceptor::Probe(value);
    REFCOUNT_ADD(*interceptor)
    return NOERROR;
}

ECode CBasicHttpProcessor::ClearResponseInterceptors()
{
    mResponseInterceptors = NULL;
    return NOERROR;
}

ECode CBasicHttpProcessor::SetInterceptors(
    /* [in] */ IList* itcps)
{
    if (itcps == NULL) {
        Logger::E("CBasicHttpProcessor", "List must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mRequestInterceptors != NULL) {
        mRequestInterceptors->Clear();
    }
    if (mResponseInterceptors != NULL) {
        mResponseInterceptors->Clear();
    }
    AutoPtr<IIterator> it;
    itcps->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<IHttpRequestInterceptor> request = IHttpRequestInterceptor::Probe(value);
        if (request != NULL) {
            AddInterceptor(request);
        }
        AutoPtr<IHttpResponseInterceptor> response = IHttpResponseInterceptor::Probe(value);
        if (response != NULL) {
            AddInterceptor(response);
        }
    }
    return NOERROR;
}

ECode CBasicHttpProcessor::ClearInterceptors()
{
    ClearRequestInterceptors();
    ClearResponseInterceptors();
    return NOERROR;
}

ECode CBasicHttpProcessor::Process(
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context)
{
    if (mRequestInterceptors != NULL) {
        AutoPtr<IIterator> it;
        mRequestInterceptors->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while(it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> value;
            it->GetNext((IInterface**)&value);
            AutoPtr<IHttpRequestInterceptor> interceptor = IHttpRequestInterceptor::Probe(value);
            interceptor->Process(request, context);
        }
    }
    return NOERROR;
}

ECode CBasicHttpProcessor::Process(
    /* [in] */ IHttpResponse* request,
    /* [in] */ IHttpContext* context)
{
    if (mResponseInterceptors != NULL) {
        AutoPtr<IIterator> it;
        mResponseInterceptors->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while(it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> value;
            it->GetNext((IInterface**)&value);
            AutoPtr<IHttpResponseInterceptor> interceptor = IHttpResponseInterceptor::Probe(value);
            interceptor->Process(request, context);
        }
    }
    return NOERROR;
}

void CBasicHttpProcessor::CopyInterceptors(
    /* [in] */ IBasicHttpProcessor* target)
{
    AutoPtr<CBasicHttpProcessor> processor = (CBasicHttpProcessor*)target;
    if (mRequestInterceptors != NULL) {
        AutoPtr<IArrayList> al;
        AutoPtr<ICollection> collection =ICollection::Probe(mRequestInterceptors);
        CArrayList::New(collection, (IArrayList**)&al);
        processor->mRequestInterceptors = IList::Probe(al);
    }
    if (mResponseInterceptors != NULL) {
        AutoPtr<IArrayList> al;
        AutoPtr<ICollection> collection =ICollection::Probe(mResponseInterceptors);
        CArrayList::New(collection, (IArrayList**)&al);
        processor->mResponseInterceptors = IList::Probe(al);\
    }
}

ECode CBasicHttpProcessor::Copy(
    /* [out] */ IBasicHttpProcessor** processor)
{
    VALIDATE_NOT_NULL(processor)
    CBasicHttpProcessor::New(processor);
    CopyInterceptors(*processor);
    return NOERROR;
}

ECode CBasicHttpProcessor::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<IBasicHttpProcessor> processor;
    Copy((IBasicHttpProcessor**)&processor);
    *obj = processor->Probe(EIID_IInterface);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CBasicHttpProcessor::constructor()
{
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

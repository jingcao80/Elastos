// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8SVGElementWrapperFactory_h
#define V8SVGElementWrapperFactory_h

#include "V8SVGElement.h"
#include "V8SVGElement.h"
#include <v8.h>

namespace WebCore {

class SVGElement;

const WrapperTypeInfo* findWrapperTypeForSVGTagName(const AtomicString& name);

v8::Handle<v8::Object> createV8SVGWrapper(SVGElement*, v8::Handle<v8::Object> creationContext, v8::Isolate*);
inline v8::Handle<v8::Object> createV8SVGDirectWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return V8SVGElement::createWrapper(element, creationContext, isolate);
}
inline v8::Handle<v8::Object> createV8SVGFallbackWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return V8SVGElement::createWrapper(element, creationContext, isolate);
}

}

#endif

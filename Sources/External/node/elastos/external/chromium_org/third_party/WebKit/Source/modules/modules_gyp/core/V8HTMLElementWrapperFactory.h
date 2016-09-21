// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8HTMLElementWrapperFactory_h
#define V8HTMLElementWrapperFactory_h

#include "V8HTMLElement.h"
#include "V8HTMLUnknownElement.h"
#include <v8.h>

namespace WebCore {

class HTMLElement;

const WrapperTypeInfo* findWrapperTypeForHTMLTagName(const AtomicString& name);

v8::Handle<v8::Object> createV8HTMLWrapper(HTMLElement*, v8::Handle<v8::Object> creationContext, v8::Isolate*);
inline v8::Handle<v8::Object> createV8HTMLDirectWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return V8HTMLElement::createWrapper(element, creationContext, isolate);
}
inline v8::Handle<v8::Object> createV8HTMLFallbackWrapper(HTMLUnknownElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return V8HTMLUnknownElement::createWrapper(element, creationContext, isolate);
}

}

#endif

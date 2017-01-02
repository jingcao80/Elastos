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

#include "TransformerFactory.h"

namespace Elastosx {
namespace Xml {
namespace Transform {

CAR_INTERFACE_IMPL(TransformerFactory, Object, ITransformerFactory)

AutoPtr<ITransformerFactory> TransformerFactory::NewInstance() /*throws TransformerFactoryConfigurationError*/
{
    assert(0);
    //TODO
    // String className = "org.apache.xalan.processor.TransformerFactoryImpl";
    // try {
    //     return (TransformerFactory) Class.forName(className).newInstance();
    // } catch (Exception e) {
    //     throw new NoClassDefFoundError(className);
    // }
    return NULL;
}

AutoPtr<ITransformerFactory> TransformerFactory::NewInstance(
    /* [in] */ const String& factoryClassName,
    /* [in] */ IClassLoader* classLoader) /*throws TransformerFactoryConfigurationError*/
{
    assert(0);
    //TODO
    // if (factoryClassName == NULL) {
    //     throw new TransformerFactoryConfigurationError("factoryClassName == null");
    // }
    // if (classLoader == null) {
    //     classLoader = Thread.currentThread().getContextClassLoader();
    // }
    // try {
    //     Class<?> type = classLoader != null
    //             ? classLoader.loadClass(factoryClassName)
    //             : Class.forName(factoryClassName);
    //     return (TransformerFactory) type.newInstance();
    // } catch (ClassNotFoundException e) {
    //     throw new TransformerFactoryConfigurationError(e);
    // } catch (InstantiationException e) {
    //     throw new TransformerFactoryConfigurationError(e);
    // } catch (IllegalAccessException e) {
    //     throw new TransformerFactoryConfigurationError(e);
    // }
    return NULL;
}

} // namespace Transform
} // namespace Xml
} // namespace Elastosx

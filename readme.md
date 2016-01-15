Elastos Operating System
========================

## Overview

#### What is CAR?
CAR means The Component Assembly Runtime (CAR). It is Java with machine code. It is COM with metadata.

It is a component-oriented programming model and also a programming idea, and it is described by special standards divided in two parts: specification and implementation. The specification part tells how to define the components and how to communicate among components. Following the specification, any language can be used. Thus, CAR can be implemented in many ways. Elastos operating system implements the CAR core services.

CAR components provide service using interface, component interface needs the metadata to describe the component so that other users can know how to use the service. Metadata describe the relationship between services and calls. With this description, calling between different components becomes possible, and members of the long-range, inter-process communication can be properly carried out. The major solved problems by CAR component technology are: component from different sources can interoperate; components upgrade but affect no other component; component is independent of the programming language; transparency of component operating environment.

#### What is Elastos?
Elastos is a development framework.

The programming idea with CAR is the essence technology in Elastos, it runs through the entire technology system implementation. In Elastos both the operating system kernel and the component library provided by Elastos platform are implemented using CAR technology. Closely integrated of the operating system kernel with CAR technology runtime environment can provide strong support to Elastos architecture.

## Contribution

Your contribution to Elastos development is very welcome!

You may contribute in the following ways:

* [Repost issues and feedback]( http://elastos.org/redmine/projects/elastosrdk5 )
* Submit fixes, features via Pull Request
* Write/polish documentation


## License
Apache License Version 2.0 see http://www.apache.org/licenses/LICENSE-2.0.html

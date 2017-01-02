#=========================================================================
# Copyright (C) 2012 The Elastos Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#=========================================================================

lube.lbo :  $(MAKEDIR)/background/backcpp.lub       \
            $(MAKEDIR)/background/background.lub    \
            $(MAKEDIR)/background/backh.lub         \
            $(MAKEDIR)/background/nakedbackh.lub \
            $(MAKEDIR)/background/nakedbackcpp.lub \
            $(MAKEDIR)/background/clsobjh.lub       \
            $(MAKEDIR)/background/clsobjcpp.lub     \
            $(MAKEDIR)/background/declaspect.lub    \
            $(MAKEDIR)/background/declbase.lub      \
            $(MAKEDIR)/background/declcallback.lub  \
            $(MAKEDIR)/background/declfilter.lub    \
            $(MAKEDIR)/background/declregime.lub   \
            $(MAKEDIR)/background/declsync.lub      \
            $(MAKEDIR)/background/implaspect.lub    \
            $(MAKEDIR)/background/implbase.lub      \
            $(MAKEDIR)/background/implcallback.lub  \
            $(MAKEDIR)/background/implfilter.lub    \
            $(MAKEDIR)/background/implregime.lub   \
            $(MAKEDIR)/background/implsync.lub      \
            $(MAKEDIR)/background/modpub.lub        \
            $(MAKEDIR)/background/callbacksinkcpp.lub       \
            $(MAKEDIR)/background/callbacksinkh.lub         \
            $(MAKEDIR)/background/sinkcpp.lub       \
            $(MAKEDIR)/background/sinkh.lub         \
            $(MAKEDIR)/background/aspectintf.lub    \
            $(MAKEDIR)/background/superh.lub        \
            $(MAKEDIR)/background/genericnew.lub  \
            $(MAKEDIR)/background/makefile.lub    \
	        $(MAKEDIR)/carusage/carusage.lub        \
            $(MAKEDIR)/carusage/methodusage.lub     \
            $(MAKEDIR)/carusage/interfaceusage.lub  \
            $(MAKEDIR)/carusage/classusage.lub      \
            $(MAKEDIR)/carusage/objectusage.lub     \
            $(MAKEDIR)/carusage/callbackusage.lub   \
            $(MAKEDIR)/carusage/genericusage.lub    \
            $(MAKEDIR)/carusage/aspectusage.lub     \
            $(MAKEDIR)/carusage/regimeusage.lub    \
            $(MAKEDIR)/carusage/enumusage.lub       \
            $(MAKEDIR)/carusage/constusage.lub      \
            $(MAKEDIR)/carusage/structusage.lub     \
            $(MAKEDIR)/foreground/forecpp.lub       \
            $(MAKEDIR)/foreground/forecpph.lub      \
            $(MAKEDIR)/foreground/foregeneric.lub   \
            $(MAKEDIR)/foreground/foreground.lub    \
            $(MAKEDIR)/foreground/foreh.lub         \
            $(MAKEDIR)/foreground/nakedforeh.lub     \
            $(MAKEDIR)/foreground/nakedforecpp.lub   \
            $(MAKEDIR)/foreground/sources.lub       \
            $(MAKEDIR)/header/baseh.lub             \
            $(MAKEDIR)/header/trigger.lub           \
            $(MAKEDIR)/header/classobj.lub          \
            $(MAKEDIR)/header/basenew.lub           \
            $(MAKEDIR)/header/nakedbasenew.lub      \
            $(MAKEDIR)/header/supernew.lub          \
            $(MAKEDIR)/header/classnew.lub          \
            $(MAKEDIR)/header/nakedclassnew.lub     \
            $(MAKEDIR)/header/supercallback.lub     \
            $(MAKEDIR)/header/clientcallback.lub    \
            $(MAKEDIR)/header/cstyleh.lub           \
            $(MAKEDIR)/header/decltype.lub          \
            $(MAKEDIR)/header/decluuid.lub          \
            $(MAKEDIR)/header/header.lub            \
            $(MAKEDIR)/header/header2.lub           \
            $(MAKEDIR)/header/clienth.lub           \
            $(MAKEDIR)/header/serverh.lub           \
            $(MAKEDIR)/header/genericcallback.lub \
            $(MAKEDIR)/headercpp/headercpp.lub     \
            $(MAKEDIR)/headercpp/basecpp.lub       \
            $(MAKEDIR)/public/cls2abrg.lub          \
            $(MAKEDIR)/public/cls2car.lub           \
            $(MAKEDIR)/public/public.lub            \
            $(MAKEDIR)/public/uuidcpp.lub           \
            $(MAKEDIR)/public/uuidh.lub

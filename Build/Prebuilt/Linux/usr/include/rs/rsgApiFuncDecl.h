/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

extern "C" void rsContextDestroy (RsContext rsc);
extern "C" RsMessageToClientType rsContextGetMessage (RsContext rsc, void * data, size_t data_length, size_t * receiveLen, size_t receiveLen_length, uint32_t * usrID, size_t usrID_length);
extern "C" RsMessageToClientType rsContextPeekMessage (RsContext rsc, size_t * receiveLen, size_t receiveLen_length, uint32_t * usrID, size_t usrID_length);
extern "C" void rsContextSendMessage (RsContext rsc, uint32_t id, const uint8_t * data, size_t data_length);
extern "C" void rsContextInitToClient (RsContext rsc);
extern "C" void rsContextDeinitToClient (RsContext rsc);
extern "C" RsType rsTypeCreate (RsContext rsc, RsElement e, uint32_t dimX, uint32_t dimY, uint32_t dimZ, bool mipmaps, bool faces, uint32_t yuv);
extern "C" RsAllocation rsAllocationCreateTyped (RsContext rsc, RsType vtype, RsAllocationMipmapControl mipmaps, uint32_t usages, uintptr_t ptr);
extern "C" RsAllocation rsAllocationCreateFromBitmap (RsContext rsc, RsType vtype, RsAllocationMipmapControl mipmaps, const void * data, size_t data_length, uint32_t usages);
extern "C" RsAllocation rsAllocationCubeCreateFromBitmap (RsContext rsc, RsType vtype, RsAllocationMipmapControl mipmaps, const void * data, size_t data_length, uint32_t usages);
extern "C" RsNativeWindow rsAllocationGetSurface (RsContext rsc, RsAllocation alloc);
extern "C" void rsAllocationSetSurface (RsContext rsc, RsAllocation alloc, RsNativeWindow sur);
extern "C" void rsContextFinish (RsContext rsc);
extern "C" void rsContextDump (RsContext rsc, int32_t bits);
extern "C" void rsContextSetPriority (RsContext rsc, int32_t priority);
extern "C" void rsContextDestroyWorker (RsContext rsc);
extern "C" void rsAssignName (RsContext rsc, RsObjectBase obj, const char * name, size_t name_length);
extern "C" void rsObjDestroy (RsContext rsc, RsAsyncVoidPtr objPtr);
extern "C" RsElement rsElementCreate (RsContext rsc, RsDataType mType, RsDataKind mKind, bool mNormalized, uint32_t mVectorSize);
extern "C" RsElement rsElementCreate2 (RsContext rsc, const RsElement * elements, size_t elements_length, const char ** names, size_t names_length_length, const size_t * names_length, const uint32_t * arraySize, size_t arraySize_length);
extern "C" void rsAllocationCopyToBitmap (RsContext rsc, RsAllocation alloc, void * data, size_t data_length);
extern "C" void * rsAllocationGetPointer (RsContext rsc, RsAllocation va, uint32_t lod, RsAllocationCubemapFace face, uint32_t z, uint32_t array, size_t * stride, size_t stride_length);
extern "C" void rsAllocation1DData (RsContext rsc, RsAllocation va, uint32_t xoff, uint32_t lod, uint32_t count, const void * data, size_t data_length);
extern "C" void rsAllocation1DElementData (RsContext rsc, RsAllocation va, uint32_t x, uint32_t lod, const void * data, size_t data_length, size_t comp_offset);
extern "C" void rsAllocation2DData (RsContext rsc, RsAllocation va, uint32_t xoff, uint32_t yoff, uint32_t lod, RsAllocationCubemapFace face, uint32_t w, uint32_t h, const void * data, size_t data_length, size_t stride);
extern "C" void rsAllocation3DData (RsContext rsc, RsAllocation va, uint32_t xoff, uint32_t yoff, uint32_t zoff, uint32_t lod, uint32_t w, uint32_t h, uint32_t d, const void * data, size_t data_length, size_t stride);
extern "C" void rsAllocationGenerateMipmaps (RsContext rsc, RsAllocation va);
extern "C" void rsAllocationRead (RsContext rsc, RsAllocation va, void * data, size_t data_length);
extern "C" void rsAllocation1DRead (RsContext rsc, RsAllocation va, uint32_t xoff, uint32_t lod, uint32_t count, void * data, size_t data_length);
extern "C" void rsAllocation2DRead (RsContext rsc, RsAllocation va, uint32_t xoff, uint32_t yoff, uint32_t lod, RsAllocationCubemapFace face, uint32_t w, uint32_t h, void * data, size_t data_length, size_t stride);
extern "C" void rsAllocationSyncAll (RsContext rsc, RsAllocation va, RsAllocationUsageType src);
extern "C" void rsAllocationResize1D (RsContext rsc, RsAllocation va, uint32_t dimX);
extern "C" void rsAllocationCopy2DRange (RsContext rsc, RsAllocation dest, uint32_t destXoff, uint32_t destYoff, uint32_t destMip, uint32_t destFace, uint32_t width, uint32_t height, RsAllocation src, uint32_t srcXoff, uint32_t srcYoff, uint32_t srcMip, uint32_t srcFace);
extern "C" void rsAllocationCopy3DRange (RsContext rsc, RsAllocation dest, uint32_t destXoff, uint32_t destYoff, uint32_t destZoff, uint32_t destMip, uint32_t width, uint32_t height, uint32_t depth, RsAllocation src, uint32_t srcXoff, uint32_t srcYoff, uint32_t srcZoff, uint32_t srcMip);
extern "C" RsSampler rsSamplerCreate (RsContext rsc, RsSamplerValue magFilter, RsSamplerValue minFilter, RsSamplerValue wrapS, RsSamplerValue wrapT, RsSamplerValue wrapR, float mAniso);
extern "C" void rsScriptBindAllocation (RsContext rsc, RsScript vtm, RsAllocation va, uint32_t slot);
extern "C" void rsScriptSetTimeZone (RsContext rsc, RsScript s, const char * timeZone, size_t timeZone_length);
extern "C" void rsScriptInvoke (RsContext rsc, RsScript s, uint32_t slot);
extern "C" void rsScriptInvokeV (RsContext rsc, RsScript s, uint32_t slot, const void * data, size_t data_length);
extern "C" void rsScriptForEach (RsContext rsc, RsScript s, uint32_t slot, RsAllocation ain, RsAllocation aout, const void * usr, size_t usr_length, const RsScriptCall * sc, size_t sc_length);
extern "C" void rsScriptForEachMulti (RsContext rsc, RsScript s, uint32_t slot, RsAllocation * ains, size_t ains_length, RsAllocation aout, const void * usr, size_t usr_length, const RsScriptCall * sc, size_t sc_length);
extern "C" void rsScriptSetVarI (RsContext rsc, RsScript s, uint32_t slot, int value);
extern "C" void rsScriptSetVarObj (RsContext rsc, RsScript s, uint32_t slot, RsObjectBase value);
extern "C" void rsScriptSetVarJ (RsContext rsc, RsScript s, uint32_t slot, int64_t value);
extern "C" void rsScriptSetVarF (RsContext rsc, RsScript s, uint32_t slot, float value);
extern "C" void rsScriptSetVarD (RsContext rsc, RsScript s, uint32_t slot, double value);
extern "C" void rsScriptSetVarV (RsContext rsc, RsScript s, uint32_t slot, const void * data, size_t data_length);
extern "C" void rsScriptGetVarV (RsContext rsc, RsScript s, uint32_t slot, void * data, size_t data_length);
extern "C" void rsScriptSetVarVE (RsContext rsc, RsScript s, uint32_t slot, const void * data, size_t data_length, RsElement e, const uint32_t * dims, size_t dims_length);
extern "C" RsScript rsScriptCCreate (RsContext rsc, const char * resName, size_t resName_length, const char * cacheDir, size_t cacheDir_length, const char * text, size_t text_length);
extern "C" RsScript rsScriptIntrinsicCreate (RsContext rsc, uint32_t id, RsElement eid);
extern "C" RsScriptKernelID rsScriptKernelIDCreate (RsContext rsc, RsScript sid, int slot, int sig);
extern "C" RsScriptFieldID rsScriptFieldIDCreate (RsContext rsc, RsScript sid, int slot);
extern "C" RsScriptGroup rsScriptGroupCreate (RsContext rsc, RsScriptKernelID * kernels, size_t kernels_length, RsScriptKernelID * src, size_t src_length, RsScriptKernelID * dstK, size_t dstK_length, RsScriptFieldID * dstF, size_t dstF_length, const RsType * type, size_t type_length);
extern "C" void rsScriptGroupSetOutput (RsContext rsc, RsScriptGroup group, RsScriptKernelID kernel, RsAllocation alloc);
extern "C" void rsScriptGroupSetInput (RsContext rsc, RsScriptGroup group, RsScriptKernelID kernel, RsAllocation alloc);
extern "C" void rsScriptGroupExecute (RsContext rsc, RsScriptGroup group);
extern "C" void rsAllocationIoSend (RsContext rsc, RsAllocation alloc);
extern "C" void rsAllocationIoReceive (RsContext rsc, RsAllocation alloc);
extern "C" RsProgramStore rsProgramStoreCreate (RsContext rsc, bool colorMaskR, bool colorMaskG, bool colorMaskB, bool colorMaskA, bool depthMask, bool ditherEnable, RsBlendSrcFunc srcFunc, RsBlendDstFunc destFunc, RsDepthFunc depthFunc);
extern "C" RsProgramRaster rsProgramRasterCreate (RsContext rsc, bool pointSprite, RsCullMode cull);
extern "C" void rsProgramBindConstants (RsContext rsc, RsProgram vp, uint32_t slot, RsAllocation constants);
extern "C" void rsProgramBindTexture (RsContext rsc, RsProgramFragment pf, uint32_t slot, RsAllocation a);
extern "C" void rsProgramBindSampler (RsContext rsc, RsProgramFragment pf, uint32_t slot, RsSampler s);
extern "C" RsProgramFragment rsProgramFragmentCreate (RsContext rsc, const char * shaderText, size_t shaderText_length, const char ** textureNames, size_t textureNames_length_length, const size_t * textureNames_length, const uintptr_t * params, size_t params_length);
extern "C" RsProgramVertex rsProgramVertexCreate (RsContext rsc, const char * shaderText, size_t shaderText_length, const char ** textureNames, size_t textureNames_length_length, const size_t * textureNames_length, const uintptr_t * params, size_t params_length);
extern "C" RsFont rsFontCreateFromFile (RsContext rsc, const char * name, size_t name_length, float fontSize, uint32_t dpi);
extern "C" RsFont rsFontCreateFromMemory (RsContext rsc, const char * name, size_t name_length, float fontSize, uint32_t dpi, const void * data, size_t data_length);
extern "C" RsMesh rsMeshCreate (RsContext rsc, RsAllocation * vtx, size_t vtx_length, RsAllocation * idx, size_t idx_length, uint32_t * primType, size_t primType_length);
extern "C" RsPath rsPathCreate (RsContext rsc, RsPathPrimitive pp, bool isStatic, RsAllocation vertex, RsAllocation loops, float quality);
extern "C" void rsContextBindProgramStore (RsContext rsc, RsProgramStore pgm);
extern "C" void rsContextBindProgramFragment (RsContext rsc, RsProgramFragment pgm);
extern "C" void rsContextBindProgramVertex (RsContext rsc, RsProgramVertex pgm);
extern "C" void rsContextBindProgramRaster (RsContext rsc, RsProgramRaster pgm);
extern "C" void rsContextBindFont (RsContext rsc, RsFont pgm);
extern "C" void rsContextSetSurface (RsContext rsc, uint32_t width, uint32_t height, RsNativeWindow sur);
extern "C" void rsContextBindRootScript (RsContext rsc, RsScript sampler);
extern "C" void rsContextPause (RsContext rsc);
extern "C" void rsContextResume (RsContext rsc);



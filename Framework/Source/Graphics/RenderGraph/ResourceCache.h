/***************************************************************************
# Copyright (c) 2018, NVIDIA CORPORATION. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of NVIDIA CORPORATION nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#pragma once
#include "Graphics/RenderGraph/RenderPassReflection.h"
#include "API/Texture.h"

namespace Falcor
{
    class RenderPass;
    class Resource;
    
    class ResourceCache : public std::enable_shared_from_this<ResourceCache>
    {
    public:
        using SharedPtr = std::shared_ptr<ResourceCache>;
        static SharedPtr create();

        struct DefaultProperties
        {
            uint32_t width = 0;
            uint32_t height = 0;
            ResourceFormat colorFormat = ResourceFormat::Unknown;
            ResourceFormat depthFormat = ResourceFormat::Unknown;
        };

        // Add/Remove reference to a resource not owned by the cache
        void registerExternalResource(const std::string& name, const std::shared_ptr<Resource>& pResource);
        void removeExternalResource(const std::string& name);
        const std::shared_ptr<Resource>& getExternalResource(const std::string& name) const;

        /**  Register a field that requires resources to be allocated.
            \param[in] name String in the format of PassName.FieldName
            \param[in] field Reflection data for the field
            \param[in] alias Optional. Another field name described in the same way as 'name'.
                If specified, and the field exists in the cache, the resource will be aliased with 'name' and field properties will be merged.
        */
        void registerField(const std::string& name, const RenderPassReflection::Field& field, const std::string& alias = "");

        const std::shared_ptr<Resource>& getResource(const std::string& name) const;

        void allocateResources(const DefaultProperties& params);
        void reset();

    private:
        ResourceCache() = default;

        Texture::SharedPtr createTextureForPass(const DefaultProperties& params, const RenderPassReflection::Field& field);
        
        struct ResourceData
        {
            RenderPassReflection::Field field;
            std::shared_ptr<Resource> pResource;
        };
        
        // Resources and properties to be allocated for fields within a render graph
        std::unordered_map<std::string, uint32_t> mFieldMap;
        std::vector<ResourceData> mResourceData;

        // References to output resources not to be allocated by the render graph
        std::unordered_map<std::string, std::shared_ptr<Resource>> mExternalResources;
    };

}
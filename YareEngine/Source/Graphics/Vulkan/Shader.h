#ifndef YARE_SHADER_H
#define YARE_SHADER_H

#include "Graphics/Vulkan/Vk.h"
#include <vector>
#include <string>
#include <memory>

namespace Yare::Graphics {
    enum class ShaderType {
                           Vertex = VK_SHADER_STAGE_VERTEX_BIT,
                           Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
                           Geometry = VK_SHADER_STAGE_GEOMETRY_BIT,
                           Tessellation_Control = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
                           Tessellation_Evaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
                           Compute = VK_SHADER_STAGE_COMPUTE_BIT,
                           Unknown = 0
    };

    class Shader {
    public:
        Shader(const std::string& filePath, const std::string& shaderName);
        ~Shader();

        const uint32_t getStageCount() const { return m_StageCount; }
        VkPipelineShaderStageCreateInfo* getShaderStages() const { return m_ShaderStages; }
        void unloadModules();


    private:
        void readShaderFiles();

        VkPipelineShaderStageCreateInfo* m_ShaderStages;
        std::string m_ShaderName;
        std::string m_FilePath;
        std::vector<std::string> m_ShaderSource;
        uint32_t m_StageCount;
        std::vector<ShaderType> m_ShaderType;
    };
}

#endif // YARE_SHADER_H

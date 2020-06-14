#ifndef YARE_IMGUI_RENDERER_H
#define YARE_IMGUI_RENDERER_H

#include "Graphics/Renderers/YzRenderer.h"

#include "Graphics/Vulkan/Pipeline.h"
#include "Graphics/Vulkan/Buffer.h"
#include "Graphics/Vulkan/DescriptorSet.h"

#include <imgui/imgui.h>

namespace Yare::Graphics {

    class ImGuiRenderer : public YzRenderer {
    public:
        ImGuiRenderer(YzVkRenderPass* renderPass, uint32_t windowWidth, uint32_t windowHeight);
        ~ImGuiRenderer();
        void prepareScene() override;
        void present(YzVkCommandBuffer* commandBuffer) override;
        void onResize(YzVkRenderPass* renderPass, uint32_t newWidth, uint32_t newHeight) override;

    private:
        void init(YzVkRenderPass* renderPass, uint32_t windowWidth, uint32_t windowHeight) override;
        void createGraphicsPipeline(YzVkRenderPass* renderPass);
        void createDescriptorSet();
        void newFrame();
        void postFrame();
        void updateBuffers();

        struct PushConstBlock {
            glm::vec2 scale = {};
            glm::vec2 translate = {};
        } m_PushConstBlock;

        YzVkImage* m_Font;
        YzVkPipeline* m_Pipeline;
        YzVkBuffer* m_IndexBuffer = nullptr;
        YzVkBuffer* m_VertexBuffer = nullptr;
        YzVkDescriptorSet* m_DescriptorSet;
    };

}

#endif //YARE_IMGUI_RENDERER_H
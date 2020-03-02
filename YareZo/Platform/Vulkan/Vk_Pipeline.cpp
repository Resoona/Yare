#include "Platform/Vulkan/Vk_Pipeline.h"
#include "Platform/Vulkan/Vk_Devices.h"
#include "Utilities/YzLogger.h"

namespace Yarezo {
    namespace Graphics {
        YzVkPipeline::YzVkPipeline() {
        }

        YzVkPipeline::~YzVkPipeline() {
        }

        void YzVkPipeline::init(PipelineInfo& pipelineInfo) {
            // A descriptor is a special opaque shader variable that shaders use to access buffer and image
            // resources in an indirect fashion. It can be thought of as a "pointer" to a resource.
            // The layout is used to describe the content of a list of descriptor sets
            createDescriptorSetLayout();

            // Pipeline yay
            createGraphicsPipeline(pipelineInfo);

            // Descriptor sets can't be created directly, they must be allocated from a pool like command buffers. We create those here.
            createDescriptorPool(pipelineInfo);
        }

        void YzVkPipeline::cleanUp() {
            if (m_PipelineLayout) {
                vkDestroyPipelineLayout(YzVkDevice::instance()->getDevice(), m_PipelineLayout, nullptr);
            }
            if (m_GraphicsPipeline) {
                vkDestroyPipeline(YzVkDevice::instance()->getDevice(), m_GraphicsPipeline, nullptr);
            }
            if (m_DescriptorPool) {
                vkDestroyDescriptorPool(YzVkDevice::instance()->getDevice(), m_DescriptorPool, nullptr);
            }
        }

        void YzVkPipeline::cleanupDescSetLayout() {
            if (m_DescriptorSetLayout) {
                vkDestroyDescriptorSetLayout(YzVkDevice::instance()->getDevice(), m_DescriptorSetLayout, nullptr);
            }
        }

        void YzVkPipeline::createDescriptorSetLayout() {

            VkDescriptorSetLayoutBinding uboLayoutBinding = {};
            uboLayoutBinding.binding = 0;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.descriptorCount = 1;
            // We are only using this in the vertex shader
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            uboLayoutBinding.pImmutableSamplers = nullptr; // Optional Param

            VkDescriptorSetLayoutCreateInfo layoutInfo = {};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = 1;
            layoutInfo.pBindings = &uboLayoutBinding;

            if (vkCreateDescriptorSetLayout(YzVkDevice::instance()->getDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
                YZ_ERROR("Vulkan was unable to create a descriptor set layout.");
                throw std::runtime_error("Vulkan was unable to create a descriptor set layout.");
            }
        }
        void YzVkPipeline::createGraphicsPipeline(PipelineInfo& pipelineInfo) {
            auto bindingDescription = Vertex::getBindingDescription();
            auto attributeDescriptions = Vertex::getAttributeDescriptions();

            VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
            vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

            VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            VkViewport viewport = {};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float)pipelineInfo.swapchain->getExtent().width;
            viewport.height = (float)pipelineInfo.swapchain->getExtent().height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            VkRect2D scissor = {};
            scissor.offset = { 0,0 };
            scissor.extent = pipelineInfo.swapchain->getExtent();

            VkPipelineViewportStateCreateInfo viewportState = {};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports = &viewport;
            viewportState.scissorCount = 1;
            viewportState.pScissors = &scissor;

            VkPipelineRasterizationStateCreateInfo rasterizer = {};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

            // If this is enabled, then fragments beyond near and far plains are clamped instead of discarded
            rasterizer.depthClampEnable = VK_FALSE;
            // If this is enabled, then we skip the rasterizer stage (we wont get output)
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            // Fill the polygon, just draw lines, or points set here
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f; //optional
            rasterizer.depthBiasClamp = 0.0f; //optional
            rasterizer.depthBiasSlopeFactor = 0.0f; //optional

            VkPipelineMultisampleStateCreateInfo multisampling = {};

            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            multisampling.minSampleShading = 1.0f; //optional
            multisampling.pSampleMask = nullptr; //optional
            multisampling.alphaToCoverageEnable = VK_FALSE; //optional

            VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;

            VkPipelineColorBlendStateCreateInfo colorBlending = {};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f;
            colorBlending.blendConstants[1] = 0.0f;
            colorBlending.blendConstants[2] = 0.0f;
            colorBlending.blendConstants[3] = 0.0f;

            VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;

            if (vkCreatePipelineLayout(YzVkDevice::instance()->getDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
                YZ_ERROR("Vulkan Pipeline Layout was unable to be created.");
                throw std::runtime_error("Vulkan Pipeline Layout was unable to be created.");
            }

            VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
            pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineCreateInfo.stageCount = 2;
            pipelineCreateInfo.pStages = pipelineInfo.shaders;
            pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
            pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
            pipelineCreateInfo.pViewportState = &viewportState;
            pipelineCreateInfo.pRasterizationState = &rasterizer;
            pipelineCreateInfo.pMultisampleState = &multisampling;
            pipelineCreateInfo.pDepthStencilState = nullptr; // Optional
            pipelineCreateInfo.pColorBlendState = &colorBlending;
            pipelineCreateInfo.pDynamicState = nullptr; // Optional
            pipelineCreateInfo.layout = m_PipelineLayout;
            pipelineCreateInfo.renderPass = pipelineInfo.renderpass->getRenderPass();
            pipelineCreateInfo.subpass = 0;
            pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

            if (vkCreateGraphicsPipelines(YzVkDevice::instance()->getDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS) {
                YZ_ERROR("Vulkan failed to create the graphics pipeline.");
                throw std::runtime_error("Vulkan failed to create the graphics pipeline.");
            }


        }
        void YzVkPipeline::createDescriptorPool(PipelineInfo& pipelineInfo) {
            size_t swapchainImagesSize = pipelineInfo.swapchain->getImagesSize();

            VkDescriptorPoolSize poolSize = {};
            poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSize.descriptorCount = static_cast<uint32_t>(swapchainImagesSize);

            VkDescriptorPoolCreateInfo poolInfo = {};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = 1;
            poolInfo.pPoolSizes = &poolSize;
            poolInfo.maxSets = static_cast<uint32_t>(swapchainImagesSize);

            if (vkCreateDescriptorPool(YzVkDevice::instance()->getDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
                YZ_ERROR("Vulkan creation of descriptor pool failed.");
                throw std::runtime_error("Vulkan creation of descriptor pool failed.");
            }

        }
    }
}

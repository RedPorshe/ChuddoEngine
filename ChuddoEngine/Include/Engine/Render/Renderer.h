#pragma once
#include "Vulkan/VulkanInterface.h"
#include <vector>
#include "Vulkan/Managers/BufferManager.h"
#include "RenderInfo.h"

struct FEngineInfo;
class CSwapChainManager;
class CCommandManager;
class CSyncManager;
class CRenderPassManager;
class CPipelineManager;
class CBufferManager;


class CRenderer : public IVulkanManager
    {
    public:
        CRenderer ( FEngineInfo & inInfo );
        virtual ~CRenderer ();

        bool Initialize () override;
        void Shutdown () override;
        const char * GetManagerName () const override;
        void SetInfoForRender (const FRenderInfo& RenderInfo );
        bool RenderScene ();

    private:
        bool StartFrame ( uint32_t & ImageIndex );
        bool EndFrame ( uint32_t ImageIndex );
        bool CreateSyncObjects ();
        bool CreateCommandBuffers ();
        bool RecordCommandBuffer ( VkCommandBuffer CommandBuffer, uint32_t ImageIndex );
        bool RecreateSwapChainResources ();
        void TriangleStub ( VkCommandBuffer CommandBuffer);
    private:
        // Managers (cached for fast access)
        FRenderInfo m_RenderInfo ;
        CSwapChainManager * m_SwapChainManager = nullptr;
        CCommandManager * m_CommandManager = nullptr;
        CSyncManager * m_SyncManager = nullptr;
        CRenderPassManager * m_RenderPassManager = nullptr;
        CPipelineManager * m_PipelineManager = nullptr;
        CBufferManager * m_BufferManager = nullptr;

        // Command buffers (one per framebuffer)
        std::vector<VkCommandBuffer> m_CommandBuffers;

        // Triangle pipeline
        VkPipeline m_TrianglePipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_TrianglePipelineLayout = VK_NULL_HANDLE;
        FBuffer m_TriangleVertexBuffer;

        // Frame sync objects
        bool m_SyncObjectsCreated = false;
    };
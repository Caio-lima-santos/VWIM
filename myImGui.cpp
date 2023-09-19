
#include "server2.cpp"
#include <omp.h>
#include "arquivos_ES.cpp"
#include <string>

using namespace ImGui;
server Server;

const char* nome = "cliente";


Armazenamento armazenamento = Armazenamento("tex.txt.txt");



void (*body)(void);
class GUI {



    struct input {
        const char* nome;
        float* valor;
    };

    struct Janela {

        const char* nome;
        int altura;
        int largura;
        bool aberto;


    };


    std::vector<void*>alocs;
    std::vector<input*> inputs;





public:

    void initImGui(vulkan& app,void Rbody(void)) {
        body= Rbody;


        //  bits = initSint();



        VkDescriptorPool descpoll{};

        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };


        {


            VkDescriptorPoolCreateInfo descinfo{};

            descinfo.flags = 0;
            descinfo.maxSets = 50;
            descinfo.pPoolSizes = pool_sizes;
            descinfo.poolSizeCount = 11;
            descinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

            if (vkCreateDescriptorPool(app.vkDevice, &descinfo, nullptr, &descpoll) != VK_SUCCESS)
                throw std::runtime_error("erro no desc poll");




        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        ImGui_ImplGlfw_InitForVulkan(app.window, true);

        ImGui_ImplVulkan_InitInfo info{};
        info.Allocator = nullptr;
        info.CheckVkResultFn = nullptr;
        info.ColorAttachmentFormat = app.swapChainImageFormat;
        info.DescriptorPool = descpoll;
        info.Device = app.vkDevice;
        info.ImageCount = 2;
        info.Instance = app.instance;
        info.MinImageCount = 2;
        info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        info.PhysicalDevice = app.physicalDevice;
        info.PipelineCache = nullptr;
        info.Queue = app.graphicsQueue;
        info.QueueFamily = 0;
        info.Subpass = 0;
        info.UseDynamicRendering = false;


        ImGui_ImplVulkan_Init(&info, app.renderPass);




        VkCommandBuffer command_buffer = app.beginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
        app.endSingleTimeCommands(command_buffer);



    }

    input* pegarPorNome(const char* nome) {

        for (int i = 0; i < inputs.size(); i++) {
            if (strcmp(nome, inputs[i]->nome)) {
                return inputs[i];
            }
        }

    }

    void receberFloat(const char* nome, float* flutuante) {

        input* in = new input();
        alocs.push_back(in);
        inputs.push_back(in);

        in->nome = nome;
        in->valor = flutuante;

        InputFloat(nome, flutuante);

    }

    void clear() {
        for (int i = 0; i < alocs.size(); i++) {
            free(alocs[i]);
        };

    }


};


bool open = true;
const char* texto = "texto";
ImGuiID windosID;
ImGuiWindow* window;
bool atualizar = false;
bool conectar = false;



VkCommandBuffer imGuiL(vulkan& app, int imageindex) {

    VkCommandBuffer cmd = app.beginSingleTimeCommands();
    char temp[DEFAULT_BUFLEN];


    glfwPollEvents();
    {
      ImGui_ImplVulkan_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      
          body();

          ImGui::EndFrame();
          ImGui::Render();
      }
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = app.renderPass;
    renderPassInfo.framebuffer = app.swapChainFramebuffers[imageindex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = app.swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };


    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();



            vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

            vkCmdEndRenderPass(cmd);


            vkEndCommandBuffer(cmd);

        
         






    return cmd;


}


void end() {



}


//cores


#define COR_TRANSPARENTE ImVec4(0,0,0,0);  
#define COR_PRETA   ImVec4(1.0f,1.0f,1.0f,1.0f);
#define COR_VERMELHA  ImVec4(1.0f, 0, 0, 1.0f);
#define COR_AZUL  ImVec4(0, 0, 1.0f, 1.0f);

#include "Camera.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "Buffer.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class CWidhet : public GLWidget
{
    // ShaderProgram _sp{"../glsl/playground/trans.vs", "../glsl/playground/basic.fs"};
    ShaderProgram _sp{"../glsl/playground/trans.vs", "../glsl/playground/bp.fs"};
    GLuint _wood = TEXTURE_MANAGER.load_texture("../resources/textures/wood.png");
    GLuint spec_texture = TEXTURE_MANAGER.load_texture("../resources/textures/container2_specular.png", TEXTURE_2D_RGBA);
    GLuint diff_texture = TEXTURE_MANAGER.load_texture("../resources/textures/container2.png", TEXTURE_2D_RGBA);
    VertexArray plane_va;
    VertexArray cube_va;
    void renderPlane()
    {
        plane_va.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
    void renderCube()
    {
        cube_va.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
    virtual void application() override
    {
        _sp.use();
        _sp.set_sampler(0, "material.ambient");
        _sp.set_sampler(1, "material.diffuse");
        _sp.set_sampler(2, "material.specular");

        _sp.set_uniform("material.shininess", 128);

        _sp.set_uniform("light.position", glm::vec3(0.0, 1.0, 3.0));
        _sp.set_uniform("light.color", glm::vec3(1.0, 1.0, 1.0));

    }

    virtual void render_loop() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        _sp.use();
        _sp.set_uniform("view", CAMERA.get_view_matrix());
        _sp.set_uniform("view_pos", CAMERA.get_position());
        _sp.set_uniform("projection", get_projection());

        glm::mat4 model = glm::mat4(1.0f);
        // floor
        _sp.active_sampler(0, _wood);
        _sp.active_sampler(1, _wood);
        _sp.active_sampler(2, 0);        
        _sp.set_uniform<glm::mat4>("model", model);
        _sp.set_uniform<glm::mat3>("normalM", glm::transpose(glm::inverse(glm::mat3(model))));
        renderPlane();
        // cubes
        _sp.active_sampler(0, diff_texture);
        _sp.active_sampler(1, diff_texture);
        _sp.active_sampler(2, spec_texture);  
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        _sp.set_uniform<glm::mat4>("model", model);
        _sp.set_uniform<glm::mat3>("normalM", glm::transpose(glm::inverse(glm::mat3(model))));
        renderCube();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
        model = glm::scale(model, glm::vec3(0.5f));
        _sp.set_uniform<glm::mat4>("model", model);
        _sp.set_uniform<glm::mat3>("normalM", glm::transpose(glm::inverse(glm::mat3(model))));
        renderCube();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
        model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        model = glm::scale(model, glm::vec3(0.25));
        _sp.set_uniform<glm::mat4>("model", model);
        _sp.set_uniform<glm::mat3>("normalM", glm::transpose(glm::inverse(glm::mat3(model))));
        renderCube();
    }

public:
    CWidhet(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {
        {
            float planeVertices[] = 
            {
            // positions            // normals         // texcoords
            25.0f, -0.5f, 25.0f, 0.0f,  1.0f,   0.0f,  25.0f,  0.0f, -25.0f, -0.5f, 25.0f,  0.0f,
            1.0f,  0.0f,  0.0f,  0.0f,  -25.0f, -0.5f, -25.0f, 0.0f, 1.0f,   0.0f,  0.0f,   25.0f,

            25.0f, -0.5f, 25.0f, 0.0f,  1.0f,   0.0f,  25.0f,  0.0f, -25.0f, -0.5f, -25.0f, 0.0f,
            1.0f,  0.0f,  0.0f,  25.0f, 25.0f,  -0.5f, -25.0f, 0.0f, 1.0f,   0.0f,  25.0f,  25.0f
            };
            GLuint vb = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices);
            plane_va.attach_vertex_buffer(PNT_LAYOUT, vb);
        }
        {
            float vertices[] = 
            {
                // back face
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
                 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
            };
            GLuint vb = BUFFER.generate_buffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices);
            cube_va.attach_vertex_buffer(PNT_LAYOUT, vb);
        }
    }
};

class guiWidhet : public GLWidget
{
    bool checkbox_value;
    float slider_value;
    char text_buffer[256] = "Hello, ImGui!";
    virtual void application() override
    {

    }

    virtual void render_loop() override
    {
        // 开始新的 ImGui 帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin(u8"我的控制面板");

        ImGui::Text(u8"这是一个带控件的 ImGui 窗口");
        ImGui::Checkbox(u8"启用功能", &checkbox_value);

        ImGui::SliderFloat(u8"亮度", &slider_value, 0.0f, 1.0f);

        ImGui::InputText(u8"输入文本", text_buffer, sizeof(text_buffer));

        int counter = 0;
        if (ImGui::Button(u8"点击我!")) {
            counter++;
        }
        ImGui::SameLine();
        ImGui::Text(u8"已点击 %d 次", counter);

        ImGui::Spacing();
        if (ImGui::Button("重置计数器")) {
            counter = 0;
        }

        ImGui::End();        
        // 渲染
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());        
    }

public:
    guiWidhet(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {
        // 初始化 ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;

        // 设置样式（可选：Dark / Light）
        ImGui::StyleColorsDark();

        // 初始化平台和渲染后端
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 150"); // macOS 推荐指定 GLSL 版本        
    }
};

class guiTriangleWidhet : public GLWidget
{
    float r{255};
    float g{255};
    float b{255};
    std::vector<float> only_vertices
    {
        // 位置            
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f,
    };
    VertexArray _va;
    ShaderProgram _sp{"../glsl/playground/gui.vs", "../glsl/playground/gui.fs"};
    virtual void application() override
    {
        _va.attach_vertex_buffer(P_LAYOUT, 
            BUFFER.generate_vertex_buffer(only_vertices.size() * sizeof(float), only_vertices.data()));
    }

    virtual void render_loop() override
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        _va.bind();
        _sp.use();
        _sp.set_uniform("color", glm::vec3(r, g, b));
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // 开始新的 ImGui 帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SliderFloat(u8"r", &r, 0.0, 255.0);
        ImGui::SliderFloat(u8"g", &g, 0.0, 255.0);
        ImGui::SliderFloat(u8"b", &b, 0.0, 255.0);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());        
    }

public:
    guiTriangleWidhet(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {
        // 初始化 ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;

        // 设置样式（可选：Dark / Light）
        ImGui::StyleColorsDark();

        // 初始化平台和渲染后端
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 150"); // macOS 推荐指定 GLSL 版本        
    }
};

int main()
{
    guiTriangleWidhet c{800, 600, "1111"};
    c.render();
    return 0;
}
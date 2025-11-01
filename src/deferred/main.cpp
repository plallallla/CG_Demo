#include "Camera.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.hpp"
#include "TextureAttributes.hpp"
#include "Texture.hpp"
#include "FrameBuffer.hpp"
#include "Model.hpp"
#include "QuadRender.hpp"
#include "VertexArray.hpp"

const unsigned int NR_LIGHTS = 64;

class DefferedWidget : public GLWidget
{
public:
    DefferedWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {
        srand(13);
        for (unsigned int i = 0; i < NR_LIGHTS; i++)
        {
            // calculate slightly random offsets
            float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
            float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
            float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
            lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
            // also calculate random color
            float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
            float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
            float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
            lightColors.push_back(glm::vec3(rColor, gColor, bColor));
        }
        objectPositions.push_back(glm::vec3(-3.0,  -0.5, -3.0));
        objectPositions.push_back(glm::vec3( 0.0,  -0.5, -3.0));
        objectPositions.push_back(glm::vec3( 3.0,  -0.5, -3.0));
        objectPositions.push_back(glm::vec3(-3.0,  -0.5,  0.0));
        objectPositions.push_back(glm::vec3( 0.0,  -0.5,  0.0));
        objectPositions.push_back(glm::vec3( 3.0,  -0.5,  0.0));
        objectPositions.push_back(glm::vec3(-3.0,  -0.5,  3.0));
        objectPositions.push_back(glm::vec3( 0.0,  -0.5,  3.0));
        objectPositions.push_back(glm::vec3( 3.0,  -0.5,  3.0));
    }
private:
    ShaderProgram _gbuffer{"../glsl/deferred/gbuffer.vs", "../glsl/deferred/gbuffer.fs"};
    ShaderProgram _box{"../glsl/deferred/box.vs", "../glsl/deferred/box.fs"};
    ShaderProgram sp1{"../glsl/model/model.vs", "../glsl/model/model.fs"};
    VertexArray _cube_va;
    QuadRender _debug;
    QuadRender _render{"../glsl/deferred/render.fs"};
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    std::vector<glm::vec3> objectPositions;
    FrameBuffer _fb;
    GLuint scene_geometry = TEXTURE_MANAGER.generate_texture_buffer(_width*2, _height*2, TEXTURE_2D_RGBA16F);
    GLuint scene_normal = TEXTURE_MANAGER.generate_texture_buffer(_width*2, _height*2, TEXTURE_2D_RGBA16F);
    GLuint scene_albedo = TEXTURE_MANAGER.generate_texture_buffer(_width*2, _height*2, TEXTURE_2D_RGBA );
    Model cyborg{"../resources/cyborg/cyborg.obj"};
    void render_scene(const ShaderProgram& sp)
    {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 model = glm::mat4(1.0f);
        sp.use();
        sp.set_uniform("projection", get_projection());
        sp.set_uniform("view", CAMERA.get_view_matrix());
        for (unsigned int i = 0; i < objectPositions.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, objectPositions[i]);
            model = glm::scale(model, glm::vec3(0.5f));
            sp.set_uniform("model", model);
            cyborg.render_elements(sp);
        }
    }
    virtual void application() override
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
        _cube_va.attach_vertex_buffer(PNT_LAYOUT, BUFFER.generate_vertex_buffer(sizeof(vertices), vertices));

        _fb.bind();
        _fb.create_render_object(_width*2, _height*2);
        _fb.attach_color_texture(0, scene_geometry);
        _fb.attach_color_texture(1, scene_normal);
        _fb.attach_color_texture(2, scene_albedo);
        _fb.active_draw_buffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2}); 
        _fb.checkFramebufferStatus();
        _fb.unbind();
        _render._sp.use();
        _render._sp.set_sampler(0, "gPosition");
        _render._sp.set_sampler(1, "gNormal");
        _render._sp.set_sampler(2, "gAlbedoSpec");
        const float linear = 0.7f;
        const float quadratic = 1.8f;
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            _render._sp.set_uniform("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
            _render._sp.set_uniform("lights[" + std::to_string(i) + "].Color", lightColors[i]);
            _render._sp.set_uniform("lights[" + std::to_string(i) + "].Linear", linear);
            _render._sp.set_uniform("lights[" + std::to_string(i) + "].Quadratic", quadratic);
        }
        _render._sp.set_uniform("viewPos", CAMERA.get_position());
    }
    virtual void render_loop() override
    {
        // pass1 渲染获取gbuffer数据
        _fb.bind();
        render_scene(_gbuffer);
        _fb.unbind();
        // _debug.render_texture(scene_geometry);
        // _debug.render_texture(scene_normal);
        // _debug.render_texture(scene_albedo);

        // pass2 根据gbuffer数据进行绘制
        // 渲染光照结果
        _render.render_texture({scene_geometry, scene_normal, scene_albedo});
        // 渲染灯光
        _box.use();
        _cube_va.bind();
        _box.set_uniform("projection", get_projection());
        _box.set_uniform("view", CAMERA.get_view_matrix());
        for (int i = 0; i < NR_LIGHTS; i++)
        {
            glm::mat4 model(1.0);
            model = glm::translate(model, lightPositions[i]);
            model = glm::scale(model, glm::vec3(0.125f));
            _box.set_uniform("lightColor", lightColors[i]);
            _box.set_uniform("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
};

int main()
{
    DefferedWidget w{800, 600, "DefferedWidget"};
    w.render();
    return 0;
}

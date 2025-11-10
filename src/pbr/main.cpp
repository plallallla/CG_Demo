#include "Camera.hpp"
#include "FrameBuffer.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include <OpenGL/gltypes.h>

#include "PrecomputedRender.hpp"
#include "SkyboxRender.hpp"
#include "TextureAttributes.hpp"

class LightWidhet : public GLWidget
{
    Sphere _s;
    ShaderProgram _sp{"../glsl/pbr/light.vs", "../glsl/pbr/light.fs"};
    std::vector<glm::vec3> lightPositions
    {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3( 10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3( 10.0f, -10.0f, 10.0f),
    };
    std::vector<glm::vec3> lightColors
    {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };
    int nrRows    = 7;
    int nrColumns = 7;
    float spacing = 2.5;
    virtual void application() override
    {
        glEnable(GL_DEPTH_TEST);
        _sp.use();
        _sp.set_uniform("ao", 1.0f);
        _sp.set_uniform("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
        _sp.set_uniform("projection", get_projection());

    }

    virtual void render_loop() override
    {

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _sp.use();
        _sp.set_uniform("view", CAMERA.get_view_matrix());
        _sp.set_uniform("camPos", CAMERA.get_position());

        glm::mat4 model = glm::mat4(1.0f);
        for (int row = 0; row < nrRows; ++row) 
        {
            _sp.set_uniform("metallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; ++col) 
            {
                // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
                // on direct lighting.
                _sp.set_uniform("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));
                
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(
                    (col - (nrColumns / 2)) * spacing, 
                    (row - (nrRows / 2)) * spacing, 
                    0.0f
                ));
                _sp.set_uniform("model", model);
                _sp.set_uniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
                _s.render();
            }
        }

        // render light source (simply re-render sphere at light positions)
        // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
        // keeps the codeprint small.
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            _sp.set_uniform("lightPositions[" + std::to_string(i) + "]", newPos);
            _sp.set_uniform("lightColors[" + std::to_string(i) + "]", lightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            _sp.set_uniform("model", model);
            _sp.set_uniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            _s.render();
        }        

    }

public:
    LightWidhet(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {
    }
};


class MapWidhet : public GLWidget
{
    Sphere _s;
    ShaderProgram _sp{"../glsl/pbr/light.vs", "../glsl/pbr/map.fs"};
    GLuint albedo = TEXTURE_MANAGER.auto_load_texture("../resources/textures/pbr/rusted_iron/albedo.png");
    GLuint normal = TEXTURE_MANAGER.auto_load_texture("../resources/textures/pbr/rusted_iron/normal.png");
    GLuint metallic = TEXTURE_MANAGER.auto_load_texture("../resources/textures/pbr/rusted_iron/metallic.png");
    GLuint roughness = TEXTURE_MANAGER.auto_load_texture("../resources/textures/pbr/rusted_iron/roughness.png");
    GLuint ao = TEXTURE_MANAGER.auto_load_texture("../resources/textures/pbr/rusted_iron/ao.png");
    std::vector<glm::vec3> lightPositions
    {
        glm::vec3(0.0f, 0.0f, 10.0f),
    };
    std::vector<glm::vec3> lightColors
    {
        glm::vec3(150.0f, 150.0f, 150.0f),
    };
    int nrRows    = 7;
    int nrColumns = 7;
    float spacing = 2.5;
    virtual void application() override
    {

        glEnable(GL_DEPTH_TEST);
        _sp.use();
        _sp.set_uniform("ao", 1.0f);
        _sp.set_uniform("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
        _sp.set_uniform("projection", get_projection());

        _sp.set_sampler(0, "albedoMap");
        _sp.active_sampler(0, albedo);
        _sp.set_sampler(1, "normalMap");
        _sp.active_sampler(1, normal);
        _sp.set_sampler(2, "metallicMap");
        _sp.active_sampler(2, metallic);
        _sp.set_sampler(3, "roughnessMap");
        _sp.active_sampler(3, roughness);
        _sp.set_sampler(4, "aoMap");
        _sp.active_sampler(4, ao);

    }

    virtual void render_loop() override
    {

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _sp.use();
        _sp.set_uniform("view", CAMERA.get_view_matrix());
        _sp.set_uniform("camPos", CAMERA.get_position());

        glm::mat4 model = glm::mat4(1.0f);
        for (int row = 0; row < nrRows; ++row) 
        {
            _sp.set_uniform("metallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; ++col) 
            {
                // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
                // on direct lighting.
                _sp.set_uniform("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));
                
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(
                    (col - (nrColumns / 2)) * spacing, 
                    (row - (nrRows / 2)) * spacing, 
                    0.0f
                ));
                _sp.set_uniform("model", model);
                _sp.set_uniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
                _s.render();
            }
        }

        // render light source (simply re-render sphere at light positions)
        // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
        // keeps the codeprint small.
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            _sp.set_uniform("lightPositions[" + std::to_string(i) + "]", newPos);
            _sp.set_uniform("lightColors[" + std::to_string(i) + "]", lightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            _sp.set_uniform("model", model);
            _sp.set_uniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            _s.render();
        }        

    }

public:
    MapWidhet(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {
    }
};

class PraticeWidget : public GLWidget
{
    Sphere _s;
    ShaderProgram _sp{"../glsl/pbr/geometry.vs", "../glsl/pbr/point_cook_torrance.fs"};
    std::vector<glm::vec3> lightPositions
    {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3( 10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3( 10.0f, -10.0f, 10.0f),
    };
    std::vector<glm::vec3> lightColors
    {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };

    float roughness{0.5};
    float metallic{0.5};

    virtual void application() override
    {
        glEnable(GL_DEPTH_TEST);
        _sp.use();
        _sp.set_uniform("ao", 1.0f);
        _sp.set_uniform("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
        _sp.set_uniform("projection", get_projection());

    }

    virtual void render_loop() override
    {

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _sp.use();
        _sp.set_uniform("view", CAMERA.get_view_matrix());
        _sp.set_uniform("view_pos", CAMERA.get_position());
        _sp.set_uniform("ambient_intensity", 0.05f);

        glm::mat4 model = glm::mat4(1.0f);

        _sp.set_uniform("metallic", metallic);
        _sp.set_uniform("roughness", roughness);

        model = glm::mat4(1.0f);
        _sp.set_uniform("model", model);
        _sp.set_uniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            _sp.set_uniform("light_array[" + std::to_string(i) + "].position", newPos);
            _sp.set_uniform("light_array[" + std::to_string(i) + "].color", lightColors[i]);
        }
        _s.render();
    }

    virtual void gui_operation() override
    {
        ImGui::SliderFloat(u8"metallic", &metallic, 0.05, 1.0);
        ImGui::SliderFloat(u8"roughness", &roughness, 0.05, 1.0);
    }    

public:
    PraticeWidget(int width, int height, std::string_view title) : GLWidget(width,height,title,true) 
    {
    }
};

class IBLWidget : public GLWidget
{

    SkyboxRender _skybox;
    Sphere _s;
    Cube _cube;
    ShaderProgram pbrShader{"../glsl/ibl/pbr.vs", "../glsl/ibl/pbr.fs"};
    ShaderProgram equirectangularToCubemapShader{"../glsl/ibl/cube.vs", "../glsl/ibl/cube.fs"};
    ShaderProgram backgroundShader{"../glsl/ibl/background.vs", "../glsl/ibl/background.fs"};
    ShaderProgram irradianceShader{"../glsl/ibl/cube.vs", "../glsl/ibl/irradiance_convolution.fs"};


    HdrCubeRender hdr_pass;
    DiffuseIrradianceIBL di_pass;

    GLuint hdrTexture;
    std::vector<glm::vec3> lightPositions
    {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3( 10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3( 10.0f, -10.0f, 10.0f),
    };
    std::vector<glm::vec3> lightColors
    {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    std::vector<glm::mat4> captureViews
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };    
    float roughness{0.5};
    float metallic{0.5};
    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;

    GLuint envCubemap = TEXTURE_MANAGER.generate_cube_texture_buffer(512, 512, TEXTURE_CUBE_RGB);


    // pbr: setup framebuffer
    // ----------------------
    // unsigned int captureFBO;
    FrameBuffer captureFBO;
    unsigned int captureRBO;
    unsigned int irradianceMap;
    virtual void application() override
    {
        glEnable(GL_DEPTH_TEST);
        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
        stbi_set_flip_vertically_on_load(true);        

        // build and compile shaders
        // -------------------------
        pbrShader.use();
        pbrShader.set_sampler(0, "irradianceMap");
        pbrShader.set_uniform("albedo", glm::vec3{0.5f, 0.0f, 0.0f});
        pbrShader.set_uniform("ao", 1.0f);

        backgroundShader.use();
        backgroundShader.set_sampler(0, "environmentMap");

        hdrTexture = TEXTURE_MANAGER.auto_load_texture("../resources/textures/hdr/newport_loft.hdr");
        // hdrTexture = TEXTURE_MANAGER.auto_load_texture("../resources/textures/hdr/appart.hdr");

        // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
        // ----------------------------------------------------------------------------------------------
        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };


        hdr_pass.execute(hdrTexture);
        envCubemap = hdr_pass;

        // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
        // --------------------------------------------------------------------------------
        irradianceMap = TEXTURE_MANAGER.generate_cube_texture_buffer(32, 32, TEXTURE_CUBE_RGB_FLOAT);
        captureFBO.bind();
        captureFBO.create_render_object(32, 32);
        irradianceShader.use();
        irradianceShader.set_sampler(0, "environmentMap");
        irradianceShader.set_uniform("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        for (unsigned int i = 0; i < 6; ++i)
        {
            irradianceShader.set_uniform("view", captureViews[i]);
            captureFBO.attach_color_texture(0, irradianceMap, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _cube.render();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // di_pass.execute(hdr_pass);
        // irradianceMap = di_pass;

        // then before rendering, configure the viewport to the original framebuffer's screen dimensions
        int scrWidth, scrHeight;
        glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
        glViewport(0, 0, scrWidth, scrHeight);        

        // initialize static shader uniforms before rendering
        // --------------------------------------------------
        pbrShader.use();
        pbrShader.set_uniform("projection", get_projection());
        backgroundShader.use();
        backgroundShader.set_uniform("projection", get_projection());


      
    }

    virtual void render_loop() override
    {

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render scene, supplying the convoluted irradiance map to the final shader.
        // ------------------------------------------------------------------------------------------
        pbrShader.use();
        pbrShader.set_uniform("view", CAMERA.get_view_matrix());
        pbrShader.set_uniform("camPos", CAMERA.get_position());

        // bind pre-computed IBL data
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

        // render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
        glm::mat4 model = glm::mat4(1.0f);
        for (int row = 0; row < nrRows; ++row)
        {
            pbrShader.set_uniform("metallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; ++col)
            {
                // we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
                // on direct lighting.
                pbrShader.set_uniform("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(
                    (float)(col - (nrColumns / 2)) * spacing,
                    (float)(row - (nrRows / 2)) * spacing,
                    -2.0f
                ));
                pbrShader.set_uniform("model", model);
                pbrShader.set_uniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
                _s.render();
            }
        }


        // render light source (simply re-render sphere at light positions)
        // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
        // keeps the codeprint small.
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            pbrShader.set_uniform("lightPositions[" + std::to_string(i) + "]", newPos);
            pbrShader.set_uniform("lightColors[" + std::to_string(i) + "]", lightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            pbrShader.set_uniform("model", model);
            pbrShader.set_uniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            _s.render();
        }

        // _skybox.render_texture(envCubemap, get_projection());
        _skybox.render_texture(irradianceMap, get_projection());
    }

public:
    IBLWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {
    }
};

int main()
{
    // LightWidhet c{800, 600, "pbr"};
    IBLWidget c{900, 720, "pbr"};
    c.render();
    return 0;
}
#include "Camera.hpp"
#include "FrameBuffer.hpp"
#include "GLWidget.hpp"
#include "ShaderProgram.hpp"
#include "Shape.hpp"
#include "Texture.hpp"
#include <OpenGL/gltypes.h>
#include <vector>

#include "Precompution.hpp"
#include "SkyboxRender.hpp"

#include "Material.hpp"


// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

#include "QuadRender.hpp"

class IBLWidget : public GLWidget
{
    SkyboxRender _skybox;

    ShaderProgram pbrShader{"../glsl/ibl/pbr.vs", "../glsl/ibl/final_pbr.fs"};
    ShaderProgram prefilterShader{"../glsl/ibl/cube.vs", "../glsl/ibl/prefilter.fs"};
    ShaderProgram brdfShader{"../glsl/ibl/brdf.vs", "../glsl/ibl/brdf.fs"};

    EquirectConvertRender hdr_pass;
    DiffuseIrradianceIBL di_pass;    
    BRDF_LUT budf_lut;

    SpecularPrefilterIBL prefilter;

    SkyboxRender _sky;

    GLuint hdrTexture;

    GLuint rusted_iron_albedo = TEXTURE_MANAGER.auto_load_texture("../resources/textures/pbr/rusted_iron/albedo.png");
    GLuint rusted_iron_normal = TEXTURE_MANAGER.auto_load_texture("../resources/textures/pbr/rusted_iron/normal.png");
    GLuint rusted_iron_metallic = TEXTURE_MANAGER.auto_load_texture("../resources/textures/pbr/rusted_iron/metallic.png");
    GLuint rusted_iron_roughness = TEXTURE_MANAGER.auto_load_texture("../resources/textures/pbr/rusted_iron/roughness.png");
    GLuint rusted_iron_ao = TEXTURE_MANAGER.auto_load_texture("../resources/textures/pbr/rusted_iron/ao.png");    

    GLuint gold_albedo = TEXTURE_MANAGER.auto_load_texture("../resources/textures/pbr/gold/albedo.png");
    GLuint gold_normal = TEXTURE_MANAGER.auto_load_texture("../resources/textures/pbr/gold/normal.png");
    GLuint gold_metallic = TEXTURE_MANAGER.auto_load_texture("../resources/textures/pbr/gold/metallic.png");
    GLuint gold_roughness = TEXTURE_MANAGER.auto_load_texture("../resources/textures/pbr/gold/roughness.png");
    GLuint gold_ao = TEXTURE_MANAGER.auto_load_texture("../resources/textures/pbr/gold/ao.png");        

    Material rusted_iron{"../resources/textures/pbr/gold"};
    // Material rusted_iron{"../resources/textures/pbr/rusted_iron"};

    FrameBuffer captureFBO;
    unsigned int captureRBO;

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
    
    unsigned int prefilterMap;

    QuadRender _debug;

    virtual void application() override
    {

        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);
        // set depth function to less than AND equal for skybox depth trick.
        glDepthFunc(GL_LEQUAL);
        // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        pbrShader.use();
        pbrShader.set_uniform<int>("irradianceMap", 0);
        pbrShader.set_uniform<int>("prefilterMap", 1);
        pbrShader.set_uniform<int>("brdfLUT", 2);
        pbrShader.set_uniform<int>("albedoMap", 3);
        pbrShader.set_uniform<int>("normalMap", 4);
        pbrShader.set_uniform<int>("metallicMap", 5);
        pbrShader.set_uniform<int>("roughnessMap", 6);
        pbrShader.set_uniform<int>("aoMap", 7);

        // pbr: setup framebuffer
        // ----------------------
        unsigned int captureFBO;
        unsigned int captureRBO;
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

        // pbr: load the HDR environment map
        // ---------------------------------
        stbi_set_flip_vertically_on_load(true);        
        hdrTexture = TEXTURE_MANAGER.auto_load_texture("../resources/textures/hdr/newport_loft.hdr");

        // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
        // ----------------------------------------------------------------------------------------------
        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };

        hdr_pass.execute(hdrTexture);

        // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
        glBindTexture(GL_TEXTURE_CUBE_MAP, hdr_pass);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        di_pass.execute(hdr_pass);

        budf_lut.execute();

        // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
        // --------------------------------------------------------------------------------
        glGenTextures(1, &prefilterMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
        // ----------------------------------------------------------------------------------------------------
        prefilterShader.use();
        prefilterShader.set_uniform<int>("environmentMap", 0);
        prefilterShader.set_uniform("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, hdr_pass);

        prefilter.execute(hdr_pass);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        unsigned int maxMipLevels = 5;
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
        {
            // reisze framebuffer according to mip-level size.
            unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
            unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
            glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);

            float roughness = (float)mip / (float)(maxMipLevels - 1);
            prefilterShader.set_uniform("roughness", roughness);
            for (unsigned int i = 0; i < 6; ++i)
            {
                prefilterShader.set_uniform("view", captureViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                Shape::render_cube();
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // initialize static shader uniforms before rendering
        // --------------------------------------------------
        pbrShader.use();
        pbrShader.set_uniform("projection", get_projection());

        // then before rendering, configure the viewport to the original framebuffer's screen dimensions
        int scrWidth, scrHeight;
        glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
        glViewport(0, 0, scrWidth, scrHeight);

        update_viewport();        

    }

    virtual void render_loop() override
    {

        // _debug.render_texture(budf_lut);
        // return;
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render scene, supplying the convoluted irradiance map to the final shader.
        // ------------------------------------------------------------------------------------------
        pbrShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        pbrShader.set_uniform("view", CAMERA.get_view_matrix());
        pbrShader.set_uniform("camPos", CAMERA.get_position());

        // bind pre-computed IBL data
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, di_pass);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, budf_lut);

        // rusted iron
        // glActiveTexture(GL_TEXTURE3);
        // glBindTexture(GL_TEXTURE_2D, rusted_iron_albedo);
        // glActiveTexture(GL_TEXTURE4);
        // glBindTexture(GL_TEXTURE_2D, rusted_iron_normal);
        // glActiveTexture(GL_TEXTURE5);
        // glBindTexture(GL_TEXTURE_2D, rusted_iron_metallic);
        // glActiveTexture(GL_TEXTURE6);
        // glBindTexture(GL_TEXTURE_2D, rusted_iron_roughness);
        // glActiveTexture(GL_TEXTURE7);
        // glBindTexture(GL_TEXTURE_2D, rusted_iron_ao);

        // rusted_iron.active(3);        

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, gold_albedo);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, gold_normal);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, gold_metallic);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, gold_roughness);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, gold_ao);        

        model = glm::mat4(1.0f);
        pbrShader.set_uniform("model", model);
        pbrShader.set_uniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        Shape::render_sphere();



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
            Shape::render_sphere();
        }

        _sky.render_texture(hdr_pass, get_projection());
        // _sky.render_texture(prefilterMap, get_projection());
        _sky.render_texture(prefilter, get_projection());

    }   

public:
    IBLWidget(int width, int height, std::string_view title) : GLWidget(width,height,title) 
    {
    }
};


int main()
{
    IBLWidget c{900, 720, "IBL"};
    c.render();
    return 0;
}
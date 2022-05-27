#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // This is equivalent to V-Sync
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK){
        std::cout << "Error!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    // 6 vertex positions (x, y) in screenspace and (x, y) for texture
    // Note repeate of some vertices, these are extra and wasteful
    float positions[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, // 0
         0.5f, -0.5f, 1.0f, 0.0f, // 1
         0.5f,  0.5f, 1.0f, 1.0f, // 2
        -0.5f,  0.5f, 0.0f, 1.0f, // 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    // This tells OpenGL how to handle blending the alpha channel
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    VertexArray va;

    /* define vertext buffer */
    // 4 vertices * 4 data points * size of data points (floats)
    VertexBuffer vb(positions, 4 * 4 * sizeof(float));

    VertexBufferLayout layout;
    // 2 floats for screenspace (x, y)
    layout.Push<float>(2);
    // 2 floats for texture coords (x, y)
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    /* define index buffer */
    IndexBuffer ib(indices, 6);

    /* Create projection matrix */
    // this is an orthographic matrix that matches our 4:3 aspect ratio
    glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

    Shader shader("/home/maholder/git_repos/hackathon-open-gl/res/shaders/Basic.shader");
    shader.Bind();

    Texture texture("/home/maholder/git_repos/hackathon-open-gl/res/textures/rh-logo.png");
    texture.Bind();

    // '0' here is the texture slot we assigned the texture. 0 is default, otherwise match whatever is in texture.Bind(x) above
    shader.SetUniform1i("u_Texture", 0);

    // Set the projection matrix in the shader
    shader.SetUniformMat4f("u_MVP", proj);

    va.Unbind();
    shader.Unbind();
    vb.Unbind();
    ib.Unbind();

    Renderer renderer;

    float r = 0.0f;
    float increment = 0.05f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();

        shader.Bind();

        renderer.Draw(va, ib, shader);

        if (r > 1.0f)
        {
            increment = -0.05f;
        }
        if (r < 0.0f)
        {
            increment = 0.05f;
        }

        r += increment;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
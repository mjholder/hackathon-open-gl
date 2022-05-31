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
    // Due to projection these are now in pixel coords
    float positions[] = {
        100.0f, 100.0f, 0.0f, 0.0f, // 0
        540.0f, 100.0f, 1.0f, 0.0f, // 1
        540.0f, 380.0f, 1.0f, 1.0f, // 2
        100.0f, 380.0f, 0.0f, 1.0f, // 3
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
    // Project our space as a per pixel space rather than normalized
    glm::mat4 proj = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f);

    /* Create view matrix */
    // Move "camera" to the right by moving everything to the left
    //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));

    /* Create model matrix */
    // the hat will move up 200 pixels and to the right 200 pixel
    //glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));

    /* Start construction of our full MVP matrix */
    //glm::mat4 mvp = proj * view * model;
    glm::mat4 mvp = proj;

    Shader shader("/home/maholder/git_repos/hackathon-open-gl/res/shaders/Basic.shader");
    shader.Bind();

    Texture texture("/home/maholder/git_repos/hackathon-open-gl/res/textures/rh-logo.png");
    texture.Bind();

    // '0' here is the texture slot we assigned the texture. 0 is default, otherwise match whatever is in texture.Bind(x) above
    shader.SetUniform1i("u_Texture", 0);

    // Set the projection matrix in the shader
    shader.SetUniformMat4f("u_MVP", mvp);

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
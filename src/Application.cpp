#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define GLCall(x) GLClearError;\
    x;\
    GLLogCall(#x, __FILE__, __LINE__)

static void GLClearError()
{
    // keep polling backlog of errors until we read them all
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    // loop through errors, printing them
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << ")" 
        << function << ' ' << file << ':' << line << std::endl;
        return false;
    }

    return true;
}

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};


static ShaderProgramSource ParseShader(const std::string& filepath)
{
    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1,
    };

    std::fstream stream(filepath);
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    GLCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));

        std::cout << "Failed to compile " <<
        (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
        << " shader!" << std::endl;

        std::cout << message << std::endl;

        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

// These two strings are going to be the source code for the vertex and 
// fragment shader
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    GLCall(unsigned int program = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    // these are no longer needed as the program has the shader already
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

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

    // 6 vertex positions (x, y)
    // Note repeate of some vertices, these are extra and wasteful
    float positions[12] = {
        -0.5f, -0.5f, // 0
         0.5f, -0.5f, // 1
         0.5f,  0.5f, // 2
        -0.5f,  0.5f, // 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    /* define vertext buffer */
    /* `buffer` holds the id of this buffer */
    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    // This sets the state that OpenGL is in, the later Draw Call uses the state
    // that this is setting up
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(0));
    // Tell OpenGL how to read the vertices we provide
    // the first argument is binding index 0 of the vao to the currently bound GL_ARRAY_BUFFER
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    /* define index buffer */
    /* `ibo` holds the id of this buffer */
    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    // This sets the state that OpenGL is in, the later Draw Call uses the state
    // that this is setting up
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    ShaderProgramSource source = ParseShader("/home/maholder/git_repos/hackathon-open-gl/res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    // Binding shader to program
    GLCall(glUseProgram(shader));

    // Retrieve location of u_Color variable
    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    // Set value of u_Color in shader
    GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    float r = 0.0f;
    float increment = 0.05f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        GLCall(glUseProgram(shader));
        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

        GLCall(glBindVertexArray(vao));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

        /* Draw call that will draw our triangle */
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

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
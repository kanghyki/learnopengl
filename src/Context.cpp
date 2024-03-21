#include "Context.hpp"

Context::Context()
{}

Context::~Context()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    pProgram = nullptr;
}

std::unique_ptr<Context> Context::create()
{
    auto context = std::unique_ptr<Context>(new Context());
    if (!context->init())
    {
        return nullptr;
    }

    return std::move(context);
}

void Context::render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    pProgram->Use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool Context::init()
{
    auto vertexShader = Shader::createFromFile("shader/vs.glsl", GL_VERTEX_SHADER);
    auto fragmentShader = Shader::createFromFile("shader/fs.glsl", GL_FRAGMENT_SHADER);
    if (!vertexShader || !fragmentShader)
    {
        return 0;
    }

    pProgram = Program::create({vertexShader, fragmentShader});
    if (!pProgram)
    {
        return 0;
    }
    vertexShader = nullptr;
    fragmentShader = nullptr;

    float vertices[] = {
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    // VAO/VBO/EBO
    glGenVertexArrays(1, &VAO);  
    glGenBuffers(1, &VBO);  
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    // wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    return true;
}
#include "Base/ECSManager.h"
#include "Components/ComponentHeaders.h"
#include "Systems/SystemHeader.h"


#include <iostream>
#include <chrono>
#include <GLFW/glfw3.h>

ECSManager EcsManager;


int main()
{
    //ECS setup
    EcsManager.Setup();

    //Register components
    EcsManager.RegisterComponent<RendererData>();

    //Register systems
    auto rendererComponent = EcsManager.RegisterSystem<Renderer>();

    //Setup signatures
    EcsManager.SetSignature<Renderer>(Renderer::GetSignature());

    std::default_random_engine random;

    //Add entities
    for (Entity entity = 0; entity < MAXENTITIES; ++entity)
    {
        EcsManager.CreateEntity();

        std::uniform_real_distribution<float> randomPosition(0.0f, 700.0f);
        std::uniform_real_distribution<float> randomSize(1.0f, 30.0f);
        std::uniform_real_distribution<float> randomColor(0.0f, 1.0f);

        EcsManager.AddComponent(entity, RendererData{randomPosition(random), randomPosition(random), randomSize(random), randomSize(random), randomColor(random), randomColor(random), randomColor(random)});
    }


    //Initialize glfw3
    if (!glfwInit())
    {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(700, 700, "Rendering Test", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    //Set vsync (0 = disabled)
    glfwSwapInterval( 0 );


    //Set up the viewport
    glViewport(0, 0, 700, 700);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 700, 0, 700, -1, 1);


    int frameCount = 0;
    auto start = std::chrono::steady_clock::now();


    while (!glfwWindowShouldClose(window))
    {
        frameCount++;

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - start;

        double dt = 1.0 / frameCount;

        if (elapsed.count() >= 1.0)
        {
            std::cout << "FPS: " << frameCount << "   Delta: " << dt << std::endl;

            frameCount = 0;
            start = now;
        }

        //Render
        glClear(GL_COLOR_BUFFER_BIT);
        rendererComponent->Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
// simple_simulation/main.cpp
// Author: Kai Lindskog
// Date: November 11, 2025

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "simulation.h"

using namespace std;

int main(int, char**) {
    if (!glfwInit()) return 1;
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "A-Life Simulation - Kai Lindskog", NULL, NULL);
    if (!window) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    Simulation sim;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        sim.update();

        // Render food
        ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
        for (const auto& f : sim.food) {
            if (!f.eaten) {
                draw_list->AddCircleFilled(ImVec2(f.x, f.y), 5.0f, IM_COL32(100, 255, 100, 255));
            }
        }

        // Render organisms
        for (const auto& org : sim.organisms) {
            draw_list->AddCircleFilled(ImVec2(org.x, org.y), 8.0f, 
                IM_COL32(org.r * 255, org.g * 255, org.b * 255, 255));
            draw_list->AddCircle(ImVec2(org.x, org.y), 8.0f, IM_COL32(255, 255, 255, 100), 12, 1.0f);
        }

        // UI Controls
        {
            ImGui::Begin("Simulation Controls");
            ImGui::Text("Organisms: %zu", sim.organisms.size());
            ImGui::Text("Food: %zu", sim.food.size());
            ImGui::Separator();
            
            ImGui::SliderInt("Starting Organisms", &sim.starting_organisms, 1, 20);
            ImGui::SliderInt("Starting Food", &sim.starting_food, 10, 100);
            ImGui::SliderFloat("Food Spawn Rate", &sim.food_spawn_rate, 0.01f, 0.1f, "%.3f");
            
            if (ImGui::Button("Add Organism")) sim.spawn_organism();
            ImGui::SameLine();
            if (ImGui::Button("Add Food (10)")) sim.spawn_food(10);
            
            if (ImGui::Button("Reset Simulation")) sim.reset();
            ImGui::End();
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

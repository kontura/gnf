//#include <cstdlib>
//#include <fstream>
//#include <map>
//#include <iostream>
//
#include <libdnf/base/base.hpp>
#include <libdnf/rpm/repo.hpp>
#include <libdnf/rpm/solv_query.hpp>

////#define GLFW_INCLUDE_VULKAN
//#include <GLFW/glfw3.h>
//
//
//#include <stdlib.h>
//#include <stdio.h>

//static const struct
//{
//    float x, y;
//    float r, g, b;
//} vertices[3] =
//{
//    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
//    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
//    {   0.f,  0.6f, 0.f, 0.f, 1.f }
//};
// 
//static const char* vertex_shader_text =
//"#version 110\n"
//"uniform mat4 MVP;\n"
//"attribute vec3 vCol;\n"
//"attribute vec2 vPos;\n"
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
//"    color = vCol;\n"
//"}\n";
// 
//static const char* fragment_shader_text =
//"#version 110\n"
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"    gl_FragColor = vec4(color, 1.0);\n"
//"}\n";
// 
//static void error_callback(int error, const char* description)
//{
//    fprintf(stderr, "Error: %s\n", description);
//}
// 
//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, GLFW_TRUE);
//}
//
//
//
//int main(int argc, const char *argv[])
//{
//
//	std::string sourcePath = argv[2];

    // create a new Base object
//    libdnf::Base base;
//    auto & conf = base.get_config();
//    std::string installroot("/home/amatej/tmp/sourcetrail_rpm_test");
//    conf.installroot().set(libdnf::Option::Priority::RUNTIME, installroot);
//    conf.cachedir().set(libdnf::Option::Priority::RUNTIME, installroot + "/var/cache/dnf");
//    auto & repo_sack = base.get_rpm_repo_sack();
//        std::cout << "pkg: " << sourcePath <<  std::endl;
//    repo_sack.new_repos_from_file(sourcePath);
//    auto repos = repo_sack.new_query();
//
//    std::map<std::string, std::string> m { {"basearch", "x86_64"}, {"releasever", "35"}, };
//
//    // create a reference to the Base's rpm_sack for better code readability
//    auto & solv_sack = base.get_rpm_solv_sack();
//
//    using LoadFlags = libdnf::rpm::SolvSack::LoadRepoFlags;
//    auto flags = LoadFlags::USE_FILELISTS | LoadFlags::USE_PRESTO | LoadFlags::USE_UPDATEINFO | LoadFlags::USE_OTHER;
//
//    for (auto & repo : repos.get_data()) {
//        (*repo.get()).set_substitutions(m);
//        (*repo.get()).load();
//        solv_sack.load_repo((*repo.get()), flags);
//    }
//
//    libdnf::rpm::SolvQuery query(&solv_sack);
//    query.ifilter_arch({"x86_64"}).ifilter_latest(1);
//
//	std::cout << "done!" << std::endl;
    //PFN_vkCreateInstance instance = (PFN_vkCreateInstance) glfwGetInstanceProcAddress(NULL, "vkCreateInstance");
    //PFN_vkCreateDevice pfnCreateDevice = (PFN_vkCreateDevice) glfwGetInstanceProcAddress(instance, "vkCreateDevice");
    //PFN_vkGetDeviceProcAddr pfnGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr) glfwGetInstanceProcAddress(instance, "vkGetDeviceProcAddr");

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(void)
{
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);

        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);

        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(-0.6f, -0.4f, 0.f);
        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(0.6f, -0.4f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.6f, 0.f);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}



//    if (glfwVulkanSupported())
//    {
//        printf("Supported vulkan\n\n");
//        // Vulkan is available, at least for compute
//    } else {
//        printf("vulkan not Supported\n\n");
//    }
//
//
//    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//    GLFWwindow* window = glfwCreateWindow(640, 480, "Window Title", NULL, NULL);
    //VkSurfaceKHR surface;
    //VkResult err = glfwCreateWindowSurface(instance, window, NULL, &surface);
    //if (err)
    //{
    //    // Window surface creation failed
    //}


//    getchar(); // wait for ENTER


//    GLFWwindow* window;
//    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
//    GLint mvp_location, vpos_location, vcol_location;
// 
//    glfwSetErrorCallback(error_callback);
// 
//    if (!glfwInit())
//        exit(EXIT_FAILURE);
// 
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//
//        window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
//    if (!window)
//    {
//        glfwTerminate();
//        exit(EXIT_FAILURE);
//    }
// 
//    glfwSetKeyCallback(window, key_callback);
// 
//    glfwMakeContextCurrent(window);
//    gladLoadGL(glfwGetProcAddress);
//    glfwSwapInterval(1);
// 
//    // NOTE: OpenGL error checks have been omitted for brevity
// 
//    glGenBuffers(1, &vertex_buffer);
//    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// 
//    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
//    glCompileShader(vertex_shader);
// 
//    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
//    glCompileShader(fragment_shader);
// 
//    program = glCreateProgram();
//    glAttachShader(program, vertex_shader);
//    glAttachShader(program, fragment_shader);
//    glLinkProgram(program);
// 
//    mvp_location = glGetUniformLocation(program, "MVP");
//    vpos_location = glGetAttribLocation(program, "vPos");
//    vcol_location = glGetAttribLocation(program, "vCol");
// 
//    glEnableVertexAttribArray(vpos_location);
//    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
//                          sizeof(vertices[0]), (void*) 0);
//    glEnableVertexAttribArray(vcol_location);
//    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
//                          sizeof(vertices[0]), (void*) (sizeof(float) * 2));
// 
//    while (!glfwWindowShouldClose(window))
//    {
//        float ratio;
//        int width, height;
//        mat4x4 m, p, mvp;
// 
//        glfwGetFramebufferSize(window, &width, &height);
//        ratio = width / (float) height;
// 
//        glViewport(0, 0, width, height);
//        glClear(GL_COLOR_BUFFER_BIT);
// 
//        mat4x4_identity(m);
//        mat4x4_rotate_Z(m, m, (float) glfwGetTime());
//        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
//        mat4x4_mul(mvp, p, m);
// 
//        glUseProgram(program);
//        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
//        glDrawArrays(GL_TRIANGLES, 0, 3);
// 
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
// 
//    glfwDestroyWindow(window);
// 
//    glfwTerminate();
//    exit(EXIT_SUCCESS);


//
//	return 0;
//}

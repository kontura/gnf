#include "gnf.hpp"
#include "package_layout.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#define DISPLAY_WIDTH 1920
#define DISPLAY_HEIGHT 1080

gnfContext gnf = {
    .width = DISPLAY_WIDTH,
    .height = DISPLAY_HEIGHT,
};

//TODO(amatej): Do not render stuff outside of view, but how though?

const char *const vert_shader_source =
    "#version 330 core\n"
    "\n"
    "uniform vec2 resolution;\n"
    "\n"
    "layout(location = 0) in vec2 position;\n"
    "layout(location = 1) in vec4 color;\n"
    "layout(location = 2) in vec2 uv;\n"
    "\n"
    "out vec4 output_color;\n"
    "out vec2 output_uv;\n"
    "\n"
    "vec2 flip(vec2 p) {\n"
    "    return vec2(p.x, resolution.y - p.y);\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    gl_Position = vec4((flip(position) - resolution * 0.5) / (resolution * 0.5), 0.0, 1.0) - vec4(0, 0, 0, 0);\n"
    "    output_color = color;\n"
    "    output_uv = uv;\n"
    "}\n"
    "\n";

const char *const frag_shader_source =
    "#version 330 core\n"
    "\n"
    "uniform sampler2D font;\n"
    "\n"
    "in vec4 output_color;\n"
    "in vec2 output_uv;\n"
    "out vec4 final_color;\n"
    "\n"
    "void main() {\n"
    "    vec4 pixel = texture(font, output_uv);\n"
    "    final_color = pixel.x * output_color;\n"
    "}\n"
    "\n";

const char *shader_type_as_cstr(GLuint shader)
{
    switch (shader) {
    case GL_VERTEX_SHADER:
        return "GL_VERTEX_SHADER";
    case GL_FRAGMENT_SHADER:
        return "GL_FRAGMENT_SHADER";
    default:
        return "(Unknown)";
    }
}

bool compile_shader_source(const GLchar *source, GLenum shader_type, GLuint *shader)
{
    *shader = glCreateShader(shader_type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);

    GLint compiled = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(*shader, sizeof(message), &message_size, message);
        fprintf(stderr, "ERROR: could not compile %s\n", shader_type_as_cstr(shader_type));
        fprintf(stderr, "%.*s\n", message_size, message);
        return false;
    }

    return true;
}

bool link_program(GLuint vert_shader, GLuint frag_shader, GLuint *program)
{
    *program = glCreateProgram();

    glAttachShader(*program, vert_shader);
    glAttachShader(*program, frag_shader);
    glLinkProgram(*program);

    GLint linked = 0;
    glGetProgramiv(*program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLsizei message_size = 0;
        GLchar message[1024];

        glGetProgramInfoLog(*program, sizeof(message), &message_size, message);
        fprintf(stderr, "Program Linking: %.*s\n", message_size, message);
    }

    return program;
}

typedef enum {
    GNF_POSITION_ATTRIB = 0,
    GNF_COLOR_ATTRIB,
    GNF_UV_ATTRIB,
    COUNT_GNF_ATTRIBS
} gnf_Attribs;

typedef struct {
    GLuint vao;
    GLuint vert_vbo;
    GLuint font_texture;
} gnf_GL;

void gnf_gl_begin(gnf_GL *gnf_gl, const gnfContext *gnf)
{
    glGenVertexArrays(1, &gnf_gl->vao);
    glBindVertexArray(gnf_gl->vao);

    glGenBuffers(1, &gnf_gl->vert_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gnf_gl->vert_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(gnf->vertices),
                 gnf->vertices,
                 GL_DYNAMIC_DRAW);

    // Position
    {
        const gnf_Attribs attrib = GNF_POSITION_ATTRIB;
        glEnableVertexAttribArray(attrib);
        glVertexAttribPointer(
            attrib,             // index
            2,                  // numComponents
            GL_FLOAT,           // type
            0,                  // normalized
            sizeof(gnf->vertices[0]), // stride
            (void*) offsetof(Vertex, position)  // offset
        );
    }

    // Color
    {
        const gnf_Attribs attrib = GNF_COLOR_ATTRIB;
        glEnableVertexAttribArray(attrib);
        glVertexAttribPointer(
            attrib,             // index
            4,                  // numComponents
            GL_FLOAT,           // type
            0,                  // normalized
            sizeof(gnf->vertices[0]), // stride
            (void*) offsetof(Vertex, color) // offset
        );
    }

    // UV
    {
        const gnf_Attribs attrib = GNF_UV_ATTRIB;
        glEnableVertexAttribArray(attrib);
        glVertexAttribPointer(
            attrib,             // index
            2,                  // numComponents
            GL_FLOAT,           // type
            0,                  // normalized
            sizeof(gnf->vertices[0]), // stride
            (void*) offsetof(Vertex, uv) // offset
        );
    }

    static_assert(COUNT_GNF_ATTRIBS == 3, "The amount of gnf Vertex attributes have changed");

    // Font Texture
    {
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &gnf_gl->font_texture);
        glBindTexture(GL_TEXTURE_2D, gnf_gl->font_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FONT_WIDTH, FONT_HEIGHT, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, FONT);

        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

void gnf_gl_render(gnf_GL *gnf_gl, const gnfContext *gnf)
{
    glBindVertexArray(gnf_gl->vao);
    glBindBuffer(GL_ARRAY_BUFFER, gnf_gl->vert_vbo);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        gnf->vertices_count * sizeof(gnf->vertices[0]),
        gnf->vertices);

    glDrawElements(GL_TRIANGLES,
                   gnf->triangles_count * TRIANGLE_COUNT,
                   GL_UNSIGNED_INT,
                   gnf->triangles);
}

void MessageCallback(GLenum source,
                     GLenum type,
                     GLuint id,
                     GLenum severity,
                     GLsizei length,
                     const GLchar* message,
                     const void* userParam)
{
    (void) source;
    (void) id;
    (void) length;
    (void) userParam;
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    (void) window;
    glViewport(
        width / 2 - DISPLAY_WIDTH / 2,
        height / 2 - DISPLAY_HEIGHT / 2,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    const double offset_x = width/2 - DISPLAY_WIDTH/2;
    const double offset_y = height/2 - DISPLAY_HEIGHT/2;

    gnf_mouse_move(&gnf, xpos-offset_x, ypos-offset_y);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    (void) mods;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            gnf_mouse_down(&gnf);
        }

        if (action == GLFW_RELEASE) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            gnf_mouse_up(&gnf);
        }
    }
}

static void load_and_select_pkg(gnfContext *gnf) {
//     std::string sourcePath = argv[2];

    // create a new Base object
    libdnf::Base & base = gnf->base;
    auto & conf = base.get_config();
    std::string installroot("/home/amatej/usr/src/gnf2/build");
    conf.installroot().set(libdnf::Option::Priority::RUNTIME, installroot);
    conf.cachedir().set(libdnf::Option::Priority::RUNTIME, installroot + "/var/cache/dnf");
    auto & repo_sack = base.get_rpm_repo_sack();
    repo_sack.new_repos_from_file("/etc/yum.repos.d/fedora-rawhide.repo");
    auto repos = repo_sack.new_query();

    std::map<std::string, std::string> m { {"basearch", "x86_64"}, {"releasever", "35"}, };

    // create a reference to the Base's rpm_sack for better code readability
    auto & solv_sack = base.get_rpm_solv_sack();

    using LoadFlags = libdnf::rpm::SolvSack::LoadRepoFlags;
    auto flags = LoadFlags::USE_FILELISTS | LoadFlags::USE_PRESTO | LoadFlags::USE_UPDATEINFO | LoadFlags::USE_OTHER;

    for (auto & repo : repos.get_data()) {
        (*repo.get()).set_substitutions(m);
        (*repo.get()).load();
        solv_sack.load_repo((*repo.get()), flags);
    }
}

int main(void)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "gnf", NULL, NULL);

    if (!window)
    {
        fprintf(stderr, "ERROR: could not create a window.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    glewInit();

    glEnable(GL_DEBUG_OUTPUT);

    glDebugMessageCallback(MessageCallback, 0);


    glfwSetFramebufferSizeCallback(window, window_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint vert_shader = 0;
    if (!compile_shader_source(vert_shader_source, GL_VERTEX_SHADER, &vert_shader)) {
        exit(1);
    }

    GLuint frag_shader = 0;
    if (!compile_shader_source(frag_shader_source, GL_FRAGMENT_SHADER, &frag_shader)) {
        exit(1);
    }

    GLuint program = 0;
    if (!link_program(vert_shader, frag_shader, &program)) {
        exit(1);
    }
    glUseProgram(program);

    GLuint resolutionUniform = glGetUniformLocation(program, "resolution");
    glUniform2f(resolutionUniform,
                (float) DISPLAY_WIDTH,
                (float) DISPLAY_HEIGHT);

    gnf_GL gnf_gl = {0};

    gnf_gl_begin(&gnf_gl, &gnf);

    load_and_select_pkg(&gnf);
    packageLayoutData pkgLayout = {
        //TODO(amatej): don't store queries but packagesets? or no?
        .package = libdnf::rpm::SolvQuery(&(gnf.base.get_rpm_solv_sack()), libdnf::rpm::SolvQuery::InitFlags::EMPTY),
        .requires = libdnf::rpm::SolvQuery(&(gnf.base.get_rpm_solv_sack()), libdnf::rpm::SolvQuery::InitFlags::EMPTY),
        .provides = libdnf::rpm::SolvQuery(&(gnf.base.get_rpm_solv_sack()), libdnf::rpm::SolvQuery::InitFlags::EMPTY),
        .reqs = libdnf::rpm::ReldepList(&(gnf.base.get_rpm_solv_sack())),
        .provs = libdnf::rpm::ReldepList(&(gnf.base.get_rpm_solv_sack())),
        .selectedActiveProvideReldeps = libdnf::rpm::ReldepList(&(gnf.base.get_rpm_solv_sack())),
        .selectedActiveRequireReldeps = libdnf::rpm::ReldepList(&(gnf.base.get_rpm_solv_sack())),
        .selectedActivePackages = libdnf::rpm::SolvQuery(&(gnf.base.get_rpm_solv_sack()), libdnf::rpm::SolvQuery::InitFlags::EMPTY),
    };
    load_package_data(&gnf, &pkgLayout, "libdnf");


    while (!glfwWindowShouldClose(window))
    {
        gnf_begin(&gnf);

        layout_package(&gnf, &pkgLayout);

        if (gnf.mouse_buttons & BUTTON_LEFT) {
            if (gnf.active == 0) {
                gnf.view_offset = vec2(gnf.view_offset.x - gnf.mouse_pos_delta.x, gnf.view_offset.y - gnf.mouse_pos_delta.y);
                gnf.mouse_pos_delta = vec2(0, 0);
            }
        }

        //search_box(&gnf);

        printf("vertices counte: %lu\n", gnf.vertices_count);
        gnf_end(&gnf);

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        gnf_gl_render(&gnf_gl, &gnf);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);

}


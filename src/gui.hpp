#ifndef GNF_GUI_HEADER
#define GNF_GUI_HEADER

#include <stdlib.h>
#include <cassert>
#include <string.h>
#include <math.h>

#include <libdnf/base/base.hpp>
#include <libdnf/rpm/repo.hpp>
#include <libdnf/rpm/solv_query.hpp>

#include "font.hpp"

#define VERTICES_CAPACITY (1024 * 1024 * 20)
#define TRIANGLES_CAPACITY (1024 * 1024 * 20)

#define GNF_BUTTON_COLOR rgba(0.36f, 0.847f, 0.35f, 1.0f)
#define GNF_BUTTON_COLOR_HOT rgba(0.0627f, 0.470f, 0.411f, 1.0f)
#define GNF_BUTTON_COLOR_ACTIVE rgba(1.0f, 0.0f, 0.0f, 1.0f)
#define GNF_PACKAGE_COLOR_TEXT_BG rgba(0.0f, 0.0f, 0.0f, 1.0f)
#define GNF_BUTTON_TEXT_SCALE 1.0f
#define GNF_BUTTON_TEXT_COLOR rgba(1.0f, 1.0f, 1.0f, 1.0f)

#define GNF_WHITE rgba(1.0f, 1.0f, 1.0f, 1.0f)
#define GNF_BLACK rgba(0.0f, 0.0f, 0.0f, 1.0f)

struct Vec2 {
    float x, y;
};


Vec2 vec2(float x, float y);
Vec2 operator*(const Vec2& v1, float s);
Vec2 operator+(const Vec2& v1, const Vec2& v2);
Vec2 operator+=(Vec2& v1, const Vec2& v2);
Vec2 operator-(const Vec2& v1, const Vec2& v2);
Vec2 normalize(Vec2 i);


#define RGBA_COUNT 4

typedef struct {
    float r, g, b, a;
} RGBA;

RGBA rgba(float r, float g, float b, float a);
RGBA operator+(const RGBA& v1, const RGBA& v2);

typedef struct {
    Vec2 position;
    RGBA color;
    Vec2 uv;
} Vertex;

Vertex vertex(Vec2 position, RGBA color, Vec2 uv);

#define TRIANGLE_COUNT 3

typedef struct {
    unsigned int a, b, c;
} Triangle;

Triangle triangle(unsigned int a, unsigned int b, unsigned int c);

typedef enum {
    BUTTON_LEFT = 1,
} Buttons;

typedef enum {
    GNF_KEY_NONE = 0,
    GNF_KEY_BACKSPACE = 1,
    GNF_KEY_ENTER = 2,
} Key;

typedef enum {
    GNF_GUI_STATE_PACKAGE_LAYOUT = 0,
    GNF_GUI_STATE_PACKAGE_GRAPH = 1,
} GUIState;

typedef int gnf_ID;

typedef struct {
    size_t width, height;

    gnf_ID active;

    Vec2 mouse_pos;
    Vec2 mouse_pos_delta;
    Buttons mouse_buttons;
    Key pressed_key;
    char pressed_character;

    Vec2 view_offset;
    float zoom;

    Vertex vertices[VERTICES_CAPACITY];
    size_t vertices_count;

    Triangle triangles[TRIANGLES_CAPACITY];
    size_t triangles_count;
    Vec2 last_widget_position;

    libdnf::Base base;

    bool absolute_screen_coords;

    GUIState state;
} gnfContext;

void gnf_render_char(gnfContext *gnf, Vec2 p, float s, RGBA color, int c);
bool gnf_render_selectable_text(gnfContext *gnf, Vec2 p, float s, RGBA text_color, const char *text, bool active);
void gnf_render_text(gnfContext *gnf, Vec2 p, float s, RGBA color, const char *text);

void gnf_mouse_down(gnfContext *gnf);
void gnf_mouse_up(gnfContext *gnf);
void gnf_mouse_move(gnfContext *gnf, float x, float y);
void gnf_set_pressed_character(gnfContext *gnf, const char character);
void gnf_set_pressed_key(gnfContext *gnf, Key key);
void gnf_begin(gnfContext *gnf);
void gnf_end(gnfContext *gnf);

void gnf_text(gnfContext *gnf, const char *text);
bool gnf_rect_contains(gnfContext *gnf, Vec2 p, Vec2 s);
void gnf_fill_rect(gnfContext *gnf, Vec2 p, Vec2 s, RGBA c);

bool gnf_button(gnfContext *gnf, Vec2 p, Vec2 s, const char *text, gnf_ID id);

void gnf_fill_quad(gnfContext *gnf, Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3, RGBA c);

void gnf_fill_quad_with_char(gnfContext *gnf, Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3, RGBA c, int i);

void gnf_draw_line(gnfContext *gnf, Vec2 start, Vec2 end, float thickness, RGBA color);

void gnf_draw_cube(gnfContext *gnf, Vec2 p, float size);

#endif // GNF_GUI_HEADER

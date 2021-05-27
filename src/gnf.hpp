#ifndef GNF_HEADER
#define GNF_HEADER

#include <stdlib.h>
#include <cassert>
#include <string.h>
#include <math.h>

#include <libdnf/base/base.hpp>
#include <libdnf/rpm/repo.hpp>
#include <libdnf/rpm/solv_query.hpp>

#include "font.hpp"

#define VERTICES_CAPACITY (1024 * 1024 * 10)
#define TRIANGLES_CAPACITY (1024 * 1024 * 10)

#define GNF_BUTTON_COLOR rgba(0.36f, 0.847f, 0.35f, 1.0f)
#define GNF_BUTTON_COLOR_HOT rgba(0.0627f, 0.470f, 0.411f, 1.0f)
#define GNF_BUTTON_COLOR_ACTIVE rgba(1.0f, 0.0f, 0.0f, 1.0f)
#define GNF_PACKAGE_COLOR_TEXT_BG rgba(0.0f, 0.0f, 0.0f, 1.0f)
#define GNF_BUTTON_TEXT_SCALE 1.0f
#define GNF_BUTTON_TEXT_COLOR rgba(1.0f, 1.0f, 1.0f, 1.0f)

#define GNF_WHITE rgba(1.0f, 1.0f, 1.0f, 1.0f)

typedef struct {
    float x, y;
} Vec2;


Vec2 vec2(float x, float y)
{
    return (Vec2) {
        x, y
    };
}

Vec2 normalize(Vec2 i) {
    float len = sqrt(i.x*i.x + i.y*i.y);
    return vec2(i.x/len, i.y/len);
}


#define RGBA_COUNT 4

typedef struct {
    float r, g, b, a;
} RGBA;

RGBA rgba(float r, float g, float b, float a)
{
    return (RGBA) {
        r, g, b, a
    };
}

typedef struct {
    Vec2 position;
    RGBA color;
    Vec2 uv;
} Vertex;

Vertex vertex(Vec2 position, RGBA color, Vec2 uv)
{
    return (Vertex) {
        .position = position,
        .color = color,
        .uv =uv,
    };
}

#define TRIANGLE_COUNT 3

typedef struct {
    unsigned int a, b, c;
} Triangle;

Triangle triangle(unsigned int a, unsigned int b, unsigned int c)
{
    return (Triangle) {
        .a = a,
        .b = b,
        .c = c,
    };
}

typedef enum {
    BUTTON_LEFT = 1,
} Buttons;

typedef int gnf_ID;

typedef struct {
    size_t width, height;

    gnf_ID active;

    Vec2 mouse_pos;
    Vec2 mouse_pos_delta;
    Buttons mouse_buttons;

    Vec2 view_offset;

    Vertex vertices[VERTICES_CAPACITY];
    size_t vertices_count;

    Triangle triangles[TRIANGLES_CAPACITY];
    size_t triangles_count;
    Vec2 last_widget_position;

    libdnf::Base base;
} gnfContext;

static unsigned int gnf_append_vertex(gnfContext *gnf, Vertex v)
{
    v.position = vec2(v.position.x + gnf->view_offset.x, v.position.y + gnf->view_offset.y);

    //const Vec2 p = vec2(position.x + gnf->view_offset.x, position.y + gnf->view_offset.y);
    assert(gnf->vertices_count < VERTICES_CAPACITY);
    unsigned int result = gnf->vertices_count;
    gnf->vertices[gnf->vertices_count++] = v;
    return result;
}

static void gnf_append_triangle(gnfContext *gnf, Triangle t)
{
    assert(gnf->triangles_count < TRIANGLES_CAPACITY);
    gnf->triangles[gnf->triangles_count++] = t;
}

static Vec2 gnf_font_uv(size_t pix_x, size_t pix_y) {
    const float uv_x = (float) pix_x / (float) FONT_WIDTH;
    const float uv_y = (float) pix_y / (float) FONT_HEIGHT;
    return vec2(uv_x, uv_y);
}

static void gnf_char_uv(int c, Vec2 *uv_p, Vec2 *uv_s) {
    if (32 <= c && c <= 127) {
        const size_t index = c - 32;
        const size_t cell_x = index % FONT_COLS;
        const size_t cell_y = index / FONT_COLS;

        const size_t pix_x = cell_x * FONT_CHAR_WIDTH;
        const size_t pix_y = cell_y * FONT_CHAR_HEIGHT;

        *uv_p = gnf_font_uv(pix_x, pix_y);
        *uv_s = gnf_font_uv(FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT);
    } else {
        gnf_char_uv(FONT_SOLID_CHAR, uv_p, uv_s);
    }
}

static void gnf_fill_rect_char(gnfContext *gnf, Vec2 p, Vec2 s, RGBA c, int ch) {
    Vec2 uv_p, uv_s;
    gnf_char_uv(ch, &uv_p, &uv_s);

    const unsigned int p0 = gnf_append_vertex(gnf, vertex(p, c, uv_p));
    const unsigned int p1 = gnf_append_vertex(gnf, vertex(vec2(p.x + s.x, p.y), c, vec2(uv_p.x + uv_s.x, uv_p.y)));
    const unsigned int p2 = gnf_append_vertex(gnf, vertex(vec2(p.x, p.y + s.y), c, vec2(uv_p.x, uv_p.y + uv_s.y)));
    const unsigned int p3 = gnf_append_vertex(gnf, vertex(vec2(p.x + s.x, p.y + s.y), c, vec2(uv_p.x + uv_s.x, uv_p.y + uv_s.y)));

    gnf_append_triangle(gnf, triangle(p0, p1, p2));
    gnf_append_triangle(gnf, triangle(p1, p2, p3));
}

static void gnf_fill_rect(gnfContext *gnf, Vec2 p, Vec2 s, RGBA c) {
    gnf_fill_rect_char(gnf, p, s, c, FONT_SOLID_CHAR);
}

//static void gnf_fill_rect_border(gnfContext *gnf, Vec2 origin, Vec2 s, float thickness, RGBA c)
//{
//    const unsigned int outer0 = gnf_append_vertex(gnf, vertex(origin, c));
//    const unsigned int outer1 = gnf_append_vertex(gnf, vertex(vec2(origin.x + s.x, origin.y), c));
//    const unsigned int outer2 = gnf_append_vertex(gnf, vertex(vec2(origin.x, origin.y + s.y), c));
//    const unsigned int outer3 = gnf_append_vertex(gnf, vertex(vec2(origin.x + s.x, origin.y + s.y), c));
//
//    const unsigned int inner0 = gnf_append_vertex(gnf, vertex(vec2(origin.x + thickness, origin.y + thickness), c));
//    const unsigned int inner1 = gnf_append_vertex(gnf, vertex(vec2(origin.x + s.x - thickness, origin.y + thickness), c));
//    const unsigned int inner2 = gnf_append_vertex(gnf, vertex(vec2(origin.x + thickness, origin.y + s.y - thickness), c));
//    const unsigned int inner3 = gnf_append_vertex(gnf, vertex(vec2(origin.x + s.x - thickness, origin.y + s.y - thickness), c));
//
//    //top
//    gnf_append_triangle(gnf, triangle(outer0, outer1, inner0));
//    gnf_append_triangle(gnf, triangle(outer1, inner0, inner1));
//
//    //left
//    gnf_append_triangle(gnf, triangle(outer0, outer2, inner0));
//    gnf_append_triangle(gnf, triangle(outer2, inner0, inner2));
//
//    //right
//    gnf_append_triangle(gnf, triangle(outer1, outer3, inner1));
//    gnf_append_triangle(gnf, triangle(outer3, inner1, inner3));
//
//    //bottom
//    gnf_append_triangle(gnf, triangle(outer2, outer3, inner2));
//    gnf_append_triangle(gnf, triangle(outer3, inner2, inner3));
//}

void gnf_render_char(gnfContext *gnf, Vec2 p, float s, RGBA color, int c)
{
    gnf_fill_rect_char(gnf, p, vec2((float) FONT_CHAR_WIDTH * s, (float) FONT_CHAR_HEIGHT * s), color, c);
}

static bool gnf_rect_contains(gnfContext *gnf, Vec2 p, Vec2 s)
{
    Vec2 t = gnf->mouse_pos;
    p = vec2(p.x + gnf->view_offset.x, p.y + gnf->view_offset.y);
    return p.x <= t.x && t.x < p.x + s.x &&
           p.y <= t.y && t.y < p.y + s.y;
}

bool gnf_render_selectable_text(gnfContext *gnf, Vec2 p, float s, RGBA text_color, const char *text, bool active)
{
    const size_t n = strlen(text);
    bool hot = false;
    bool cursos_overlap = gnf_rect_contains(gnf, p, vec2(n*FONT_CHAR_WIDTH*s, FONT_CHAR_HEIGHT*s));
    if (cursos_overlap || active) {
        gnf_fill_rect(
            gnf,
            vec2(p.x, p.y),
            vec2(n*FONT_CHAR_WIDTH*s, FONT_CHAR_HEIGHT*s),
            GNF_PACKAGE_COLOR_TEXT_BG);
        if (cursos_overlap) {
            hot = true;
        }
    }
    for (size_t i = 0; i < n; ++i) {
        gnf_render_char(
            gnf,
            vec2(p.x + i * s * FONT_CHAR_WIDTH, p.y),
            s,
            text_color,
            text[i]);
    }
    return hot;
}

void gnf_render_text(gnfContext *gnf, Vec2 p, float s, RGBA color, const char *text)
{
    const size_t n = strlen(text);
    for (size_t i = 0; i < n; ++i) {
        gnf_render_char(
            gnf,
            vec2(p.x + i * s * FONT_CHAR_WIDTH, p.y),
            s,
            color,
            text[i]);
    }
}

void gnf_mouse_down(gnfContext *gnf)
{
    gnf->mouse_buttons = (Buttons) (gnf->mouse_buttons | BUTTON_LEFT);
}

void gnf_mouse_up(gnfContext *gnf)
{
    gnf->mouse_buttons = (Buttons) (gnf->mouse_buttons & (~BUTTON_LEFT));
}

void gnf_mouse_move(gnfContext *gnf, float x, float y)
{
    gnf->mouse_pos_delta = vec2(gnf->mouse_pos.x - x, gnf->mouse_pos.y - y);
    gnf->mouse_pos = vec2(x, y);
}

void gnf_begin(gnfContext *gnf)
{
    gnf->vertices_count = 0;
    gnf->triangles_count = 0;
    gnf->last_widget_position = vec2(0.0f, 0.0f);
}

void gnf_text(gnfContext *gnf, const char *text)
{
    (void) gnf;
    (void) text;
}

bool gnf_button(gnfContext *gnf, Vec2 p, Vec2 s, const char *text, gnf_ID id)
{
    bool clicked = false;
    RGBA color = GNF_BUTTON_COLOR;

    if (gnf->active != id) {
        if (gnf_rect_contains(gnf, p, s)) {
            if (gnf->mouse_buttons & BUTTON_LEFT) {
                if (gnf->active == 0) {
                    gnf->active = id;
                }
            }
            color = GNF_BUTTON_COLOR_HOT;
        }
    } else {
        color = GNF_BUTTON_COLOR_ACTIVE;
        if (!(gnf->mouse_buttons & BUTTON_LEFT)) {
            if (gnf_rect_contains(gnf, p, s)) {
                clicked = true;
            }
            gnf->active = 0;
        }
    }

    gnf_fill_rect(
        gnf,
        p,
        s,
        color);

    const float text_height = FONT_CHAR_HEIGHT * GNF_BUTTON_TEXT_SCALE;
    const float text_width = FONT_CHAR_WIDTH * GNF_BUTTON_TEXT_SCALE * strlen(text);

    // TODO: gnf_button does not handle the situation when the text is too big to fit into the boundaries of the button
    gnf_render_text(
        gnf,
        vec2(
            p.x + s.x * 0.5f - text_width * 0.5f,
            p.y + s.y * 0.5f - text_height * 0.5f),
        GNF_BUTTON_TEXT_SCALE,
        GNF_BUTTON_TEXT_COLOR,
        text);

    return clicked;
}

void gnf_fill_quad(gnfContext *gnf, Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3, RGBA c) {
    Vec2 uv_p, uv_s;
    gnf_char_uv(FONT_SOLID_CHAR, &uv_p, &uv_s);

    const unsigned int pv0 = gnf_append_vertex(gnf, vertex(p0, c, uv_p));
    const unsigned int pv1 = gnf_append_vertex(gnf, vertex(p1, c, vec2(uv_p.x + uv_s.x, uv_p.y)));
    const unsigned int pv2 = gnf_append_vertex(gnf, vertex(p2, c, vec2(uv_p.x, uv_p.y + uv_s.y)));
    const unsigned int pv3 = gnf_append_vertex(gnf, vertex(p3, c, vec2(uv_p.x + uv_s.x, uv_p.y + uv_s.y)));

    gnf_append_triangle(gnf, triangle(pv0, pv1, pv2));
    gnf_append_triangle(gnf, triangle(pv1, pv2, pv3));
}

void gnf_draw_line(gnfContext *gnf, Vec2 start, Vec2 end, float thickness, RGBA color) {
    thickness = thickness/2;
    Vec2 vector = vec2(end.x - start.x, end.y - start.y);
    Vec2 perp_vec1 = normalize(vec2(-1*vector.y, vector.x));
    Vec2 perp_vec2 = normalize(vec2(vector.y, -1*vector.x));

    //Parametric eq -> new_point = old_point + t * dir
    Vec2 start_p1 = vec2(start.x + thickness*perp_vec1.x, start.y + thickness*perp_vec1.y);
    Vec2 start_p2 = vec2(start.x + thickness*perp_vec2.x, start.y + thickness*perp_vec2.y);

    Vec2 end_p1 = vec2(end.x + thickness*perp_vec1.x, end.y + thickness*perp_vec1.y);
    Vec2 end_p2 = vec2(end.x + thickness*perp_vec2.x, end.y + thickness*perp_vec2.y);

    gnf_fill_quad(gnf, start_p1, start_p2, end_p1, end_p2, color);
}

void gnf_end(gnfContext *gnf)
{
    (void) gnf;
}

#endif // GNF_HEADER

#ifndef GNF_HEADER_PACKAGE_LAYOUT
#define GNF_HEADER_PACKAGE_LAYOUT

#include <libdnf/base/base.hpp>
#include <libdnf/rpm/repo.hpp>
#include <libdnf/rpm/solv_query.hpp>
#include "gnf.hpp"
#include "util.hpp"

#define GNF_PADDING 10.0f
#define GNF_BUTTON_SIZE vec2(100.0f, 50.0f)
#define GNF_PACKAGE_TEXT_SCALE 1.0f
#define GNF_PACKAGE_HEADER_SCALE 2.0f

typedef struct {
    libdnf::rpm::SolvQuery package;
    libdnf::rpm::SolvQuery my_dependencies;
    libdnf::rpm::SolvQuery dependent_on_me;
    libdnf::rpm::ReldepList reqs;
    libdnf::rpm::ReldepList provs;
    libdnf::rpm::ReldepList selectedActiveProvideReldeps;
    libdnf::rpm::ReldepList selectedActiveRequireReldeps;
    libdnf::rpm::SolvQuery selectedActivePackages;
    Vec2 selectedActivePoint;
} packageLayoutData;

Vec2 gnf_package_expanded(gnfContext *gnf, packageLayoutData *pkgLayout, Vec2 position, gnf_ID id)
{
    //TODO(amatej): this pkg should be one specific NEVRA (there should be tabs or some such for different available nevras)
    auto & solv_sack = gnf->base.get_rpm_solv_sack();
    auto pkg = pkgLayout->package.begin();

    const char * name = (*pkg).get_name().c_str();

    const float text_height = FONT_CHAR_HEIGHT * GNF_PACKAGE_TEXT_SCALE;
    const float header_height = FONT_CHAR_HEIGHT * GNF_PACKAGE_HEADER_SCALE;
    const float text_width = FONT_CHAR_WIDTH * GNF_PACKAGE_TEXT_SCALE * strlen(name);

    //This should happen just once...
    // Find longest require
    float longest_require = 0;
    for (auto req : pkgLayout->reqs) {
        const float req_width = FONT_CHAR_WIDTH * GNF_PACKAGE_TEXT_SCALE * strlen(req.get_name());
        if (req_width > longest_require) {
            longest_require = req_width;
        }
    }
    // Find longest provide
    float longest_provide = 0;
    for (auto prov : pkgLayout->provs) {
        const float prov_width = FONT_CHAR_WIDTH * GNF_PACKAGE_TEXT_SCALE * strlen(prov.get_name());
        if (prov_width > longest_provide) {
            longest_provide = prov_width;
        }
    }


    Vec2 p = position;
    float width = 4.0f*GNF_PADDING + longest_require + longest_provide;
    size_t max_chars_per_line = (width-2.0f*GNF_PADDING) / FONT_CHAR_WIDTH;
    std::string desc = (*pkg).get_description();
    int desc_lines = desc.size()/max_chars_per_line;

    float height = GNF_PADDING + header_height //headline
                   + (desc_lines+1)*text_height //description
                   + GNF_PADDING + text_height   // REQUIRES/PROVIDES title
                   +((pkgLayout->reqs.size() > pkgLayout->provs.size()) ? pkgLayout->reqs.size() : pkgLayout->provs.size()) * text_height*1.1f // height of reqs or provs
                   + GNF_PADDING; //space at the end

    Vec2 s = vec2(width, height);

    gnf_fill_rect(
        gnf,
        p,
        s,
        GNF_BUTTON_COLOR_HOT);

    // Offset to the top for header
    p = vec2(p.x + GNF_PADDING, p.y - text_height * 1.1f);
    gnf_render_text(
        gnf,
        p,
        GNF_PACKAGE_HEADER_SCALE,
        GNF_BUTTON_TEXT_COLOR,
        name);

    // Offset down from header and add padding
    p.y += GNF_PADDING + header_height;

    std::string part_of_description;
    for (int i=0; i<desc_lines+1; i++) {
        part_of_description.clear();
        if ((i+1)*max_chars_per_line > desc.size()) {
            part_of_description.append(desc, i*max_chars_per_line, desc.size() - i*max_chars_per_line);
        } else {
            part_of_description.append(desc, i*max_chars_per_line, max_chars_per_line);
        }
        gnf_render_text(
            gnf,
            p,
            GNF_PACKAGE_TEXT_SCALE,
            GNF_BUTTON_TEXT_COLOR,
            part_of_description.c_str());
        p.y += text_height;
    }

    // Offset down from description
    p.y += GNF_PADDING;

    gnf_render_text(gnf,
                    p,
                    GNF_PACKAGE_TEXT_SCALE,
                    GNF_BUTTON_TEXT_COLOR,
                    "REQUIRES:");
    const float prov_width = FONT_CHAR_WIDTH * GNF_PACKAGE_TEXT_SCALE * strlen("Provides:");
    gnf_render_text(gnf,
                    vec2(p.x + s.x - prov_width - GNF_PADDING, p.y),
                    GNF_PACKAGE_TEXT_SCALE,
                    GNF_BUTTON_TEXT_COLOR,
                    "PROVIDES:");

    // Offset down from REQUIRES and PROVIDES titles
    p.y += GNF_PADDING + text_height;
    int i = 0;
    for (auto req : pkgLayout->reqs) {
        libdnf::rpm::ReldepList rel_list(&solv_sack);
        rel_list.add(req);
        auto copy_query = libdnf::rpm::SolvQuery(pkgLayout->selectedActivePackages);
        bool active = false;
        if (copy_query.ifilter_provides(rel_list).size() > 0) {
            gnf_draw_line(gnf,
                          pkgLayout->selectedActivePoint,
                          vec2(p.x, p.y + GNF_PADDING + i*(text_height * 1.1f) - text_height/2),
                          2.0f,
                          GNF_PACKAGE_COLOR_TEXT_BG);
            active = true;
        }
        if (gnf_render_selectable_text(gnf,
                                       vec2(p.x, p.y + i*(text_height * 1.1f)),
                                       GNF_PACKAGE_TEXT_SCALE,
                                       GNF_BUTTON_TEXT_COLOR,
                                       req.get_name(),
                                       active)) {
            auto copy_provides_query = libdnf::rpm::SolvQuery(pkgLayout->my_dependencies);
            pkgLayout->selectedActiveRequireReldeps = rel_list;
            pkgLayout->selectedActivePoint = vec2(p.x, p.y + GNF_PADDING + i*(text_height * 1.1f) - text_height/2);
        }
        i++;
    }

    i = 0;
    for (auto prov : pkgLayout->provs) {
        const float prov_width = FONT_CHAR_WIDTH * GNF_PACKAGE_TEXT_SCALE * strlen(prov.get_name());
        libdnf::rpm::ReldepList rel_list(&solv_sack);
        rel_list.add(prov);
        auto copy_query = libdnf::rpm::SolvQuery(pkgLayout->selectedActivePackages);
        bool active = false;
        if (copy_query.ifilter_requires(rel_list).size() > 0) {
            gnf_draw_line(gnf,
                          pkgLayout->selectedActivePoint,
                          vec2(p.x+s.x-GNF_PADDING, p.y + GNF_PADDING + i*(text_height * 1.1f) - text_height/2),
                          2.0f,
                          GNF_PACKAGE_COLOR_TEXT_BG);
            active = true;
        }
        if (gnf_render_selectable_text(gnf,
                                       vec2(p.x + s.x - prov_width - GNF_PADDING, p.y + i*(text_height * 1.1f)),
                                       GNF_PACKAGE_TEXT_SCALE,
                                       GNF_BUTTON_TEXT_COLOR,
                                       prov.get_name(),
                                       active)) {
            pkgLayout->selectedActiveProvideReldeps = rel_list;
            pkgLayout->selectedActivePoint = vec2(p.x + s.x, p.y + GNF_PADDING + i*(text_height * 1.1f) - text_height/2);
        }
        i++;
    }

    return s;
}

bool gnf_package_collapsed(gnfContext *gnf, packageLayoutData *pkgLayout, Vec2 p, Vec2 s, libdnf::rpm::Package *pkg, gnf_ID id)
{
    auto & solv_sack = gnf->base.get_rpm_solv_sack();
    bool clicked = false;
    RGBA color = GNF_BUTTON_COLOR;

    if (gnf->active != id) {
        if (gnf_rect_contains(gnf, p, s)) {
            // mouse hovering the button
            if (gnf->mouse_buttons & BUTTON_LEFT) {
                if (gnf->active == 0) {
                    gnf->active = id;
                }
            }
            color = GNF_BUTTON_COLOR_HOT;
            //We know we are either working with provides or requires -> though this doesn't scale to recommends, supplementes..
            auto copy_query_provides = libdnf::rpm::SolvQuery(pkgLayout->dependent_on_me);
            auto copy_query_requires = libdnf::rpm::SolvQuery(pkgLayout->my_dependencies);
            copy_query_provides.ifilter_nevra({pkg->get_nevra()});
            copy_query_requires.ifilter_nevra({pkg->get_nevra()});
            if (copy_query_provides.size() > 0) {
                pkgLayout->selectedActivePoint = vec2(p.x, p.y + s.y/2);
            }
            if (copy_query_requires.size() > 0) {
                pkgLayout->selectedActivePoint = vec2(p.x+s.x, p.y + s.y/2);
            }
            copy_query_provides.update(copy_query_requires);
            pkgLayout->selectedActivePackages = copy_query_provides;
        } else {
            // mouse not hovering the button
            auto single_pkg_query1 = libdnf::rpm::SolvQuery(&solv_sack, libdnf::rpm::SolvQuery::InitFlags::EMPTY);
            auto single_pkg_query2 = libdnf::rpm::SolvQuery(&solv_sack, libdnf::rpm::SolvQuery::InitFlags::EMPTY);
            single_pkg_query1.add(*pkg);
            single_pkg_query2.add(*pkg);
            //// We need to connection point on a different side of the button depending on what we are doing
            //// TODO(amatej): this could maybe be fixed by picking the shortests path?
            //// The following query is also a huge performace blow (have a hashtable? (keys are reldeps, values are packages that provide/need them)
            if (pkgLayout->selectedActiveRequireReldeps.size() > 0) {
                //TODO(amatej): this ifilter_provides is just shomehow very broken -> since it takes forever for empty stuff
                //if (single_pkg_query1.ifilter_provides(pkgLayout->selectedActiveRequireReldeps).size()) {
                if (reldeplist_contains((*pkg).get_provides(), pkgLayout->selectedActiveRequireReldeps.get(0))) {
                    color = GNF_BUTTON_COLOR_HOT;
                    gnf_draw_line(gnf,
                            vec2(p.x + GNF_BUTTON_SIZE.x,
                                p.y + GNF_BUTTON_SIZE.y/2),
                            vec2(p.x + pkgLayout->selectedActivePoint.x - p.x,
                                p.y + pkgLayout->selectedActivePoint.y - p.y),
                            2.0f, GNF_PACKAGE_COLOR_TEXT_BG);
                }
            }
            if (pkgLayout->selectedActiveProvideReldeps.size() > 0) {
                //if (single_pkg_query2.ifilter_requires(pkgLayout->selectedActiveProvideReldeps).size()) {
                if (reldeplist_contains((*pkg).get_requires(), pkgLayout->selectedActiveProvideReldeps.get(0))) {
                    color = GNF_BUTTON_COLOR_HOT;
                    gnf_draw_line(gnf,
                                  vec2(p.x,
                                       p.y + GNF_BUTTON_SIZE.y/2),
                                  vec2(p.x + pkgLayout->selectedActivePoint.x - p.x,
                                       p.y + pkgLayout->selectedActivePoint.y - p.y),
                                  2.0f, GNF_PACKAGE_COLOR_TEXT_BG);
                }
            }
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
    const float text_width = FONT_CHAR_WIDTH * GNF_BUTTON_TEXT_SCALE * strlen(pkg->get_name().c_str());

    // TODO: gnf_button does not handle the situation when the text is too big to fit into the boundaries of the button
    gnf_render_text(
        gnf,
        vec2(
            p.x + s.x * 0.5f - text_width * 0.5f,
            p.y + s.y * 0.5f - text_height * 0.5f),
        GNF_BUTTON_TEXT_SCALE,
        GNF_BUTTON_TEXT_COLOR,
        pkg->get_name().c_str());

    return clicked;
}


void load_package_data(gnfContext *gnf, packageLayoutData *pkgLayout, std::string pkg_name) {
    auto & solv_sack = gnf->base.get_rpm_solv_sack();
    libdnf::rpm::SolvQuery main_pkg(&solv_sack);
    libdnf::rpm::SolvQuery requires(&solv_sack);
    libdnf::rpm::SolvQuery provides(&solv_sack);

    main_pkg.ifilter_arch({"x86_64", "noarch"}).ifilter_latest(1).ifilter_name({pkg_name});
    auto pkg = *main_pkg.begin();

    pkgLayout->reqs = pkg.get_requires();
    pkgLayout->provs = pkg.get_provides();

    pkgLayout->package = main_pkg;
    pkgLayout->my_dependencies = requires.ifilter_arch({"x86_64", "noarch"}).ifilter_provides(pkgLayout->reqs);
    pkgLayout->dependent_on_me = provides.ifilter_arch({"x86_64", "noarch"}).ifilter_requires(pkgLayout->provs);

}


void layout_package(gnfContext *gnf, packageLayoutData *pkgLayout) {
    float mid_width = gnf->width/2;
    float mid_height = gnf->height/2;
    int global_id = 1;

    Vec2 size_of_main_pkg = gnf_package_expanded(gnf, pkgLayout, vec2(mid_width, mid_height), global_id);
    global_id++;

    pkgLayout->selectedActivePackages = libdnf::rpm::SolvQuery(&(gnf->base.get_rpm_solv_sack()), libdnf::rpm::SolvQuery::InitFlags::EMPTY);
    int i = 0;
    //TODO(amatej): fix the naming in pkgLayout (provides and requires are confusing)
    for(auto pkg: pkgLayout->dependent_on_me) {
        Vec2 button_pos = vec2(mid_width + size_of_main_pkg.x + 50.0f + (150 * ((i/16))), mid_height-30*16 + 55*(i%16));
        if (gnf_package_collapsed(gnf,
                                  pkgLayout,
                                  button_pos,
                                  GNF_BUTTON_SIZE,
                                  &pkg,
                                  global_id)) {
            printf("clicked: %s\n", pkg.get_nevra().c_str());
            load_package_data(gnf, pkgLayout, pkg.get_name());
        }
        global_id++;
        i++;
    }

    i = 0;
    for(auto pkg: pkgLayout->my_dependencies) {
        Vec2 button_pos = vec2(mid_width-150, mid_height-30*pkgLayout->my_dependencies.size() + 55*i);
        if (gnf_package_collapsed(gnf,
                                  pkgLayout,
                                  button_pos,
                                  GNF_BUTTON_SIZE,
                                  &pkg,
                                  global_id)) {
            printf("clicked: %s\n", pkg.get_nevra().c_str());
            load_package_data(gnf, pkgLayout, pkg.get_name());
        }
        global_id++;
        i++;
    }

    //printf("pkgLayout->dependent_on_me size: %lu\n", pkgLayout->dependent_on_me.size());
    //printf("pkgLayout->my_dependencies size: %lu\n", pkgLayout->my_dependencies.size());

    pkgLayout->selectedActiveRequireReldeps = libdnf::rpm::ReldepList(&(gnf->base.get_rpm_solv_sack()));
    pkgLayout->selectedActiveProvideReldeps = libdnf::rpm::ReldepList(&(gnf->base.get_rpm_solv_sack()));

}


#endif // GNF_HEADER_PACKAGE_LAYOUT

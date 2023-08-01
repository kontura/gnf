#include "package_layout.hpp"
#include "util.hpp"

void load_package_layout_data(gnfContext *gnf, packageLayoutData *pkgLayout, std::string pkg_name, size_t index) {
    libdnf5::rpm::PackageQuery main_pkg(gnf->base);
    pkgLayout->package_index = index;

    const libdnf5::ResolveSpecSettings settings{.ignore_case = true, .with_provides = false};
    main_pkg.resolve_pkg_spec(pkg_name, settings, true);
    if (main_pkg.size() > 0) {
        auto pkg = main_pkg.begin();
        for (int i=0; i<pkgLayout->package_index; i++) {
            pkg++;
        }

        pkgLayout->reqs = (*pkg).get_requires();
        pkgLayout->conflicts = (*pkg).get_conflicts();
        pkgLayout->obsoletes = (*pkg).get_obsoletes();
        pkgLayout->provs = (*pkg).get_provides();

        printf("pkgLayout->obsoletes size: %lu\n", pkgLayout->obsoletes.size());
        printf("pkgLayout->conflicts size: %lu\n", pkgLayout->conflicts.size());

        pkgLayout->active_name_packages = main_pkg;

        pkgLayout->my_dependencies = libdnf5::rpm::PackageQuery(gnf->base);
        pkgLayout->my_dependencies.filter_provides(pkgLayout->reqs);
        pkgLayout->my_conflicts = libdnf5::rpm::PackageQuery(gnf->base);
        pkgLayout->my_conflicts.filter_provides(pkgLayout->conflicts);
        pkgLayout->obsoleted_by_me = libdnf5::rpm::PackageQuery(gnf->base);
        pkgLayout->obsoleted_by_me.filter_provides(pkgLayout->obsoletes);
        pkgLayout->dependent_on_me = libdnf5::rpm::PackageQuery(gnf->base);
        pkgLayout->dependent_on_me.filter_requires(pkgLayout->provs);

        //printf("pkgLayout->dependent_on_me size: %lu\n", pkgLayout->dependent_on_me.size());
        //printf("pkgLayout->my_dependencies size: %lu\n", pkgLayout->my_dependencies.size());
        printf("pkgLayout->obsoleted_by_me size: %lu\n", pkgLayout->obsoleted_by_me.size());
        printf("pkgLayout->my_conflicts size: %lu\n", pkgLayout->my_conflicts.size());
    }

    gnf->state = GNF_GUI_STATE_PACKAGE_LAYOUT;

}

Vec2 gnf_package_expanded(gnfContext *gnf, packageLayoutData *pkgLayout, Vec2 position, gnf_ID id)
{
    //TODO(amatej): this pkg should be one specific NEVRA (there should be tabs or some such for different available nevras)
    auto pkg = pkgLayout->active_name_packages.begin();
    for (int i=0; i<pkgLayout->package_index; i++) {
        pkg++;
    }

    std::string name_str = (*pkg).get_nevra();
    const char * name = name_str.c_str();
    const size_t name_len = name_str.size();

    const float text_height = FONT_CHAR_HEIGHT * GNF_PACKAGE_TEXT_SCALE;
    const float header_height = FONT_CHAR_HEIGHT * GNF_PACKAGE_HEADER_SCALE;

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

    float height = 3*GNF_PADDING + header_height //headline
                   + (desc_lines+1)*text_height //description
                   + GNF_PADDING + text_height   // REQUIRES/PROVIDES title
                   +((pkgLayout->reqs.size() > pkgLayout->provs.size()) ? pkgLayout->reqs.size() : pkgLayout->provs.size()) * text_height*1.1f // height of reqs or provs
                   + GNF_PADDING; //space at the end

    Vec2 s = vec2(width, height);
    p = p - vec2(-2*GNF_PADDING*pkgLayout->active_name_packages.size(), 2*GNF_PADDING*pkgLayout->active_name_packages.size());
    RGBA other_focused_color = GNF_BUTTON_COLOR_HOT;
    other_focused_color.g += 0.16;

    int package_index = 0;
    for (const auto pkg_tmp : pkgLayout->active_name_packages) {
        if (package_index == pkgLayout->package_index) {
            package_index++;
            continue;
        }
        Vec2 p_next = p + vec2(-2*GNF_PADDING, 2*GNF_PADDING);
        other_focused_color.g -= 0.04f;
        RGBA draw_color;
        if (gnf_rect_contains(gnf, p, s) && !gnf_rect_contains(gnf, p_next, s)) {
            if (gnf->mouse_buttons & BUTTON_LEFT) {
                load_package_layout_data(gnf, pkgLayout, pkg_tmp.get_name(), package_index);
            }
            draw_color = other_focused_color + rgba(.2, .2, .2, 0);
        } else {
            draw_color = other_focused_color;

        }
        gnf_fill_rect(
            gnf,
            p,
            s,
            draw_color);

        gnf_render_text(
            gnf,
            p + vec2(s.x, 0) - vec2(FONT_CHAR_WIDTH * GNF_PACKAGE_TEXT_SCALE * pkg_tmp.get_nevra().size(), 0),
            GNF_PACKAGE_TEXT_SCALE,
            GNF_BUTTON_TEXT_COLOR,
            pkg_tmp.get_nevra().c_str());

        p = p_next;
        package_index++;
    }

    other_focused_color.g -= 0.04f;

    gnf_fill_rect(
        gnf,
        p,
        s,
        other_focused_color);

    const float header_width = FONT_CHAR_WIDTH * GNF_PACKAGE_HEADER_SCALE * name_len;
    Vec2 p_header = vec2(p.x + s.x - header_width - GNF_PADDING, p.y + GNF_PADDING);
    gnf_render_text(
        gnf,
        p_header,
        GNF_PACKAGE_HEADER_SCALE,
        GNF_BUTTON_TEXT_COLOR,
        name);

    // Offset to the top for header
    p = vec2(p.x + GNF_PADDING, p.y + GNF_PADDING);
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
                    vec2(p.x + s.x - prov_width - GNF_PADDING - GNF_PADDING, p.y),
                    GNF_PACKAGE_TEXT_SCALE,
                    GNF_BUTTON_TEXT_COLOR,
                    "PROVIDES:");

    // Offset down from REQUIRES and PROVIDES titles
    p.y += GNF_PADDING + text_height;
    int i = 0;
    for (auto req : pkgLayout->reqs) {
        libdnf5::rpm::ReldepList rel_list(gnf->base);
        rel_list.add(req);
        auto copy_query = libdnf5::rpm::PackageQuery(pkgLayout->selectedActivePackages);
        copy_query.filter_provides(rel_list);
        bool active = false;
        if (copy_query.size() > 0) {
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
            auto copy_provides_query = libdnf5::rpm::PackageQuery(pkgLayout->my_dependencies);
            pkgLayout->selectedActiveRequireReldeps = rel_list;
            pkgLayout->selectedActivePoint = vec2(p.x, p.y + GNF_PADDING + i*(text_height * 1.1f) - text_height/2);
        }
        i++;
    }

    i = 0;
    for (auto prov : pkgLayout->provs) {
        const float prov_width = FONT_CHAR_WIDTH * GNF_PACKAGE_TEXT_SCALE * strlen(prov.get_name());
        libdnf5::rpm::ReldepList rel_list(gnf->base);
        rel_list.add(prov);
        auto copy_query = libdnf5::rpm::PackageQuery(pkgLayout->selectedActivePackages);
        copy_query.filter_requires(rel_list);
        bool active = false;
        if (copy_query.size() > 0) {
            gnf_draw_line(gnf,
                          pkgLayout->selectedActivePoint,
                          vec2(p.x+s.x-GNF_PADDING, p.y + GNF_PADDING + i*(text_height * 1.1f) - text_height/2),
                          2.0f,
                          GNF_PACKAGE_COLOR_TEXT_BG);
            active = true;
        }
        if (gnf_render_selectable_text(gnf,
                                       vec2(p.x + s.x - prov_width - GNF_PADDING - GNF_PADDING, p.y + i*(text_height * 1.1f)),
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

bool gnf_package_collapsed(gnfContext *gnf, packageLayoutData *pkgLayout, Vec2 p, libdnf5::rpm::Package *pkg, RGBA color, gnf_ID id)
{
    bool clicked = false;
    const size_t name_len = strlen(pkg->get_nevra().c_str());
    const float text_height = FONT_CHAR_HEIGHT * GNF_BUTTON_TEXT_SCALE;
    const float text_width = FONT_CHAR_WIDTH * GNF_BUTTON_TEXT_SCALE * name_len;
    Vec2 s = vec2(text_width, text_height);

    if (gnf->active != id) {
        if (gnf_rect_contains(gnf, p, s)) {
            // mouse hovering the button
            if (gnf->mouse_buttons & BUTTON_LEFT) {
                if (gnf->active == 0) {
                    gnf->active = id;
                }
            }
            color = GNF_BUTTON_COLOR_HOT;
            //TODO(amatej): we need to fix this: when on createrepo_c and I hover rpm nothing happens..
            //We know we are either working with provides or requires -> though this doesn't scale to recommends, supplementes..
            auto copy_query_provides = libdnf5::rpm::PackageQuery(pkgLayout->dependent_on_me);
            auto copy_query_requires = libdnf5::rpm::PackageQuery(pkgLayout->my_dependencies);
            copy_query_provides.filter_nevra({pkg->get_nevra()});
            copy_query_requires.filter_nevra({pkg->get_nevra()});
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
            auto single_pkg_query1 = libdnf5::rpm::PackageQuery(gnf->base, libdnf5::sack::ExcludeFlags::APPLY_EXCLUDES, true);
            auto single_pkg_query2 = libdnf5::rpm::PackageQuery(gnf->base, libdnf5::sack::ExcludeFlags::APPLY_EXCLUDES, true);
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

    //p = vec2(p.x + s.x * 0.5f - text_width * 0.5f, p.y + s.y * 0.5f - text_height * 0.5f);

    //TODO(amatej): fix this logic (we want to draw bg only when hot/active)
    if (color.r == GNF_BUTTON_COLOR_HOT.r)
    gnf_fill_rect(
        gnf,
        p + vec2(-1.0f, -1.0f),
        vec2(name_len*FONT_CHAR_WIDTH*GNF_BUTTON_TEXT_SCALE+2, FONT_CHAR_HEIGHT*GNF_BUTTON_TEXT_SCALE+2),
        color);
    //gnf_fill_rect(
    //    gnf,
    //    p,
    //    s,
    //    color);



    gnf_render_text(
        gnf,
        p,
        GNF_BUTTON_TEXT_SCALE,
        GNF_BUTTON_TEXT_COLOR,
        pkg->get_nevra().c_str());

    gnf_draw_cube(gnf, p + vec2(-13,0), 0.2f);

    return clicked;
}


void layout_package(gnfContext *gnf, packageLayoutData *pkgLayout) {
    float mid_width = gnf->width/2;
    float mid_height = gnf->height/2;
    int global_id = 1;

    Vec2 size_of_main_pkg = gnf_package_expanded(gnf, pkgLayout, vec2(mid_width, mid_height), global_id);
    global_id++;

    pkgLayout->selectedActivePackages = libdnf5::rpm::PackageQuery(gnf->base, libdnf5::sack::ExcludeFlags::APPLY_EXCLUDES, true);
    int i = 0;
    //TODO(amatej): fix the naming in pkgLayout (provides and requires are confusing)
    for(auto pkg: pkgLayout->dependent_on_me) {
        Vec2 button_pos = vec2(mid_width + size_of_main_pkg.x + 50.0f + 50, mid_height-30*16 + 25*i);
        if (gnf_package_collapsed(gnf,
                                  pkgLayout,
                                  button_pos,
                                  &pkg,
                                  GNF_BLACK,
                                  global_id)) {
            printf("clicked: %s\n", pkg.get_nevra().c_str());
            load_package_layout_data(gnf, pkgLayout, pkg.get_name(), 0);
        }
        global_id++;
        i++;
    }

    i = 0;
    for(auto pkg: pkgLayout->my_dependencies) {
        Vec2 button_pos = vec2(mid_width-150, mid_height-30*pkgLayout->my_dependencies.size() + 25*i);
        if (gnf_package_collapsed(gnf,
                                  pkgLayout,
                                  button_pos,
                                  &pkg,
                                  GNF_BLACK,
                                  global_id)) {
            printf("clicked: %s\n", pkg.get_nevra().c_str());
            load_package_layout_data(gnf, pkgLayout, pkg.get_name(), 0);
        }
        global_id++;
        i++;
    }

    i = 0;
    for(auto pkg: pkgLayout->obsoleted_by_me) {
        Vec2 button_pos = vec2(mid_width-150 + 55*i, mid_height+130*pkgLayout->my_dependencies.size());
        if (gnf_package_collapsed(gnf,
                                  pkgLayout,
                                  button_pos,
                                  &pkg,
                                  GNF_PKG_OBSOLETE_COLOR,
                                  global_id)) {
            printf("clicked: %s\n", pkg.get_nevra().c_str());
            load_package_layout_data(gnf, pkgLayout, pkg.get_name(), 0);
        }
        global_id++;
        i++;
    }

    i = 0;
    for(auto pkg: pkgLayout->my_conflicts) {
        Vec2 button_pos = vec2(mid_width-150 + 55*i, mid_height+130*pkgLayout->my_dependencies.size());
        if (gnf_package_collapsed(gnf,
                                  pkgLayout,
                                  button_pos,
                                  &pkg,
                                  GNF_PKG_CONFLICT_COLOR,
                                  global_id)) {
            printf("clicked: %s\n", pkg.get_nevra().c_str());
            load_package_layout_data(gnf, pkgLayout, pkg.get_name(), 0);
        }
        global_id++;
        i++;
    }

    //TODO(amatej): need to add weak deps: recommends, suppl...

    pkgLayout->selectedActiveRequireReldeps = libdnf5::rpm::ReldepList(gnf->base);
    pkgLayout->selectedActiveProvideReldeps = libdnf5::rpm::ReldepList(gnf->base);

}

#ifndef GNF_HEADER_PACKAGE_LAYOUT
#define GNF_HEADER_PACKAGE_LAYOUT

#include <libdnf/base/base.hpp>
#include <libdnf/rpm/repo.hpp>
#include <libdnf/rpm/solv_query.hpp>
#include "gnf.hpp"

#define GNF_PADDING 10.0f
#define GNF_BUTTON_SIZE vec2(100.0f, 50.0f)

typedef struct {
    libdnf::rpm::SolvQuery package;
    libdnf::rpm::SolvQuery requires;
    libdnf::rpm::SolvQuery provides;
} packageLayoutData;

void load_package_data(gnfContext *gnf, packageLayoutData *pkgLayout, std::string pkg_name) {
    auto & solv_sack = gnf->base.get_rpm_solv_sack();
    libdnf::rpm::SolvQuery main_pkg(&solv_sack);
    libdnf::rpm::SolvQuery requires(&solv_sack);
    libdnf::rpm::SolvQuery provides(&solv_sack);

    main_pkg.ifilter_arch({"x86_64", "noarch"}).ifilter_latest(1).ifilter_name({pkg_name});
    auto pkg = *main_pkg.begin();

    libdnf::rpm::ReldepList reqs = pkg.get_requires();
    libdnf::rpm::ReldepList provs = pkg.get_provides();

    pkgLayout->package = main_pkg;
    pkgLayout->requires = requires.ifilter_arch({"x86_64", "noarch"}).ifilter_provides(reqs);
    pkgLayout->provides = provides.ifilter_arch({"x86_64", "noarch"}).ifilter_requires(provs);

}


void layout_package(gnfContext *gnf, packageLayoutData *pkgLayout) {
    float mid_width = gnf->width/2;
    float mid_height = gnf->height/2;
    int global_id = 1;

    if (gnf_package(gnf, vec2(mid_width-100, mid_height-100), vec2(200, 200), (*(pkgLayout->package.begin())).get_name().c_str(), global_id)) {
        printf("clicked: %s\n", (*(pkgLayout->package.begin())).get_nevra().c_str());
    }
    global_id++;

    int i = 0;
    for(auto pkg: pkgLayout->provides) {
        if (gnf_button(gnf,
                       vec2(mid_width+(150 * ((i/16)+1)), mid_height-30*16 + 55*(i%16)),
                       GNF_BUTTON_SIZE,
                       pkg.get_name().c_str(),
                       global_id)) {
            printf("clicked: %s\n", pkg.get_nevra().c_str());
            load_package_data(gnf, pkgLayout, pkg.get_name());
        }
        global_id++;
        i++;
    }

    i = 0;
    for(auto pkg: pkgLayout->requires) {
        if (gnf_button(gnf,
                       vec2(mid_width-250, mid_height-30*pkgLayout->requires.size() + 55*i),
                       GNF_BUTTON_SIZE,
                       pkg.get_name().c_str(),
                       global_id)) {
            printf("clicked: %s\n", pkg.get_nevra().c_str());
            load_package_data(gnf, pkgLayout, pkg.get_name());
        }
        global_id++;
        i++;
    }

}


#endif // GNF_HEADER_PACKAGE_LAYOUT

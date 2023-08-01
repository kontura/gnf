#ifndef GNF_HEADER_PACKAGE_LAYOUT
#define GNF_HEADER_PACKAGE_LAYOUT

#include <libdnf5/base/base.hpp>
#include <libdnf5/rpm/package_query.hpp>
#include "gui.hpp"

#define GNF_PADDING 10.0f
#define GNF_BUTTON_SIZE vec2(100.0f, 50.0f)
#define GNF_PACKAGE_TEXT_SCALE 1.0f
#define GNF_PACKAGE_HEADER_SCALE 2.0f

#define GNF_PKG_CONFLICT_COLOR rgba(1.0f, 0.0f, 0.0f, 1.0f)
#define GNF_PKG_OBSOLETE_COLOR rgba(0.0f, 0.0f, 1.0f, 1.0f)

typedef struct {
    libdnf5::rpm::PackageQuery active_name_packages;
    int package_index;
    //TODO(amatej): might want to divide on prereq and regulat reqs
    libdnf5::rpm::PackageQuery my_dependencies;
    libdnf5::rpm::PackageQuery dependent_on_me;
    libdnf5::rpm::PackageQuery my_conflicts;
    libdnf5::rpm::PackageQuery obsoleted_by_me;
    libdnf5::rpm::ReldepList reqs;
    //TODO(amatej): It seems the provs are somehow broken or more coplicated? bash should be needed by systemd-rpm-macros (which requires it) but it doesn't show up
    libdnf5::rpm::ReldepList provs;
    libdnf5::rpm::ReldepList conflicts;
    libdnf5::rpm::ReldepList obsoletes;
    libdnf5::rpm::ReldepList selectedActiveProvideReldeps;
    libdnf5::rpm::ReldepList selectedActiveRequireReldeps;
    libdnf5::rpm::PackageQuery selectedActivePackages;
    Vec2 selectedActivePoint;
} packageLayoutData;

void load_package_layout_data(gnfContext *gnf, packageLayoutData *pkgLayout, std::string pkg_name, size_t index);
Vec2 gnf_package_expanded(gnfContext *gnf, packageLayoutData *pkgLayout, Vec2 position, gnf_ID id);
bool gnf_package_collapsed(gnfContext *gnf, packageLayoutData *pkgLayout, Vec2 p, libdnf5::rpm::Package *pkg, RGBA color, gnf_ID id);
void layout_package(gnfContext *gnf, packageLayoutData *pkgLayout);

#endif // GNF_HEADER_PACKAGE_LAYOUT

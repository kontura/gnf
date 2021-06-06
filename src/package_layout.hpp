#ifndef GNF_HEADER_PACKAGE_LAYOUT
#define GNF_HEADER_PACKAGE_LAYOUT

#include <libdnf/base/base.hpp>
#include <libdnf/rpm/repo.hpp>
#include <libdnf/rpm/solv_query.hpp>
#include "gui.hpp"

#define GNF_PADDING 10.0f
#define GNF_BUTTON_SIZE vec2(100.0f, 50.0f)
#define GNF_PACKAGE_TEXT_SCALE 1.0f
#define GNF_PACKAGE_HEADER_SCALE 2.0f

#define GNF_PKG_CONFLICT_COLOR rgba(1.0f, 0.0f, 0.0f, 1.0f)
#define GNF_PKG_OBSOLETE_COLOR rgba(0.0f, 0.0f, 1.0f, 1.0f)

typedef struct {
    libdnf::rpm::SolvQuery active_name_packages;
    int package_index;
    //TODO(amatej): might want to divide on prereq and regulat reqs
    libdnf::rpm::SolvQuery my_dependencies;
    libdnf::rpm::SolvQuery dependent_on_me;
    libdnf::rpm::SolvQuery my_conflicts;
    libdnf::rpm::SolvQuery obsoleted_by_me;
    libdnf::rpm::ReldepList reqs;
    libdnf::rpm::ReldepList provs;
    libdnf::rpm::ReldepList conflicts;
    libdnf::rpm::ReldepList obsoletes;
    libdnf::rpm::ReldepList selectedActiveProvideReldeps;
    libdnf::rpm::ReldepList selectedActiveRequireReldeps;
    libdnf::rpm::SolvQuery selectedActivePackages;
    Vec2 selectedActivePoint;
} packageLayoutData;

void load_package_data(gnfContext *gnf, packageLayoutData *pkgLayout, std::string pkg_name, size_t index);
Vec2 gnf_package_expanded(gnfContext *gnf, packageLayoutData *pkgLayout, Vec2 position, gnf_ID id);
bool gnf_package_collapsed(gnfContext *gnf, packageLayoutData *pkgLayout, Vec2 p, libdnf::rpm::Package *pkg, RGBA color, gnf_ID id);
void layout_package(gnfContext *gnf, packageLayoutData *pkgLayout);

#endif // GNF_HEADER_PACKAGE_LAYOUT

#ifndef GNF_HEADER_PACKAGE_GRAPH
#define GNF_HEADER_PACKAGE_GRAPH

#include <vector>
#include <unordered_map>

#include <libdnf5/base/base.hpp>
#include <libdnf5/rpm/package_query.hpp>

#include "gui.hpp"

struct Node;

struct Node {
    Vec2 position;
    Vec2 size;
    int layer;
    //TODO(amatej): maybe we should keep just pointer to pkg
    //              but where would the packages live?
    const libdnf5::rpm::Package *pkg;
};

typedef std::unordered_map<int, std::vector<int>> PackageDependencyAdjacencyList;
typedef std::unordered_map<int, Node> PackageMapById;
typedef std::unordered_map<int, float> LayerCrossingNumbers;

typedef struct {
    libdnf5::rpm::PackageQuery active_name_packages;
    int package_index;
    //libdnf::rpm::SolvQuery my_dependencies;
    std::vector<libdnf5::rpm::Package> deps;
    PackageMapById nodes;
    PackageDependencyAdjacencyList adjList;
} packageGraphData;

void load_package_graph_data(gnfContext *gnf, packageGraphData *pkgGraph, std::string pkg_name, size_t index);
void layout_graph(gnfContext *gnf, packageGraphData *pkgGraph);




#endif // GNF_HEADER_PACKAGE_GRAPH

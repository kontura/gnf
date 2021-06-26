#ifndef GNF_HEADER_PACKAGE_GRAPH
#define GNF_HEADER_PACKAGE_GRAPH

#include <vector>
#include <unordered_map>

#include <libdnf/base/base.hpp>
#include <libdnf/rpm/repo.hpp>
#include <libdnf/rpm/solv_query.hpp>

#include "gui.hpp"

struct Node;

struct Node {
    Vec2 position;
    Vec2 size;
    int layer;
    //TODO(amatej): maybe we should keep just pointer to pkg
    //              but where would the packages live?
    const libdnf::rpm::Package pkg;
};

typedef std::unordered_map<int, std::vector<int>> PackageDependencyAdjacencyList;
typedef std::unordered_map<int, Node> PackageMapById;

typedef struct {
    libdnf::rpm::SolvQuery active_name_packages;
    int package_index;
    //libdnf::rpm::SolvQuery my_dependencies;
    std::vector<libdnf::rpm::Package> deps;
    PackageMapById nodes;
    PackageDependencyAdjacencyList adjList;
} packageGraphData;

void load_package_graph_data(gnfContext *gnf, packageGraphData *pkgGraph, std::string pkg_name, size_t index);
void layout_graph(gnfContext *gnf, packageGraphData *pkgGraph);




#endif // GNF_HEADER_PACKAGE_GRAPH

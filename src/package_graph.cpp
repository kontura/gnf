#include "package_graph.hpp"

#include <libdnf/base/goal.hpp>
#include <iostream>

void dump_graph(PackageDependencyAdjacencyList *g, PackageMapById *m) {
    for (auto i = g->begin(); i != g->end(); i++)
    {
        std::cout << m->at(i->first).pkg.get_nevra().c_str() << " : " << std::endl;
        for (auto const j : i->second) {
            std::cout << "  " << m->at(j).pkg.get_nevra() << std::endl;
        }
        std::cout << std::endl;
    }
}

static void breakCyclesRecursive(PackageDependencyAdjacencyList *g, int pkg_id, std::unordered_map<int, bool> *visited, std::unordered_map<int, bool> *active) {
    (*visited)[pkg_id] = true;
    (*active)[pkg_id] = true;

    // This is code for when the edges point to packages that depend on me
    //std::vector<libdnf::rpm::Package>::iterator it = (*g)[n].begin();
    //while (it != (*g)[n].end()) {
    //    if ((*active)[(*it).get_id().id]) {
    //        // reverse the edge
    //        libdnf::rpm::Package target = *it;
    //        it = (*g)[n].erase(it);
    //        (*g)[target].push_back(n);
    //    } else if (!((*visited)[(*it).get_id().id])) {
    //        breakCyclesRecursive(g, (*it), visited, active);
    //        it++;
    //    } else {
    //        it++;
    //    }
    //}

    // This is code for when the edges point to packages that are my dependencies
    for (auto adjListLine = g->begin(); adjListLine != g->end(); adjListLine++) {
        auto idIter = adjListLine->second.begin();
        while (idIter != adjListLine->second.end()) {
            if ((*idIter) == pkg_id) {
                if ((*active)[adjListLine->first]) {
                    //printf("reversing: %s\n", adjListLine->first.get_nevra().c_str());
                    (*g).at(pkg_id).push_back(adjListLine->first);
                    idIter = adjListLine->second.erase(idIter);
                } else if (!((*visited)[adjListLine->first])) {
                    breakCyclesRecursive(g, adjListLine->first, visited, active);
                }
                break; //we know there n cannot be twice in adjListLine->second
            } else {
                idIter++;
            }
        }
    }

    (*active)[pkg_id] = false;
}

static std::vector<int> successors (PackageDependencyAdjacencyList *g, int pkg_id) {
    std::vector<int> result;
    for (auto m = g->begin(); m != g->end(); m++) {
        for (auto dependency = m->second.begin(); dependency != m->second.end(); dependency++) {
            if (*dependency == pkg_id) {
                result.emplace_back(m->first);
                break;
            }
        }
    }
    return result;
}

//https://ssw.jku.at/Research/Papers/Wuerthinger07Master/Wuerthinger07Master.pdf
static void hierarchical_graph_layout(PackageDependencyAdjacencyList *g, PackageMapById *m) {
    //check for self loops
    for (auto n = g->begin(); n != g->end(); n++) {
        for (const auto id : n->second) {
            if (n->first == id) {
                fprintf(stderr, "Self loop detected, we can't handle that yet\n");
                abort();
            }
        }
    }

    // BREAK CYCLES
    std::unordered_map<int, bool> visited; //indexed by pkg id
    std::unordered_map<int, bool> active; //indexed by pkg id
    for (auto n = g->begin(); n != g->end(); n++) {
        visited[n->first] = false;
        active[n->first] = false;
    }
    for (auto n = g->begin(); n != g->end(); n++) {
        if (n->second.size() == 0) {
            printf("has no deps: %s\n", m->at(n->first).pkg.get_nevra().c_str());
            breakCyclesRecursive(g, n->first, &visited, &active);
        }
    }

    // ASSIGN LAYERS
    std::vector<int> to_process; //set of ids
    for (auto n = g->begin(); n != g->end(); n++) {
        if (n->second.size() == 0) {
            m->at(n->first).layer = 1;
            to_process.emplace_back(n->first);
            printf("asssing layer 1 to: %s\n", m->at(n->first).pkg.get_nevra().c_str());
        }
    }
    int i = 2;
    while (!to_process.empty()) {
        std::vector<int> to_process_next;

        for (auto pkg_id = to_process.begin(); pkg_id != to_process.end(); pkg_id++) {
            std::vector<int> succs = successors(g, *pkg_id);
            for (const auto n : succs) {
                bool all_predecessors_of_n_are_already_assigned_to_a_layer = true;
                auto predecessors = g->at(n);
                for (const auto predecessor : predecessors) {
                    if (m->at(predecessor).layer == 0) { // 0 mean no layer, layers are numbered from 1
                        all_predecessors_of_n_are_already_assigned_to_a_layer = false;
                        break;
                    }
                }

                if (all_predecessors_of_n_are_already_assigned_to_a_layer) {
                    to_process_next.emplace_back(n);
                }
            }
        }

        for (auto pkg_id = to_process_next.begin(); pkg_id != to_process_next.end(); pkg_id++) {
            m->at(*pkg_id).layer = i;
        }
        to_process = to_process_next;
        i++;
    }

    //for (auto adjListLine = g->begin(); adjListLine != g->end(); adjListLine++) {
    //    printf("pkg: %s has layer: %i\n", m->at(adjListLine->first).pkg.get_nevra().c_str(), m->at(adjListLine->first).layer);
    //}

    // INSERT DUMMY NODES
    //for (auto n = g->begin(); n != g->end(); n++) {
    //    int source_node = n->first;
    //    int last_node = source_node;
    //    int i = m->at(source_node).layer + 1;
    //    for (const auto target_node : n->second) {
    //        while (i != m->at(target_node).layer) {


    //            i++;
    //        }
    //        // iterating over all edges
    //    }

    //}

}

void layout_graph(gnfContext *gnf, packageGraphData *pkgGraph) {
    for (auto n = pkgGraph->nodes.begin(); n != pkgGraph->nodes.end(); n++) {
        Vec2 p = vec2((*n).second.layer * 290, (*n).second.position.y);
        gnf_render_text(
            gnf,
            p,
            GNF_BUTTON_TEXT_SCALE,
            GNF_BUTTON_TEXT_COLOR,
            (*n).second.pkg.get_nevra().c_str());

        std::vector<int> dep_ids = pkgGraph->adjList.at((*n).first);

        for (const auto dep_id : dep_ids) {
            Node dep = pkgGraph->nodes.at(dep_id);
            Vec2 dep_position = vec2(dep.layer * 290, dep.position.y);
            gnf_draw_line(gnf, p, dep_position+vec2(dep.size.x,0),
                          2.0f,
                          GNF_PACKAGE_COLOR_TEXT_BG);
        }

    }

}

void load_package_graph_data(gnfContext *gnf, packageGraphData *pkgGraph, std::string pkg_name, size_t index) {
    auto & solv_sack = gnf->base.get_rpm_solv_sack();
    //TODO(amatej): This should be configured with installroot?
    //              It works for me now since I am on 35 and working with 34 metadata.
    //              But what is the right behavior though?
    libdnf::Goal goal(&(gnf->base));
    goal.add_rpm_install(pkg_name);
    goal.resolve(false);

    pkgGraph->deps = goal.list_rpm_installs();

    std::vector<std::string> nevras;
    for (const auto pkg : pkgGraph->deps) {
        nevras.push_back(pkg.get_full_nevra());
    }

    //TODO(amatej): I might want to store this?
    libdnf::rpm::SolvQuery pkgs = libdnf::rpm::SolvQuery(&solv_sack).ifilter_nevra(nevras);

    // Adjacency list graph representation
    // maybe matrix or direct translation would be better?
    //TODO(amatej): maybe it would be worth it to add also my deps (duplicit information but faster)?
    int tmp_x_coord = 0;
    for (const auto pkg : pkgGraph->deps) {
        const float pkg_width = FONT_CHAR_WIDTH * GNF_BUTTON_TEXT_SCALE * strlen(pkg.get_nevra().c_str());
        Node node = {vec2(0,tmp_x_coord*30), vec2(pkg_width,0), 0, pkg};
        pkgGraph->nodes.emplace(pkg.get_id().id, node);

        auto copy_pkgs = libdnf::rpm::SolvQuery(pkgs);
        copy_pkgs.ifilter_provides(pkg.get_requires());

        // Gather dependencies
        std::vector<int> dependencies;
        for (const auto dep : copy_pkgs) {
            dependencies.push_back(dep.get_id().id);
        }

        //printf("%i :depend on me\n", copy_pkgs.size());
        pkgGraph->adjList.emplace(pkg.get_id().id, dependencies);
        tmp_x_coord++;
    }

    dump_graph(&(pkgGraph->adjList), &(pkgGraph->nodes));

    hierarchical_graph_layout(&(pkgGraph->adjList), &(pkgGraph->nodes));

    gnf->state = GNF_GUI_STATE_PACKAGE_GRAPH;
    return;
}

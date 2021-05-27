#ifndef GNF_UTIL_HEADER
#define GNF_UTIL_HEADER

#include <libdnf/base/base.hpp>
#include <libdnf/rpm/repo.hpp>
#include <libdnf/rpm/solv_query.hpp>



//this is just until ifilter_provides is fixed and performant
bool reldeplist_contains(const libdnf::rpm::ReldepList list, libdnf::rpm::Reldep in_reldep) {
    for (const auto reldep: list) {
        if (reldep == in_reldep) {

            return true;
        }
    }

    return false;
}



#endif // GNF_UTIL_HEADER

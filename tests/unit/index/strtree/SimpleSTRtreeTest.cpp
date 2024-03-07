#include <tut/tut.hpp>
// geos
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Point.h>
#include <geos/index/strtree/SimpleSTRtree.h>
#include <geos/index/strtree/GeometryItemDistance.h>
#include <geos/index/ItemVisitor.h>
#include <geos/io/WKTReader.h>
#include <geos/geom/Envelope3d.h>
#include <iostream>
#include <ctime>
#include <iostream>
#include <thread>
#include <chrono>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#include <psapi.h>
//#include <tlhelp32.h>
#include <direct.h>
#include <process.h>
#else
#include <sys/stat.h>
//#include <sys/sysinfo.h>
#include <sys/time.h>
#include <unistd.h>
#endif
#include <google/protobuf/io/coded_stream.h>
// get current process pid
inline int GetCurrentPid()
{
    return getpid();
}

#define VMRSS_LINE 22


using namespace geos;

namespace tut {
// dummy data, not used
struct test_simplestrtree_data {};

using group = test_group<test_simplestrtree_data>;
using object = group::object;
group test_simplestrtree_group("geos::index::strtree::SimpleSTRtree");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    index::strtree::SimpleSTRtree t(10);
    std::vector<std::unique_ptr<geom::Geometry>> geoms;
    const int gridSize = 10;

    auto gf = geom::GeometryFactory::create();
    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            geom::Coordinate c((double)i, (double)j);
            geom::Point* pt = gf->createPoint(c);
            geoms.emplace_back(pt);
            t.insert(pt);
        }
    }

    geom::Envelope qe(-0.5, 1.5, -0.5, 1.5);
    std::vector<void*> matches;
    t.query(&qe, matches);
    // std::cout << matches.size() << std::endl;
    ensure(matches.size() == 4);

    // std::cout << t << std::endl;

    class SimpleTestVisitor: public index::ItemVisitor {
        public:
            std::size_t count;

            SimpleTestVisitor()
                : count(0)
                {}

            void
            visitItem(void* item) override
            {
                geom::Point* pt = static_cast<geom::Point*>(item);
                if (!pt->isEmpty())
                    count++;
                // std::cout << pt << std::endl;
            }
    };

    SimpleTestVisitor vis;
    t.query(&qe, vis);
    ensure(vis.count == 4);
}


template<>
template<>
void object::test<2>
()
{
    const int gridSize = 10;
    index::strtree::SimpleSTRtree t1(10);
    index::strtree::SimpleSTRtree t2(10);
    std::vector<std::unique_ptr<geom::Geometry>> geoms;

    auto gf = geom::GeometryFactory::create();
    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            geom::Coordinate c1((double)i, (double)j);
            geom::Coordinate c2((double)(i+gridSize+1), (double)(j+gridSize+1));
            geom::Point *pt1 = gf->createPoint(c1);
            geom::Point *pt2 = gf->createPoint(c2);
            geoms.emplace_back(pt1);
            geoms.emplace_back(pt2);
            t1.insert(pt1);
            t2.insert(pt2);
        }
    }

    std::pair<const void*, const void*> rslt;
    index::strtree::GeometryItemDistance gi;
    rslt = t1.nearestNeighbour(t2, &gi);

    const geom::Point* g1 = static_cast<const geom::Point*>(rslt.first);
    const geom::Point* g2 = static_cast<const geom::Point*>(rslt.second);

    // std::cout << *g1 << std::endl;
    // std::cout << *g2 << std::endl;

    ensure_equals(g1->getX(), 9.0);
    ensure_equals(g1->getY(), 9.0);
    ensure_equals(g2->getX(), 11.0);
    ensure_equals(g2->getY(), 11.0);
}


template<>
template<>
void object::test<3>
()
{
    auto gf = geom::GeometryFactory::create();
    geos::io::WKTReader wkt(*gf);
    index::strtree::SimpleSTRtree t(10);
    std::vector<std::unique_ptr<geom::Geometry>> geoms;
    geoms.emplace_back(wkt.read(std::string("LINESTRING(0 0, 10 10)")).release());
    geoms.emplace_back(wkt.read(std::string("LINESTRING(5 5, 15 15)")).release());
    geoms.emplace_back(wkt.read(std::string("LINESTRING(10 10, 20 20)")).release());
    geoms.emplace_back(wkt.read(std::string("LINESTRING(15 15, 25 25)")).release());

    for (auto& g: geoms) {
        t.insert(g.get());
    }

    const std::size_t leaf_before = t.getRoot()->getNumLeafNodes();
    // std::cout << "leaf_before " << leaf_before << std::endl;
    const std::size_t all_before = t.getRoot()->getNumNodes();
    // std::cout << "all_before " << all_before << std::endl;
    ensure_equals(leaf_before, 4u);
    ensure_equals(all_before, 5u);

    // std::cout << t << std::endl;

    t.remove(geoms[3]->getEnvelopeInternal(), geoms[3].get());

    const std::size_t leaf_after = t.getRoot()->getNumLeafNodes();
    // std::cout << "leaf_after " << leaf_after << std::endl;
    const std::size_t all_after = t.getRoot()->getNumNodes();
    // std::cout << "all_after " << all_after << std::endl;
    ensure_equals(leaf_after, 3u);
    ensure_equals(all_after, 4u);
}


// get specific process physical memeory occupation size by pid (MB)
inline float GetMemoryUsage(int pid)
{
#ifdef WIN32
    uint64_t mem = 0, vmem = 0;
PROCESS_MEMORY_COUNTERS pmc;

// get process hanlde by pid
HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
if (GetProcessMemoryInfo(process, &pmc, sizeof(pmc)))
{
    mem = pmc.WorkingSetSize;
    vmem = pmc.PagefileUsage;
}
CloseHandle(process);

// use GetCurrentProcess() can get current process and no need to close handle

// convert mem from B to MB
return mem / 1024.0 / 1024.0;

#else
    char file_name[64] = { 0 };
    FILE* fd;
    char line_buff[512] = { 0 };
    sprintf(file_name, "/proc/%d/status", pid);

    fd = fopen(file_name, "r");
    if (nullptr == fd)
        return 0;

    char name[64];
    int vmrss = 0;
    for (int i = 0; i < VMRSS_LINE - 1; i++)
        fgets(line_buff, sizeof(line_buff), fd);

    fgets(line_buff, sizeof(line_buff), fd);
    sscanf(line_buff, "%s %d", name, &vmrss);
    fclose(fd);

    // cnvert VmRSS from KB to MB
    return float(vmrss / 1024.0);
#endif
}



template<>
template<>
void object::test<4>
        () {

    index::strtree::SimpleSTRtree t(10);
    std::vector<std::unique_ptr<geom::Geometry>> geoms;
    const int gridSize = 100;
    const int gap = 10;
    auto gf = geom::GeometryFactory::create();

    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            for (int k= 0; k < gridSize; ++k) {
                    geos::geom::Envelope3d e(i, i + gap, j, j + gap, k, k + gap);
                    const auto ep = &e;
                    t.insert(ep, static_cast<void*>(&e));
            }
        }
    }
    t.getRoot3d();

}

} // namespace tut


#include "tut/tut.hpp"
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
#include <thread>
#include <chrono>
#include <string.h>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include "treeNode.pb.h"
#include <google/protobuf/io/coded_stream.h>
//#include "third_party/protobuf/stubs/version.h"

#include <adios2.h>
#include <ios>       //std::ios_base::failure
#include <stdexcept> //std::invalid_argument std::exception
#include <vector>
#include <limits>
#include <rocksdb/db.h>

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

// get current process pid
inline int GetCurrentPid() {
    return getpid();
}

#define VMRSS_LINE 22


using namespace geos;
using ROCKSDB_NAMESPACE::WriteBatch;

namespace tut {
// dummy data, not used
    struct test_simplestrtree_data {
    };

    using group = test_group<test_simplestrtree_data>;
    using object = group::object;
    group test_simplestrtree_group("geos::index::strtree::SimpleSTRtree");

//
// Test Cases
//

    void deSerialize(geos::index::strtree::SimpleSTRnode3d *&node_ptr, Tree *serializeList, int &seq) {
        const TreeNode &node = (*serializeList).treenodes(seq);
        geos::geom::Envelope3d e(node.minx(), node.maxx(), node.miny(), node.maxy(), node.minz(), node.maxz(), node.slice_id());
        node_ptr = new geos::index::strtree::SimpleSTRnode3d(static_cast<size_t>(node.level()), &e,
                                                             static_cast<void *>(&e));
        if (node.childsize() != 0) {
            for (int j = 0; j < node.childsize(); j++) {
                geos::index::strtree::SimpleSTRnode3d *child = nullptr;
                seq = seq + 1;
                deSerialize(child, serializeList, seq);
                node_ptr->addChildNode3d(child);
            }
        }
    }

    void matchEnv(geos::index::strtree::SimpleSTRnode3d *node_ptr, geos::geom::Envelope3d *e, std::vector<geos::geom::Envelope3d *> &envolope_list) {
        geos::geom::Envelope3d* envelopePtr = static_cast<geos::geom::Envelope3d*>(const_cast<void*>(node_ptr->getBounds()));
        if (envelopePtr->intersects(e)) {
            if (node_ptr->isLeaf()) {
                envolope_list.push_back(envelopePtr);
            } else {
                for (const auto &child : node_ptr->getChildNodes()) {
                    matchEnv(child, e, envolope_list);
                }
            }
        }
    }

    void queryByEnvelope(rocksdb::DB* db, std::string key, geos::geom::Envelope3d *e, std::vector<geos::geom::Envelope3d *> &envolope_list) {
        // Query the database
        rocksdb::ReadOptions read_options;
        std::string read_value;
        db->Get(read_options, key, &read_value);
        Tree readSerializeList;
        if (!readSerializeList.ParseFromString(read_value)) {
            std::cerr << "Failed to parse file." << std::endl;
        }
        // deserialize the first level tree
        geos::index::strtree::SimpleSTRnode3d *root_node = nullptr;
        int seq = 0;
        deSerialize(root_node, &readSerializeList, seq);
        // std::cout << "deSerialize successfully" << std::endl;

        // if the root node is null, return
        if (root_node == nullptr) {
            std::cout << "root node is null" << std::endl;
            return;
        }

        // match the envolope with the tree
        matchEnv(root_node, e, envolope_list);
    }

    template<>
    template<>
    void object::test<1>
            () {
        
        // set iteration & type
        int iteration = 100;
        std::string particleCharacters = "momentum";
        std::string particleType = "electrons";
        std::string firstLevelKey = "/data/" + std::to_string(iteration) + "/particles/" + particleType + "/" + particleCharacters + "/";

        // set the envolope size (x, y, z)
//        double min_x = -DoubleInfinity;
//        double max_x = DoubleInfinity;
//        double min_y = -DoubleInfinity;
//        double max_y = DoubleInfinity;
//        double min_z = -DoubleInfinity;
//        double max_z = DoubleInfinity;

        double min_x = 1.24e-28;
        double max_x = 1.24e-30;
        double min_y = 0;
        double max_y = 1.24e-30;
        double min_z = -DoubleInfinity;
        double max_z = 1.24e-30;

        geos::geom::Envelope3d e(min_x, max_x, min_y, max_y, min_z, max_z, -1);
        
        // Open RocksDB database
        rocksdb::DB* db;
        rocksdb::Options options;
        const std::string db_path = "/tmp/rocksdb-gc";
        rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db);
        if (!status.ok()) {
            std::cerr << "Failed to open database: " << status.ToString() << std::endl;
        }

        // match the envolope with the tree
        std::vector<geos::geom::Envelope3d *> envelope_list;
        queryByEnvelope(db, firstLevelKey, &e, envelope_list);
        std::cout << "first level match num" << envelope_list.size() << std::endl;

        // match the envolope with the tree
        std::vector<geos::geom::Envelope3d *> envelope_list_second;
        for (auto &envelope : envelope_list) {
            std::string secondLevelKey = "/data/" + std::to_string(iteration) + "/particles/" + particleType + "/" + particleCharacters + "/" + std::to_string(envelope->getSliceId());
            queryByEnvelope(db, secondLevelKey, &e, envelope_list_second);
            // todo one problem is: envelop_list_second only contains slice_id, but not block number
            // 1. declare envelope_list_second inside for loop, a map outside the for loop
            // 2. add block info to each leaf node
        }
        std::cout << "second level match num" << envelope_list_second.size() << std::endl;

    }


    template<>
    template<>
    void object::test<2>
            () {
        // Open RocksDB database
        rocksdb::DB* db;
        rocksdb::Options options;
        const std::string db_path = "/tmp/rocksdb-gc";
        rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db);
        if (!status.ok()) {
            std::cerr << "Failed to open database: " << status.ToString() << std::endl;
        }

        // Create an iterator
        rocksdb::ReadOptions read_options;
        read_options.fill_cache = false; // set to false to avoid caching
        rocksdb::Iterator* it = db->NewIterator(read_options);

        // Iterate over all keys and print them
        for (it->SeekToFirst(); it->Valid(); it->Next()) {
            std::cout << "Key: " << it->key().ToString() << std::endl;
        }

        // Check for errors
        if (!it->status().ok()) {
            std::cerr << "Failed to iterate over keys: " << it->status().ToString() << std::endl;
            delete it;
            delete db;
        }

        // Clean up
        delete it;
        delete db;
    }



    template<>
    template<>
    void object::test<3>
            () {
        // Open RocksDB database
        rocksdb::DB* db;
        rocksdb::Options options;
        const std::string db_path = "/tmp/rocksdb-gc";
        rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db);
        if (!status.ok()) {
            std::cerr << "Failed to open database: " << status.ToString() << std::endl;
        }

        // Create an iterator
        rocksdb::ReadOptions read_options;
        std::string read_value;
        db->Get(read_options, "/data/100/particles/electrons/momentum/0", &read_value);
        Tree readSerializeList;
        if (!readSerializeList.ParseFromString(read_value)) {
            std::cerr << "Failed to parse file." << std::endl;
        }
        geos::index::strtree::SimpleSTRnode3d *root_node = nullptr;
        int seq = 0;
        deSerialize(root_node, &readSerializeList, seq);
        std::cout << "deSerialize successfully" << std::endl;

        db->Get(read_options, "/data/100/particles/electrons/momentum/", &read_value);
        Tree readSerializeList2;
        if (!readSerializeList2.ParseFromString(read_value)) {
            std::cerr << "Failed to parse file." << std::endl;
        }
        geos::index::strtree::SimpleSTRnode3d *root_node2= nullptr;
        seq = 0;
        deSerialize(root_node2, &readSerializeList2, seq);
        std::cout << "deSerialize successfully" << std::endl;
    }


// get specific process physical memeory occupation size by pid (MB)
    inline float GetMemoryUsage(int pid) {
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
        char file_name[64] = {0};
        FILE *fd;
        char line_buff[512] = {0};
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


    void serialize(geos::index::strtree::SimpleSTRnode3d *node_ptr, Tree &serializeList) {
        geos::geom::Envelope3d *bounds = (geom::Envelope3d *) node_ptr->getBounds();
        std::size_t nChildren = node_ptr->getChildNodes().size();
        TreeNode signleNode;
        signleNode.set_level((int) node_ptr->getLevel());
        signleNode.set_childsize((int) node_ptr->getChildNodes().size());
        signleNode.set_minx(bounds->getMinX());
        signleNode.set_maxx(bounds->getMaxX());
        signleNode.set_miny(bounds->getMinY());
        signleNode.set_maxy(bounds->getMaxY());
        signleNode.set_minz(bounds->getMinZ());
        signleNode.set_maxz(bounds->getMaxZ());
        signleNode.set_slice_id((int) bounds->getSliceId());

        serializeList.add_treenodes()->CopyFrom(signleNode);
        for (std::size_t j = 0; j < nChildren; j++) {
            serialize(node_ptr->getChildNodes()[j], serializeList);
        }
    }



    void writeTreeToBatch(geos::index::strtree::SimpleSTRtree &tree, WriteBatch &batch, const std::string &key) {
        Tree serializeList;
        serialize(tree.root3d, serializeList);
        std::string serializeString;
        serializeList.SerializeToString(&serializeString);
        batch.Put(key, serializeString);
    }

    inline void
    buildSTRtree3d(const std::vector<double> &myDoubleX,
                     const std::vector<double> &myDoubleY,
                     const std::vector<double> &myDoubleZ,
                     const std::string &key,
                     WriteBatch& batch,
                     geos::geom::Envelope3d* blockSliceE,
                     const int &STRtreeLeafSize = 10,
                     const int &blockSliceSize = 100) {

        index::strtree::SimpleSTRtree secondLevelBlockTree(STRtreeLeafSize);
        geos::geom::Envelope3d sliceEnvelope3d;
        for (size_t i = 0; i < myDoubleX.size(); i++) {
            sliceEnvelope3d.expandToInclude(myDoubleX[i], myDoubleY[i], myDoubleZ[i]);

            if ((i + 1) % blockSliceSize == 0 || i == myDoubleX.size() - 1) {
                sliceEnvelope3d.setSliceId(i / blockSliceSize);

                // insert the slice envelope into the R-Tree
                const auto ep = &sliceEnvelope3d;
                secondLevelBlockTree.insert(ep, static_cast<void *>(&sliceEnvelope3d));

                // update the block slice envelope
                blockSliceE->expandToInclude(sliceEnvelope3d);

                // reset the slice envelope
                sliceEnvelope3d.setToNull();
            }
        }
        secondLevelBlockTree.getRoot3d();
        writeTreeToBatch(secondLevelBlockTree, batch, key);
    }


    template<>
    template<>
    void object::test<4>
            () {
        int STRtreeLeafSize = 10;
        int blockSliceSize = 100;
        bool isBlockTurnOnMinMax = false;
        std::string bpFilename = "/data/gc/rocksdb-index/WarpX/build/bin/diags/diag1/openpmd.bp";

        WriteBatch batch;
        geos::geom::Envelope3d blockSliceE;
        std::vector<std::string> metaKeys;

        rocksdb::DB *db;
        rocksdb::Options options;
        options.create_if_missing = true;
        // momentum or position
        std::string particleCharacters = "momentum";
        const std::string db_path = "/tmp/rocksdb-gc";

        adios2::ADIOS adios;
        adios2::IO bpIO = adios.DeclareIO("ReadBP");
        adios2::Engine bpReader = bpIO.Open(bpFilename, adios2::Mode::Read);
        const std::map<std::string, adios2::Params> variables = bpIO.AvailableVariables(true);

        for (const auto &variablePair : variables) {
            if (variablePair.first.find(particleCharacters) != std::string::npos && variablePair.first.back() == 'x')
                metaKeys.push_back(variablePair.first);
        }
        // Sort the elements in metaKeys
        std::sort(metaKeys.begin(), metaKeys.end());

        for (const auto &metakey : metaKeys) {
            index::strtree::SimpleSTRtree firstLevelBlockTree(STRtreeLeafSize);

            // std::string key = "/data/100/particles/electrons/position/";
            std::string key = metakey.substr(0, metakey.length() - 1);
            // Write variable for buffering
            adios2::Variable<double> x_100_position = bpIO.InquireVariable<double>(key + "x");
            adios2::Variable<double> y_100_position = bpIO.InquireVariable<double>(key + "y");
            adios2::Variable<double> z_100_position = bpIO.InquireVariable<double>(key + "z");

            // Read out all block info
            auto xBlocksInfo = bpReader.AllStepsBlocksInfo(x_100_position);
            auto yBlocksInfo = bpReader.AllStepsBlocksInfo(y_100_position);
            auto zBlocksInfo = bpReader.AllStepsBlocksInfo(z_100_position);

            if (x_100_position && y_100_position && z_100_position) {
                // Iterate over the maps
                for (auto it1 = xBlocksInfo.begin(), it2 = yBlocksInfo.begin(), it3 = zBlocksInfo.begin();
                     it1 != xBlocksInfo.end() && it2 != yBlocksInfo.end() && it3 != zBlocksInfo.end();
                     ++it1, ++it2, ++it3) {
                    const auto &var_vec1 = it1->second;
                    const auto &var_vec2 = it2->second;
                    const auto &var_vec3 = it3->second;

                    // Iterate over the vectors
                    for (size_t i = 0; i < var_vec1.size() && i < var_vec2.size() && i < var_vec3.size(); ++i) {
                        const auto &var_info1 = var_vec1[i];
                        const auto &var_info2 = var_vec2[i];
                        const auto &var_info3 = var_vec3[i];
                        // Compare the blockid in each vector
                        if (var_info1.BlockID == var_info2.BlockID && var_info1.BlockID == var_info3.BlockID) {

                            // Access var_info1, var_info2, var_info3 simultaneously with the same blockid
                            x_100_position.SetSelection({var_info1.Start, var_info1.Count});
                            y_100_position.SetSelection({var_info2.Start, var_info2.Count});
                            z_100_position.SetSelection({var_info3.Start, var_info3.Count});

                            std::vector<double> myDoubleX;
                            std::vector<double> myDoubleY;
                            std::vector<double> myDoubleZ;

                            bpReader.Get<double>(x_100_position, myDoubleX, adios2::Mode::Sync);
                            bpReader.Get<double>(y_100_position, myDoubleY, adios2::Mode::Sync);
                            bpReader.Get<double>(z_100_position, myDoubleZ, adios2::Mode::Sync);

                            buildSTRtree3d(myDoubleX, myDoubleY, myDoubleZ, key + std::to_string(var_info1.BlockID),
                                             batch, &blockSliceE, STRtreeLeafSize, blockSliceSize);
                            blockSliceE.setSliceId(i);
                            firstLevelBlockTree.insert(&blockSliceE, static_cast<void *>(&blockSliceE));
                            blockSliceE.setToNull();
                        }
                    }
                }
            }
            firstLevelBlockTree.getRoot3d();
            writeTreeToBatch(firstLevelBlockTree, batch, key);

            rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db);
            status = db->Write(rocksdb::WriteOptions(), &batch);
            if (status.ok()) {
                std::cout << key << "\tWriting to DB successfully" << std::endl;
                delete db;
            }

            batch.Clear();
            std::cout << firstLevelBlockTree.root3d->getEnvelope().toString() << std::endl;
        }
        /** Close bp file, engine becomes unreachable after this*/
        bpReader.Close();
    }
} // namespace tut


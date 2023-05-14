//
// Created by change_change_2 on 2023/1/31.
//

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

// get current process pid
inline int GetCurrentPid()
{
    return getpid();
}

#define VMRSS_LINE 22
#define PROCESS_ITEM 14

// get specific process cpu occupation ratio by pid
//#ifdef WIN32
////
//static uint64_t convert_time_format(const FILETIME* ftime)
//{
//    LARGE_INTEGER li;
//
//    li.LowPart = ftime->dwLowDateTime;
//    li.HighPart = ftime->dwHighDateTime;
//    return li.QuadPart;
//}
//#else
// FIXME: can also get cpu and mem status from popen cmd
// the info line num in /proc/{pid}/status file



/*

        index::strtree::SimpleSTRtree t(10);
        std::vector<std::unique_ptr<geom::Geometry>> geoms;
        const int gridSize = 10;

        auto gf = geom::GeometryFactory::create();
        for (int i = 0; i < gridSize; ++i) {
            for (int j = 0; j < gridSize; ++j) {
                geom::Coordinate c((double) i, (double) j);
                geom::Point *pt = gf->createPoint(c);
                geoms.emplace_back(pt);
                t.insert(pt);
            }
        }

        geom::Envelope qe(-0.5, 1.5, -0.5, 1.5);
        std::vector<void *> matches;
        t.query(&qe, matches);
        // std::cout << matches.size() << std::endl;
        ensure(matches.size() == 4);

        // std::cout << t << std::endl;

        class SimpleTestVisitor : public index::ItemVisitor {
        public:
            std::size_t count;

            SimpleTestVisitor()
                    : count(0) {}

            void
            visitItem(void *item) override {
                geom::Point *pt = static_cast<geom::Point *>(item);
                if (!pt->isEmpty())
                    count++;
                // std::cout << pt << std::endl;
            }
        };

        SimpleTestVisitor vis;
        t.query(&qe, vis);
        ensure(vis.count == 4);


        Tree serializeList;
        serialize(secondLevelBlockTree.root3d, serializeList);
//        std::ofstream output("tree.idx", std::ios::out | std::ios::binary);
//        serializeList.SerializeToOstream(&output);
//        output.close();
        std::string binary_data;
        serializeList.SerializeToString(&binary_data);

        end = clock();
        std::cout << "Serializing R-Tree time: " << (double) (end - start) / CLOCKS_PER_SEC << "S" << std::endl;

        // Write a key-value pair to the database

        rocksdb::Status status = batch.Put(key, binary_data);
        std::cout << "write status: " << status.ok() << std::endl;




        const int gridSize = 10;
        index::strtree::SimpleSTRtree t1(10);
        index::strtree::SimpleSTRtree t2(10);
        std::vector<std::unique_ptr<geom::Geometry>> geoms;

        auto gf = geom::GeometryFactory::create();
        for (int i = 0; i < gridSize; ++i) {
            for (int j = 0; j < gridSize; ++j) {
                geom::Coordinate c1((double) i, (double) j);
                geom::Coordinate c2((double) (i + gridSize + 1), (double) (j + gridSize + 1));
                geom::Point *pt1 = gf->createPoint(c1);
                geom::Point *pt2 = gf->createPoint(c2);
                geoms.emplace_back(pt1);
                geoms.emplace_back(pt2);
                t1.insert(pt1);
                t2.insert(pt2);
            }
        }

        std::pair<const void *, const void *> rslt;
        index::strtree::GeometryItemDistance gi;
        rslt = t1.nearestNeighbour(t2, &gi);

        const geom::Point *g1 = static_cast<const geom::Point *>(rslt.first);
        const geom::Point *g2 = static_cast<const geom::Point *>(rslt.second);

        // std::cout << *g1 << std::endl;
        // std::cout << *g2 << std::endl;

        ensure_equals(g1->getX(), 9.0);
        ensure_equals(g1->getY(), 9.0);
        ensure_equals(g2->getX(), 11.0);
        ensure_equals(g2->getY(), 11.0);












void read(){
    // Read the value for the key
    std::string read_value;
    status = db->Get(rocksdb::ReadOptions(), key, &read_value);
    std::cout << "read status: " << status.ok() << std::endl;
    std::cout << "origin value: " << binary_data << "\tread value: " << read_value << std::endl;

    std::cout << "Start deSerialize R-Tree " << std::endl;
    start = clock();

//        std::ifstream input_file("tree.idx", std::ios::in | std::ios::binary);

    // Check if the file was opened successfully
//        if (!input_file) {
//            std::cerr << "Failed to open file." << std::endl;
//        }

    Tree readSerializeList;
//        if (!readSerializeList.ParseFromIstream(&input_file)) {
    if (!readSerializeList.ParseFromString(binary_data)) {
        std::cerr << "Failed to parse file." << std::endl;
    }
    geos::index::strtree::SimpleSTRnode3d *root_node = nullptr;
    int seq = 0;
    deSerialize(root_node, &readSerializeList, seq);

//    for (int i = 0; i < readSerializeList.treenodes_size(); i++) {
//        const TreeNode& node = readSerializeList.treenodes(i);
//        // Process the node.
//        std::cout << "Field value" << std::endl;
//    }

    // Close the file
//        input_file.close();

    end = clock();
    std::cout << "de-serializing R-Tree time: " << (double) (end - start) / CLOCKS_PER_SEC << "S" << std::endl;

    std::cout << "Debug break point here" << std::endl;
}


 //        const std::map<std::string, adios2::Params> variables = bpIO.AvailableVariables(true);
        const std::map<std::string, adios2::Params> variables = bpIO.AvailableVariables();



        try {

            std::cout << "List of variables:";
            for (const auto &variablePair : variables)
            {
                std::cout << "  " << variablePair.first << "\n";

                for (const auto &parameter : variablePair.second)
                {
                    std::cout << "\t" << parameter.first << ": " << parameter.second
                              << "\n";
                }
            }





































//            std::cout << "Double vector inside " << filename << ": {" << "\n";
//            std::cout << "Size: " << myDouble.size() << ": {" << "\n";
//            for (auto &x : myDouble)
//            {
//                std::cout << x << ", ";
//            }
//            std::cout << "}\n";


//    buildSTRtree((int)100, 100);
//    std::cout << "Protocol Buffers version: " << GOOGLE_PROTOBUF_VERSION << std::endl;

//            adios2::Variable<double> x_100_position =
//                    bpIO.InquireVariable<double>("/data/100/particles/electrons/momentum/x");
//            auto charsBlocksInfo = bpReader.AllStepsBlocksInfo(x_100_position);
//            if (x_100_position)
//            {
//                // Iterate over the map and print its contents
//                for (const auto& map_pair : charsBlocksInfo) {
//                    std::cout << "Map key: " << map_pair.first << std::endl;
//
//                    const auto& var_vec = map_pair.second;
//
//                    for (const auto& var_info : var_vec) {
//                        for (size_t i = 0; i < var_info.Start.size(); i++){
//                            std::cout << "Variable Start: " << var_info.Start[i] << "\tVariable Count: " << var_info.Count[i] << std::endl;
//
//                        }
//                        x_100_position.SetSelection({var_info.Start, var_info.Count});
//                        std::cout << "Variable Min: " << var_info.Min << std::endl;
//                        std::cout << "Variable Max: " << var_info.Max << std::endl;
//
//                        std::vector<double> myDouble;
//                        bpReader.Get<double>(x_100_position, myDouble, adios2::Mode::Sync);
//                        std::cout << "Double vector inside " << filename << ": {" << "\n";
//                        std::cout << "Size: " << myDouble.size() << ": {" << "\n";
//                        //            for (auto &x : myDouble)
//                        //            {
//                        //                std::cout << x << ", ";
//                        //            }
//                        std::cout << "}\n";
//
//
//                    }
//                }

//            if (y_100_position) {
//                std::vector<double> myDouble;
//                bpReader.Get<double>(y_100_position, myDouble, adios2::Mode::Sync);
//                std::cout << "Double vector inside " << filename << ": {" << "\n";
//                std::cout << "Size: " << myDouble.size() << ": {" << "\n";
//                //            for (auto &x : myDouble)
//                //            {
//                //                std::cout << x << ", ";
//                //            }
//                std::cout << "}\n";
//            }



//        adios2::Variable<double> z_100_position =
//                bpIO.InquireVariable<double>("/data/100/particles/electrons/momentum/z");

//            if (z_100_position) {
//                std::vector<double> myDouble;
//                bpReader.Get<double>(z_100_position, myDouble, adios2::Mode::Sync);
//                std::cout << "Double vector inside " << filename << ": {" << "\n";
//                std::cout << "Size: " << myDouble.size() << ": {" << "\n";
//                //            for (auto &x : myDouble)
//                //            {
//                //                std::cout << x << ", ";
//                //            }
//                std::cout << "}\n";
//            }
//
//            adios2::Variable<float> bpFloats =
//                    bpIO.InquireVariable<float>("bpFloats");
//
//            adios2::Variable<int> bpInts = bpIO.InquireVariable<int>("bpInts");
//
//            if (bpFloats) {
//                std::vector<float> myFloats;
//                bpReader.Get<float>(bpFloats, myFloats, adios2::Mode::Sync);
//                std::cout << "Float vector inside " << filename << ": {";
//                for (auto &x: myFloats) {
//                    std::cout << x << ", ";
//                }
//                std::cout << "}\n";
//            }
//
//            if (bpInts) {
//                std::vector<int> myInts;
//                bpReader.Get<int>(bpInts, myInts, adios2::Mode::Sync);
//                std::cout << "Int vector inside " << filename << ": {";
//                for (auto &x: myInts) {
//                    std::cout << x << ", ";
//                }
//                std::cout << "}\n";
//            } else {
//                std::cout << "There are no integer datasets in " << filename
//                          << ".\n";
//            }


 //    buildSTRtree((int)100, 100);

//    for (int i = 0; i < 5; i++)
//        buildSTRtree((int)pow(10, i+1), 10*(i+1));
//        std::thread t(buildSTRtree).join();
//    std::thread t1(buildSTRtree);
//    std::thread t2(buildSTRtree);
//    std::thread t3(buildSTRtree);
//    std::thread t4(buildSTRtree);
//    std::thread t5(buildSTRtree);
//    std::thread t6(buildSTRtree);
//    std::thread t7(buildSTRtree);
//    std::thread t8(buildSTRtree);
//    std::thread t5(buildSTRtree);
//
//    t1.join();
//    t2.join();
//    t3.join();
//    t4.join();
//    t5.join();
//    t6.join();
//    t7.join();
//    t8.join();
//    t5.join();

//    int current_pid = GetCurrentPid(); // or you can set a outside program pid
//    float cpu_usage_ratio = GetCpuUsageRatio(current_pid);
//    float memory_usage = GetMemoryUsage(current_pid);

//    while (true)
//    {
//    std::cout << "current pid: " << current_pid << std::endl;
//        std::cout << "cpu usage ratio: " << cpu_usage_ratio * 100 << "%" << std::endl;
//    std::cout << "memory usage: " << memory_usage << "MB" << std::endl;

//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//    }



*/



















//static const char* get_items(const char* buffer, unsigned int item)
//{
//    // read from buffer by offset
//    const char* p = buffer;
//
//    int len = (int)strlen(buffer);
//    int count = 0;
//
//    for (int i = 0; i < len; i++)
//    {
//        if (' ' == *p)
//        {
//            count++;
//            if (count == (int)item - 1)
//            {
//                p++;
//                break;
//            }
//        }
//        p++;
//    }
//
//    return p;
//}
//
//static inline unsigned long get_cpu_total_occupy()
//{
//    // get total cpu use time
//
//    // different mode cpu occupy time
//    unsigned long user_time;
//    unsigned long nice_time;
//    unsigned long system_time;
//    unsigned long idle_time;
//
//    FILE* fd;
//    char buff[1024] = { 0 };
//
//    fd = fopen("/proc/stat", "r");
//    if (nullptr == fd)
//        return 0;
//
//    fgets(buff, sizeof(buff), fd);
//    char name[64] = { 0 };
//    sscanf(buff, "%s %ld %ld %ld %ld", name, &user_time, &nice_time, &system_time, &idle_time);
//    fclose(fd);
//
//    return (user_time + nice_time + system_time + idle_time);
//}
//
//static inline unsigned long get_cpu_proc_occupy(int pid)
//{
//    // get specific pid cpu use time
//    unsigned int tmp_pid;
//    unsigned long utime;  // user time
//    unsigned long stime;  // kernel time
//    unsigned long cutime; // all user time
//    unsigned long cstime; // all dead time
//
//    char file_name[64] = { 0 };
//    FILE* fd;
//    char line_buff[1024] = { 0 };
//    sprintf(file_name, "/proc/%d/stat", pid);
//
//    fd = fopen(file_name, "r");
//    if (nullptr == fd)
//        return 0;
//
//    fgets(line_buff, sizeof(line_buff), fd);
//
//    sscanf(line_buff, "%u", &tmp_pid);
//    const char* q = get_items(line_buff, PROCESS_ITEM);
//    sscanf(q, "%ld %ld %ld %ld", &utime, &stime, &cutime, &cstime);
//    fclose(fd);
//
//    return (utime + stime + cutime + cstime);
//}
//#endif

//inline float GetCpuUsageRatio(int pid)
//{
//#ifdef WIN32
//    static int64_t last_time = 0;
//    static int64_t last_system_time = 0;
//
//    FILETIME now;
//    FILETIME creation_time;
//    FILETIME exit_time;
//    FILETIME kernel_time;
//    FILETIME user_time;
//    int64_t system_time;
//    int64_t time;
//    int64_t system_time_delta;
//    int64_t time_delta;
//
//    // get cpu num
//    SYSTEM_INFO info;
//    GetSystemInfo(&info);
//    int cpu_num = info.dwNumberOfProcessors;
//
//    float cpu_ratio = 0.0;
//
//    // get process hanlde by pid
//    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
//    // use GetCurrentProcess() can get current process and no need to close handle
//
//    // get now time
//    GetSystemTimeAsFileTime(&now);
//
//    if (!GetProcessTimes(process, &creation_time, &exit_time, &kernel_time, &user_time))
//    {
//        // We don't assert here because in some cases (such as in the Task Manager)
//        // we may call this function on a process that has just exited but we have
//        // not yet received the notification.
//        printf("GetCpuUsageRatio GetProcessTimes failed\n");
//        return 0.0;
//    }
//
//    // should handle the multiple cpu num
//    system_time = (convert_time_format(&kernel_time) + convert_time_format(&user_time)) / cpu_num;
//    time = convert_time_format(&now);
//
//    if ((last_system_time == 0) || (last_time == 0))
//    {
//        // First call, just set the last values.
//        last_system_time = system_time;
//        last_time = time;
//        return 0.0;
//    }
//
//    system_time_delta = system_time - last_system_time;
//    time_delta = time - last_time;
//
//    CloseHandle(process);
//
//    if (time_delta == 0)
//    {
//        printf("GetCpuUsageRatio time_delta is 0, error\n");
//        return 0.0;
//    }
//
//    // We add time_delta / 2 so the result is rounded.
//    cpu_ratio = (int)((system_time_delta * 100 + time_delta / 2) / time_delta); // the % unit
//    last_system_time = system_time;
//    last_time = time;
//
//    cpu_ratio /= 100.0; // convert to float number
//
//    return cpu_ratio;
//#else
//    unsigned long totalcputime1, totalcputime2;
//    unsigned long procputime1, procputime2;
//
//    totalcputime1 = get_cpu_total_occupy();
//    procputime1 = get_cpu_proc_occupy(pid);
//
//    // FIXME: the 200ms is a magic number, works well
//    usleep(200000); // sleep 200ms to fetch two time point cpu usage snapshots sample for later calculation
//
//    totalcputime2 = get_cpu_total_occupy();
//    procputime2 = get_cpu_proc_occupy(pid);
//
//    float pcpu = 0.0;
//    if (0 != totalcputime2 - totalcputime1)
//        pcpu = float(procputime2 - procputime1) / float(totalcputime2 - totalcputime1); // float number
//
//    int cpu_num = get_nprocs();
//    pcpu *= (float)cpu_num; // should multiply cpu num in multiple cpu machine
//
//    return pcpu;
//#endif
//}

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



int main()
{
    // launch some task to occupy cpu and memory
    for (int i = 0; i < 5; i++)
        std::thread([]
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }).detach();

    int current_pid = GetCurrentPid(); // or you can set a outside program pid
//    float cpu_usage_ratio = GetCpuUsageRatio(current_pid);
    float memory_usage = GetMemoryUsage(current_pid);

    while (true)
    {
        std::cout << "current pid: " << current_pid << std::endl;
//        std::cout << "cpu usage ratio: " << cpu_usage_ratio * 100 << "%" << std::endl;
        std::cout << "memory usage: " << memory_usage << "MB" << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}





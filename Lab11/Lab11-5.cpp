#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <cstring>

using namespace std;

// TODO 1: Performance Metrics
struct IOMetrics {
    string testName;
    double throughput;
    double latency;
    int totalOps;
    double duration;

    void display() {
        cout << left << setw(15) << testName 
             << " | TP: " << fixed << setprecision(2) << throughput / 1024 / 1024 << " MB/s"
             << " | Latency: " << latency << " ms" << endl;
    }
};

// TODO 2: Simulated Disk
class SimulatedDisk {
private:
    vector<vector<uint8_t>> sectors;
    size_t sectorSize;
    size_t numSectors;

public:
    SimulatedDisk(size_t sz, size_t num) : sectorSize(sz), numSectors(num) {
        sectors.resize(num, vector<uint8_t>(sz, 0));
    }

    double sequentialWrite(size_t n) {
        auto start = chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n; i++) sectors[i][0] = 1;
        auto end = chrono::high_resolution_clock::now();
        return chrono::duration<double>(end - start).count();
    }

    double randomWrite(size_t n) {
        auto start = chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n; i++) sectors[rand() % numSectors][0] = 1;
        auto end = chrono::high_resolution_clock::now();
        return chrono::duration<double>(end - start).count();
    }
    
    size_t getSectorSize() { return sectorSize; }
};

// TODO 3: Buffer Size Impact
class BufferTest {
public:
    static IOMetrics testBuffer(size_t bSize, size_t total) {
        vector<uint8_t> src(total, 1), dst(total, 0);
        auto start = chrono::high_resolution_clock::now();
        int ops = 0;
        for (size_t i = 0; i < total; i += bSize) {
            memcpy(dst.data() + i, src.data() + i, min(bSize, total - i));
            ops++;
        }
        auto end = chrono::high_resolution_clock::now();
        double dur = chrono::duration<double>(end - start).count();
        return {"Buf " + to_string(bSize), total / dur, (dur / ops) * 1000, ops, dur};
    }
};

int main() {
    cout << "--- I/O Performance Measurement ---" << endl;
    
    // Disk Test
    SimulatedDisk disk(512, 10000);
    size_t ops = 5000;
    double sTime = disk.sequentialWrite(ops);
    double rTime = disk.randomWrite(ops);

    cout << "Sequential Write Time: " << sTime << " s" << endl;
    cout << "Random Write Time: " << rTime << " s" << endl;

    // Buffer Impact
    cout << "\n--- Buffer Size Impact ---" << endl;
    BufferTest::testBuffer(64, 1024 * 1024).display();
    BufferTest::testBuffer(4096, 1024 * 1024).display();

    return 0;
}

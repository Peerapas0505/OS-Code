#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <map>
#include <mutex>
#include <thread>

using namespace std;

// TODO 1: I/O Request Structure
struct IORequest {
    enum class Type { READ, WRITE };
    enum class Priority { HIGH, MEDIUM, LOW };

    int requestId;
    Type type;
    Priority priority;
    size_t blockNumber;
    size_t dataSize;
    vector<uint8_t> data;
    chrono::time_point<chrono::steady_clock> arrivalTime;

    IORequest(int id, Type t, Priority p, size_t block, size_t size)
        : requestId(id), type(t), priority(p), blockNumber(block), dataSize(size) {
        arrivalTime = chrono::steady_clock::now();
        data.resize(size, 0);
    }

    void display() {
        string tStr = (type == Type::READ) ? "READ" : "WRITE";
        cout << "Req[" << requestId << "] " << tStr << " Block:" << blockNumber 
             << " Priority:" << (int)priority << endl;
    }
};

// TODO 2: FCFS I/O Scheduler
class FCFSScheduler {
private:
    queue<IORequest> requestQueue;
    int processedCount = 0;

public:
    void addRequest(IORequest req) { requestQueue.push(req); }

    void processAll() {
        cout << "\n[FCFS] Processing..." << endl;
        while (!requestQueue.empty()) {
            IORequest req = requestQueue.front();
            requestQueue.pop();
            req.display();
            processedCount++;
        }
    }
};

// TODO 3: Priority I/O Scheduler
class PriorityScheduler {
private:
    queue<IORequest> high, medium, low;
    int processedCount = 0;

public:
    void addRequest(IORequest req) {
        if (req.priority == IORequest::Priority::HIGH) high.push(req);
        else if (req.priority == IORequest::Priority::MEDIUM) medium.push(req);
        else low.push(req);
    }

    bool processNext() {
        if (!high.empty()) { high.front().display(); high.pop(); }
        else if (!medium.empty()) { medium.front().display(); medium.pop(); }
        else if (!low.empty()) { low.front().display(); low.pop(); }
        else return false;
        processedCount++;
        return true;
    }

    void processAll() {
        cout << "\n[PRIORITY] Processing..." << endl;
        while (processNext());
    }
};

// TODO 4: Circular Buffer
class CircularBuffer {
private:
    vector<uint8_t> buffer;
    int head, tail, capacity, count;
    mutex bufMutex;

public:
    CircularBuffer(int size) : capacity(size), head(0), tail(0), count(0) {
        buffer.resize(size);
    }

    bool write(uint8_t data) {
        lock_guard<mutex> lock(bufMutex);
        if (count == capacity) return false;
        buffer[tail] = data;
        tail = (tail + 1) % capacity;
        count++;
        return true;
    }

    bool read(uint8_t& data) {
        lock_guard<mutex> lock(bufMutex);
        if (count == 0) return false;
        data = buffer[head];
        head = (head + 1) % capacity;
        count--;
        return true;
    }
};

// TODO 5: I/O Cache (LRU)
class IOCache {
private:
    struct CacheEntry {
        vector<uint8_t> data;
        chrono::time_point<chrono::steady_clock> lastAccess;
    };
    map<size_t, CacheEntry> cache;
    size_t maxEntries;

public:
    IOCache(size_t maxSize) : maxEntries(maxSize) {}

    bool lookup(size_t blockNum, vector<uint8_t>& data) {
        if (cache.count(blockNum)) {
            cache[blockNum].lastAccess = chrono::steady_clock::now();
            data = cache[blockNum].data;
            cout << "[Cache] HIT: " << blockNum << endl;
            return true;
        }
        cout << "[Cache] MISS: " << blockNum << endl;
        return false;
    }

    void insert(size_t blockNum, vector<uint8_t>& data) {
        if (cache.size() >= maxEntries) {
            auto oldest = cache.begin();
            for (auto it = cache.begin(); it != cache.end(); ++it) {
                if (it->second.lastAccess < oldest->second.lastAccess) oldest = it;
            }
            cache.erase(oldest);
        }
        cache[blockNum] = {data, chrono::steady_clock::now()};
    }
};

int main() {
    cout << "--- Kernel I/O Subsystem Simulation ---" << endl;
    
    // Test Scheduler
    PriorityScheduler prio;
    prio.addRequest(IORequest(1, IORequest::Type::READ, IORequest::Priority::LOW, 5, 512));
    prio.addRequest(IORequest(2, IORequest::Type::WRITE, IORequest::Priority::HIGH, 1, 512));
    prio.processAll();

    // Test Cache
    IOCache cache(2);
    vector<uint8_t> dummy(512, 1);
    vector<uint8_t> res;
    cache.insert(1, dummy);
    cache.insert(2, dummy);
    cache.lookup(1, res);
    cache.insert(3, dummy); // Evicts 2 (LRU)
    cache.lookup(2, res);   // Miss

    return 0;
}

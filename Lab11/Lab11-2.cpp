#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>

using namespace std;

// TODO 1: จำลอง Memory
class Memory {
private:
    vector<uint8_t> data;
    size_t size;

public:
    Memory(size_t size) : size(size), data(size, 0) {}

    void write(size_t address, uint8_t value) {
        if (address < size) {
            data[address] = value;
        }
    }

    uint8_t read(size_t address) {
        if (address < size) {
            return data[address];
        }
        return 0;
    }

    void display(size_t start, size_t length) {
        for (size_t i = start; i < start + length && i < size; i++) {
            cout << (int)data[i] << " ";
        }
        cout << endl;
    }
};

// TODO 2: จำลอง Programmed I/O
class ProgrammedIO {
private:
    int cpuCyclesUsed;

public:
    ProgrammedIO() : cpuCyclesUsed(0) {}

    long long transfer(Memory& mem, size_t destAddr, vector<uint8_t>& source, size_t size) {
        auto start = chrono::high_resolution_clock::now();
        cout << "[PIO] CPU transferring " << size << " bytes..." << endl;
        
        for (size_t i = 0; i < size; i++) {
            // TODO: จำลอง CPU copy ทีละ byte
            mem.write(destAddr + i, source[i]);
            cpuCyclesUsed++; // CPU ต้องจัดการทุก byte
        }

        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();
        cout << "[PIO] Complete. CPU cycles used: " << cpuCyclesUsed << endl;
        return duration;
    }
};

// TODO 3: จำลอง DMA Controller
class DMAController {
private:
    uint32_t sourceAddress;
    uint32_t destAddress;
    uint32_t transferCount;
    bool busy;
    int cpuCyclesUsed;

public:
    DMAController() : busy(false), cpuCyclesUsed(0) {}

    // TODO: setup()
    void setup(uint32_t src, uint32_t dest, uint32_t count) {
        sourceAddress = src;
        destAddress = dest;
        transferCount = count;
    }

    // TODO: transfer()
    long long transfer(Memory& mem, vector<uint8_t>& source) {
        auto start = chrono::high_resolution_clock::now();
        busy = true;
        cout << "[DMA] Starting transfer of " << transferCount << " bytes..." << endl;
        cout << "[DMA] CPU is FREE to do other work!" << endl;

        // DMA ใช้ CPU cycles แค่ตอน setup และ interrupt เท่านั้น
        cpuCyclesUsed = 2; 

        for (uint32_t i = 0; i < transferCount; i++) {
            mem.write(destAddress + i, source[sourceAddress + i]);
        }

        busy = false;
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();
        
        cout << "[DMA] Transfer complete! CPU cycles used: " << cpuCyclesUsed << endl;
        cout << "[DMA] CPU interrupt sent to notify completion" << endl;
        return duration;
    }
};

int main() {
    cout << "DMA vs Programmed I/O Comparison" << endl;
    const size_t DATA_SIZE = 1024; // 1KB
    Memory memory1(4096);
    vector<uint8_t> sourceData(DATA_SIZE);

    for (size_t i = 0; i < DATA_SIZE; i++) {
        sourceData[i] = static_cast<uint8_t>(i % 256);
    }

    // TODO: ทดสอบ PIO
    cout << "\n--- Programmed I/O ---" << endl;
    ProgrammedIO pio;
    long long pioTime = pio.transfer(memory1, 0, sourceData, DATA_SIZE);

    // TODO: ทดสอบ DMA
    cout << "\n--- DMA Transfer ---" << endl;
    Memory memory2(4096);
    DMAController dma;
    dma.setup(0, 0, DATA_SIZE);
    long long dmaTime = dma.transfer(memory2, sourceData);

    // TODO: เปรียบเทียบผล
    cout << "\n--- Performance Comparison ---" << endl;
    cout << "PIO Time: " << pioTime << " microseconds" << endl;
    cout << "DMA Time: " << dmaTime << " microseconds" << endl;

    return 0;
}

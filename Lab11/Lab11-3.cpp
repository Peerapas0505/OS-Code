#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <queue>
#include <stdexcept>

using namespace std;

// TODO 1: Base class IODevice
class IODevice {
protected:
    string deviceName;
    bool isOpen;

public:
    IODevice(string name) : deviceName(name), isOpen(false) {}
    virtual ~IODevice() {}

    virtual bool open() = 0;
    virtual void close() = 0;
    virtual string getDeviceType() = 0;
    virtual void displayInfo() = 0;

    string getName() { return deviceName; }
    bool getStatus() { return isOpen; }
};

// TODO 2: Block Device
class BlockDevice : public IODevice {
private:
    size_t blockSize;
    size_t totalBlocks;
    vector<vector<uint8_t>> blocks;

public:
    BlockDevice(string name, size_t blockSz, size_t totalBlk)
        : IODevice(name), blockSize(blockSz), totalBlocks(totalBlk) {
        blocks.resize(totalBlocks, vector<uint8_t>(blockSize, 0));
    }

    bool open() override {
        isOpen = true;
        cout << "[Block Device] " << deviceName << " opened" << endl;
        return true;
    }

    void close() override {
        isOpen = false;
        cout << "[Block Device] " << deviceName << " closed" << endl;
    }

    // TODO 3: readBlock
    vector<uint8_t> readBlock(size_t blockNum) {
        if (!isOpen) throw runtime_error("Device not open");
        if (blockNum >= totalBlocks) throw out_of_range("Block number out of range");
        return blocks[blockNum];
    }

    // TODO 4: writeBlock
    void writeBlock(size_t blockNum, vector<uint8_t>& data) {
        if (!isOpen) throw runtime_error("Device not open");
        if (blockNum >= totalBlocks) throw out_of_range("Block number out of range");
        if (data.size() != blockSize) throw invalid_argument("Data size mismatch");
        blocks[blockNum] = data;
        cout << "[Block Device] Block " << blockNum << " written" << endl;
    }

    string getDeviceType() override { return "Block Device"; }
    void displayInfo() override {
        cout << "Device: " << deviceName << "\nType: " << getDeviceType() 
             << "\nCapacity: " << (blockSize * totalBlocks) << " bytes" << endl;
    }
};

// TODO 5: Character Device
class CharacterDevice : public IODevice {
private:
    queue<char> inputBuffer;
    string outputBuffer;

public:
    CharacterDevice(string name) : IODevice(name) {}

    bool open() override {
        isOpen = true;
        cout << "[Char Device] " << deviceName << " opened" << endl;
        return true;
    }

    void close() override {
        isOpen = false;
        cout << "[Char Device] " << deviceName << " closed" << endl;
    }

    // TODO 6: getChar
    char getChar() {
        if (!isOpen) throw runtime_error("Device not open");
        if (inputBuffer.empty()) return '\0';
        char c = inputBuffer.front();
        inputBuffer.pop();
        return c;
    }

    // TODO 7: putChar
    void putChar(char c) {
        if (!isOpen) throw runtime_error("Device not open");
        outputBuffer += c;
        cout << "[Char Device] Output: " << c << endl;
    }

    void simulateInput(string input) {
        for (char c : input) inputBuffer.push(c);
    }

    string getDeviceType() override { return "Character Device"; }
    void displayInfo() override {
        cout << "Device: " << deviceName << "\nType: " << getDeviceType() << endl;
    }
};

// TODO 8: Network Device
class NetworkDevice : public IODevice {
private:
    string ipAddress;
    int port;
    bool connected;
    vector<string> receivedPackets;

public:
    NetworkDevice(string name, string ip, int p)
        : IODevice(name), ipAddress(ip), port(p), connected(false) {}

    bool open() override { isOpen = true; return true; }
    void close() override { isOpen = false; connected = false; }

    // TODO 9: Network Ops
    bool connect() {
        if (!isOpen) return false;
        connected = true;
        cout << "[Network] Connected to " << ipAddress << ":" << port << endl;
        return true;
    }

    void sendPacket(string data) {
        if (!connected) throw runtime_error("Not connected");
        cout << "[Network] Sending: " << data << endl;
    }

    string getDeviceType() override { return "Network Device"; }
    void displayInfo() override {
        cout << "Device: " << deviceName << "\nIP: " << ipAddress << endl;
    }
};

// TODO 10: Device Manager
class DeviceManager {
private:
    map<string, shared_ptr<IODevice>> deviceTable;

public:
    void registerDevice(shared_ptr<IODevice> device) {
        deviceTable[device->getName()] = device;
        cout << "[DevMgr] Registered: " << device->getName() << endl;
    }

    shared_ptr<IODevice> getDevice(string name) {
        if (deviceTable.find(name) == deviceTable.end())
            throw runtime_error("Device not found");
        return deviceTable[name];
    }

    void listAllDevices() {
        cout << "\n--- Registered Devices ---" << endl;
        for (auto const& [name, dev] : deviceTable) {
            cout << "- " << name << " [" << dev->getDeviceType() << "]" << endl;
        }
    }

    void openDevice(string name) { getDevice(name)->open(); }
};

int main() {
    cout << "--- Application I/O Interface Demo ---" << endl;
    DeviceManager devMgr;

    auto disk = make_shared<BlockDevice>("sda", 512, 100);
    auto keyboard = make_shared<CharacterDevice>("keyboard");
    auto ethernet = make_shared<NetworkDevice>("eth0", "192.168.1.1", 8080);

    devMgr.registerDevice(disk);
    devMgr.registerDevice(keyboard);
    devMgr.registerDevice(ethernet);

    // ทดสอบ Block Device
    devMgr.openDevice("sda");
    vector<uint8_t> testData(512, 0xAB);
    disk->writeBlock(0, testData);

    // ทดสอบ Character Device
    devMgr.openDevice("keyboard");
    keyboard->simulateInput("Hello OS!");
    cout << "Read char: " << keyboard->getChar() << endl;

    return 0;
}

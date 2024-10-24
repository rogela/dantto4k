#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

uint16_t swapEndian16(uint16_t num);
uint32_t swapEndian32(uint32_t num);
uint64_t swapEndian64(uint64_t num);

class StreamBase {
public:
    virtual ~StreamBase() {};

    virtual bool isEOF() const { return true; }

    virtual size_t leftBytes() { return 0; }

    virtual void skip(uint64_t pos) { }

    virtual size_t read(void* buffer, size_t size) { return 0; }

    virtual size_t peek(void* buffer, size_t size) { return 0; }

    uint8_t get8U() {
        uint8_t buffer;
        read((char*)&buffer, sizeof(buffer));
        return buffer;
    }

    uint16_t getBe16U() {
        uint16_t buffer;
        read((char*)&buffer, sizeof(buffer));
        return swapEndian16(buffer);
    }

    uint32_t getBe32U() {
        uint32_t buffer;
        read((char*)&buffer, sizeof(buffer));
        return swapEndian32(buffer);
    }

    uint64_t getBe64U() {
        uint64_t buffer;
        read((char*)&buffer, sizeof(buffer));
        return swapEndian64(buffer);
    }

    uint8_t peek8U() {
        uint8_t buffer;
        peek((char*)&buffer, sizeof(buffer));
        return buffer;
    }

    uint16_t peekBe16U() {
        uint16_t buffer;
        peek((char*)&buffer, sizeof(buffer));
        return swapEndian16(buffer);
    }

    uint32_t peekBe32U() {
        uint32_t buffer;
        peek((char*)&buffer, sizeof(buffer));
        return swapEndian32(buffer);
    }

    uint64_t peekBe64U() {
        uint64_t buffer;
        peek((char*)&buffer, sizeof(buffer));
        return swapEndian64(buffer);
    }

};
class Stream : public StreamBase {
public:
    Stream(const std::vector<uint8_t>& data);
    Stream(const std::vector<uint8_t>& data, uint32_t size);
    Stream(Stream& stream, uint32_t size);
    Stream(Stream& stream);

    virtual ~Stream() {};

    virtual bool isEOF() const {
        return size == cur;
    }

    virtual size_t leftBytes() {
        return size - cur;
    }

    virtual void skip(uint64_t pos) {
        if (size < cur + pos) {
            throw std::out_of_range("");
        }
        cur += pos;
    }

    virtual size_t read(void* buffer, size_t size) {
        size_t ret = peek(buffer, size);
        cur += size;
        return size;
    }

    virtual size_t peek(void* buffer, size_t size) {
        if (this->size < cur + size) {
            throw std::out_of_range("");
        }

        memcpy(buffer, data.data() + cur, size);
        return size;
    }

    uint8_t get8U() {
        uint8_t buffer;
        read((char*)&buffer, sizeof(buffer));
        return buffer;
    }

    uint16_t getBe16U() {
        uint16_t buffer;
        read((char*)&buffer, sizeof(buffer));
        return swapEndian16(buffer);
    }

    uint32_t getBe32U() {
        uint32_t buffer;
        read((char*)&buffer, sizeof(buffer));
        return swapEndian32(buffer);
    }

    uint64_t getBe64U() {
        uint64_t buffer;
        read((char*)&buffer, sizeof(buffer));
        return swapEndian64(buffer);
    }

    uint8_t peek8U() {
        uint8_t buffer;
        peek((char*)&buffer, sizeof(buffer));
        return buffer;
    }

    uint16_t peekBe16U() {
        uint16_t buffer;
        peek((char*)&buffer, sizeof(buffer));
        return swapEndian16(buffer);
    }

    uint32_t peekBe32U() {
        uint32_t buffer;
        peek((char*)&buffer, sizeof(buffer));
        return swapEndian32(buffer);
    }

    uint64_t peekBe64U() {
        uint64_t buffer;
        peek((char*)&buffer, sizeof(buffer));
        return swapEndian64(buffer);
    }

    uint64_t cur = 0;

protected:
    const std::vector<uint8_t>& data;
    bool hasSize = false;
    uint64_t size = 0;
};

class FileStream : public StreamBase {
public:
    FileStream(const std::string& filename) {
        fsIn.open(filename, std::ios::in | std::ios::binary);
        if (!fsIn.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
    }

    ~FileStream() override {
        if (fsIn.is_open()) {
            fsIn.close();
        }
    }

    virtual size_t leftBytes() {
        std::streampos current = fsIn.tellg();
        fsIn.seekg(0, std::ios::end);
        std::streampos end = fsIn.tellg();
        fsIn.seekg(current, std::ios::beg);

        return end - current;
    }

    virtual bool isEOF() const {
        return fsIn.eof();
    }

    void skip(uint64_t pos) override {
        fsIn.seekg(pos, std::ios::cur);

        if (fsIn.fail()) {
            throw std::out_of_range("Failed to seek in file.");
        }
    }

    size_t read(void* buffer, size_t size) override {
        fsIn.read((char*)buffer, size);
        if (fsIn.fail() && !fsIn.eof()) {
            throw std::out_of_range("Failed to read from file.");
        }
        return fsIn.gcount();
    }

    size_t peek(void* buffer, size_t size) override {
        std::streampos currentPos = fsIn.tellg();
        fsIn.read((char*)buffer, size);

        if (fsIn.fail() && !fsIn.eof()) {
            throw std::out_of_range("Failed to read from file.");
        }
        fsIn.seekg(currentPos);
        return fsIn.gcount();
    }

private:
    std::ifstream fsIn;
    bool writeMode = false;
};
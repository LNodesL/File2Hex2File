#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

// Function to convert a hexadecimal string to a byte array
std::vector<unsigned char> hexStringToBytes(const std::string& hex) {
    std::vector<unsigned char> bytes;

    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16));
        bytes.push_back(byte);
    }

    return bytes;
}

// Function to save the byte array to a file in binary format
void saveBytesToFile(const std::vector<unsigned char>& bytes, const std::string& fileName) {
    std::ofstream outputFile(fileName, std::ios::binary);

    if (!outputFile.is_open()) {
        std::cerr << "Failed to open " << fileName << std::endl;
        return;
    }

    for (auto byte : bytes) {
        outputFile.put(static_cast<char>(byte));
    }

    std::cout << "Binary file created successfully: " << fileName << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <inputHexFile> <outputBinaryFile>" << std::endl;
        return 1;
    }

    std::ifstream hexFile(argv[1]);
    if (!hexFile.is_open()) {
        std::cerr << "Failed to open " << argv[1] << std::endl;
        return 1;
    }

    // Read the entire hexadecimal string from the file
    std::stringstream hexStream;
    hexStream << hexFile.rdbuf();
    std::string hexString = hexStream.str();

    // Convert the hexadecimal string to bytes
    std::vector<unsigned char> bytes = hexStringToBytes(hexString);

    // Save the bytes to the specified output binary file
    saveBytesToFile(bytes, argv[2]);

    return 0;
}

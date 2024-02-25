#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>

// Function to convert the byte array to a hexadecimal string
std::string toHexString(const std::vector<unsigned char>& data) {
    std::stringstream hexStream;
    hexStream << std::hex << std::setfill('0');
    for (auto byte : data) {
        hexStream << std::setw(2) << static_cast<int>(byte);
    }
    return hexStream.str();
}

// Function to save the hexadecimal string to a file
void saveHexToFile(const std::string& hexString, const std::string& fileName) {
    std::ofstream outputFile(fileName);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open " << fileName << std::endl;
        return;
    }

    outputFile << hexString;
    std::cout << "Hexadecimal string saved successfully: " << fileName << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.original> <outputFile>" << std::endl;
        return 1;
    }

    std::ifstream originalFile(argv[1], std::ios::binary);
    if (!originalFile.is_open()) {
        std::cerr << "Failed to open " << argv[1] << std::endl;
        return 1;
    }

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(originalFile), {});

    // Convert the buffer to a hexadecimal string
    std::string hexString = toHexString(buffer);

    // Save the hexadecimal string to the specified output file
    saveHexToFile(hexString, argv[2]);

    return 0;
}

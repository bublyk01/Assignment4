#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <stdexcept>
#include <vector>

class CaesarCipher {
public:
    CaesarCipher(const char* dllPath);
    ~CaesarCipher();

    char* encrypt(char* rawText, int key);
    char* decrypt(char* encryptedText, int key);

private:
    HMODULE hDll;
    typedef char* (*EncryptFunc)(char*, int);
    typedef char* (*DecryptFunc)(char*, int);

    EncryptFunc encryptFunc;
    DecryptFunc decryptFunc;
};

CaesarCipher::CaesarCipher(const char* dllPath) {
    hDll = LoadLibraryA(dllPath);
    if (!hDll) {
        throw std::runtime_error("Could not load the DLL");
    }

    encryptFunc = (EncryptFunc)GetProcAddress(hDll, "encrypt");
    decryptFunc = (DecryptFunc)GetProcAddress(hDll, "decrypt");

    if (!encryptFunc || !decryptFunc) {
        FreeLibrary(hDll);
        throw std::runtime_error("Could not find the function");
    }
}

CaesarCipher::~CaesarCipher() {
    if (hDll) {
        FreeLibrary(hDll);
    }
}

char* CaesarCipher::encrypt(char* rawText, int key) {
    if (encryptFunc) {
        return encryptFunc(rawText, key);
    }
    return nullptr;
}

char* CaesarCipher::decrypt(char* encryptedText, int key) {
    if (decryptFunc) {
        return decryptFunc(encryptedText, key);
    }
    return nullptr;
}

class CommandLineInterface {
public:
    void run();

private:
    void encryptFile(const std::string& inputPath, const std::string& outputPath, int key);
    void decryptFile(const std::string& inputPath, const std::string& outputPath, int key);
};

void CommandLineInterface::run() {
    std::string operation;
    std::cout << "Do you want to encrypt or decrypt text? ";
    std::cin >> operation;

    std::string inputPath;
    std::cout << "Enter the path to the file you want to process: ";
    std::cin >> inputPath;

    std::string outputPath;
    std::cout << "Enter the path to save the processed file: ";
    std::cin >> outputPath;

    int key;
    std::cout << "Enter the key: ";
    std::cin >> key;

    try {
        if (operation == "encrypt") {
            encryptFile(inputPath, outputPath, key);
        }
        else if (operation == "decrypt") {
            decryptFile(inputPath, outputPath, key);
        }
        else {
            std::cerr << "Invalid operation" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void CommandLineInterface::encryptFile(const std::string& inputPath, const std::string& outputPath, int key) {
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Could not open the input file");
    }

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Could not open the output file");
    }

    const size_t chunkSize = 16;
    char buffer[chunkSize];
    CaesarCipher cipher("DLL1.dll");

    while (inFile.read(buffer, chunkSize) || inFile.gcount() > 0) {
        std::vector<char> chunkBuffer(buffer, buffer + inFile.gcount());
        chunkBuffer.push_back('\0');

        char* encryptedChunk = cipher.encrypt(chunkBuffer.data(), key);
        if (encryptedChunk) {
            outFile.write(encryptedChunk, strlen(encryptedChunk));
            delete[] encryptedChunk;
        }
        else {
            throw std::runtime_error("Could not encrypt the file");
        }
    }

    inFile.close();
    outFile.close();
}

void CommandLineInterface::decryptFile(const std::string& inputPath, const std::string& outputPath, int key) {
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Could not open the input file");
    }

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Could not open the output file");
    }

    const size_t chunkSize = 16;
    char buffer[chunkSize];
    CaesarCipher cipher("DLL1.dll");

    while (inFile.read(buffer, chunkSize) || inFile.gcount() > 0) {
        std::vector<char> chunkBuffer(buffer, buffer + inFile.gcount());
        chunkBuffer.push_back('\0');

        char* decryptedChunk = cipher.decrypt(chunkBuffer.data(), key);
        if (decryptedChunk) {
            outFile.write(decryptedChunk, strlen(decryptedChunk));
            delete[] decryptedChunk;
        }
        else {
            throw std::runtime_error("Could not decrypt the file");
        }
    }

    inFile.close();
    outFile.close();
}

int main() {
    CommandLineInterface cli;
    cli.run();
    return 0;
}

#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <stdexcept>
#include <vector>

class Text {
public:
    Text(const std::string& filePath);
    const std::string& getText() const;

private:
    std::string text;
};

Text::Text(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open the file");
    }

    std::string line;
    while (std::getline(file, line)) {
        text += line + "\n";
    }
    file.close();
}

const std::string& Text::getText() const {
    return text;
}

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
    std::cout << "Enter the path to the file you want to encrypt: ";
    std::cin >> inputPath;

    std::string outputPath;
    std::cout << "Enter the path to save the file: ";
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
            std::cerr << "Wrong operation" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void CommandLineInterface::encryptFile(const std::string& inputPath, const std::string& outputPath, int key) {
    Text text(inputPath);
    std::string rawText = text.getText();

    std::vector<char> rawTextCStr(rawText.size() + 1);
    strcpy_s(rawTextCStr.data(), rawTextCStr.size(), rawText.c_str());

    CaesarCipher cipher("DLL1.dll");
    char* encryptedText = cipher.encrypt(rawTextCStr.data(), key);

    std::ofstream outFile(outputPath);
    if (outFile.is_open()) {
        outFile << encryptedText;
        outFile.close();
    }
    else {
        std::cerr << "Could not open the file" << std::endl;
    }

    delete[] encryptedText;
}

void CommandLineInterface::decryptFile(const std::string& inputPath, const std::string& outputPath, int key) {
    Text text(inputPath);
    std::string encryptedText = text.getText();

    std::vector<char> encryptedTextCStr(encryptedText.size() + 1);
    strcpy_s(encryptedTextCStr.data(), encryptedTextCStr.size(), encryptedText.c_str());

    CaesarCipher cipher("DLL1.dll");
    char* decryptedText = cipher.decrypt(encryptedTextCStr.data(), key);

    std::ofstream outFile(outputPath);
    if (outFile.is_open()) {
        outFile << decryptedText;
        outFile.close();
    }
    else {
        std::cerr << "Failed to open output file" << std::endl;
    }

    delete[] decryptedText;
}

int main() {
    CommandLineInterface cli;
    cli.run();
    return 0;
}

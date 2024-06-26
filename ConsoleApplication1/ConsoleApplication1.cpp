#include <iostream>
#include <Windows.h>
#include <stdexcept>
#include <fstream>
#include <string>


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
        throw std::runtime_error("Failed to load DLL");
    }

    encryptFunc = (EncryptFunc)GetProcAddress(hDll, "encrypt");
    decryptFunc = (DecryptFunc)GetProcAddress(hDll, "decrypt");

    if (!encryptFunc || !decryptFunc) {
        FreeLibrary(hDll);
        throw std::runtime_error("Failed to get function address");
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

int main() {
    try {
        CaesarCipher cipher("DLL1.dll");

        char rawText[] = "Hello, World!";
        int key = 1;

        char* encryptedText = cipher.encrypt(rawText, key);
        std::cout << "Encrypted text: " << encryptedText << std::endl;

        char* decryptedText = cipher.decrypt(encryptedText, key);
        std::cout << "Decrypted text: " << decryptedText << std::endl;

        delete[] encryptedText;
        delete[] decryptedText;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

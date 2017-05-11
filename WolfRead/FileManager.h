#pragma once

#include <string>
#include <vector>

class FileManager {
private:
    //const unsigned long long CHUNKSIZE = 1024 * 1024 * 2; // 2mb
    //std::ifstream file;
    //unsigned long long filesize;
    std::streampos filesize;
    std::wstring filePath;
    std::string filename;
    bool succeed;
public:
    std::ifstream file;
    bool succeeded() { return succeed; }
    std::streampos getFilesize() { return filesize; }
    void openDialog();
    std::string getfilename() { return filename; }
    FileManager();
    ~FileManager();
};

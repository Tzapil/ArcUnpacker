#include <iostream>
#include <fstream>
#include <string>
#include <locale>

// =================FUNCTONS==============
std::string wstring2string(const std::wstring &source, std::locale const &loc = std::locale::classic()) {
    std::string result(source.size(), char());
    const std::ctype<char>& ct = std::use_facet<std::ctype<char> >(loc);
    std::string::iterator dest(result.begin());
    std::wstring::const_iterator it(source.begin());
    std::wstring::const_iterator end(source.end());
    for (; it != end; ++it, ++dest)
    *dest = ct.narrow(*it, '\0');
    return result;
}

// =================STRUCT=================
struct ARC_HEAD {
    unsigned long file_count;
    unsigned long head_length;
};

struct FILES_TABLE {
  unsigned long length;
  unsigned long offset;
};

// ================MAIN===================
int main(int argc, char** argv)
{
    if(argc != 2) {
        std::cout << "Arc depacker v0.01" << std::endl
                  << "Using: " << argv[0] << " <filename.arc>" << std::endl;
        return -1;
    }

    const char* name = argv[1];

    std::fstream input;
    input.open (name, std::ios::in | std::ios::binary);

    if (!input.is_open()) {
        std::cout << "Cant open file!" << std::endl;
        return -1;
    }


    ARC_HEAD head_inf;
    input.read((char*)&head_inf, sizeof(head_inf));

    char* head_buff = new char[head_inf.head_length];
    input.read(head_buff, head_inf.head_length);

    char* pointer = head_buff;
    for(unsigned i = 0; i < head_inf.file_count; ++i) {
        FILES_TABLE* entry = (FILES_TABLE*) pointer;
        pointer += sizeof(*entry);

        wchar_t* filename = (wchar_t*) pointer;
        std::wstring str = std::wstring(filename);
        std::wcout << str << std::endl;
        pointer += ((str.length() + 1) * 2);

        unsigned long  len  = (*entry).length;
        unsigned long  offset  = (*entry).offset + head_inf.head_length;
        char* file_buff = new char[len];
        input.seekg(offset);
        input.read(file_buff, len);

        std::fstream output;
        output.open(wstring2string(str).c_str(), std::ios::out | std::ios::binary);
        output.write(file_buff, len);
        output.close();

        input.seekg(offset);
    }

    input.close();

    return 0;
}


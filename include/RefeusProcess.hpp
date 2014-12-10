#pragma once
#include <string>
#include <map>
#include <vector>

class RefeusProcess {
    
    private: std::string executable;
    private: std::map<std::string,std::string> environmentmap;
    private: std::vector<std::string> parametersvector;

    private: void configureNewRefeusDocument();
    private: void configureOpenRefeusDocument(std::string pathname);
    private: void configureCloudSetting();
    private: void usage();
    
    public: RefeusProcess();
    public: bool argParser(char* cmd);
    public: void langCheck(int check);
    public: void setEnvironment(std::string env_name, std::string env_value);   
    public: std::vector<std::string> &split(const std::string &string_to_split, char delimiter_character, std::vector<std::string> &element_vector);
    public: bool start();    
};
#endif

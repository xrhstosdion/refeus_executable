#ifndef REFEUS_FUNCTIONS_H
#define REFEUS_FUNCTIONS_H
class RefeusProcess {
    
    private: std::string executable;
    private: std::map<std::string,std::string> environmentmap;
    private: std::vector<std::string> parametersvector;
    private: void newRefeusDocument();
    private: void openRefeusDocument(std::string pathname);
    private: void cloudSetting();
    private: void usage();
    
    public: void RefeusProcess::setEnvironment(std::string env_name,
        std::string env_value);   
    public: RefeusProcess();
    //Function &split, splits a string
    public: std::vector<std::string> &split(const std::string &s, char delim,
        std::vector<std::string> &elems) {
      std::stringstream ss(s);
      std::string item;
      while (std::getline(ss, item, delim)) {
        elems.push_back(item);
      }
      return elems;
    }  
    public: bool argParser(char* cmd);
    public: bool start();
    public: void langCheck(int check);
    
};
#endif

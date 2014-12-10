#include <windows.h>
#pragma comment(lib,"user32.lib")
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include "h/refeus_functions.h"
void RefeusProcess::newRefeusDocument() {
   environmentmap["open_refeus_database"] = "true";
}
void RefeusProcess::openRefeusDocument(std::string path_name) {
   environmentmap["refeus_database"] = path_name;
}
void RefeusProcess::cloudSetting() {
   environmentmap["CLOUD_ENABLED"] = "true";
}
void RefeusProcess::usage() {
   MessageBox(NULL, "--help displays all possible arguments\n"
       "--new opens a 'save as' dialog window\n"
       "--open \"C:/file_name\" starts refeus with specific file\n"
       "--plus sets the ini file to plus.ini\n"
       "--refeus sets the ini file to refeus.ini\n"
       "--cloud-enabled sets CLOUD_ENABLED to true"     
       , "Help!", MB_OK);
}
void RefeusProcess::setEnvironment(std::string env_name, std::string env_value) {
  environmentmap[env_name] = env_value;
}
 RefeusProcess::RefeusProcess() {
   parametersvector.push_back(" startup.ini");
}
/**
  * Function langCheck takes as parameter the Language ID of the system
  * Checks if it is German->French->Polish->English
  * Then sets the appropriate Envir-Variables to the language that got 
  * matched first
  */
void RefeusProcess::langCheck(int check) {
  switch ( check ){
    case 1031:  //German
      environmentmap["DSC_Language"] = "German";
      environmentmap["Language"] = "German";
      //putenv("DSC_Language=German");
      //putenv("Language=German");
    break;
    case 1036:  //French
      environmentmap["DSC_Language"] = "French";
      environmentmap["Language"] = "French";
      //putenv("DSC_Language=French");
      //putenv("Language=French");
    break;
    case 1045:  //Polish
      environmentmap["DSC_Language"] = "Polish";
      environmentmap["Language"] = "Polish";
      //putenv("DSC_Language=Polish");
      //putenv("Language=Polish");
    break;
   default:  //English
     environmentmap["DSC_Language"] = "English";
     environmentmap["Language"] = "English";
     //putenv("DSC_Language=English");
     //putenv("Language=English");
   break;
  }
}
/** Function argParser parses takes as parameter the command line then parses
  *  all the arguments that are separated from each other with a single blank
  *  --help outputs all possible arguments
  *  --new opens a 'save file as' dialog window
  *  --open "C:/works with/spaces too.txt" but path name must be inside " "
  *  --refeus sets to refeus.ini 
  *  --plus sets to plus.ini
  *  --cloud-enabled sets CLOUD_ENABLED to true
  */
bool RefeusProcess::argParser(char* cmd) {
  std::vector<std::string> per_blank_vector,per_quotes_vector;
  //splitting command line per single blanks
  split(cmd, ' ', per_blank_vector);
  std::vector<std::string>::iterator it;
  for ( it = per_blank_vector.begin(); it < per_blank_vector.end(); ++it) {
    if ( *it == "--help" )
      usage();
    if ( *it == "--new" )
      newRefeusDocument();
    else if ( *it == "--open" ){
      //splitting command line per double quotes
      split(cmd, '\"\"', per_quotes_vector);
      //per_quotes_vector.erase(per_quotes_vector.begin());      
      if ( per_quotes_vector.size() > 1 )
        openRefeusDocument(per_quotes_vector.at(1));
      else
        newRefeusDocument();
    }
    else if ( *it == "--plus" ){
      parametersvector.clear();
      parametersvector.push_back(" plus.ini");
    }
    else if ( *it == "--refeus" ){
      parametersvector.clear();
      parametersvector.push_back(" refeus.ini");
    }
    else if ( *it == "--cloud-enabled" ){
      cloudSetting();
    }
  }
  return 1;
}
/** Function start takes no arguments 
  *  sets all environment variables that are saved in environmenenmap
  *  prepares the full_executable path with the pre-configured ini file
  *  then creates the process with the appropriate env. variables and ini file
  */
bool RefeusProcess:: start() {
  std::string executable = "refeus.exe";
  std::string full_exe_str = executable;
  std::vector<std::string>::iterator it2;
  for ( it2 = parametersvector.begin(); it2 < parametersvector.end(); ++it2) {
    full_exe_str = full_exe_str + *it2;
  }
  std::map<std::string, std::string>::iterator it3;
  for ( it3 = environmentmap.begin(); it3 != environmentmap.end(); ++it3) {
    std::string env = it3->first + "=" + it3->second;
    char * newenv = new char[env.size() + 1];
    std::copy(env.begin(), env.end(), newenv);
    newenv[env.size()] = '\0';
    putenv(newenv);
  }
  STARTUPINFO StartupInfo;                        //This is an [in] parameter
  PROCESS_INFORMATION ProcessInfo; //This is what we get as an [out] parameter 
  ZeroMemory(&StartupInfo, sizeof(StartupInfo));
  ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
  StartupInfo.cb = sizeof(StartupInfo) ;            //Only compulsory field
  //Formatting exe to const char* and full_exe to char* for CreateProcess
  const char* exe = executable.c_str();
  char* full_exe = new char [full_exe_str.size() + 1];
  std::copy(full_exe_str.begin(), full_exe_str.end(), full_exe);
  full_exe[full_exe_str.size()] = '\0';
  bool start_refeus = CreateProcess( exe 
                                   , full_exe              //exe + ini names
                                   , 0
                                   , 0
                                   , FALSE
                                   , CREATE_DEFAULT_ERROR_MODE
                                   , 0
                                   , 0
                                   , &StartupInfo
                                   , &ProcessInfo);
    if ( start_refeus ){
    WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
    CloseHandle(ProcessInfo.hThread);
    CloseHandle(ProcessInfo.hProcess);
   }
   else
      MessageBox(NULL, "The process could not be started... \n"    
       , "Process Failed!", MB_OK);
   return 1;
}

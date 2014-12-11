#ifdef win32
#include <windows.h>
#pragma comment(lib,"user32.lib")
#endif
#ifdef unix
#include <iostream>
#include <stdlib.h> /* putenv */
#include <string.h> /* strdup */
#include <unistd.h> /* execvp */
#include <errno.h> /* errno */
#endif
#include <sstream>
#include <RefeusProcess.hpp>

/** constructor
 */
RefeusProcess::RefeusProcess() {
  parametersvector.push_back("startup.ini");
  #ifdef win32
  executable = "refeus.exe";
  #endif
  #ifdef unix
  executable = "refeus.sh";
  #endif
}
/** Argument Parser parses takes as parameter the command line then parses
  *  all the arguments that are separated from each other with a single blank
  *  --help outputs all possible arguments
  *  --new opens a 'save file as' dialog window
  *  --open "C:/works with/spaces too.txt" but path name must be inside " "
  *  --refeus sets to refeus.ini 
  *  --plus sets to plus.ini
  *  --cloud-enabled sets CLOUD_ENABLED to true
  */
bool RefeusProcess::argParser(std::string command_line) {
  std::vector<std::string> per_blank_vector,per_quotes_vector;
  //splitting command line per single blanks
  split(command_line, ' ', per_blank_vector);
  std::vector<std::string>::iterator it;
  for ( it = per_blank_vector.begin()
      ; it < per_blank_vector.end()
      ; ++it
      ) {
    if ( *it == "--help"
       ||*it == "/?"
       ) {
      usage();
      return false;
    }
    if ( *it == "--new" ) {
      configureNewRefeusDocument();
    } else if ( *it == "--open" ) {
      //splitting command line per double quotes
      // THIS is invalid: split takes character as parameter, you give string
      split(command_line, '\"\"', per_quotes_vector);
      //per_quotes_vector.erase(per_quotes_vector.begin());      
      if ( per_quotes_vector.size() > 1 ) {
        //TODO: use refeus_database_autostart
        configureOpenRefeusDocument(per_quotes_vector.at(1));
      } else {
        configureNewRefeusDocument();
      }
    } else if ( *it == "--plus" ) {
      parametersvector.clear();
      parametersvector.push_back("plus.ini");
    } else if ( *it == "--refeus" ) {
      parametersvector.clear();
      parametersvector.push_back("refeus.ini");
    } else if ( *it == "--cloud-enabled" ) {
      configureCloudSetting();
    } else if ( *it == "--debug" ) {
      configureDebug();
    } else if ( *it == "--language" ) {
      //TODO: check for next prameter 'de' 'en' etc and override langcheck
      configureDebug();
    } else if ( *it == "--auto-backup" ) {
      //TODO: set AUTO_BACKUP=YES
    } else if ( *it == "--no-auto-backup" ) {
      //TODO: set AUTO_BACKUP=NO
    } else if ( *it == "--skip-maintenance" ) {
      //TODO: set SKIP_MAINTENANCE=YES
    } else if ( *it == "--startup-activity" ) {
      //TODO: set STARTUP_ACTIVITY=next parameter (eg manage::overview)
    }
    /**
     * REFEUS_SETTINGS_LOCATION=[when set: ini-file for portable]
     * REFEUS_DOCUMENTS_LOCATION=path/to/documents
     * REFEUS_PICTURES_LOCATION=path/to/pictures
     */
  }
  return true;
}
void RefeusProcess::configureCloudSetting() {
   environmentmap["CLOUD_ENABLED"] = "true";
}
void RefeusProcess::configureDebug() {
  environmentmap["WKE_DEBUG"] = "YES";
  environmentmap["WKE_DEBUG_CONSOLE"] = "YES";
}
void RefeusProcess::configureNewRefeusDocument() {
   environmentmap["open_refeus_database"] = "true";
}
void RefeusProcess::configureOpenRefeusDocument(std::string path_name) {
   environmentmap["refeus_database"] = path_name;
}
/**
  * Function langCheck takes as parameter the Language ID of the system
  * Checks if it is German->French->Polish->English
  * Then sets the appropriate Envir-Variables to the language that got 
  * matched first
  * \param api_language_code - windows-api language code
  */
void RefeusProcess::langCheck(int api_language_code) {
  switch ( api_language_code ) {
    case 1031:  //German
      environmentmap["DSC_Language"] = "German";
      environmentmap["Language"] = "German";
      break;
    case 1036:  //French
      environmentmap["DSC_Language"] = "French";
      environmentmap["Language"] = "French";
      break;
    case 1045:  //Polish
      environmentmap["DSC_Language"] = "Polish";
      environmentmap["Language"] = "Polish";
      break;
    default:  //English
      environmentmap["DSC_Language"] = "English";
      environmentmap["Language"] = "English";
      break;
  }
}
void RefeusProcess::setEnvironment(std::string env_name, std::string env_value) {
  environmentmap[env_name] = env_value;
}
/** \brief Split String into Vector using a delimter
  * \param string_to_split - some string to be split by a specific character
  * \param delimiter_character - character for splitting
  * \param element_vector - reference vector for storing the split result
  */
std::vector<std::string> &RefeusProcess::split(const std::string &string_to_split, char delimiter_character, std::vector<std::string> &element_vector) {
  std::stringstream sstream(string_to_split);
  std::string item;
  while (std::getline(sstream, item, delimiter_character)) {
    element_vector.push_back(item);
  }
  return element_vector;
}  
/** Function start takes no arguments 
  *  sets all environment variables that are saved in environmenenmap
  *  prepares the full_executable path with the pre-configured ini file
  *  then creates the process with the appropriate env. variables and ini file
  */
int RefeusProcess:: start() {
  std::string executable_with_parameter = executable;
  std::vector<std::string>::iterator parameter_iterator;
  std::map<std::string, std::string>::iterator map_iterator;

  #ifdef win32
  STARTUPINFO StartupInfo;                        //This is an [in] parameter
  PROCESS_INFORMATION ProcessInfo; //This is what we get as an [out] parameter 
  #endif
  for ( map_iterator = environmentmap.begin(); map_iterator != environmentmap.end(); ++map_iterator) {
    std::string env = map_iterator->first + "=" + map_iterator->second;
    char * env_cstr = new char[env.size() + 1];
    std::copy(env.begin(), env.end(), env_cstr);
    env_cstr[env.size()] = '\0';
    putenv(env_cstr);
  }

  #ifdef win32
  /**
   * win32 implementation only
   */
  for ( parameter_iterator = parametersvector.begin()
      ; parameter_iterator < parametersvector.end()
      ; ++parameter_iterator
      ) {
    executable_with_parameter = executable_with_parameter + " " + *parameter_iterator;
  }
  ZeroMemory(&StartupInfo, sizeof(StartupInfo));
  ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
  StartupInfo.cb = sizeof(StartupInfo) ;            //Only compulsory field
  //Formatting exe to const char* and full_exe to char* for CreateProcess
  
  char* executable_with_parameter_cstr = new char [executable_with_parameter.size() + 1];
  const char* executable_cstr = strdup(executable.c_str());
  std::copy(executable_with_parameter.begin(), executable_with_parameter.end(), executable_with_parameter_cstr);
  executable_with_parameter_cstr[executable_with_parameter.size()] = '\0';
  bool process_started = CreateProcess( executable_cstr 
                                   , executable_with_parameter_cstr              //exe + ini names
                                   , 0
                                   , 0
                                   , FALSE
                                   , CREATE_DEFAULT_ERROR_MODE
                                   , 0
                                   , 0
                                   , &StartupInfo
                                   , &ProcessInfo);
    if ( process_started ){
    WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
    CloseHandle(ProcessInfo.hThread);
    CloseHandle(ProcessInfo.hProcess);
  } else {
     MessageBox(NULL, "The process could not be started... \n" , "Process Failed!", MB_OK);
     return 1;
  }
  #endif
  #ifdef unix
  /**
   * unix implementation only
   */
  char **exec_argv = static_cast<char**>(malloc(sizeof(char*) * (parametersvector.size() + 1) ));
  int parameter_iterator_index = 0;
  for ( parameter_iterator = parametersvector.begin()
      ; parameter_iterator < parametersvector.end()
      ; ++parameter_iterator
      ) {
    exec_argv[parameter_iterator_index] = strdup((*parameter_iterator).c_str());
    parameter_iterator_index++;
  }
  exec_argv[parameter_iterator_index] = NULL;
  if ( execvp(executable.c_str(),exec_argv) == -1 ){
    std::cerr << strerror(errno);
  }
  #endif
  return 0;
}
void RefeusProcess::usage() {
  const char* help_string = "--help displays all possible arguments\n"
       "--new opens a 'save as' dialog window\n"
       "--open \"C:/file_name\" starts refeus with specific file\n"
       "--plus sets the ini file to plus.ini\n"
       "--refeus sets the ini file to refeus.ini\n"
       "--debug sets the application to debug-mode (allows shift+ctrl+i)\n"
       "--cloud-enabled sets CLOUD_ENABLED to true\n" 
       "TODO: add more parameters"
       ;    
  #ifdef win32
  MessageBox(NULL, help_string, "Help!", MB_OK);
  #endif
  #ifdef unix
  std::cout << help_string << std::endl;
  #endif
}

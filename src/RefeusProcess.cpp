#ifdef _WIN32
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
#include <iostream>
#include <sstream>
#include <RefeusProcess.h>
#include <config.h>

/** constructor
 */
RefeusProcess::RefeusProcess()
: environmentmap()
#ifdef _WIN32
, executable("refeus.exe")
#else
, executable("refeus.sh")
#endif
, parametersvector()
, debug(false) {
  parametersvector.push_back("startup.ini");
  configureAutoBackup(true);
}
/**
 * get the next argument from a vector that is split by blanks
 * @return string
 * @param arguments_separated_by_blank (unmodifyable vector, passed by reference for performance)
 * @param start_iterator reference to move iterator for the argParser
 * --last "noblank"
 * --last "with blank"
 * --last "with many blanks"
 * --last " "
 * --last " beforeblank"
 * --last "afterblank "
 * --last " centeredinblanks "
 * --last " escaped\"quote " (no working!)
 * --last " escaped \"quote with\" blank " (no working!)
 */
std::string RefeusProcess::argParserNext(const std::vector<std::string> &arguments_separated_by_blank, std::vector<std::string>::iterator &start_iterator){
  std::vector<std::string>::iterator it;
  std::string next_arg = "";
  if ( start_iterator == arguments_separated_by_blank.end() ){
    return next_arg;
  }
  bool in_quotes = false;
  next_arg = "";
  for ( it = start_iterator
      ; it != arguments_separated_by_blank.end()
      ; it++
      ){
    if ( (*it).size() == 0 ) {
      // empty string no next arg
      break;
    }

    if ( (*it).at(0) == '\"'
      && (*it).at((*it).size()-1) != '\"'
      && (*it).size() > 1
      ){
      if ( debug ) { std::cout << "begin quotes" << std::endl << std::flush; }
      next_arg = (*it).substr(1,(*it).size()-1);
      in_quotes = true;
      continue;
    }
    if ( (*it).at(0) == '\"'
      && (*it).at((*it).size()-1) == '\"'
      && (*it).size() > 1
      ){
      if ( debug ) { std::cout << "full-quoted" << std::endl << std::flush; }
      next_arg = (*it).substr(1,(*it).size()-2);
      break;
    }

    if ( in_quotes && (*it).at((*it).size()-1) == '\"' ){
      if ( debug ) { std::cout << "end quotes" << std::endl << std::flush; }
      next_arg+= " " + (*it).substr(0,(*it).size()-1);
      break;
    }

    if ( in_quotes ){
      next_arg+= " " + *it;
      continue;
    }
    next_arg+= *it;
    break;
  }
  start_iterator = ++it;
  // if ( debug ) { std::cout << "next_arg: ret:[" << next_arg << "]" << std::endl << std::flush; }
  return next_arg;
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
  std::vector<std::string> per_blank_vector;
  std::vector<std::string>::iterator it;
  //splitting command line per single blanks
  split(command_line, ' ', per_blank_vector);
  for ( it = per_blank_vector.begin()
      ; it < per_blank_vector.end()
      ; ++it
      ) {
    if ( debug ) std::cout << "param: [" << *it << "]" << std::endl << std::flush;
    if ( *it == "--help"
       ||*it == "/?"
       ) {
      usage();
      return false;
    }
    if ( *it == "--new" ) {
      configureNewRefeusDocument();
    } else if ( *it == "--open" ) {
      ++it; // scroll to next (careful, processing in for-loop)
      std::string document_path = argParserNext(per_blank_vector, it);
      if ( document_path != "" ) {
        configureOpenRefeusDocument(document_path);
      } else {
        configureNewRefeusDocument();
      }
      if ( it == per_blank_vector.end() ){
        break;
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
      ++it; // scroll to next (careful, processing in for-loop)
      std::string iso_language = argParserNext(per_blank_vector, it);
      configureLanguageFromIsoString(iso_language);
      if ( it == per_blank_vector.end() ){
        break;
      }
    } else if ( *it == "--auto-backup" ) {
      configureAutoBackup(true);
    } else if ( *it == "--no-auto-backup" ) {
      configureAutoBackup(false);
    } else if ( *it == "--skip-maintenance" ) {
      configureSkipMaintenance(true);
    } else if ( *it == "--no-skip-maintenance" ) {
      configureSkipMaintenance(false);
    } else if ( *it == "--startup-activity" ) {
      ++it; // scroll to next (careful, processing in for-loop)
      std::string startup_activity = argParserNext(per_blank_vector, it);
      configureStartupActivity(startup_activity);
      if ( it == per_blank_vector.end() ){
        break;
      }
    } else if ( *it == "--infopool" ) {
      configureInfopool();
    } else {
      #ifndef _WIN32
      /* does not work in win32 because $0 is always part of the commandline and cannot be correctly predicted */
      usage();
      return false;
      #endif
    }
    /**
     * REFEUS_SETTINGS_LOCATION=[when set: ini-file for portable]
     * REFEUS_DOCUMENTS_LOCATION=path/to/documents
     * REFEUS_PICTURES_LOCATION=path/to/pictures
     */
  }
  return true;
}

/**
 * configure to enable auto-backup
 */
void RefeusProcess::configureAutoBackup(bool enabled){
  if ( enabled ){
    environmentmap["AUTO_BACKUP"] = "YES";
  } else {
    environmentmap["AUTO_BACKUP"] = "NO";
  }
}

/**
 * configure to enable certain features that are only available
 * with the infopool2 + webdav service that allows syncronizing and
 * storing document databases on remote systems
 */
void RefeusProcess::configureCloudSetting() {
   environmentmap["CLOUD_ENABLED"] = "true";
}

/**
 * configure debug mode
 */
void RefeusProcess::configureDebug() {
  #ifdef _WIN32
  executable = "code.exe"; /** start a commandline window to output debug messages*/
  #endif
  environmentmap["WKE_DEBUG"] = "YES";
  environmentmap["WKE_DEBUG_CONSOLE"] = "YES";
  debug = true;
}

/**
 * show a save-as dialog instead of the startup dialog
 * the given filename will be created and used as the docdb
 */
void RefeusProcess::configureNewRefeusDocument() {
   environmentmap["open_refeus_database"] = "true";
}

/**
 * run the infopool nodejs server
 * TODO: taskbar icon to close the application
 */
void RefeusProcess::configureInfopool() {
   environmentmap.clear();
   parametersvector.clear();
   #ifdef _WIN32
   executable = "node.exe";
   SetCurrentDirectory("infopool");
   #else
   executable = "node";
   std::string ip_path = DATAROOTDIR;
   ip_path+= "/infopool";
   chdir(ip_path.c_str());
   #endif
   parametersvector.push_back("app.js");
}

/**
  * Function langCheck takes as parameter the Language ID of the system
  * Checks if it is German->French->Polish->English
  * Then sets the appropriate Envir-Variables to the language that got 
  * matched first
  * \param api_language_code - windows-api language code
  */
void RefeusProcess::configureLanguageFromAPICode(int api_language_code) {
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

/**
 * based on given string, configure application to load language
 * the implementation could use a better method to map from iso to win32 codes
 */
void RefeusProcess::configureLanguageFromIsoString(std::string iso_language) {
  if ( iso_language == "en" ){
    configureLanguageFromAPICode(0); //default
  } else if (iso_language == "de") {
    configureLanguageFromAPICode(1031);
  } else if (iso_language == "fr") {
    configureLanguageFromAPICode(1036);
  } else if (iso_language == "pl") {
    configureLanguageFromAPICode(1045);
  }
}

/**
 * show file-open dialog, not the default startup
 */
void RefeusProcess::configureOpenRefeusDocument() {
   environmentmap["open_refeus_database"] = "true";
}

/**
 * show given path_name and open directly
 */
void RefeusProcess::configureOpenRefeusDocument(std::string path_name) {
   environmentmap["refeus_database"] = path_name;
   environmentmap["refeus_database_autostart"] = "true";
}


/**
 * configure to skip the database maintenance which can be anoying in
 * large document projects
 */
void RefeusProcess::configureSkipMaintenance(bool enabled){
  if ( enabled ){
    environmentmap["SKIP_MAINTENANCE"] = "YES";
  } else {
    environmentmap["SKIP_MAINTENANCE"] = "NO";
  }
}

/**
 * configure to set the default startup activity
 * only useful in combinantion with --open and other
 * options skipping the startup launcher
 */
 void RefeusProcess::configureStartupActivity(std::string activity_name){
   environmentmap["STARTUP_ACTIVITY"] = activity_name;
 }

/**
 * stores the given key-value pair to apply it to the executable
 * that is started in the end
 */
void RefeusProcess::setEnvironment(std::string env_name, std::string env_value) {
  environmentmap[env_name] = env_value;
}

/** \brief Split String into Vector using a delimter
  * \param string_to_split - some string to be split by a specific character
  * \param delimiter_character - character for splitting
  * \param element_vector - reference vector for storing the split result
  */
std::vector<std::string> &RefeusProcess::split(const std::string &string_to_split, const char delimiter_character, std::vector<std::string> &element_vector) {
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

  #ifdef _WIN32
  STARTUPINFO StartupInfo;                        //This is an [in] parameter
  PROCESS_INFORMATION ProcessInfo; //This is what we get as an [out] parameter 
  #endif
  for ( map_iterator = environmentmap.begin(); map_iterator != environmentmap.end(); ++map_iterator) {
    std::string env = map_iterator->first + "=" + map_iterator->second;
    char * env_cstr = new char[env.size() + 1];
    std::copy(env.begin(), env.end(), env_cstr);
    env_cstr[env.size()] = '\0';
    putenv(env_cstr);
    if ( debug ) { std::cout << "env: " << env << std::endl << std::flush; }
  }

  #ifdef _WIN32
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
  if ( debug ) { std::cout << "executable: " << executable << std::endl << std::flush; }
  for ( parameter_iterator = parametersvector.begin()
      ; parameter_iterator < parametersvector.end()
      ; ++parameter_iterator
      ) {
    if ( debug ) { std::cout << "parameter: " << *parameter_iterator << std::endl << std::flush; }
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

/**
 * display a dialog / terminal output with helpful information
 */
void RefeusProcess::usage() {
  const char* help_string = "--help displays all possible arguments\n"
       "--new opens a 'save as' dialog window\n"
       "--open \"C:/file_name\" starts refeus with specific file\n"
       "--plus sets the ini file to plus.ini\n"
       "--refeus sets the ini file to refeus.ini\n"
       "--debug sets the application to debug-mode (allows shift+ctrl+i)\n"
       "--cloud-enabled allows operations on webdav storage\n"
       "--[no]-auto-backup enable or disable the automatic backup each time refeus starts\n"
       "--[no]-skip-maintenance enable or disable the initial maintenance check when refeus opens\n" 
       "--language [de|en|fr*|pl*] set default language when application starts\n" 
       "--infopool start local infopool (plus only)\n"
       "TODO: add more parameters"
       ;    
  #ifdef _WIN32
  MessageBox(NULL, help_string, "Help!", MB_OK);
  #endif
  #ifdef unix
  std::cout << help_string << std::endl;
  #endif
}

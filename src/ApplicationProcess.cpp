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
#include <string>
#include <config.h>
#include <ApplicationProcess.h>
#define std_debug(expr) {std::stringstream ss; ss << expr; _debug(ss.str());}

int ApplicationProcess:: start() {
  std::string executable_with_parameter = executable;
  std::vector<std::string>::iterator parameter_iterator;
  std::map<std::string, std::string>::iterator map_iterator;
  #ifdef _WIN32
/** set the current working directory to the location of the module + bin 
  * this is required because registry association of *.docdb executes with
  * weird current directory
  */
  if ( !SetCurrentDirectory(configureBinPath().c_str()) ){
    std::stringstream message_stream;
    message_stream << "Sorry,"
                   << std::endl << executable << " could not be started."
                   << std::endl << "Please reinstall the software or contact your System Admisistrator."
                   << std::endl << "The installation is expected to have a wrapper/launcher in the bin/ directory"
                   << std::endl << "relative to this launcher, which cannot be found or executeddd."
                   ;
	std::string message = message_stream.str();
    MessageBox(NULL,message.c_str(),"Check Installation!",MB_OK);
	return 1;
  }
  #endif
  for ( map_iterator = environmentmap.begin()
      ; map_iterator != environmentmap.end()
      ; ++map_iterator
      ){
    std::string env = map_iterator->first + "=" + map_iterator->second;
    char * env_cstr = new char[env.size() + 1];
    std::copy(env.begin(), env.end(), env_cstr);
    env_cstr[env.size()] = '\0';
    putenv(env_cstr);
    std_debug("env: " << env);
  }
  #ifdef _WIN32
  /**
   * win32 implementation only
   */
  for ( parameter_iterator = parametersvector.begin()
      ; parameter_iterator < parametersvector.end()
      ; ++parameter_iterator
      ){
    executable_with_parameter = executable_with_parameter + " " + *parameter_iterator;
  }
  STARTUPINFO StartupInfo;                        //This is an [in] parameter
  PROCESS_INFORMATION ProcessInfo; //This is what we get as an [out] parameter 
  ZeroMemory(&StartupInfo, sizeof(StartupInfo));
  ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
  StartupInfo.cb = sizeof(StartupInfo) ;            //Only compulsory field
  //Formatting exe to const char* and full_exe to char* for CreateProcess
  
  char* executable_with_parameter_cstr = new char [executable_with_parameter.size() + 1];
  const char* executable_cstr = strdup(executable.c_str());
  std::copy(executable_with_parameter.begin(), executable_with_parameter.end(), executable_with_parameter_cstr);
  executable_with_parameter_cstr[executable_with_parameter.size()] = '\0';
  std_debug("executing: [" << executable_cstr << "] " << executable_with_parameter_cstr);
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
    std::stringstream message_stream;
    message_stream << "Sorry,"
                   << std::endl << executable << " could not be started."
                   << std::endl << "Please reinstall the software or contact your System Admisistrator."
                   << std::endl << "The installation is expected to have a wrapper/launcher in the bin/ directory"
                   << std::endl << "relative to this launcher, which cannot be found or executed."
                   ;
	std::string message = message_stream.str();
    MessageBox(NULL,message.c_str(),"Check Installation!",MB_OK);
    return 1;
  }
  #endif
  #ifdef unix
  /**
   * unix implementation only
   */
  char **exec_argv = static_cast<char**>(malloc(sizeof(char*) * (parametersvector.size() + 1) ));
  int parameter_iterator_index = 0;
  std_debug("executable: " << executable);
  for ( parameter_iterator = parametersvector.begin()
      ; parameter_iterator < parametersvector.end()
      ; ++parameter_iterator
      ){
    std_debug("parameter: " << *parameter_iterator);
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

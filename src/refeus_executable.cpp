#include <windows.h>
#include "refeus_functions.cpp"
 
#ifdef win32
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {
#endif
#ifdef unix
int main(int argc, char** argv) {
#endif
  RefeusProcess r;
  #ifdef win32
  char *command_line_cstr = GetCommandLine();
  std::string command_line = command_line_cstr;
  r.argParser(command_line);
  r.langCheck(GetUserDefaultLCID());
  #endif
  #ifdef unix
  std::string command_line= "";
  for ( int i = 1
      ; i < argc
      ; i++
      ) {
    command_line.append(std::string(argv[i]).append(" "));
  }
  r.argParser(command_line);
  r.langCheck(1031);
  #endif
  //rp.setEnvironment("window_size", "400x800");
  //rip.setExecutable("node.exe");
  //rip.setExecutableParameter("app.js");
  //r.start();
  return r.start();
  //rip.start();
  //system("pause");
}

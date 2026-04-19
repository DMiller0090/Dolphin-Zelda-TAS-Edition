#include "utilmodule.h"

#include "Common/FileUtil.h"
#include "Common/StringUtil.h"
#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"
#include "Core/Core.h"
#include "Core/ConfigManager.h"
#include "Core/System.h"
#include "Scripting/Python/Utils/module.h"
#include "Scripting/Python/Utils/as_py_func.h"

#ifdef _WIN32
#include <Windows.h>
#include <commdlg.h>
#endif

struct FileState
{
  // This is unused besides initializion of the module
};

static std::string scriptDir;

static PyObject* get_game_id(PyObject* module, PyObject* args)
{
  return Py_BuildValue("s", SConfig::GetInstance().GetGameID().c_str());
}

static PyObject* get_script_dir(PyObject* module, PyObject* args)
{
  return Py_BuildValue("s", scriptDir.c_str());
}

static PyObject* start_framedump(PyObject* module, PyObject* args)
{
  Config::SetBaseOrCurrent(Config::MAIN_MOVIE_DUMP_FRAMES, true);
  Py_RETURN_NONE;
}

static PyObject* stop_framedump(PyObject* module, PyObject* args)
{
  Config::SetBaseOrCurrent(Config::MAIN_MOVIE_DUMP_FRAMES, false);
  Py_RETURN_NONE;
}

static PyObject* is_framedumping(PyObject* module, PyObject* args)
{
  if(Config::Get(Config::MAIN_MOVIE_DUMP_FRAMES))
    Py_RETURN_TRUE;
  Py_RETURN_FALSE;
}

static PyObject* start_audiodump(PyObject* module, PyObject* args)
{
  Config::SetBaseOrCurrent(Config::MAIN_DUMP_AUDIO, true);
  Py_RETURN_NONE;
}

static PyObject* stop_audiodump(PyObject* module, PyObject* args)
{
  Config::SetBaseOrCurrent(Config::MAIN_DUMP_AUDIO, false);
  Py_RETURN_NONE;
}

static PyObject* is_audiodumping(PyObject* module, PyObject* args)
{
  if(Config::Get(Config::MAIN_DUMP_AUDIO))
    Py_RETURN_TRUE;
  Py_RETURN_FALSE;
}

static PyObject* save_screenshot(PyObject* module, PyObject* args, PyObject* kwargs)
{
  char* filename = nullptr;
  char* temp = const_cast<char*>("filename");
  static char* kwlist[] = {temp, NULL};

  if(!PyArg_ParseTupleAndKeywords(args, kwargs,"|s", kwlist, &filename))
  	return nullptr;

  if(filename)
  	Core::SaveScreenShot(filename);
  else
  	Core::SaveScreenShot();
  
  Py_RETURN_NONE;
}

static PyObject* open_file(PyObject* module, PyObject* args)
{
#ifdef _WIN32
  wchar_t file_path[MAX_PATH] = {};
  wchar_t filter[] = L"All Files\0*.*\0\0";

  OPENFILENAMEW open_file_name = {};
  open_file_name.lStructSize = sizeof(open_file_name);
  open_file_name.lpstrFilter = filter;
  open_file_name.lpstrFile = file_path;
  open_file_name.nMaxFile = MAX_PATH;
  open_file_name.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

  if(!GetOpenFileNameW(&open_file_name))
    Py_RETURN_NONE;

  const std::string utf8_path = WStringToUTF8(file_path);
  return Py_BuildValue("s", utf8_path.c_str());
#else
  Py_RETURN_NONE;
#endif
}

static PyObject* toggle_play(PyObject* module, PyObject* args)
{
  // Play/Pause can only be done on the host thread
  Core::QueueHostJob([](Core::System& system) {
    Core::State current_state = Core::GetState(system);

    switch (current_state)
    {
    case Core::State::Paused:
      Core::SetState(system, Core::State::Running);
      break;
    case Core::State::Running:
      Core::SetState(system, Core::State::Paused);
      break;
    default:
      break;
    }
  });

  Py_RETURN_NONE;
}

static void setup_file_module(PyObject* module, FileState* state)
{
  // I don't think we need anything here yet
}

PyMODINIT_FUNC PyInit_dol_utils()
{
  scriptDir = File::GetUserPath(D_SCRIPTS_IDX);
  static PyMethodDef methods[] = {{"get_game_id", get_game_id, METH_NOARGS, ""},
                                  {"get_script_dir", get_script_dir, METH_NOARGS, ""},
                                  {"open_file", open_file, METH_NOARGS, ""},
                                  {"start_framedump", start_framedump, METH_NOARGS, ""},
                                  {"stop_framedump", stop_framedump, METH_NOARGS, ""},
                                  {"is_framedumping", is_framedumping, METH_NOARGS, ""},
                                  {"start_audiodump", start_audiodump, METH_NOARGS, ""},
                                  {"stop_audiodump", stop_audiodump, METH_NOARGS, ""},
                                  {"is_audiodumping", is_audiodumping, METH_NOARGS, ""},
                                  {"save_screenshot", (PyCFunction) save_screenshot, METH_VARARGS | METH_KEYWORDS, ""},
                                  {"toggle_play", toggle_play, METH_NOARGS, ""},
                                  {nullptr, nullptr, 0, nullptr}};
  static PyModuleDef module_def =
      Py::MakeStatefulModuleDef<FileState, setup_file_module>("dolphin_utils", methods);
  return PyModuleDef_Init(&module_def);
}

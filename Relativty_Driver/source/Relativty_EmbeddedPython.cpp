// Copyright (C) 2020  Max Coutte, Gabriel Combe
// Copyright (C) 2020  Relativty.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment (lib, "Setupapi.lib")
#pragma comment (lib, "python38.lib")

#include <iostream>
#include <filesystem>
#include <string>
#include "Python.h"
#include "Relativty_EmbeddedPython.h"
#include "Relativty_ServerDriver.hpp"

namespace fs = std::filesystem;

void startPythonTrackingClient_threaded(std::string PyPath) {
	std::string fileName = PyPath + "/Client.py";
	FILE* fp;
	fp = fopen(fileName.c_str(), "rb");

	std::string singleQuote = "\'";
	PyPath = "PyPATH = " + singleQuote + PyPath + singleQuote;
	Py_Initialize();
	PyRun_SimpleString(PyPath.c_str());
	Relativty::ServerDriver::Log("Thread4: starting Client.py \n");
	PyRun_AnyFileExFlags(fp, "Client.py", 0, NULL);
	Py_Finalize();
}
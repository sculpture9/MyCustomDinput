#pragma once
#include "framework.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

BOOL ReadDataFromCSV(const LPCSTR &csvPath, std::vector<std::vector<std::string>> &result);

bool CharVector2String(const std::vector<char> &chars, std::string &str);
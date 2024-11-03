#pragma once
#include "framework.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

bool ReadDataFromCSV(const LPCSTR &csvPath, std::vector<std::vector<std::string>> &result, long &lineFlag);

bool CharVector2String(const std::vector<char> &chars, std::string &str);
#pragma once
#include "framework.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

bool ReadDataFromCSV(std::vector<std::vector<std::string>> &result, const LPCSTR &csvPath, long &lineFlag, bool needEscape = true);

bool CharVector2String(const std::vector<char> &chars, std::string &str);
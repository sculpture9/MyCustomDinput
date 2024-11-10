#include "tool_log.h"
#include "define_text.h"
#include <fstream>
using namespace std;
void Log(std::string content, bool isRewrite)
{
    ofstream fileWriter;
    if (isRewrite) fileWriter.open(YS_TRANSLATION_LOG_TXT_PATH, ios::out);
    else fileWriter.open(YS_TRANSLATION_LOG_TXT_PATH, ios::app);
    if (!fileWriter.is_open()) return;

    fileWriter << content << endl;
    fileWriter.close();
}

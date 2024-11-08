#include "csv_reader.h"
using namespace std;

bool ReadDataFromCSV(vector<vector<string>> &result, const LPCSTR &csvPath, long &lineFlag, bool needEscape)
{
    ifstream reader;
    reader.open(csvPath);
    if (!reader.is_open())
    {
        reader.close();
        return FALSE;
    }
    string line, temp;
    lineFlag = -1;
    while (std::getline(reader, line))
    {
        vector<string> columns;
        char c;
        bool colTransFlag = false, inSentenceWithComma = false, escapeFlag = false;
        vector<char> wret;
        long lineSize = line.length();
        lineFlag++;
        for (size_t i = 0; i < lineSize; i++)
        {
            c = line[i];
            //just started translate
            if (!colTransFlag)
            {
                //will input string, here we don't push '"' to result;
                if (c == '"')
                {
                    inSentenceWithComma = true;
                    colTransFlag = true;
                }
                //input number
                else if (iswdigit(c))
                {
                    wret.push_back(c);
                    colTransFlag = true;
                }
                //input sentence without comma
                else
                {
                    //maybe no problems
                    wret.push_back(c);
                    colTransFlag = true;
                }
                continue;
            }
            //judge the last char
            if (i == lineSize - 1)
            {
                if (inSentenceWithComma)
                {
                    return FALSE;
                }
            }
            //now translating
            //not in sentence with comma
            if (!inSentenceWithComma)
            {
                //','and '' meaning maybe one column done
                if (c == ',' || (i == lineSize - 1))
                {
                    if (c != ',') { wret.push_back(c); }
                    string column;
                    CharVector2String(wret, column);
                    columns.push_back(column);
                    wret.clear();
                    inSentenceWithComma = false;
                    colTransFlag = false;
                }
                else if (iswdigit(c))
                {
                    wret.push_back(c);
                }
                //input sentence without comma
                else
                {
                    //maybe no problems
                    wret.push_back(c);
                }
                continue;
            }
            //now in sentence
            if (i == lineSize - 1) { return FALSE; }
            if (escapeFlag)
            {
                switch (c)
                {
                    case '"':
                        wret.push_back('\"');
                        break;
                    case 'n':
                        wret.push_back('\n');
                        break;
                    case '0':
                        wret.push_back('\0');
                        break;
                    default:
                        break;
                }
                escapeFlag = false;
                continue;
            }
            //meaning one sentence is over
            if (c == '"')
            {
                inSentenceWithComma = false;
                continue;
            }
            if (c == '\\' && needEscape)
            {
                escapeFlag = true;
                continue;
            }
            wret.push_back(c);  //now c is a normal wchar
        }
        //now handle one line
        result.push_back(columns);
    }
    reader.close();
    lineFlag++;  //lineFlag = max line + 1, mean all line is correct.
    return TRUE;
}

bool CharVector2String(const vector<char> &chars, string &str)
{
    str.clear();
    for (int i = 0; i < chars.size(); i++)
    {
        wchar_t wc = chars[i];
        str.push_back(wc);
    }
    return true;
}
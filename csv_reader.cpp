#include "csv_reader.h"
using namespace std;

BOOL ReadDataFromCSV(const LPCSTR &csvPath, vector<vector<string>> &result)
{
    ifstream reader;
    reader.open(csvPath);
    if (!reader.is_open())
    {
        reader.close();
        return FALSE;
    }
    string line, temp;
    while (std::getline(reader, line))
    {
        vector<string> columns;
        char c;
        bool isColTrans = false, inSentenceWithComma = false, isEscape = false;
        vector<char> wret;
        long lineSize = line.length();
        for (size_t i = 0; i < lineSize; i++)
        {
            c = line[i];
            //just started translate
            if (!isColTrans)
            {
                //will input string, here we don't push '"' to result;
                if (c == '"')
                {
                    inSentenceWithComma = true;
                    isColTrans = true;
                }
                //input number
                else if (iswdigit(c))
                {
                    wret.push_back(c);
                    isColTrans = true;
                }
                //input sentence without comma
                else
                {
                    //maybe no problems
                    wret.push_back(c);
                    isColTrans = true;
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
                    isColTrans = false;
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
            if (isEscape)
            {
                if (c == '"')
                {
                    wret.push_back(c);
                }
                else
                {
                    wret.push_back('\\');
                    wret.push_back(c);
                    isEscape = false;
                }
                continue;
            }
            //meaning one sentence is over
            if (c == '"')
            {
                inSentenceWithComma = false;
                continue;
            }
            if (c == '\\')
            {
                isEscape = true;
                continue;
            }
            wret.push_back(c);  //now c is a normal wchar
        }
        //now handle one line
        result.push_back(columns);
    }
    reader.close();
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
#include "csv_reader.h"

vector<vector<string>> ReadDataFromCSV(const LPCSTR &csvPath)
{
    vector<vector<string>> result;
    ifstream reader;
    reader.open(csvPath);
    if (!reader.is_open())
    {
        reader.close();
        return result;
    }
    string line, temp;
    while (std::getline(reader, line))
    {
        vector<string> columns;
        char c;
        bool isColTrans = false, inSentence = false, isEscape = false;
        vector<char> wret;
        for (size_t i = 0; i < line.length(); i++)
        {
            c = line[i];
            //just started translate
            if (!isColTrans)
            {
                //will input string, here we don't push '"' to result;
                if (c == '"')
                {
                    inSentence = true;
                    isColTrans = true;
                }
                //input number
                else if (iswdigit(c))
                {
                    wret.push_back(c);
                    isColTrans = true;
                }
                else
                {
                    //invalid
                    return result;
                }
                continue;
            }
            //now translating
            if (!inSentence)
            {
                //',' meaning maybe one column done
                if (c == ',')
                {
                    string column;
                    CharVector2String(wret, column);
                    columns.push_back(column);
                    wret.clear();
                    inSentence = false;
                    isColTrans = false;
                }
                if (iswdigit(c))
                {
                    wret.push_back(c);
                }
                else
                {
                    //invalid
                    return result;
                }
                continue;
            }
            //now in sentence
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
                inSentence = false;
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
    return result;
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
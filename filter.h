#ifndef FILTER
#define FILTER
#define HASH_UNIT 83313

#include <iostream>
#include <string>
#include <vector>
#define MAX_PATH_LENGTH 200
using namespace std;


class Filter
{
    public:
         Filter(const char * , const char * );
         ~Filter();
    public :
         bool   ConstructRule();
         bool GetIPFromQuery(const char *);
         void Split(string str, string pattern);
         bool IfValidateIP(const char *  IP);
         bool HasMask(const char * IPAddress);
         int GetMaskBit(const char * IP);
         unsigned int IP2Unsignedint(char * IP);
         bool IfAccept();

         bool MatchIP();
         int GetAcceptCount()
         {return m_acceptCount;}
         int GetTotalQuery()
         {return m_totalQuery;}

    private:
        char m_ruleFilePath[MAX_PATH_LENGTH];
        char m_queryFilePath[MAX_PATH_LENGTH];

        
        char m_IP[20];

//        vector<string> m_splitResult;
        int m_acceptCount;
        int m_totalQuery;
        
        vector<vector<unsigned int> *>  *m_hashHead;

};
#endif

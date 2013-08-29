#include "filter.h"
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <arpa/inet.h>
Filter::Filter(const char * rulePath, const char * queryPath)
{
    m_acceptCount = 0;
    m_totalQuery = 0;
    int i=0;
    while(*rulePath != '\0')
    {
        m_ruleFilePath[i] = *rulePath;
        ++rulePath;
        ++i;
    }

    m_ruleFilePath[i] = '\0';

    i = 0;
    while(*queryPath != '\0')
    {
        m_queryFilePath[i] = *queryPath;
        ++queryPath;
        ++i;
    }
    m_queryFilePath[i] = '\0';
}

Filter::~Filter()
{
   if(m_hashHead != NULL)
   {
       for(vector<vector<unsigned int>*>::iterator it = (*m_hashHead).begin();
               it != (*m_hashHead).end(); 
               ++it)
       {
           if((*it) != NULL)
           {
               delete (*it);
           }

       }
       delete m_hashHead;
   }
}

bool Filter::GetIPFromQuery(const char * query)
{
    m_IP[0] = '\0';
    
    char * targetPointer = strstr(query,"ip=");
    if(targetPointer == NULL)
        return true;

    int offset = targetPointer - query;
    char * IPendPointer = strchr((query + offset), '&');
    if(IPendPointer == NULL)
    {
        IPendPointer  = strchr((query + offset),'\n');
    }
    
    int i = 0;
    while((targetPointer + (i + 3)) != IPendPointer)
    {
        m_IP[i] = *(targetPointer + (i +3));
        ++i;
    }
    m_IP[i] = '\0';
    return true;
}


bool Filter::HasMask(const char * IP) // false: does not have mask, true:have mask
{
    int i = 0;
    while(*(IP + i)!= '\0' && IP[i] != '/')
        ++i;
    if(*(IP + i) == '\0')
        return false;
    return true;
}


bool Filter::IfValidateIP(const char * IP)
{
    int i = 0;
    int xiegangIndex = -1;
    int dotCount = 0;
}


unsigned int Filter::IP2Unsignedint(char * IP)
{
    /*char charIP[50];
    int i = 0;
    while(*(IP + i) != '\0')
    {
        charIP[i] = *(IP +i);
        ++i;
    }
    charIP[i] = '\0';*/

    const char *d = ".";
    int intgerResult[4];
    char * p = strtok(IP,d);
    int i = 0;
    while(p)
    {
        intgerResult[i++] = atoi(p);
        p = strtok(NULL,d);
    }
    unsigned int result = 0;
    unsigned int binaryNumber[32];
    unsigned int maskShift = 0;
    unsigned int mask = 1;
    for(int h =0;h<32;++h)
    {
        binaryNumber[h] = 0;
    }
    int binaryBit = 0;
    for(i = 3; i >= 0; --i)
    {
        int currentNumber = intgerResult[i];
        for(int h = 0;h < 8; ++h)
        {
            binaryNumber[binaryBit] = currentNumber % 2;
            currentNumber = currentNumber / 2;
            ++binaryBit;
        }
    }
    /*if(!HasMask(charIP))
    {
        maskShift = 0;
        printf("error\n");
        for(int h = 0; h < 32; ++h)
        {
            if(binaryNumber[h] == 1)
            {
                result = result|mask;
            }
            ++maskShift;
            mask = 1;
            mask = mask << maskShift;
        }
    }*/
    int maskBit = GetMaskBit(IP);
    maskShift = 31;
    for(int h = 31; h >= 32-maskBit; --h)
    {
        mask = mask << maskShift;
        if(binaryNumber[h] == 1)
        {
            result = result | mask;
        }
        --maskShift;
        mask = 1;
    }
    return result;
}


int Filter::GetMaskBit(const char * IP)
{
    int i = 0;
    while(*(IP + i) != '/')
        ++i;
    int j = 0;
    ++i;    // ignore the '/'
    char temp[5];
    while(*(IP + i) != '\0')
    {
        temp[j] = *(IP + i);
        ++i;
        ++j;
    }
    return (atoi(temp));
}


bool Filter::ConstructRule()
{
    FILE * ruleFile = fopen(m_ruleFilePath,"r");
    if(!ruleFile)
        return false;

    m_hashHead = new vector<vector<unsigned int> *> (HASH_UNIT,NULL);

    char IPtemp[200];
    unsigned int netSum;
    unsigned int IPSum;
    int headId;
    while(fgets(IPtemp,200,ruleFile))
    {
        if(!HasMask(IPtemp))
        {
            unsigned int inetResult = inet_addr(IPtemp);
            IPSum = ntohl(inetResult);
            headId = IPSum % HASH_UNIT;
            if(NULL == (*m_hashHead)[headId])
            {
                (*m_hashHead)[headId] = new vector<unsigned int>();
            }
            (*(*m_hashHead)[headId]).push_back(IPSum);
        }
        else              // rule IP has mask
        {
            netSum = IP2Unsignedint(IPtemp);
            int maskBit = GetMaskBit(IPtemp);
            int maxNetNumber =(int) pow(2.0,32-maskBit);
            for(int i=1;i<maxNetNumber-1;++i)
            {
                IPSum = netSum +i;
                headId = IPSum % HASH_UNIT;
                if(NULL == (*m_hashHead)[headId])
                {
                    (*m_hashHead)[headId] = new vector<unsigned int>();
                }
                (*((*m_hashHead)[headId])).push_back(IPSum);
            }
        }
    }
}


bool Filter::IfAccept()   //false:accept, true:not accept
{
    if(m_IP[0] == '\0' || m_IP[0] == 'u')
    {
        return false;
    }
/*    char charIP[20];
    int i = 0;
    while(m_IP[i] != '\0')
    {
        charIP[i] = m_IP[i];
        ++i;
    }
    charIP[i] = '\0';

*/
    unsigned int inetSum = inet_addr(m_IP);
    unsigned int IPSum = ntohl(inetSum);
    unsigned int headId = IPSum % HASH_UNIT;
    if(NULL == (*m_hashHead)[headId])
    {
        return false;
    }
    else 
    {
        vector<unsigned int>::iterator it = find((*(*m_hashHead)[headId]).begin(),(*(*m_hashHead)[headId]).end(),IPSum);
        if(it == (*(*m_hashHead)[headId]).end())
            return false;
    }
    return true;
}


bool Filter::MatchIP()
{
    FILE * queryFile = fopen(m_queryFilePath,"r");
    char tempQuery[1600];
    if(NULL == queryFile)
    {
        printf("Query file can not open\n");
        return false;
    }
    while(fgets(tempQuery,1600,queryFile))
    {
        GetIPFromQuery(tempQuery);
        m_totalQuery++;
        if(!IfAccept())
        {
            ++m_acceptCount;
        }
    }
}

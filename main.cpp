#include "filter.h"

int main()
{
    Filter filter("/home/yanglong/ipfilter/ip_blacklist","/home/yanglong/ipfilter/query");
    filter.ConstructRule();
    printf("constructRule finished\n");
    filter.MatchIP();
    printf("accept:%d\n",filter.GetAcceptCount());
    printf("%d",filter.GetTotalQuery());
    return 0;
}

#include "afx.h"
#include "word_seg_lib.h"
#include "stdio.h"
#include <string.h>
#include "stdafx.h"


using namespace std;

const int MAX_LINE = 10000;



//注意：当字符串为空时，也会返回一个空字符串  
void split(std::string& s, std::string& delim, std::vector< std::string >* ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret->push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last>0)
	{
		ret->push_back(s.substr(last, index - last));
	}
}




int init_nlpir(const char *data_dir, const char *user_word_dict)
{
	int nCount;
	int ret = 0;

	return ret;
}


int delete_nlpir()
{
	return 0;
}

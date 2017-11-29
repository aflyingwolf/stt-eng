
#ifndef  __TTS_ALL_H_
#define  __TTS_ALL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "inter_lib.h"
#include "word_seg_lib.h"

#include "pre_pro.h"

#pragma comment(lib, "lib/ttsdll.lib")


const int MAX_LINE_SIZE = 10000;
const int WAV_SIZE = 60000000;	//10+min  44100*600*2=6000 0000



inline int str_trim(char *c)
{
	if (strlen(c)>0 && c[strlen(c) - 1] == 0x0A)
	{
		c[strlen(c) - 1] = '\0';
	}
	if (strlen(c)>0 && c[strlen(c) - 1] == 0x0D)
	{
		c[strlen(c) - 1] = '\0';
	}
	return 0;
}

class TTS_ALL
{

public:
	TTS_ALL();
	~TTS_ALL();
	/*
	���� model\LC\model-like-77  ֱ�Ӿ�����ѧģ��
	ֻ��ttsdll�õ��˴� Ŀ¼
	���� tn str_pro �ִ� ��д����data������
	*/
	int init(const char* model_dir);

	/*
	// ������β���� �ҵ���һ��valueֵ����min_val�ĵ�  Ȼ�������ƶ�400ms(0.4*44100)
	//buff: buff����  ����Ϊlen
	// left:���˵��idx   right:�Ҳ�˵��idx
	// min_val: �ж���������ֵ 400
	*/
	int silence_pro(const short *buff, int len, int &left, int &right, short min_val);

	int tts(const char *input, short *buff, int buff_size);
	int del();

private:

	TTS m_tts;

	// û��NLPIR ����jieba��
	int nlpir_flag;
	FILE *fp_log;

	PreProClass m_pp;

};




#endif
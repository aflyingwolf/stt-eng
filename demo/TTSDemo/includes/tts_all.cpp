#include "afx.h"
#include "tts_all.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "flite_hts_engine.h"

#define INPUT_BUFF_SIZE 1024

/*
	���� model\LC ֱ�Ӿ�����ѧģ�� 
*/
int TTS_ALL::init(const char *model_dir)
{
	int ret = 0;
	char *tn_dir = new char[1000];

	if (fp_log != NULL)
	{
		fclose(fp_log);
		fp_log = NULL;
	}
	fp_log = fopen("word_seg.log", "w+");

	// �ַ��� Ԥ����
	_snprintf(tn_dir, 999, "data/str_pro");
	ret = this->m_pp.init(tn_dir);
	if (ret < 0){ return -1; }


	// tts ��ʼ��
	ret = m_tts.init(model_dir);
	if (ret < 0)
	{
		printf("tts init failed!\n");
		return -1;
	}

	delete tn_dir;
	return 0;
}

/*
	// ������β���� �ҵ���һ��valueֵ����min_val�ĵ�  Ȼ�������ƶ�400ms(0.4*44100) 
	//buff: buff����  ����Ϊlen
	// left:���˵��idx   right:�Ҳ�˵��idx  
	// min_val: �ж���������ֵ 400 
*/
int TTS_ALL::silence_pro(const short *buff, int len, int &left, int &right, short min_val)
{
	int ret = 0;
	left = 0;
	right = 0;
	short thr = 400;
	int pos = 4 * 1600; // 400ms

	for (int i = 0; i < len;i++)
	{
		if (abs(buff[i]) > thr)
		{
			left = i - pos;
			left = (left > 0 ? left : 0);
			break;
		}
	}
	for (int j = len - 1; j > 0;j--)
	{
		if (abs(buff[j]) > thr)
		{
			right = j + pos;
			right = (right<len ? right : len-1);
			break;
		}
	}

	return ret;
}

/*
	// �������ı����У��Ͼ䡢flite;
	// ���� tts-dll  ����label �õ�����
	// �Ե������� ������β��������
	// input: �����ı�
	// buff_size: buff����󳤶� 
*/
int TTS_ALL::tts(const char *input, short *buff, int buff_size)
{

	int ret = 0;
	int len = 0;
	int len_write = 0;  // д��buff���ܳ���
	char *delim = "\n";
	
	char *tmp;
	char lines[MAX_LINE_SIZE];
	char input_new[MAX_LINE_SIZE];
	char * input_new_tmp = input_new;
	short *buff_tmp = new short[WAV_SIZE];
	

	char line_split_tmp1[MAX_LINE_SIZE];
	char line_split_tmp2[MAX_LINE_SIZE];
	std::string str_1;
	std::string str_2;
	

	FILE *fp_test_out = fopen("temp.txt","w");
	if (fp_test_out == NULL){ return -1; }
	fprintf(fp_test_out, "%s", input);
	fclose(fp_test_out);
	fp_test_out = fopen("temp.txt", "r");
	

	///  �ı����е�ÿ�仰  
	while ( (tmp = fgets(lines, MAX_LINE_SIZE, fp_test_out)) != NULL )
	{
		str_1 = lines;
		fprintf(fp_log, "input_char:%s\n", str_1.c_str());
		fflush(fp_log);

		// �Ͼ� 
		std::vector<std::string> vec_out;			
		ret = this->m_pp.pre_pro_long(str_1, vec_out, 2);  

		// ÿһ���ַ��� �����������ַ�
		std::vector<std::string >::iterator it_vec;
		for (it_vec = vec_out.begin(); it_vec != vec_out.end(); it_vec++)
		{
			
			str_1 = *it_vec;
			fprintf(fp_log, "sentence:%s\n", str_1.c_str());
			fflush(fp_log);
			
			//"Chinese economy develops very fast in last few decades and a lot of people get rich."
			// ����flite  ����label���� 
			Flite_HTS_Engine_synthesize(str_1.c_str(), "label.txt");

			len = m_tts.line2short_array(str_1.c_str(), buff_tmp, WAV_SIZE);
			if (len < 0)
			{ 
				fprintf(fp_log, "line2short_array failed !: %s\n", line_split_tmp1);
				fflush(fp_log);
				return len; 
			}


			// �� buff_tmp������Ϊ ret�� ������β��������
			int left = 0;
			int right = len - 1;
			silence_pro(buff_tmp, len, left, right, 400);
			len = right - left;  // ���ٸ�short 

			if (buff_size < len_write+len)
			{
				fprintf(fp_log, "buff_len to small! str= %s\n", line_split_tmp1);
				fflush(fp_log);
				return len_write;
			}

			// copy ������buff ����������
			int sss = sizeof(short);
			memcpy(buff + len_write, buff_tmp + left, len*sss);
			
			len_write += len;

			fflush(fp_log);
		}

	}


	if (buff_tmp){ delete buff_tmp; buff_tmp = NULL; }

	fclose(fp_test_out);

	return len_write;

}


TTS_ALL::TTS_ALL()
{

	this->fp_log = NULL;
	this->nlpir_flag = 0;
}

TTS_ALL::~TTS_ALL()
{
	if (fp_log != NULL)
	{
		fclose(fp_log);
		fp_log = NULL;
	}

}



/* ----------------------------------------------------------------- */
/*           The English TTS System "Flite+hts_engine"               */
/*           developed by HTS Working Group                          */
/*           http://hts-engine.sourceforge.net/                      */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2005-2016  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                                                                   */
/*                2005-2008  Tokyo Institute of Technology           */
/*                           Interdisciplinary Graduate School of    */
/*                           Science and Engineering                 */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the HTS working group nor the names of its  */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission.   */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

#include "cst_synth.h"
#include "cst_utt_utils.h"
#include "cst_math.h"
#include "cst_file.h"
#include "cst_val.h"
#include "cst_string.h"
#include "cst_alloc.h"
#include "cst_item.h"
#include "cst_relation.h"
#include "cst_utterance.h"
#include "cst_tokenstream.h"
#include "cst_string.h"
#include "cst_regex.h"
#include "cst_features.h"
#include "cst_utterance.h"
#include "flite.h"
#include "cst_synth.h"
#include "cst_utt_utils.h"


#include "flite_hts_engine.h"




#define REGISTER_VOX register_cmu_us_kal
#define UNREGISTER_VOX unregister_cmu_us_kal

#define MAXBUFLEN 1024

cst_voice *REGISTER_VOX(const char *voxdir);
void UNREGISTER_VOX(cst_voice * vox);

/* create_label: create label per phoneme */
// xx^xx-pau+n=eh@xx_xx/A:xx_xx_xx/B:xx-xx-xx@xx-xx&xx-xx#xx-xx$xx-xx!xx-xx;xx-xx|xx/C:1+1+5/D:xx_xx/E:xx+xx@xx+xx&xx+xx#xx+xx/F:content_1/G:xx_xx/H:xx=xx^xx=xx|xx/I:3=3/J:14+13-2
// xx^pau-n+eh=k@1_5/A:xx_xx_xx/B:1-1-5@1-1&1-3#0-2$0-1!xx-1;xx-2|eh/C:1+0+2/D:xx_xx/E:content+1@1+3&0+1#xx+2/F:in_1/G:xx_xx/H:3=3^1=2|L-L%/I:11=10/J:14+13-2
static void create_label(cst_item * item, char *label)
{
    const char *p1 = ffeature_string(item, "p.p.name");
    const char *p2 = ffeature_string(item, "p.name");
    const char *p3 = ffeature_string(item, "name");
    const char *p4 = ffeature_string(item, "n.name");
    const char *p5 = ffeature_string(item, "n.n.name");

    if (strcmp(p3, "pau") == 0) 
    {
        /* for pause */
        int a3 = ffeature_int(item, "p.R:SylStructure.parent.R:Syllable.syl_numphones");
        int c3 = ffeature_int(item, "n.R:SylStructure.parent.R:Syllable.syl_numphones");
        int d2 = ffeature_int(item, "p.R:SylStructure.parent.parent.R:Word.word_numsyls");
        int f2 = ffeature_int(item, "n.R:SylStructure.parent.parent.R:Word.word_numsyls");
        int g1 = ffeature_int(item, "p.R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_syls_in_phrase");
        int g2 = ffeature_int(item, "p.R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_words_in_phrase");
        int i1 = ffeature_int(item, "n.R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_syls_in_phrase");
        int i2 = ffeature_int(item, "n.R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_words_in_phrase");
        int j1, j2, j3;

        if (item_next(item) != NULL) 
        {
            j1 = ffeature_int(item, "n.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_syls");
            j2 = ffeature_int(item, "n.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_words");
            j3 = ffeature_int(item, "n.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_phrases");
        } 
        else 
        {
            j1 = ffeature_int(item, "p.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_syls");
            j2 = ffeature_int(item, "p.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_words");
            j3 = ffeature_int(item, "p.R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_phrases");
        }

        //  p1^p2-p3+p4=p5
        //  @a1_b1/A:c1_a3_b3/B:c3-p6-p7
        //  @e2-b4&b5
        //  -h1#b6-b7$h2-e3!e4
        //  -j1;j2-j3|h3/C:h4
        sprintf(label, "%s^%s-%s+%s=%s@%s_xx/A:%s_%s_xx/B:%s-xx-xx@xx-xx&xx-xx#xx-xx$xx-xx!xx-%d;%d-%d|xx/C:xx",
                    strcmp(p1, "0") == 0 ? "xx" : p1, /* p1 */
                    strcmp(p2, "0") == 0 ? "xx" : p2, /* p2 */
                    p3,               /* p3 */
                    strcmp(p4, "0") == 0 ? "xx" : p4, /* p4 */
                    strcmp(p5, "0") == 0 ? "xx" : p5, /* p5 */
                    a3 == 0 ? "xx" : ffeature_string(item, "p.R:SylStructure.parent.R:Syllable.stress"),      /* a1 */
                    c3 == 0 ? "xx" : ffeature_string(item, "n.R:SylStructure.parent.R:Syllable.stress"),      /* c1 */

                    a3 == 0 ? "xx" : val_string(val_string_n(a3)),    /* a3 */
                    c3 == 0 ? "xx" : val_string(val_string_n(c3)),    /* c3 */
                    j1, j2, j3 
                );              


        //sprintf(label, "%s^%s-%s+%s=%s@xx_xx/A:%s_%s_%s/B:xx-xx-xx@xx-xx&xx-xx#xx-xx$xx-xx!xx-xx;xx-xx|xx/C:%s+%s+%s/D:%s_%s/E:xx+xx@xx+xx&xx+xx#xx+xx/F:%s_%s/G:%s_%s/H:xx=xx^xx=xx|xx/I:%s=%s/J:%d+%d-%d",      /* */
        //        strcmp(p1, "0") == 0 ? "xx" : p1, /* p1 */
        //        strcmp(p2, "0") == 0 ? "xx" : p2, /* p2 */
        //        p3,               /* p3 */
        //        strcmp(p4, "0") == 0 ? "xx" : p4, /* p4 */
        //        strcmp(p5, "0") == 0 ? "xx" : p5, /* p5 */
        //        a3 == 0 ? "xx" : ffeature_string(item, "p.R:SylStructure.parent.R:Syllable.stress"),      /* a1 */
        //        a3 == 0 ? "xx" : ffeature_string(item, "p.R:SylStructure.parent.R:Syllable.accented"),    /* a2 */
        //        a3 == 0 ? "xx" : val_string(val_string_n(a3)),    /* a3 */
        //        c3 == 0 ? "xx" : ffeature_string(item, "n.R:SylStructure.parent.R:Syllable.stress"),      /* c1 */
        //        c3 == 0 ? "xx" : ffeature_string(item, "n.R:SylStructure.parent.R:Syllable.accented"),    /* c2 */
        //        c3 == 0 ? "xx" : val_string(val_string_n(c3)),    /* c3 */
        //        d2 == 0 ? "xx" : ffeature_string(item, "p.R:SylStructure.parent.parent.R:Word.gpos"),     /* d1 */
        //        d2 == 0 ? "xx" : val_string(val_string_n(d2)),    /* d2 */
        //        f2 == 0 ? "xx" : ffeature_string(item, "n.R:SylStructure.parent.parent.R:Word.gpos"),     /* f1 */
        //        f2 == 0 ? "xx" : val_string(val_string_n(f2)),    /* f2 */
        //        g1 == 0 ? "xx" : val_string(val_string_n(g1)),    /* g1 */
        //        g2 == 0 ? "xx" : val_string(val_string_n(g2)),    /* g2 */
        //        i1 == 0 ? "xx" : val_string(val_string_n(i1)),    /* i1 */
        //        i2 == 0 ? "xx" : val_string(val_string_n(i2)),    /* i2 */
        //        j1,               /* j1 */
        //        j2,               /* j2 */
        //        j3);              /* j3 */

    } 
    else 
    {
        /* for no pause */
        int p6 = ffeature_int(item, "R:SylStructure.pos_in_syl") + 1;
        int a3 = ffeature_int(item, "R:SylStructure.parent.R:Syllable.p.syl_numphones");
        int b3 = ffeature_int(item, "R:SylStructure.parent.R:Syllable.syl_numphones");
        int b4 = ffeature_int(item, "R:SylStructure.parent.R:Syllable.pos_in_word") + 1;
        int b12 = ffeature_int(item, "R:SylStructure.parent.R:Syllable.lisp_distance_to_p_stress");
        int b13 = ffeature_int(item, "R:SylStructure.parent.R:Syllable.lisp_distance_to_n_stress");
        int b14 = ffeature_int(item, "R:SylStructure.parent.R:Syllable.lisp_distance_to_p_accent");
        int b15 = ffeature_int(item, "R:SylStructure.parent.R:Syllable.lisp_distance_to_n_accent");
        int c3 = ffeature_int(item, "R:SylStructure.parent.R:Syllable.n.syl_numphones");
        int d2 = ffeature_int(item, "R:SylStructure.parent.parent.R:Word.p.word_numsyls");
        int e2 = ffeature_int(item, "R:SylStructure.parent.parent.R:Word.word_numsyls");
        int e3 = ffeature_int(item, "R:SylStructure.parent.parent.R:Word.pos_in_phrase") + 1;
        int e7 = ffeature_int(item, "R:SylStructure.parent.parent.R:Word.lisp_distance_to_p_content");
        int e8 = ffeature_int(item, "R:SylStructure.parent.parent.R:Word.lisp_distance_to_n_content");
        int f2 = ffeature_int(item, "R:SylStructure.parent.parent.R:Word.n.word_numsyls");
        int g1 = ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.p.lisp_num_syls_in_phrase");
        int g2 = ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.p.lisp_num_words_in_phrase");
        int h2 = ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_words_in_phrase");
        int h3 = ffeature_int(item, "R:SylStructure.parent.R:Syllable.sub_phrases") + 1;
        const char *h5 = ffeature_string(item, "R:SylStructure.parent.parent.R:Phrase.parent.daughtern.R:SylStructure.daughtern.endtone");
        int i1 = ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.n.lisp_num_syls_in_phrase");
        int i2 = ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.n.lisp_num_words_in_phrase");
        int j1 = ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_syls");
        int j2 = ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_words");
        int j3 = ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_phrases");



        //  p1^p2-p3+p4=p5
        //  @a1_b1/A:c1_a3_b3/B:c3-p6-p7
        //  @e2-b4&b5
        //  -h1#b6-b7$h2-e3!e4
        //  -j1;j2-j3|h3/C:h4
        sprintf(label, "%s^%s-%s+%s=%s@%s_%d/A:%s_%s_%d/B:%s-%d-%d@%d-%d&%d-%d#%d-%d$%d-%d!%d-%d;%d-%d|%d/C:%d",
                    strcmp(p1, "0") == 0 ? "xx" : p1, /* p1 */
                    strcmp(p2, "0") == 0 ? "xx" : p2, /* p2 */
                    p3,               /* p3 */
                    strcmp(p4, "0") == 0 ? "xx" : p4, /* p4 */
                    strcmp(p5, "0") == 0 ? "xx" : p5, /* p5 */

                    a3 == 0 ? "xx" : ffeature_string(item, "R:SylStructure.parent.R:Syllable.p.stress"),      /* a1 */
                    ffeature_int(item, "R:SylStructure.parent.R:Syllable.stress"),    /* b1 */
                    c3 == 0 ? "xx" : ffeature_string(item, "R:SylStructure.parent.R:Syllable.n.stress"),      /* c1 */
                    a3 == 0 ? "xx" : val_string(val_string_n(a3)),    /* a3 */
                    b3,               /* b3 */
                    c3 == 0 ? "xx" : val_string(val_string_n(c3)),    /* c3 */
                    p6,               /* p6 */
                    b3 - p6 + 1,      /* p7 */

                    e2,               /* e2 */
                    b4,               /* b4 */
                    e2 - b4 + 1,      /* b5 */

                ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_syls_in_phrase"),       /* h1 */
                ffeature_int(item, "R:SylStructure.parent.R:Syllable.syl_in") + 1,        /* b6 */
                ffeature_int(item, "R:SylStructure.parent.R:Syllable.syl_out") + 1,       /* b7 */
                h2,               /* h2 */
                e3,               /* e3 */
                h2 - e3 + 1,      /* e4 */

                j1, j2, j3,
                h3,               /* h3 */
                j3 - h3 + 1      /* h4 */
                );              


        //sprintf(label, "%s^%s-%s+%s=%s@%d_%d/A:%s_%s_%s/B:%d-%d-%d@%d-%d&%d-%d#%d-%d$%d-%d!%s-%s;%s-%s|%s/C:%s+%s+%s/D:%s_%s/E:%s+%d@%d+%d&%d+%d#%s+%s/F:%s_%s/G:%s_%s/H:%d=%d^%d=%d|%s/I:%s=%s/J:%d+%d-%d",      /* */
        //        strcmp(p1, "0") == 0 ? "xx" : p1, /* p1 */
        //        strcmp(p2, "0") == 0 ? "xx" : p2, /* p2 */
        //        p3,               /* p3 */
        //        strcmp(p4, "0") == 0 ? "xx" : p4, /* p4 */
        //        strcmp(p5, "0") == 0 ? "xx" : p5, /* p5 */
        //        p6,               /* p6 */
        //        b3 - p6 + 1,      /* p7 */
        //        a3 == 0 ? "xx" : ffeature_string(item, "R:SylStructure.parent.R:Syllable.p.stress"),      /* a1 */
        //        a3 == 0 ? "xx" : ffeature_string(item, "R:SylStructure.parent.R:Syllable.p.accented"),    /* a2 */
        //        a3 == 0 ? "xx" : val_string(val_string_n(a3)),    /* a3 */
        //        ffeature_int(item, "R:SylStructure.parent.R:Syllable.stress"),    /* b1 */
        //        ffeature_int(item, "R:SylStructure.parent.R:Syllable.accented"),  /* b2 */
        //        b3,               /* b3 */
        //        b4,               /* b4 */
        //        e2 - b4 + 1,      /* b5 */
        //        ffeature_int(item, "R:SylStructure.parent.R:Syllable.syl_in") + 1,        /* b6 */
        //        ffeature_int(item, "R:SylStructure.parent.R:Syllable.syl_out") + 1,       /* b7 */
        //        ffeature_int(item, "R:SylStructure.parent.R:Syllable.ssyl_in"),   /* b8 */
        //        ffeature_int(item, "R:SylStructure.parent.R:Syllable.ssyl_out"),  /* b9 */
        //        ffeature_int(item, "R:SylStructure.parent.R:Syllable.asyl_in"),   /* b10 */
        //        ffeature_int(item, "R:SylStructure.parent.R:Syllable.asyl_out"),  /* b11 */
        //        b12 == 0 ? "xx" : val_string(val_string_n(b12)),  /* b12 */
        //        b13 == 0 ? "xx" : val_string(val_string_n(b13)),  /* b13 */
        //        b14 == 0 ? "xx" : val_string(val_string_n(b14)),  /* b14 */
        //        b15 == 0 ? "xx" : val_string(val_string_n(b15)),  /* b15 */
        //        ffeature_string(item, "R:SylStructure.parent.R:Syllable.syl_vowel"),      /* b16 */
        //        c3 == 0 ? "xx" : ffeature_string(item, "R:SylStructure.parent.R:Syllable.n.stress"),      /* c1 */
        //        c3 == 0 ? "xx" : ffeature_string(item, "R:SylStructure.parent.R:Syllable.n.accented"),    /* c2 */
        //        c3 == 0 ? "xx" : val_string(val_string_n(c3)),    /* c3 */
        //        d2 == 0 ? "xx" : ffeature_string(item, "R:SylStructure.parent.parent.R:Word.p.gpos"),     /* d1 */
        //        d2 == 0 ? "xx" : val_string(val_string_n(d2)),    /* d2 */
        //        ffeature_string(item, "R:SylStructure.parent.parent.R:Word.gpos"),        /* e1 */
        //        e2,               /* e2 */
        //        e3,               /* e3 */
        //        h2 - e3 + 1,      /* e4 */
        //        ffeature_int(item, "R:SylStructure.parent.parent.R:Word.content_words_in"),       /* e5 */
        //        ffeature_int(item, "R:SylStructure.parent.parent.R:Word.content_words_out"),      /* e6 */
        //        e7 == 0 ? "xx" : val_string(val_string_n(e7)),    /* e7 */
        //        e8 == 0 ? "xx" : val_string(val_string_n(e8)),    /* e8 */
        //        f2 == 0 ? "xx" : ffeature_string(item, "R:SylStructure.parent.parent.R:Word.n.gpos"),     /* f1 */
        //        f2 == 0 ? "xx" : val_string(val_string_n(f2)),    /* f2 */
        //        g1 == 0 ? "xx" : val_string(val_string_n(g1)),    /* g1 */
        //        g2 == 0 ? "xx" : val_string(val_string_n(g2)),    /* g2 */
        //        ffeature_int(item, "R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_syls_in_phrase"),       /* h1 */
        //        h2,               /* h2 */
        //        h3,               /* h3 */
        //        j3 - h3 + 1,      /* h4 */
        //        strcmp(h5, "0") == 0 ? "NONE" : h5,       /* h5 */
        //        i1 == 0 ? "xx" : val_string(val_string_n(i1)),    /* i1 */
        //        i2 == 0 ? "xx" : val_string(val_string_n(i2)),    /* i2 */
        //        j1,               /* j1 */
        //        j2,               /* j2 */
        //        j3);              /* j3 */

    }

}




/* 输入一行英文文本
	输出label序列到txt中 
*/
int Flite_HTS_Engine_synthesize(const char *txt, const char *file)
{
    int i;
    FILE *fp;
    cst_voice *v = NULL;
    cst_utterance *u = NULL;
    cst_item *s = NULL;
    char **label_data = NULL;
    int label_size = 0;

    if (txt == NULL)
      return -1;

    /* text analysis part */
    v = REGISTER_VOX(NULL);
    if (v == NULL)
      return -2;
    u = flite_synth_text(txt, v);
    if (u == NULL) {
        UNREGISTER_VOX(v);
        return -3;
    }
    for (s = relation_head(utt_relation(u, "Segment")); s; s = item_next(s))
      label_size++;
    if (label_size <= 0) {
        delete_utterance(u);
        UNREGISTER_VOX(v);
        return -4;
    }
    label_data = (char **) calloc(label_size, sizeof(char *));
    for (i = 0, s = relation_head(utt_relation(u, "Segment")); s; s = item_next(s), i++) {
        label_data[i] = (char *) calloc(MAXBUFLEN, sizeof(char));
        create_label(s, label_data[i]);
    }

	// 写label序列到文件中     
    if (file == NULL) 
	{
		printf("error:file == NULL !\n");
		return -5;
    }
	fp = fopen(file, "w");
	if (fp == NULL)
	{
		printf("error:open %s failed!\n",file);
		return -6;
	}

	// szm 测试label序列 
	for (i = 0; i < label_size; i++)
	{
		// label序列的处理   ax 换成 ah
		// 使用replace替换即可
		int len = strlen(label_data[i]);
		int k = 0; 
		while (k < len-1)
		{
			if (label_data[i][k] == 'a' && label_data[i][k+1] == 'x')
			{
				label_data[i][k] = 'a';
				label_data[i][k + 1] = 'h';
				k++;
			}
			k++;
		}
		
		
		fprintf(fp, "%s\n", label_data[i]);
	}
	
	fclose(fp);
    

    for (i = 0; i < label_size; i++)
      free(label_data[i]);
    free(label_data);

    delete_utterance(u);
    UNREGISTER_VOX(v);
	
	
    return 0;
}


typedef struct _Flite_Utterance {
    cst_voice *v;
    cst_utterance *u;
    int nitem;
    cst_item **items;
} Flite_Utterance;

/* Flite_Text_Analyzer_initialize: initialize flite front-end */
void Flite_Text_Analyzer_initialize(Flite_Text_Analyzer * analyzer)
{
    if (analyzer == NULL)
      return;
    analyzer->pointer = NULL;
}

/* Flite_Text_Analyzer_analysis: text analysis */
void Flite_Text_Analyzer_analysis(Flite_Text_Analyzer * analyzer, const char *text)
{
    int i;
    cst_item *s;
    Flite_Utterance *fu;

    if (analyzer == NULL || text == NULL)
      return;

    if (analyzer->pointer != NULL)
      Flite_Text_Analyzer_clear(analyzer);

    /* allocate */
    fu = (Flite_Utterance *) malloc(sizeof(Flite_Utterance));

    /* create voice */
    fu->v = REGISTER_VOX(NULL);
    if (fu->v == NULL) {
        free(fu);
        return;
    }

    /* create utterance */
    fu->u = flite_synth_text(text, fu->v);
    if (fu->u == NULL) {
        UNREGISTER_VOX(fu->v);
        free(fu);
        return;
    }

    /* count number of phonemes */
    for (fu->nitem = 0, s = relation_head(utt_relation(fu->u, "Segment")); s; s = item_next(s), fu->nitem++);
    if (fu->nitem == 0) {
        delete_utterance(fu->u);
        UNREGISTER_VOX(fu->v);
        free(fu);
        return;
    }

    /* save informations */
    fu->items = (cst_item **) malloc(sizeof(cst_item *) * fu->nitem);
    for (i = 0, s = relation_head(utt_relation(fu->u, "Segment")); s; s = item_next(s), i++)
      fu->items[i] = s;

    analyzer->pointer = (void *) fu;
}

/* Flite_Text_Analyzer_get_nphoneme_in_utterance: get number of phonemes */
int Flite_Text_Analyzer_get_nphoneme_in_utterance(Flite_Text_Analyzer * analyzer)
{
    Flite_Utterance *fu;

    if (analyzer == NULL || analyzer->pointer == NULL)
      return 0;

    fu = (Flite_Utterance *) analyzer->pointer;
    return fu->nitem;
}

/* Flite_Text_Analyzer_get_phoneme: get phoneme identity */
const char *Flite_Text_Analyzer_get_phoneme(Flite_Text_Analyzer * analyzer, int phoneme_index)
{
    Flite_Utterance *fu;

    if (analyzer == NULL || analyzer->pointer == NULL)
      return NULL;
    fu = (Flite_Utterance *) analyzer->pointer;
    if (phoneme_index < 0 || phoneme_index >= fu->nitem)
      return NULL;
    return ffeature_string(fu->items[phoneme_index], "name");
}

/* Flite_Text_Analyzer_get_word: get word */
const char *Flite_Text_Analyzer_get_word(Flite_Text_Analyzer * analyzer, int phoneme_index)
{
    Flite_Utterance *fu;

    if (analyzer == NULL || analyzer->pointer == NULL)
      return NULL;
    fu = (Flite_Utterance *) analyzer->pointer;
    if (phoneme_index < 0 || phoneme_index >= fu->nitem)
      return NULL;
    return ffeature_string(fu->items[phoneme_index], "R:SylStructure.parent.parent.name");
}

/* Flite_Text_Analyzer_get_nphoneme_in_syllable: get number of phonemes in syllable */
int Flite_Text_Analyzer_get_nphoneme_in_syllable(Flite_Text_Analyzer * analyzer, int phoneme_index)
{
    Flite_Utterance *fu;

    if (analyzer == NULL || analyzer->pointer == NULL)
      return 0;
    fu = (Flite_Utterance *) analyzer->pointer;
    if (phoneme_index < 0 || phoneme_index >= fu->nitem)
      return 0;
    return ffeature_int(fu->items[phoneme_index], "R:SylStructure.parent.R:Syllable.syl_numphones");
}

/* Flite_Text_Analayzer_get_nsyllable_in_word: get number of syllables in word */
int Flite_Text_Analyzer_get_nsyllable_in_word(Flite_Text_Analyzer * analyzer, int phoneme_index)
{
    Flite_Utterance *fu;

    if (analyzer == NULL || analyzer->pointer == NULL)
      return 0;
    fu = (Flite_Utterance *) analyzer->pointer;
    if (phoneme_index < 0 || phoneme_index >= fu->nitem)
      return 0;
    return ffeature_int(fu->items[phoneme_index], "R:SylStructure.parent.parent.R:Word.word_numsyls");
}

/* Flite_Text_Analyzer_get_nword_in_phrase: get number of words in phrase */
int Flite_Text_Analyzer_get_nword_in_phrase(Flite_Text_Analyzer * analyzer, int phoneme_index)
{
    Flite_Utterance *fu;

    if (analyzer == NULL || analyzer->pointer == NULL)
      return 0;
    fu = (Flite_Utterance *) analyzer->pointer;
    if (phoneme_index < 0 || phoneme_index >= fu->nitem)
      return 0;
    return ffeature_int(fu->items[phoneme_index], "R:SylStructure.parent.parent.R:Phrase.parent.lisp_num_words_in_phrase");
}

/* Flite_Text_Analyzer_get_nphrase_in_utterance: get number of phrases in utterance */
int Flite_Text_Analyzer_get_nphrase_in_utterance(Flite_Text_Analyzer * analyzer, int phoneme_index)
{
    Flite_Utterance *fu;

    if (analyzer == NULL || analyzer->pointer == NULL)
      return 0;
    fu = (Flite_Utterance *) analyzer->pointer;
    if (phoneme_index < 0 || phoneme_index >= fu->nitem)
      return 0;
    return ffeature_int(fu->items[phoneme_index], "R:SylStructure.parent.parent.R:Phrase.parent.lisp_total_phrases");
}

/* Flite_Text_Analyzer_get_accent: get accent */
int Flite_Text_Analyzer_get_accent(Flite_Text_Analyzer * analyzer, int phoneme_index)
{
    Flite_Utterance *fu;

    if (analyzer == NULL || analyzer->pointer == NULL)
      return 0;
    fu = (Flite_Utterance *) analyzer->pointer;
    if (phoneme_index < 0 || phoneme_index >= fu->nitem)
      return 0;
    return ffeature_int(fu->items[phoneme_index], "R:SylStructure.parent.R:Syllable.accented");
}

/* Flite_Text_Analyzer_get_stress: get stress */
int Flite_Text_Analyzer_get_stress(Flite_Text_Analyzer * analyzer, int phoneme_index)
{
    Flite_Utterance *fu;

    if (analyzer == NULL || analyzer->pointer == NULL)
      return 0;
    fu = (Flite_Utterance *) analyzer->pointer;
    if (phoneme_index < 0 || phoneme_index >= fu->nitem)
      return 0;
    return ffeature_int(fu->items[phoneme_index], "R:SylStructure.parent.R:Syllable.stress");
}

/* Flite_Text_Analyzer_get_label_data: get label data */
int Flite_Text_Analyzer_get_label_data(Flite_Text_Analyzer * analyzer, 
	char ***label_data, int *label_size)
{
    Flite_Utterance *fu;
    int i;
    cst_item *s;

    if (analyzer == NULL || analyzer->pointer == NULL)
      return -1;
    fu = (Flite_Utterance *) analyzer->pointer;

    if (fu->nitem <= 0) {
        *label_size = 0;
        *label_data = NULL;
        return -2;
    }

    /* save labels */
    (*label_size) = fu->nitem;
    (*label_data) = (char **) calloc(fu->nitem, sizeof(char *));
    for (i = 0, s = relation_head(utt_relation(fu->u, "Segment")); s; s = item_next(s), i++) {
        (*label_data)[i] = (char *) calloc(MAXBUFLEN, sizeof(char));
        create_label(s, (*label_data)[i]);
    }

    return 0;
}

/* Flite_Text_Analyzer_clear: finalize flite front-end */
void Flite_Text_Analyzer_clear(Flite_Text_Analyzer * analyzer)
{
    Flite_Utterance *fu;

    if (analyzer == NULL || analyzer->pointer == NULL)
      return;

    fu = (Flite_Utterance *) analyzer->pointer;
    if (fu->items != NULL)
      free(fu->items);
    if (fu->u != NULL)
      delete_utterance(fu->u);
    if (fu->v != NULL)
      UNREGISTER_VOX(fu->v);
    free(fu);

    analyzer->pointer = NULL;
}

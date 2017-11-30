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


#ifndef FLITE_HTS_ENGINE_H
#define FLITE_HTS_ENGINE_H

#ifdef __cplusplus
#define FLITE_HTS_ENGINE_H_START extern "C" {
#define FLITE_HTS_ENGINE_H_END   }
#else
#define FLITE_HTS_ENGINE_H_START
#define FLITE_HTS_ENGINE_H_END
#endif                          /* __CPLUSPLUS */

FLITE_HTS_ENGINE_H_START;



/* Flite_HTS_Engine_synthesize: synthesize speech */
int Flite_HTS_Engine_synthesize( const char *txt, const char *wav);



typedef struct _Flite_Text_Analyzer {
   void *pointer;
} Flite_Text_Analyzer;

/* Flite_Text_Analyzer_initialize: initialize flite front-end */
void Flite_Text_Analyzer_initialize(Flite_Text_Analyzer * analyzer);

/* Flite_Text_Analyzer_analysis: text analysis */
void Flite_Text_Analyzer_analysis(Flite_Text_Analyzer * analyzer, const char *text);

/* Flite_Text_Analyzer_get_nphoneme_in_utterance: get number of phonemes */
int Flite_Text_Analyzer_get_nphoneme_in_utterance(Flite_Text_Analyzer * analyzer);

/* Flite_Text_Analyzer_get_phoneme: get phoneme identity */
const char *Flite_Text_Analyzer_get_phoneme(Flite_Text_Analyzer * analyzer, int phoneme_index);

/* Flite_Text_Analyzer_get_word: get word */
const char *Flite_Text_Analyzer_get_word(Flite_Text_Analyzer * analyzer, int phoneme_index);

/* Flite_Text_Analyzer_get_nphoneme_in_syllable: get number of phonemes in syllable */
int Flite_Text_Analyzer_get_nphoneme_in_syllable(Flite_Text_Analyzer * analyzer, int phoneme_index);

/* Flite_Text_Analayzer_get_nsyllable_in_word: get number of syllables in word */
int Flite_Text_Analyzer_get_nsyllable_in_word(Flite_Text_Analyzer * analyzer, int phoneme_index);

/* Flite_Text_Analyzer_get_nword_in_phrase: get number of words in phrase */
int Flite_Text_Analyzer_get_nword_in_phrase(Flite_Text_Analyzer * analyzer, int phoneme_index);

/* Flite_Text_Analyzer_get_nphrase_in_utterance: get number of phrases in utterance */
int Flite_Text_Analyzer_get_nphrase_in_utterance(Flite_Text_Analyzer * analyzer, int phoneme_index);

/* Flite_Text_Analyzer_get_accent: get accent */
int Flite_Text_Analyzer_get_accent(Flite_Text_Analyzer * analyzer, int phoneme_index);

/* Flite_Text_Analyzer_get_stress: get stress */
int Flite_Text_Analyzer_get_stress(Flite_Text_Analyzer * analyzer, int phoneme_index);

/* Flite_Text_Analyzer_get_label_data: get label */
int Flite_Text_Analyzer_get_label_data(Flite_Text_Analyzer * analyzer, char ***label_data, int *label_size);

/* Flite_Text_Analyzer_clear: finalize flite front-end */
void Flite_Text_Analyzer_clear(Flite_Text_Analyzer * analyzer);

FLITE_HTS_ENGINE_H_END;

#endif                          /* FLITE_HTS_ENGINE_H */

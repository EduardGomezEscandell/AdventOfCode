#include "day8.h"
#include "common/file_utils.h"
#include <string.h>

char * SkipToOutput(char * const line)
{
	// Returns a pointer to the pipe
	// text | text
	//      ^

	char * it = line;
	for(; *it != '\0'; ++it)
	{
		if(*it == '|') return it;
	}
	return NULL;
}

unsigned int * Find(unsigned int array[4], unsigned int val)
{
	for(size_t i=0; i<4; ++i)
	{
		if(array[i] == val) return array + i;
	}
	return NULL;
}

int SolvePart1(const bool is_test)
{
	
	FILE * file = GetFile(is_test, 8);

	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	unsigned int unique_lengths[] = {2, 3, 4, 7};

	unsigned int count = 0;
	while((read = getline(&line, &len, file)) != -1)
	{
		char * it = SkipToOutput(line);
		
		if(!it)
		{
			fprintf(stderr, "No pipe in line %s\n", line);
			exit(EXIT_FAILURE);
		}
		it += 2; // Skipping to first word

		size_t len = 0;
		for(; *it != '\0'; ++it)
		{
			if(*it == ' ' || *it == '\n') {
				count += Find(unique_lengths, len) ? 1 : 0;
				len = 0;
			} else {
				++len;
			}
		}
	}
	
	fclose(file);
	free(line);

	return count;
}

char * FindWordWithLength(char * line, const size_t len)
{
	size_t l = 0;
	for(char * it = line; *it != '|'; ++it)
	{
		if(*it == ' ')
		{
			if(l == len) return it - len;
			l = 0;
		} else {
			++l;
		}
	}
	return NULL;
}

char FindMismatch(char * word1, char * word2)
{
	bool charset1[7] = {0,0,0,0,0,0,0};
	bool charset2[7] = {0,0,0,0,0,0,0};

	for(char * it=word1; *it != ' ' && *it != '\0'; ++it)
	{
		charset1[*it - 'a'] = true;
	}
	for(char * it=word2; *it != ' ' && *it != '\0'; ++it)
	{
		charset2[*it - 'a'] = true;
	}

	for(size_t i=0; i<7; ++i)
	{
		if(charset1[i] != charset2[i]) return i + 'a';
	}
	return '\0';
}

bool Contains(char * container, char * contained)
{
	bool charset[7] = {0,0,0,0,0,0,0};

	for(char * it=container; *it != ' ' && *it != '\0'; ++it)
	{
		charset[*it - 'a'] = true;
	}
	for(char * it=contained; *it != ' ' && *it != '\0'; ++it)
	{
		size_t i = *it - 'a';
		if(!charset[i]) return false;
	}
	return true;	
}

char IncludeExclude(char * common1, char * common2, char * exclude)
{
	bool charset1[7] = {0,0,0,0,0,0,0};
	bool charset2[7] = {0,0,0,0,0,0,0};
	bool charsete[7] = {0,0,0,0,0,0,0};

	for(char * it=common1; *it != ' ' && *it != '\0'; ++it)
	{
		charset1[*it - 'a'] = true;
	}
	for(char * it=common2; *it != ' ' && *it != '\0'; ++it)
	{
		charset2[*it - 'a'] = true;
	}
	for(char * it=exclude; *it != ' ' && *it != '\0'; ++it)
	{
		charsete[*it - 'a'] = true;
	}

	for(size_t i=0; i<7; ++i)
	{
		if(charset1[i] && charset2[i] && !charsete[i]) return 'a' + i;
	}

	return '\0';
}


char * CopyWord(char * word_begin)
{
	size_t len = 0;
	for(char * c = word_begin; *c!= ' ' && *c!= '\0'; ++c) ++len;

	char * word = malloc((len + 1) * sizeof(char));
	for(size_t i=0; i<len; ++i)
	{
		word[i] = word_begin[i];
	}
	word[len] = '\0';
	return word;
}

void PrintDecoder(Decoder * d)
{
	for(size_t i=0; i<10; ++i)
	{
		printf("%ld: ", i);
		if(d->codes[i] == NULL) printf("?\n");
		else                    printf("%s\n", d->codes[i]);
	}
}

void ClearDecoder(Decoder * d)
{
	for(size_t i=0; i<10; ++i)
	{
		free(d->codes[i]);
		d->codes[i] = NULL;
	}
}

Decoder GetDecoder(char * line)
{
	Decoder d;
	// Uniques
	d.codes[0] = NULL;
	d.codes[1] = CopyWord(FindWordWithLength(line, 2));
	d.codes[2] = NULL;
	d.codes[3] = NULL;
	d.codes[4] = CopyWord(FindWordWithLength(line, 4));
	d.codes[5] = NULL;
	d.codes[6] = NULL;
	d.codes[7] = CopyWord(FindWordWithLength(line, 3));
	d.codes[8] = CopyWord(FindWordWithLength(line, 7));
	d.codes[9] = NULL;

	// Top bar: Present in 7 but not in 1
	
	char top_bar = FindMismatch(d.codes[1], d.codes[7]);
	
	
	// Number three: The only one with 5 segments and fully including the segments in ONE
	
	char * it = FindWordWithLength(line, 5);
	while(!Contains(it, d.codes[1]))
	{
		it = FindWordWithLength(it + 6, 5);
	}
	d.codes[3] = CopyWord(it);

	
	// Middle bar: Is in THREE and FOUR, is not it ONE
	
	char middle_bar = IncludeExclude(d.codes[3], d.codes[4], d.codes[1]);

	
	// Top left: Is in FOUR, is not in ONE, is not middle bar
	
	char four_not_middle[4];
	size_t i = 0;
	for(char *c = d.codes[4]; *c != '\0'; ++c)
	{
		if(*c != middle_bar)
		{
			four_not_middle[i] = *c;
			++i;
		}
	}
	four_not_middle[3] = '\0';
	char topleft_bar = FindMismatch(four_not_middle, d.codes[1]);
	

	// FIVE: contains top, topleft and middle, has 5 segments
	{
		char search[4] = "xxx";
		sprintf(search, "%c%c%c", top_bar, topleft_bar, middle_bar);

		it = FindWordWithLength(line, 5);
		while(!Contains(it, search))
		{
			it = FindWordWithLength(it + 6, 5);
		}
		d.codes[5] = CopyWord(it);
	}

	// ZERO: Contains all except the middle bar
	{
		char search[2] = "x";
		sprintf(search, "%c", middle_bar);

		it = FindWordWithLength(line, 6);
		while(Contains(it, search))
		{
			it = FindWordWithLength(it + 7, 6);
		}
		d.codes[0] = CopyWord(it);
	}

	// TWO: Has 5 segments, is not 3 or 5
	{
		it = FindWordWithLength(line, 5);
		while(Contains(it, d.codes[3]) || Contains(it, d.codes[5]))
		{
			it = FindWordWithLength(it + 6, 5);
		}
		d.codes[2] = CopyWord(it);
	}

	// NINE: Has 6 segments, fully contains ONE, contains middle bar
	{
		char search[] = "x";
		sprintf(search, "%c", middle_bar);
		it = FindWordWithLength(line, 6);
		while(!Contains(it, d.codes[1]) || !Contains(it, search))
		{
			it = FindWordWithLength(it + 7, 6);
		}
		d.codes[9] = CopyWord(it);
	}

	// SIX: Has 6 segments, does not fully contain ONE
	{
		it = FindWordWithLength(line, 6);
		while(Contains(it, d.codes[1]))
		{
			it = FindWordWithLength(it + 7, 6);
		}
		d.codes[6] = CopyWord(it);
	}

	return d;
}

int DecodeWord(Decoder *dec, char * begin)
{
	for(size_t i = 0; i<10; ++i)
	{
		if(FindMismatch(dec->codes[i], begin) == '\0') return i;
	}
	
	fprintf(stderr, "Failed to decode word %s, begin\n", begin);
	return -1;
}

int Decode(Decoder * dec, char * line)
{
	char * output = SkipToOutput(line) + 2;

	char * begin = output;
	int result = 0;
	for(char *end = output; *end != '\0'; ++end)
	{
		if(*end == ' ' || *end == '\n')
		{
			*end = '\0';
			int x = DecodeWord(dec, begin);
			result = result*10 + x;
			begin = end+1;
		}
	}
	return result;
}


int SolvePart2(const bool is_test)
{
	FILE * file = GetFile(is_test, 8);

	char * line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	int total = 0;
	while((read = getline(&line, &len, file)) != -1)
	{
		Decoder dec = GetDecoder(line);
		// PrintDecoder(&dec);
		total += Decode(&dec, line);
		ClearDecoder(&dec);
	}

	free(line);
	fclose(file);

	return total;
}
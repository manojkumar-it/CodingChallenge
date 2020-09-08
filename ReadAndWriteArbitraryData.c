// /////////////////////////////////////////////////////////////////////////////////////////////////
// Program to read and write arbitrary data
// /////////////////////////////////////////////////////////////////////////////////////////////////
//
// Write a program in C that reads arbitrary data from standard input and writes it to a file. 
// By arbitrary data, we mean that we don’t want to put any limitations or restrictions whatsoever 
// on the data a user provides via standard input. 
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// OS verified: Windows, Linux
// Compilers verified: cl for Windows, gcc for Linux
// /////////////////////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <signal.h>

#include <uchar.h>

#define HUNDRED_KILO_BYTE 102400

#define FREE(x) { if(x) free(x); }
#define FCLOSE(x) { if(x) fclose(x); }	

// Handle different encoding formats

enum Encoding
{
	DEFAULT,
	ASCII,
	UTF_7,
	UTF_8,
	UTF_16LE,
	UTF_16BE,
	UTF_32LE,
	UTF_32BE
};

// Takes file name as input and returns the encoding format of a text file.
// It reads the first 4 bytes of the text file to get the BOM (Byte Order Mark).
// If a text file does not contain BOM, the function assumes the file is ASCII (might not be really) and returns the same.  
// This function should be used only for getting the encoding of text files.
int getTextFileEncoding(const char *pFileName)
{
	if(!pFileName)
	{
		printf("NULL pointer passed as paramter for file name\n");
		exit(EXIT_FAILURE);
	}
	
	FILE *pFile = fopen(pFileName, "rb");
	if(!pFile)
	{
		printf("fopen failed\n");
		exit(EXIT_FAILURE);
	}
		
	unsigned char bom[4];
	size_t size = 0;
	
	if (size = fread(bom,sizeof(bom[0]),4,pFile))
	{
		FCLOSE(pFile);
		
		// Analyze the BOM
		if (bom[0] == 0x2b && bom[1] == 0x2f && bom[2] == 0x76) 
		{
			printf("The encoding format of '%s' file is UTF-7\n", pFileName);
			return UTF_7;
		}
		if (bom[0] == 0xef && bom[1] == 0xbb && bom[2] == 0xbf)
		{
			printf("The encoding format of '%s' file is UTF-8\n", pFileName);
			return UTF_8;
		}
		if (bom[0] == 0xff && bom[1] == 0xfe && bom[2] == 0 && bom[3] == 0)
		{
			printf("The encoding format of '%s' file is  UTF-32LE\n", pFileName);
			return UTF_32LE;
		}			
		if (bom[0] == 0xff && bom[1] == 0xfe)
		{
			printf("The encoding format of '%s' file is UTF-16LE\n", pFileName);
			return UTF_16LE;
		}			
		if (bom[0] == 0xfe && bom[1] == 0xff)
		{
			printf("The encoding format of '%s' file is UTF-16BE\n", pFileName);
			return UTF_16BE;
		}			
		if (bom[0] == 0 && bom[1] == 0 && bom[2] == 0xfe && bom[3] == 0xff)
		{
			printf("The encoding format of '%s' file is UTF_32LE\n", pFileName);
			return UTF_32LE; 
		}
	}
	
	if(ferror(pFile))
	{
		printf("Read Failed\n");
		FCLOSE(pFile);
		exit(EXIT_FAILURE);
	}
	
	// We actually have no idea what the encoding is if we reach this point, so
	// you may wish to return null instead of defaulting to ASCII
	return ASCII;
}

//Returns the string length of a UTF-16 string
int strlen16(const char16_t* strarg)
{
   if(!strarg)
     return -1; //strarg is NULL pointer
   char16_t* str = (char16_t*)strarg;
   for(;*str;++str)
     ; // empty body
   return str-strarg;
}

static inline size_t _strlen16(register const char16_t * string) {
    if (!string) return 0;
    register size_t len = 0;
    while(string[len++]);
    return len;
}

//Pointer arrays must always include the array size, because pointers do not know about the size of the supposed array size.
void utf8_to_utf16(unsigned char* const utf8_str, int utf8_str_size, char16_t* utf16_str_output, int utf16_str_output_size) {
	//First, grab the first byte of the UTF-8 string
	unsigned char* utf8_currentCodeUnit = utf8_str;
	char16_t* utf16_currentCodeUnit = utf16_str_output;
	int utf8_str_iterator = 0;
	int utf16_str_iterator = 0;

	//In a while loop, we check if the UTF-16 iterator is less than the max output size. If true, then we check if UTF-8 iterator
	//is less than UTF-8 max string size. This conditional checking based on order of precedence is intentionally done so it
	//prevents the while loop from continuing onwards if the iterators are outside of the intended sizes.
	while (*utf8_currentCodeUnit && (utf16_str_iterator < utf16_str_output_size || utf8_str_iterator < utf8_str_size)) {
		//Figure out the current code unit to determine the range. It is split into 6 main groups, each of which handles the data
		//differently from one another.
		if (*utf8_currentCodeUnit < 0x80) {
			//0..127, the ASCII range.

			//We directly plug in the values to the UTF-16 code unit.
			*utf16_currentCodeUnit = (char16_t) (*utf8_currentCodeUnit);
			utf16_currentCodeUnit++;
			utf16_str_iterator++;

			//Increment the current code unit pointer to the next code unit
			utf8_currentCodeUnit++;

			//Increment the iterator to keep track of where we are in the UTF-8 string
			utf8_str_iterator++;
		}
		else if (*utf8_currentCodeUnit < 0xC0) {
			//0x80..0xBF, we ignore. These are reserved for UTF-8 encoding.
			utf8_currentCodeUnit++;
			utf8_str_iterator++;
		}
		else if (*utf8_currentCodeUnit < 0xE0) {
			//128..2047, the extended ASCII range, and into the Basic Multilingual Plane.

			//Work on the first code unit.
			char16_t highShort = (char16_t) ((*utf8_currentCodeUnit) & 0x1F);

			//Increment the current code unit pointer to the next code unit
			utf8_currentCodeUnit++;

			//Work on the second code unit.
			char16_t lowShort = (char16_t) ((*utf8_currentCodeUnit) & 0x3F);

			//Increment the current code unit pointer to the next code unit
			utf8_currentCodeUnit++;

			//Create the UTF-16 code unit, then increment the iterator
			int unicode = (highShort << 8) | lowShort;

			//Check to make sure the "unicode" is in the range [0..D7FF] and [E000..FFFF].
			if ((0 <= unicode && unicode <= 0xD7FF) || (0xE000 <= unicode && unicode <= 0xFFFF)) {
				//Directly set the value to the UTF-16 code unit.
				*utf16_currentCodeUnit = (char16_t) unicode;
				utf16_currentCodeUnit++;
				utf16_str_iterator++;
			}

			//Increment the iterator to keep track of where we are in the UTF-8 string
			utf8_str_iterator += 2;
		}
		else if (*utf8_currentCodeUnit < 0xF0) {
			//2048..65535, the remaining Basic Multilingual Plane.

			//Work on the UTF-8 code units one by one.
			//If drawn out, it would be 1110aaaa 10bbbbcc 10ccdddd
			//Where a is 4th byte, b is 3rd byte, c is 2nd byte, and d is 1st byte.
			char16_t fourthChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			utf8_currentCodeUnit++;
			char16_t thirdChar = (char16_t) ((*utf8_currentCodeUnit) & 0x3C) >> 2;
			char16_t secondCharHigh = (char16_t) ((*utf8_currentCodeUnit) & 0x3);
			utf8_currentCodeUnit++;
			char16_t secondCharLow = (char16_t) ((*utf8_currentCodeUnit) & 0x30) >> 4;
			char16_t firstChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			utf8_currentCodeUnit++;

			//Create the resulting UTF-16 code unit, then increment the iterator.
			int unicode = (fourthChar << 12) | (thirdChar << 8) | (secondCharHigh << 6) | (secondCharLow << 4) | firstChar;

			//Check to make sure the "unicode" is in the range [0..D7FF] and [E000..FFFF].
			//According to math, UTF-8 encoded "unicode" should always fall within these two ranges.
			if ((0 <= unicode && unicode <= 0xD7FF) || (0xE000 <= unicode && unicode <= 0xFFFF)) {
				//Directly set the value to the UTF-16 code unit.
				*utf16_currentCodeUnit = (char16_t) unicode;
				utf16_currentCodeUnit++;
				utf16_str_iterator++;
			}

			//Increment the iterator to keep track of where we are in the UTF-8 string
			utf8_str_iterator += 3;
		}
		else if (*utf8_currentCodeUnit < 0xF8) {
			//65536..10FFFF, the Unicode UTF range

			//Work on the UTF-8 code units one by one.
			//If drawn out, it would be 11110abb 10bbcccc 10ddddee 10eeffff
			//Where a is 6th byte, b is 5th byte, c is 4th byte, and so on.
			char16_t sixthChar = (char16_t) ((*utf8_currentCodeUnit) & 0x4) >> 2;
			char16_t fifthCharHigh = (char16_t) ((*utf8_currentCodeUnit) & 0x3);
			utf8_currentCodeUnit++;
			char16_t fifthCharLow = (char16_t) ((*utf8_currentCodeUnit) & 0x30) >> 4;
			char16_t fourthChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			utf8_currentCodeUnit++;
			char16_t thirdChar = (char16_t) ((*utf8_currentCodeUnit) & 0x3C) >> 2;
			char16_t secondCharHigh = (char16_t) ((*utf8_currentCodeUnit) & 0x3);
			utf8_currentCodeUnit++;
			char16_t secondCharLow = (char16_t) ((*utf8_currentCodeUnit) & 0x30) >> 4;
			char16_t firstChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			utf8_currentCodeUnit++;

			int unicode = (sixthChar << 4) | (fifthCharHigh << 2) | fifthCharLow | (fourthChar << 12) | (thirdChar << 8) | (secondCharHigh << 6) | (secondCharLow << 4) | firstChar;
			char16_t highSurrogate = (unicode - 0x10000) / 0x400 + 0xD800;
			char16_t lowSurrogate = (unicode - 0x10000) % 0x400 + 0xDC00;

			//Set the UTF-16 code units
			*utf16_currentCodeUnit = lowSurrogate;
			utf16_currentCodeUnit++;
			utf16_str_iterator++;

			//Check to see if we're still below the output string size before continuing, otherwise, we cut off here.
			if (utf16_str_iterator < utf16_str_output_size) {
				*utf16_currentCodeUnit = highSurrogate;
				utf16_currentCodeUnit++;
				utf16_str_iterator++;
			}

			//Increment the iterator to keep track of where we are in the UTF-8 string
			utf8_str_iterator += 4;
		}
		else {
			//Invalid UTF-8 code unit, we ignore.
			utf8_currentCodeUnit++;
			utf8_str_iterator++;
		}
	}

	//We clean up the output string if the UTF-16 iterator is still less than the output string size.
	while (utf16_str_iterator < utf16_str_output_size) {
		*utf16_currentCodeUnit = '\0';
		utf16_currentCodeUnit++;
		utf16_str_iterator++;
	}
}

// For handling signals
void signalHandler(int signum)
{
   printf("Caught signal %d\n",signum);
   // Cleanup and close up stuff here

   // Terminate program
   //exit(signum);
}

// Returns a pointer to the extension of 'string'.
// If no extension is found, returns a pointer to the end of 'string'. 
char* getFileExtension(const char *pFileName)
{
    assert(pFileName != NULL);
    char *pFileExtension = strrchr(pFileName, '.');
 
    if (pFileExtension == NULL)
        return (char *) pFileName + strlen(pFileName);
 
    for (char *pIterator = pFileExtension + 1; *pIterator != '\0'; pIterator++)
	{
        if (!isalnum((unsigned char)*pIterator))
            return (char *) pFileName + strlen(pFileName);
    }
 
    return pFileExtension;
}

// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char * trimLeadingAndTrailingWhiteSpaces(char *pString)
{
	char *pLastChar;
	
	// Trim leading spaces
	while(isspace((unsigned char)*pString))
		pString++;
	
	// if input string has only White spaces, we can simply return here 
	if(NULL == pString)	
		return pString;
		
	// Trim trailing spaces
	pLastChar = pString + strlen(pString) - 1;
	while(pLastChar > pString && isspace((unsigned char)*pLastChar))
		pLastChar--;
	
  // Write null terminator after the last character 
  *(pLastChar+1) = '\0';

  return pString;
}

// Returns true if file name string contains valid characters 
// Returns false if invalid characters are found in file name string
bool isValidFileName(char *pFileName)
{
	char invalidCharacters[] = "<> :\"/\\|?*";
	bool invalidCharFound = false;
	
	for (int i = 0; i < strlen(invalidCharacters); ++i)
	{
		if (strchr(pFileName, invalidCharacters[i]) != NULL)
		{
			printf("Characters <>:\"/\\|?*] are not allowed in file name. ");
			invalidCharFound = true;
			break;
		}
	}
	return !invalidCharFound;
}

// Returns true if given file exists
// Returns false if given file not found
bool isFileExists(char *pFileName) {
	
	if(NULL == pFileName)
		return false;
	
	struct stat buffer;
	bool fileExists = (stat(pFileName, &buffer) == 0);
		
	return fileExists;
}

// Gets file name string from standard input and returns a pointer to the dynamically allocated string.
// Clients of this function should should take care of deallocating memory by calling free  
char * getFileName(const char *pFileDescription)
{
	char *pFileName= (char *)malloc(FILENAME_MAX); 
	printf("Please enter a valid '%s' name (Max %d characters only): ", pFileDescription, FILENAME_MAX);
	fgets(pFileName, FILENAME_MAX, stdin);
	
	//fegets adds a new line character at the end, so using strchr to find and replace it with '\0' 
	char *pNewLinePosition=strchr(pFileName, '\n');
	if (NULL != pNewLinePosition)
		*pNewLinePosition = '\0';
	
	return pFileName;
}

// Returns a string with output file name
// If the extension of output file name provided by user doesn't match with the input parameter 'pFileExtension', a warning is displayed to the user.
char * getOutputFileName(const char *pFileExtension)
{
	char *pOutputFileName = NULL, *pOutputFileExt = NULL;
	
	do
	{
		pOutputFileName=getFileName("Output file");
	} while(!isValidFileName(pOutputFileName));
	
	pOutputFileExt = getFileExtension(pOutputFileName); //Do not free pOutputFileExt(stored in stack memory not heap) as it is only a substring of pOutputFileName
	
	if(pFileExtension)
	{
		if(strcmp(pOutputFileExt,pFileExtension) != 0)
		{
			printf("Warning: Output file might become unusable as ouput file extension '%s' is not matching with input file extension '%s'\n",pOutputFileExt,pFileExtension);
		}
	}
	
	return pOutputFileName;
}

// Returns a string with input file name
// if the given file name doesn't exists, users will be prompted to provide a valid file.
// the function returns only when a valid file name is provided.
char * getInputFileName()
{
	char *pInputFileName = NULL;
	
	do
	{
		pInputFileName = getFileName("existing input file");
	} while(!isFileExists(pInputFileName));
	
	return pInputFileName;
}

// Reads input data from input stream and writes the same to the output stream
// If read/write failed, the program terminates wih exit code 1; 
void writeData(FILE *pInputStream, FILE *pOutputStream, bool encodingConversionRequired, int textFileEncodingFormat)
{
	char buffer[HUNDRED_KILO_BYTE];
	memset(buffer,'\0',sizeof(buffer));
	size_t size;
	char fileAccessMode[4];
	memset(fileAccessMode, '\0',sizeof(fileAccessMode));
	while (size = fread(buffer, sizeof(buffer[0]), HUNDRED_KILO_BYTE, pInputStream))
	{
		if(encodingConversionRequired)
		{
			switch(textFileEncodingFormat)
			{
				case UTF_16LE:
				{
					char16_t output[2 * HUNDRED_KILO_BYTE];
					utf8_to_utf16(buffer, sizeof(buffer) / sizeof(buffer[0]), &output[0], sizeof(output) / sizeof(output[0]));
					printf("strlen of utf16 str is : '%d'\n",strlen16(output));
					size = fwrite(output, sizeof(output[0]), strlen16(output), pOutputStream);
					break;
				}
				case UTF_8:
				{
					fwrite(buffer, sizeof(buffer[0]),size, pOutputStream);
					break;
				}
				default:
				{
					printf("Error: Output file has a encoding format which is currently not supported !\n");
					// Add code to free up resources before exiting
					exit(EXIT_FAILURE);
					break;
				}
			}
		}
		else
		{
			fwrite(buffer, sizeof(buffer[0]),size, pOutputStream);
		}
		
		if(ferror(pOutputStream))
		{
			printf("Write Failed\n");
			
			FCLOSE(pOutputStream);
			FCLOSE(pInputStream);
			
			exit(EXIT_FAILURE);
		}
	}
	
	if(ferror(pInputStream))
	{
		printf("Read Failed\n");
		fclose(pInputStream);
		exit(EXIT_FAILURE);
	}
	
	printf("Success! Write Data Completed.\n");
}

int main()
{
	char *pInputFileName = NULL, *pOutputFileName = NULL, *pInputFileExt = NULL;
	FILE *pInputStream = NULL, *pOutputStream = NULL;
		
	char c;
	char str[256];
	
	// Register signal and signal handler
	signal(SIGINT, signalHandler);
	
	do
	{
		printf("Enter 0 to read from console (standard input) or 1 to provide input file name:\n");
		scanf("%s",str);
		
		// To ignore the additional newline character from previous scanf() call
		getchar();
		
		c=str[0];
		
		switch (c)
		{
		case '1':
			pInputFileName = getInputFileName();
			if(!pInputFileName)
			{
				printf("Error: Unable to get Input File Name.\n");
				//Free resources before exiting
				exit(EXIT_FAILURE);
			}
			
			pInputStream = fopen(pInputFileName, "rb");
			//Handle Error

			break;
		case '0':
			pInputStream = stdin;
			break;
		default:
			printf("Invalid input! ");
			break;
		}
	} while (c != '1' && c != '0');
	
	if(pInputFileName)
		pInputFileExt = getFileExtension(pInputFileName); //Do not free pInputFileExt ((stored in stack memory not heap)) as it is only a substring of pInputFileName
	
	if(!pInputStream)
	{
		printf("Error: Input File Stream pointer is NULL\n");
		//Add code to Free resources before exiting		
		exit(EXIT_FAILURE);
	}
	
	pOutputFileName	= getOutputFileName(pInputFileExt);
	//Handle Error
	if(!pOutputFileName)
	{
		printf("Error: Unable to get Output File Name.\n");
		//Add code to free resources before exiting
		exit(EXIT_FAILURE);
	}
	
	bool outputFileExists = false;
	if(isFileExists(pOutputFileName))
		outputFileExists = true;
	
	// Data read from stdin will always be UTF-8 encoded as this source code file is UTF-8 encoded.
	int encodingFormat = DEFAULT;
	bool encodingConversionRequired = false;
	if( stdin == pInputStream)
	{
		// If output file is already present and has a different encoding format other than UTF-8, we need to convert the UTF-8 data in to output file's encodig format before writing the data to output file.     
		if(outputFileExists)
		{
			encodingFormat = getTextFileEncoding(pOutputFileName);
			
			printf("Output File with the same name already exists. Press '1' to overwrite or press '2' to append or any other key to exit\n");
			c=getchar();
			if ('1' == c)
			{
				pOutputStream = fopen(pOutputFileName,"wb");
			}
			else if('2' == c)
			{
				switch(encodingFormat)
				{
					case UTF_16LE:
					case UTF_8:
						printf("The encoding format of the given output file is supported for appending\n");
						break;
					default:
					{
						printf("Currently, files having UTF-16LE and UTF-8 encoding with BOM are only supported for appending.\n");
						printf("Exiting...\n");
						//Free up resources before exiting
						exit(EXIT_FAILURE);
					}
				}
				pOutputStream = fopen(pOutputFileName,"ab");		
				encodingConversionRequired = true;
			}
			else
			{
				// graceful exit as user opted it, so return success.
				//Add code to free up resources before exiting
				printf("Exiting...\n");
				exit(EXIT_SUCCESS);	
			}
		}
		else
		{
			pOutputStream = fopen(pOutputFileName,"wb");
		}
		printf("Enter the input data (press ctrl+z twice & press 'Enter' to process input on Windows. on Linux press ctrl+D ): \n");
	}
	else
	{
		if(outputFileExists)
		{
			printf("Output File with the same name already exists. Press 'Y' if you want to overwrite the existing file or press any key other key to exit.\n");
			c=getchar();
			
			if( (c != 'Y') && (c != 'y') )
			{
				// graceful exit as user opted it, so return success.

				//Add code to free up resources before exiting
				printf("Char: '%c' 	Exiting...\n",c);
				exit(EXIT_SUCCESS);
			}
		}
		pOutputStream = fopen(pOutputFileName,"wb");
		//writeData(pInputStream, pOutputStream, encodingConversionRequired, DEFAULT);
	}
	
	if(!pOutputStream)
	{
		printf("Error: Output File Stream pointer is NULL\n");
		//Add code to Free resources before exiting
		exit(EXIT_FAILURE);
	}
	
	writeData(pInputStream, pOutputStream, encodingConversionRequired, encodingFormat);
		
	FREE(pInputFileName);
    FREE(pOutputFileName);
	
	FCLOSE(pInputStream);
	FCLOSE(pOutputStream);
	
	printf("Done\n");	
	
    return EXIT_SUCCESS;
}

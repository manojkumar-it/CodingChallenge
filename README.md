# CodingChallenge
NortonLifeLock Labs Team Coding Challenge:

"Write a program in C (not C++) that reads arbitrary data from standard input and writes it to a file. 
By arbitrary data, we mean that we don’t want to put any limitations or restrictions whatsoever on the data a user provides via standard input. 

To Do List:
======================

Handling output:
	- Get the output file name from the user.
	- Check output file name and path provided by the user is valid 
		  Check if the filename contains only supported characters (OS specific)
		  Check if the path to the output file exists. If path does not exist, prompt the user to provide a valid path.
		  If output file with the same name exists, prompt the user to provide a different name.
      If the extension of output file name provided by user doesn't match with the input parameter 'pFileExtension', a warning is displayed to the user.
	- Check for insufficient storage space while writing to file
		  Alert the user when storage space is very less
	- Output file encoding format

Handling Input:
	- Handle UNICODE characters (Arbitrary characters)
	- Handle control characters (\t \n \r \r\n \\ \' )
	- Handle interrupts (ctrl+Z, ctrl+C, etc.) and signals (SIGSEGV, SIGABRT, etc.) before exiting from the program.
	- handle input data of any length
	- Optimize read/write for large inputs
	- Reading data from stdin
			character-by-character vs line-by-line vs chunk-by-chunk

Error handling:
	- Handle all possible errors

cross platform support (portable code):
	- OS(Windows, Linux, Solaris, etc.)
	- Processor & Endianess dependency (Support both Big & Little Endlian processor while reading/writing data) 

Other pending Items:
Handle Input File and output File extension mismatch
Handle writing data to a existing file with a different encoding format other than input file encoding format

# CodingChallenge (NortonLifeLock Labs Team)

Write a program in C (not C++) that reads arbitrary data from standard input and writes it to a file. 
By arbitrary data, we mean that we don’t want to put any limitations or restrictions whatsoever on the data a user provides via standard input. 

### TEST CASES
|TEST CASE CATEGORY | TEST CASE DESCRIPTION | STATUS |
|------------------ | --------------------- | ------ |
|| Provide a choice to the user for reading input data from 1. Console (Standard input) 2. Existing input File | Done
|| Provide support for reading data from standard input | Done
||Provide support for reading data from an existing input file | Done
||Check whether the input file provided by user exists.| Done 
||If the input file does not exists, prompt the user to provide a valid file name. Repeat prompting for input file name, until a valid input file is provided. | Done
||Provide support for reading files containing Non-ASCII (Unicode) characters. | Done
||Provide support for 'reading from/writing to' files with any extension (*.*) i.e. provide support for both text (.TXT, .log, .html) and binary files(.JPEG, .MP4, .EXE) | Done
||provide support for getting output file name from users |	Done
||If user specified file name has unsupported characters, prompt repeatedly until the user provides a valid output file name. | Done
||If output file with given name already exists, provide a choice to the user.1. To overwrite the file 2. To append the input data to the existing file. 3. To exit the program	|Done
||Provide support for writing Non-ASCII (Unicode) characters to files. | Done
||Support for appending to existing files having UTF-8 and UTF-16LE with BOM | Done
||Provide block-by-block reading and writing support for optimal performance. | Done
||Print a warning message in the console, when the extension of output file doesn't match the input file extension, but continue with the program execution. Example: When input file has '.JPEG' extension and output file extension is '.TXT', a warning will be printed, but the program will continue and the output file will be generated with the user specified extension.| Done
|| NULL pointer checks | Done
|| Read/Write failed | Done
|| Unsupported encoding format for appending to output file	| Done
||Free resources before exiting in case of failures	| Partially done
||Processor & Endianness (Full support is only available for Little Endian machines) | Partially done
||OS (Windows, Linux, Solaris, etc.) - Supports any operating system as only standard "C" headers functions used for coding |Done
||provide support for file name with non-ASCII characters	| Open
||Support for appending to existing files having encoding formats other than UTF-8 BOM and UTF-16LE BOM	| Open
||Handle the scenario when both the input and output files are same. | Open
||Alerting the user when write is failed because of insufficient disk space	| Open
||Handling interrupts/signals | Open
||Processor & Endianness dependency (Support Big Endian processor while reading/writing data) | Open
||Support for storing output files in user specified location (Currently, the output file is stored in the same path where the executable file is run)	| Open
||Display appropriate/specific error message to the user when access to output file is denied because of read only permissions. Currently, a generic message "output file stream pointer is NULL." for any failure during fopen() call. | Open
||Segregating source code in to multiple headers and implementation files | Open
||Thread safe code in case of a multi-threaded application. Ex: Two threads trying to write to the same output file. | Open

### Test Execution Details

| Environment & other parameters |  Description | Status|
| --- | ----------------------------- | -------- |
| OS | Windows 10 x64, Linux (Ubuntu)| verified |
| Compiler | cl for Windows, gcc for Linux | verified |
| Locales | English, Japanese | verified
| Input file types | (.txt, .c, .html, .log, .jpg, .mp4, etc.) | verified |

### Reference Links
[https://en.cppreference.com/w/]
[https://docs.microsoft.com/en-us/cpp]
[https://unicodebook.readthedocs.io/good_practices.html]
[https://developer.ibm.com/technologies/systems/articles/au-endianc/]
[https://c-for-dummies.com/blog/?p=2605]
[https://stackoverflow.com/]

### About NortonLifeLock Labs 
The NortonLifeLock Labs team is made up of leading threat and security researchers supported by advanced systems to innovate security technology and threat intelligence to protect our customers against known and new threats. The main locations of the team are Chennai, India – Oslo, Norway – Dublin, Ireland and Culver City, USA. The Labs team is part of the CTO office. 
 
The members in this team overlook the portfolio of security technologies that makes for high detection without sacrificing system performance. The portfolio consists of modern and traditional components, like: 
*	Network detection 
*	Reputation and prevalence 
*	Machine Learning 
*	Cloud and Crowd detections 
*	Behavioural analysis 
*	Anti-virus engines and static scanners 
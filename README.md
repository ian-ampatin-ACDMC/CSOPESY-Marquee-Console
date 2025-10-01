# CSOPESY - Marquee Console

Developer: Ampatin, Ian Kenneth J.</br>
Last Update: 2025 - 10 - 01

Entry Class File: commandInterpreter.cpp

## Running the Program
### Compile and Run on your Local Machine with Visual Studio
1.	Open a subdirectory for the GitHub repository. </br>
    The path should look like this when you view it via the terminal: ``C:\User\[YOUR USERNAME\source\repos`` or
	``C:\User\[YOUR USERNAME]\Documents\repos``</br>
	If you already have a subdirectory for the repository, simply open the path to it.

1.	Clone the repository into the subdirectory with your preferred method.
	 
	1.	Using your SSH key </br>
		``git clone [YOUR SSH URL]`` </br> If it is the first time you are using the SSH key, type *yes*.
		Enter your SSH key password if you have set one up.
		
1.	Close the terminal.
1.	Open Visual Studio.
1.	Select **Open Project or Solution**.
1.	Open the repository and select the *[APPLICATION NAME].sln* file.
	
### Run the program with the executable only (Windows)
1.	Open a subdirectory for the GitHub repository. </br>
    The path should look like this when you view it via the terminal: ``C:\User\[YOUR USERNAME\source\repos`` or
	``C:\User\[YOUR USERNAME]\Documents\repos``</br>
	If you already have a subdirectory for the repository, simply open the path to it.

1.	Clone the repository into the subdirectory with your preferred method.
	 
	1.	Using your SSH key </br>
		``git clone [YOUR SSH URL]`` </br> If it is the first time you are using the SSH key, type *yes*.
		Enter your SSH key password if you have set one up.
		
1.	Type the following commands into the terminal.
	```
	cd [APPLICATION NAME]\x64\Release\
	start [APPLICAITON NAME].exe
	```

## The "config.txt" file
The *config.txt* file contains the default values and fixed parameters of the program.
There are three parameters that can be edited via this file.

- The width of the marquee, help function, and the default information text. **[ generalDisplayWidth ]**
- The maximum number of characters a command can have before being printed to the next line. **[ commandDisplayWidth ]**
- The default speed of the marquee animation. **[ defaultMarqueeSpeed ]**

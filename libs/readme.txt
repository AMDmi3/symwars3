

includes:

$(SolutionDir)\libs\jpeglib
$(SolutionDir)\libs\lpng
$(SolutionDir)\libs\libzip-0.11.1\lib
$(SolutionDir)\libs\

libs:

$(SolutionDir)\libs\jpeglib
$(SolutionDir)\libs\lpng\lib
$(SolutionDir)\libs\libzip-0.11.1\build\lib\Release
$(SolutionDir)\libs\zlib-1.2.5\build\Release
$(SolutionDir)\libs\GL



Building the libraries on Windows
=================================

When building with Visual Studio 2012 on Windows 7+, 
it is important to have the Windows XP toolset so that 
XP users can play the game. 

http://blogs.msdn.com/b/vcblog/archive/2012/11/26/visual-studio-2012-update-1-now-available.aspx

You need to have the XP toolset selected in all the
projects' settings.


jpeglib
-------

You can follow these instructions: 
http://bugbeebox.com/2012/11/18/building-libjpeg-8d-with-msvc-2012/

A copy is included in /doc/ 
(Building Static libjpeg 8d with MSVC 2012   BugbeeBox( void ).htm).

Needless to say you need to have Visual Studio 2012 installed.

Install MS Windows 7 SDK. 

http://www.microsoft.com/en-us/download/details.aspx?id=8279

Run "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat"
with quotes included.

Run "set INCLUDE=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include;%INCLUDE%"
to add an environment variable to include Win32.mak. This 
will only effect the current session of the Command Prompt 
and you will need to do this again if you restart the
Command Prompt.

Set a PATH variable to 
"C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin"
so you can use nmake in the command prompt.

On Windows 7 go to Control Panel -> System and Security ->
System -> Advanced system settings -> Environment Variables

And in system variables scroll down until you see "Path".
Click edit. Copy and paste this to the end:

;C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin

It's important to have that ; to seperate it from the other
entries.

Next open the Command Prompt and run nmake. If it runs, that 
means you set up the path correctly. 

"cd" to the jpeg-8d directory and there run:

nmake /f makefile.vc setup-v11

Then follow the instructions from "Building" on the 
previously mentioned website. Make sure to set the 
Windows XP toolset in Project Properties ->
General -> Platform Toolset.


zlib
----

Follow the instructions on 
http://bugbeebox.com/2012/12/29/building-static-zlib-v1-2-7-with-msvc-2012/

A copy is included in /doc
(Building Static zlib v1.2.7 with MSVC 2012   BugbeeBox( void ).htm).

Make sure to set all the projects to use the Windows XP toolset.




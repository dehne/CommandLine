# UserInput

## An Arduino library for a simple command line UI over a Stream (Serial, by default)

A command line is a sequence of ascii characters terminated by a '\r'. It is subdivided into "words" separated by ' ' or '\t' characters. The first word in a command line is the name of the command. The rest are parameters.

The basic usage pattern here is to instantiate a global UserInput object. Pass it the Stream it is to use for communication, if something other than Serial is wanted. Then, in the sketch's setup() function, to attach the default command handler function (called for unrecognized commands) and the handler functions, one for each command. Later, during execution, the handlers will be invoked to process their corresponding commands. Finally, in the sketch's loop() function, to call the UserInput object's run() function repeatedly. 

The run function reads from the Stream as bytes become available, buffering them and returning, until a '\r' is received. When run () sees a '\r' it looks at the first white-space-delimited word to determine which command the user has entered, and invokes the corresponding handler, if one exists. If none exists, run() invokes the default command handler. When the handler returns, the received user input is forgotten about, and the process repeats.

Command handlers can use the UserInput object's getWord() member function to gain convenient access to the individual words that make up the command line, or the getCommandLine() member function to retrieve the whole command line. Because getWord() returns a String, handlers can use the String member functions for further processing. For example, 
 
```
    int16_t amount = ui.getWord(1).toInt();
```

would retrieve the first parameter as an integer.

### Copyright and License

UserInput V1.0.0, October 2022
Copyright (C) 2020 - 2022 D.L. Ehnebuske

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. 
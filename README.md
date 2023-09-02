# CmdLine

## An Arduino library for a simple command line UI over a Stream (Serial, by default)

This file is a portion of the package CommandLine, a library that provides an Arduino sketch with the ability to provide a simple command line UI over a Stream (Serial, by default). It is a refactored replacement for my library, UserInput, which is no longer being maintained.

A command line is a sequence of ascii characters terminated by a '\r'. It is subdivided into "words" separated by ' ' or '\t' characters. The first word in a command line is the name of the command. The rest are parameters.

The basic usage pattern here is to instantiate a global CommandLine object. Pass it the Stream it is to use for communication, if something other than Serial is wanted. Then, in the sketch's setup() function, to attach the default command handler function (called for unrecognized commands) and the handler functions, one for each command. Later, during execution, the handlers will be invoked to process their corresponding commands. Finally, in the sketch's loop() function, to call the CommandLine object's run() function repeatedly. 

The run function reads from the Stream as bytes become available, buffering them and returning, until a '\r' is received. When run () sees a '\r' it looks at the first white-space-delimited word to determine which command the user has entered, and invokes the corresponding handler, if one exists. If none exists, run() invokes the default command handler. When the handler returns, the received user input is forgotten about, and the process repeats.

Command handlers are passed two parameters, the first is a reference to the CommandLine object, the second is a reference to the Stream object the CommandLine is using. Using its reference to the CommandLine object, a command handler can access the CommandLine object's command handler support member functions. For example, the getWord() member function provides convenient access to the individual words that make up the command line.. Because getWord() returns a String, handlers can use the String member functions for further processing. For instance, 

```
    int16_t amount = cmd->getWord(1).toInt();
```

would retrieve the first parameter as an integer. 

A command handler can use its reference to the Stream the CommandLine is using to send messages to who or whatever sent the command by using, say, 

```
     client->print("Hello world!\n");

```

## Copyright and License

CommandLine V1.0.0, October 2023
Copyright (C) 2020 - 2023 D.L. Ehnebuske

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:


The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 

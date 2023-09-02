/****
 * @file CommandLine.cpp
 * @version 1.0.0
 * @date September 1, 2023
 * 
 * This file is a portion of the package CommandLine, a library that provides 
 * an Arduino sketch with the ability to provide a simple command line UI over 
 * a Stream (e.g., Serial).
 * 
 * See CommandLine.h for details
 * 
 *****
 * 
 * CommandLine V0.2, September 2020
 * Copyright (C) 2020 D.L. Ehnebuske
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 * 
 ****/

#include "CommandLine.h"

// Public member functions

/****
 * Constructor
 ****/
CommandLine::CommandLine(Stream &s, bool echo) {
    stream = &s;
    echoing = echo;
    defaultHandler = defaultUnrecognizedCmdHandler;
    handlerCount = 0;
    commandLine = "";
    lastCommandLine = "";
    newCmd = true;
}

/****
 * attachDefaultCmdHandler()
 ****/
void CommandLine::attachDefaultCmdHandler(commandHandler_t handler) {
    defaultHandler = handler;
}

/****
 * attachCmdHandler()
 ****/
bool CommandLine::attachCmdHandler(String cmd, commandHandler_t handler) {
    if (handlerCount < CMD_MAX_HANDLERS) {
        cmds[handlerCount] = cmd;
        handlers[handlerCount++] = handler;
        return true;
    }
    return false;
}

/****
 * run()
 ****/
void CommandLine::run() {
    if (newCmd)     {
        if (echoing) {
            stream->print(CMD_PROMPT);
        }
        lastCommandLine = commandLine;
        commandLine = "";
        newCmd = false;
    }
    while (stream->available()) {
        char c = stream->read();
        switch (c) {
            case '\b':                          // Backspace: what you expect
                if (commandLine.length() > 0) {
                    commandLine = commandLine.substring(0, commandLine.length() - 1);
                    if (echoing) {
                        stream->print(F("\b \b"));
                    }
                }
                break;
            case '\r':                          // Return: execute command
                 if (echoing) {
                    stream->print(F("\n"));
                }
                process();
                newCmd = true;
                return;
            case '\n':                          // Newline: ignore
                // Ignore
                break;
            case '\t':                          // Tab: convert to ' '
                commandLine += ' ';
                if (echoing) {
                    stream->print(' ');
                }
                break;
            case 0x04:                          // ctrl-D: if nothing typed, retrieve last command
                if (commandLine.length() == 0 && lastCommandLine.length() != 0) {
                    commandLine = lastCommandLine;
                    if (echoing) {
                        stream->print(commandLine);
                    }
                }
                break;
            default:                            // Normal character: Accumulate input
                commandLine += c;
                if (echoing) {
                    stream->print(c);
                }
       }
    }
}

/****
 * cancelCmd() 
 ****/
void CommandLine::cancelCmd() {
    newCmd = true;
}

/****
 * getWord()
 ****/
String CommandLine::getWord(uint8_t ix) {
    int16_t startAt = 0;
    int16_t len = commandLine.length();
    String answer = "";
    for (int16_t i = 0; i < ix; i++){
        while (startAt < len && commandLine.charAt(startAt) == ' ') {
            startAt++;
        }
        while (startAt < len && commandLine.charAt(startAt) != ' ') {
            startAt++;
        }
    }
    while (startAt < len && commandLine.charAt(startAt) == ' ') {
        startAt++;
    }
    while (startAt < len && commandLine.charAt(startAt) != ' ') {
        answer += commandLine.charAt(startAt++);
    }
    return answer;
}

/****
 * getCommandLine()
 ****/
String CommandLine::getCommandLine() {
    return String(commandLine);
}


// Private member functions

/****
 * process()
 ****/
void CommandLine::process() {
    commandLine.trim();
    String cmd = getWord();
    // Ignore zero-length commands
    if(cmd.length() == 0) {
        return;
    }

    // Figure out which command was input
    int8_t ix = 0;
    do {
        if (cmd.equals(cmds[ix])) {
            break;
        }
    } while(++ix < handlerCount);

    // Dispatch the associated handler, or the default handler if 
    // there is one and no handler for the command is found
    if (ix == handlerCount) {
        if (defaultHandler != NULL) {
            (*defaultHandler)(this, stream);
        }
    } else {
        (*handlers[ix])(this, stream);
    }
}

/****
 * defaultUnrecognizedCmdHandler()
 ****/
void CommandLine::defaultUnrecognizedCmdHandler(CommandLine *cmdLine, Stream *clientStream) {
    clientStream->print(F("Unknown command \""));
    clientStream->print(cmdLine->getWord());
    clientStream->print(F("\".\n"));
}
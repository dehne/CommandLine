/****
 * 
 * This file is a portion of the package UserInput, a library that provides 
 * an Arduino sketch with the ability to provide a simple command line UI over 
 * a Stream (e.g., Serial).
 * 
 * See UserInput.h for details
 * 
 *****
 * 
 * UserInput V0.2, September 2020
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

#include "UserInput.h"

/****
 * Constructor
 ****/
UserInput::UserInput(Stream &s, bool echo) {
    stream = &s;
    echoing = echo;
    defaultHandler = NULL;
    handlerCount = 0;
    commandLine = "";
    lastCommandLine = "";
    newCmd = true;
}

/****
 * Attach (or replace) the handler that deals with unrecognized commands
 ****/
void UserInput::attachDefaultCmdHandler(userInputHandler handler) {
    defaultHandler = handler;
}

/****
 * Attach the command handler for a specific command.
 * 
 * Returns true if successful, false if trying to attach too many handlers.
 * (Increase UIL_MAX_HANDLERS.)
 ****/
bool UserInput::attachCmdHandler(String cmd, userInputHandler handler) {
    if (handlerCount < UIL_MAX_HANDLERS) {
        cmds[handlerCount] = cmd;
        handlers[handlerCount++] = handler;
        return true;
    }
    return false;
}

/****
 * Main processing routine. Call this repeatedly in loop().
 ****/
void UserInput::run() {
    if (newCmd)     {
        if (echoing) {
            stream->print(UIL_PROMPT);
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
 * Process user input accumulated in commandLine.
 ****/
void UserInput::process() {
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
            (*defaultHandler)();
        }
    } else {
        (*handlers[ix])();
    }
}

/****
 * 
 * Cancel any user input and start command over. If echoing, reissue the 
 * prompt.
 *  
 ****/
void UserInput::cancelCmd() {
    newCmd = true;
}

/****
 * Return the Nth word from commandLine, where a word
 * is a sequence of non-blank characters
 ****/
String UserInput::getWord(uint8_t ix) {
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
 * Return (a copy of) the whole command line
 ****/
String UserInput::getCommandLine() {
    return String(commandLine);
}
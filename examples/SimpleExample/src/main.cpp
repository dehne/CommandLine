/****
 * @file main.cpp
 * @version 1.0.0
 * @date September 4, 2023
 * 
 * This file is an example sketch for the package CommandLine, a library that provides an Arduino 
 * sketch with the ability to provide a simple command line UI over a Stream (e.g., Serial). See 
 * the file CommandLine.h for details.
 * 
 *****
 * 
 * Copyright (C) 2020-2023 D.L. Ehnebuske
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without 
 * restriction, including without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 * 
 ****/
#include <Arduino.h>
#include <CommandLine.h>

#define BANNER  "CommandLine Example, Version 1.0.0\n"

CommandLine cmdLine = CommandLine();    // Make the CommandLine object we'll use to handle commands

/**
 * 
 * @brief The "help" and "h" command handler; invoked by cmdLine.run() when a help or
 * h command is entered. Returns a list of available commands.
 * 
 */
String onHelp(CommandHandlerHelper* cl) {
    return F(
        "Help for " BANNER "\n"
        "Command        Function\n"
        "=============  ===========================================================\n"
        "help           Display this text.\n"
        "h              Same as help.\n"
        "maxcmds        Display the current maximum number of commands.\n"
        "echo <int>     Echo the integer that is the first parameter of the command.\n");
}

/**
 *
 * @brief The "maxcmds" command handler. Return the maximum number of commands 
 * CommandLine is currently set to handle.
 * 
 */
String onMaxcmds(CommandHandlerHelper* helper) {
    return  String(F("The maximum number of commands CommandLine currently supports is ")) +
            CMD_MAX_HANDLERS + F(".\n");
}


/**
 * 
 * @brief The "echo" command handler. Echo the integer entered as its parameter. 
 * 
 */
String onEcho(CommandHandlerHelper* helper) {
    String word = helper->getWord(1);
    if (word == "") {
        return String(F("Expected and integer to echo; got nothing.\n"));
    }
    return String(F("The echo command received ")) + word.toInt() + String(F(".\n"));
}

/**
 * 
 * @brief The Arduino setup() function. Called once at environment initialization
 * 
 */
void setup() {
    Serial.begin(9600);
    while(!Serial && millis()<5000) {
        // Wait for the serial monitor to connect (needed for Leo-like boards).
        // Despite what it says in the documentation, some boards never set Serial, 
        // so give up after 5 sec.
    }

    boolean attachSucceeded =                           // Tell cmdLine what to call for each command
        cmdLine.attachCmdHandler("help", onHelp) &&
        cmdLine.attachCmdHandler("h", onHelp) &&
        cmdLine.attachCmdHandler("maxcmds", onMaxcmds) &&
        cmdLine.attachCmdHandler("echo", onEcho);
    if(!attachSucceeded) {                              // Check that we don't attach too many
        Serial.print(F("Too many commands. Currently CommandLine supports "));
        Serial.print(CMD_MAX_HANDLERS);
        Serial.println(F(" commands. Can safely be redefined as needed."));
    }
    Serial.print(F(BANNER));                            // Say "hello."
    Serial.println(F("Type \"help\" for a list of commands."));
}

/**
 * 
 * @brief The Arduino loop() function. Called repeatedly until power off or reset.
 * 
 */
void loop() {
    cmdLine.run();      // Let cmdLine do its thing
}
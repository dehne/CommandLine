/****
 * 
 * This file is an example sketch for the package UserInput, a library that 
 * provides an Arduino sketch with the ability to provide a simple command 
 * line UI over a Stream (e.g., Serial). See the file UserInput.h for
 * details.
 * 
 *****
 * 
 * UserInput V1.0.0, October 2022
 * Copyright (C) 2020-2022 D.L. Ehnebuske
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
#include <Arduino.h>
#include <UserInput.h>

#define BANNER  "UserInput Example, Version 1.0.0\n"

UserInput ui = UserInput();         // Make the UserInput object we'll use to handle commands

/**
 * 
 * @brief Default command handler; invoked by ui.run() when an unrecognized 
 * command is entered. Just says the command isn't supported.
 * 
 */
void onDefaultCommand(){
    Serial.print(F("The command \""));
    Serial.print(ui.getWord());
    Serial.println(F("\" is not supported."));
}

/**
 * 
 * @brief The "help" and "h" command handler; invoked by ui.run() when a help or
 * h command is entered. Displays a list of available commands.
 * 
 */
void onHelp() {
    Serial.println(F(
        "Help for " BANNER "\n"
        "Command        Function\n"
        "=============  ===========================================================\n"
        "help           Display this text.\n"
        "h              Same as help.\n"
        "maxcmds        Display the current maximum number of commands.\n"
        "echo <int>     Echo the integer that is the first parameter of the command.\n"
    ));
}

/**
 *
 * @brief The "maxcmds" command handler. Print the maximum number of commands 
 * UserInput is currently set to handle.
 * 
 */
void onMaxcmds() {
    Serial.print(F("The maximum number of commands UserInput currently supports is "));
    Serial.print(UIL_MAX_HANDLERS);
    Serial.println(F("."));
}


/**
 * 
 * @brief The "echo" command handler. Echo the integer entered as its parameter. 
 * 
 */
void onEcho() {
    String word = ui.getWord(1);
    if (word == "") {
        Serial.println(F("Expected and integer to echo; got nothing."));
        return;
    }
    Serial.print(F("The echo command received "));
    Serial.print(word.toInt());
    Serial.println(F("."));
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

    ui.attachDefaultCmdHandler(onDefaultCommand);       // Tell ui what to call on unrecognized commands
    boolean attachSucceeded =                           // Tell ui what to call for each command
        ui.attachCmdHandler("help", onHelp) &&
        ui.attachCmdHandler("h", onHelp) &&
        ui.attachCmdHandler("maxcmds", onMaxcmds) &&
        ui.attachCmdHandler("echo", onEcho);
    if(!attachSucceeded) {                              // Check that we don't attach too many
        Serial.print(F("Too many commands. Currently UserInput supports "));
        Serial.print(UIL_MAX_HANDLERS);
        Serial.println(F(" commands. Can be changed in UserInput.h."));
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
    ui.run();                                           // Let ui do its thing
}
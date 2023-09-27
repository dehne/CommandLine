/****
 * @file CommandLine.h
 * @version 1.0.0
 * @date September 27, 2023
 * 
 * This file is a portion of the package CommandLine, a library that provides an Arduino sketch 
 * with the ability to provide a simple command line UI over a Stream (Serial, by default). It is 
 * a refactored replacement for my library, UserInput, which is no longer being actively 
 * developed. 
 * 
 * A command line is a sequence of ascii characters terminated by a '\r'. It is subdivided into 
 * "words" separated by ' ' or '\t' characters. The first word in a command line is the name of 
 * the command. The rest are whatever the command wishes to make of them.
 * 
 * The basic usage pattern here is to instantiate a global CommandLine object. Pass it the Stream 
 * it is to use for communication, Serial by default. Then, in the sketch's setup() function, 
 * attach "command handler" functions, one for each command. Later, during execution, the 
 * CommandLine object will invoke the handlers when the user enters the corresponding command. 
 * Finally, in the sketch's loop() function, call the CommandLine object's run() function 
 * repeatedly. 
 * 
 * The run function reads from the Stream as bytes become available, accumulating them and 
 * returning, until a '\r' is received.  When run () sees a '\r' it looks at accumulated input 
 * for the first white-space-delimited word. That's the command name. It uses that name to find 
 * and invoke the corresponding handler, if one exists for that command. If none exists, run() 
 * invokes the default command handler. Either way, when the handler returns, the received user 
 * input is forgotten about, and the process repeats.
 * 
 * When a command handler is invoked it is passed a reference to an object that implements the 
 * CommandHandlerHelper interface. (CommandLine implements the interface, and run() passes "this" 
 * to the command handlers it invokes.) The command handler uses the reference to access functions 
 * that implement common command handler tasks. For example, the getWord() function provides 
 * convenient access to the individual words that make up the command line the user entered.
 *  
 * At the end of its work, a command handler returns a String whose contents the CommandLine 
 * object will present to the user.
 * 
 * Note: The CommandHandlerHelper interface is separated out and the CommandLine::getHandlerFor 
 * member function is provided so that command handlers can be invoked to do their thing using 
 * mechanisms other than the CommandLine, if desired. For example, using the ESP8266- and ESP-32- 
 * based SimpleWebServer and its WebCmd extension, it's pretty easy to create a JS-free webpage 
 * simulating a "dumb terminal" command line interface that uses the same command handlers that 
 * CommandLine uses.
 * 
 *****
 * 
 * CommandLine V1.0.0, September 2023
 * Copyright (C) 2020 - 2023 D.L. Ehnebuske
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

#pragma once
#ifndef Arduino_h
#include <Arduino.h>
#endif
#include <CommandHandlerHelper.h>

#define CMD_MAX_HANDLERS        (16)        // The maximum number of callbacks
#define CMD_PROMPT              (F("> "))   // User prompt string

class CommandLine : public CommandHandlerHelper {

    public:
        /**
        * 
        * @brief  This is the constructor for a CommandLine object
        * 
        * @param Stream The Stream object to use to communicate with the user. Serial, by default.
        * @param bool   Whether the characters received on the Stream are to be echoed for the 
        *               user to see. Defaults to true. 
        */
        CommandLine(Stream &s = Serial, bool echo = true);

        /**
         * @brief   The type a sketch-provided command handler must have.
         * 
         * @details Write a function of this type, attach it using attachCmdHandler and it'll be 
         *          invoked when the command specified in the attach is entered by the user. Use 
         *          the reference to the CommandHandlerHelper to access the command handler 
         *          support member functions. Do whatever needs to be done to carry out the 
         *          command and its parameters. Return whatever the user should see as a result. .
         * 
         */
        using commandHandler_t = String (*)(CommandHandlerHelper*);


        /**
        * 
        * @brief        Attach the default (unrecognized) command handler. Use this to override 
        *               the one that's built in.
        * 
        * @param commandHandler_t   The sketch-supplied function to be called when an unrecognized 
        *                           command is encountered.
        * 
        **/
        void attachDefaultCmdHandler(commandHandler_t handler);

        /**
        * 
        * @brief    Attach the cmd handler for the specified command.
        *
        * @param String                 When the user inputs this string, invoke the corresponding 
        *                               handler.
        * @param commandHandler_t     The user-supplied function of type 
        * @return   true                If attachment succeeded, 
        * @return   false               If there are too many command handlers. 
        *                               See CMD_MAX_HANDLERS.
        */
        bool attachCmdHandler(String cmd, commandHandler_t handler);

        /**
        * 
        * @brief    Service the Stream this object is responsible for dealing with.
        * 
        * @details  Invoke this member function repeatedly; typically, every pass through loop(). 
        * 
        **/
        void run();

        /**
        * 
        * @brief    Cancel any user input and start command over. If echoing, reissue the prompt. 
        * 
        **/
        void cancelCmd();

        /**
         * @brief Return the current count of command handlers
         * 
         * @return int8_t
         */
        int8_t getHandlerCount();

        /**
         * @brief   Get the command handler for the specified command; the default command handler 
         *          if the name doesn't match that of any command.
         * 
         *          This member function is intended for the use of alternative mechanisms for 
         *          executing the commands.
         * 
         * @param commandName       The name of the command whose handler we are to return
         * @return commandHandler_t 
         */
        commandHandler_t getHandlerFor(String commandName);

        // The following are command handler helper member functions

        /**
        * 
        * @brief    Command handler helper: Return, as a String, the specified "word" from the 
        *           text the user has input. 
        * 
        * @details  A "word" is a white-space-bounded sequence of characters. The words are 
        *           numbered starting with 0. Word 0 is the command. If the user has not input the 
        *           specified word, an empty String is returned.
        * 
        * @param    int16_t     The number of the word to be returned. Defaults to 0.
        * 
        **/
        String getWord(uint8_t ix = 0);

        /**
        * 
        * @brief    Command handler helper: Return, as a String, the trimmed sequence of 
        *           characters the user has input.
        * 
        **/
        String getCommandLine();
    private:
        commandHandler_t handlers[CMD_MAX_HANDLERS];    // Pointers to the command handlers
        String cmds[CMD_MAX_HANDLERS];                  // The commands that go with the handlers
        commandHandler_t defaultHandler;                // Pointer to the handler to use if the cmd not recognized
        Stream *stream;                                 // The Stream we service
        bool echoing;                                   // Whether we are to echo user input
        int8_t handlerCount;                            // The count of registered cmd handlers
        String commandLine;                             // Where user input is accumulated
        String lastCommandLine;                         // The previous command line, if any
        bool newCmd;                                    // True after a command is processed and before additional input

        /**
         * @brief Convenience function to actually invoke the appropriate command handler.
         * 
         */
        void process();

        /**
         * @brief                       The built-in unrecognized command handler. Just responds that the 
         *                              command was not recognized.
         * 
         * @param CommandHandlerHelper* Reference to the helper that can do things like give us access to
         *                              the contents of the user's command.
         * @return String               What we want to say to the user as a result of handling the command
         */
        static String defaultUnrecognizedCmdHandler(CommandHandlerHelper* cmdLine);
};
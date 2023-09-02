/****
 * @file CommandLine.h
 * @version 1.0.0
 * @date September 1, 2023
 * 
 * This file is a portion of the package CommandLine, a library that provides an Arduino sketch 
 * with the ability to provide a simple command line UI over a Stream (Serial, by default). It is 
 * a refactored replacement for my library, UserInput, which is no longer being maintained.
 * 
 * A command line is a sequence of ascii characters terminated by a '\r'. It is subdivided into 
 * "words" separated by ' ' or '\t' characters. The first word in a command line is the name of 
 * the command. The rest are parameters.
 * 
 * The basic usage pattern here is to instantiate a global CommandLine object. Pass it the Stream 
 * it is to use for communication, if something other than Serial is wanted. Then, in the sketch's 
 * setup() function, to attach the default command handler function (called for unrecognized 
 * commands) and the handler functions, one for each command. Later, during execution, the handlers 
 * will be invoked to process their corresponding commands. Finally, in the sketch's loop() 
 * function, to call the CommandLine object's run() function repeatedly. 
 * 
 * The run function reads from the Stream as bytes become available, buffering them and returning, 
 * until a '\r' is received. When run () sees a '\r' it looks at the first white-space-delimited 
 * word to determine which command the user has entered, and invokes the corresponding handler, if 
 * one exists. If none exists, run() invokes the default command handler. When the handler 
 * returns, the received user input is forgotten about, and the process repeats.
 * 
 * Command handlers are passed two parameters, the first is a reference to the CommandLine object, 
 * the second is a reference to the Stream object the CommandLine is using. Using its reference to 
 * the CommandLine object, a command handler can access the CommandLine object's command handler 
 * support member functions. For example, the getWord() member function provides convenient access 
 * to the individual words that make up the command line.. Because getWord() returns a String, 
 * handlers can use the String member functions for further processing. For instance, 
 * 
 *     int16_t amount = cmd->getWord(1).toInt();
 * 
 * would retrieve the first parameter as an integer. 
 * 
 * A command handler can use its reference to the Stream the CommandLine is using to send messages 
 * to who or whatever sent the command by using, say, 
 * 
 *      client->print("Hello world!\n");
 * 
 *****
 * 
 * CommandLine V1.0.0, October 2023
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
#define CMD_MAX_HANDLERS        (16)        // The maximum number of callbacks
#define CMD_PROMPT              (F("> "))   // User prompt string

class CommandLine {
    public:
        /**
        * 
        * @brief  This is the constructor for a CommandLine object
        * 
        * @param Stream The Stream object to use to communicate with the user. Serial, by default.
        * @param bool   Whether the characters received on s are to be echoed back on s for the 
        *               user to see. Defaults to true. 
        */
        CommandLine(Stream &s = Serial, bool echo = true);

        /**
         * @brief   The type a sketch-provided command handler must have.
         * 
         * @details Write a function of this type, attach it using attachCmdHandler and it'll be 
         *          invoked when the command specified in the attach is entered by the user. Use 
         *          the reference to the CommandLine to access the command handler support member 
         *          functions. Use the reference to the Stream to send responses to the user.
         * 
         */
        using commandHandler_t = void (*)(CommandLine*, Stream*);


        /**
        * 
        * @brief        Attach the default (unrecognized) command handler. Use to override the one
        *               that's built in.
        * 
        * @param commandHandler_t_t   The user-supplied void function to be called when an unrecognized command is encountered.
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
        * 
        * @brief    Command handler support member function: Return, as a String, the specified 
        *           "word" from the text the user has input. 
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
        * @brief    Command handler support member function: Return, as a String, the trimmed 
        *           sequence of characters the user has input.
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
         * @brief               The built-in unrecognized command handler. Just responds that the 
         *                      command was not recognized
         * 
         * @param cmdLine       Reference to the CommandLine the handler is working on behaf of
         * @param clientStream  Reference to the Stream to use to send messages to the user
         */
        static void defaultUnrecognizedCmdHandler(CommandLine* cmdLine, Stream* clientStream);
};
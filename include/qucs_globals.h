/***************************************************************************
    author               : 2018, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QUCS_GLOBALS_H
#define QUCS_GLOBALS_H

#include "command.h"
#include "dispatcher.h"
#include "platform.h"

class TaskElement;
class Diagram;
class DocumentLanguage;
class Painting;
class Data;
class Symbol;
class Command;

extern INTERFACE Dispatcher<TaskElement> element_dispatcher;
extern INTERFACE Dispatcher<Command> commandDispatcher;
extern INTERFACE Dispatcher<Diagram> diagram_dispatcher;
extern INTERFACE Dispatcher<DocumentLanguage> languageDispatcher;
extern INTERFACE Dispatcher<Painting> painting_dispatcher;
extern INTERFACE Dispatcher<Data> dataDispatcher;
extern INTERFACE Dispatcher<Symbol> symbol_dispatcher;

#endif
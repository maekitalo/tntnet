/*
 * Copyright (C) 2008 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include "modificationtracker.h"
#include <cxxtools/string.h>

// This declares the global message object.
// The actual definition is done in rajax.ecpp.

// The message object must not be in a application scope, since application
// scope would lock the application while the request runs, but we need to do
// our own syncronization in the ModificationTracker class.

typedef ModificationTracker<cxxtools::String> MessageType;
extern MessageType message;

#endif // MESSAGE_H

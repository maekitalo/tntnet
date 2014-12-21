/*
 * Copyright (C) 2014 Tommi Maekitalo
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

#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <deque>
#include <set>
#include <cxxtools/mutex.h>
#include <cxxtools/timespan.h>
#include <cxxtools/queue.h>

// The session class manages a list of current chat messages. It also knows all
// instances, so that new messages can be sent to all of them.
//
// New messages are put to a queue, which has a wait method so that the web
// application can wait for new messages to arrive.
//
// Once a message is read from the queue, it is put into the list of current
// messages.
//
class Session
{
  public:
    typedef std::deque<std::string> Messages;

  private:
    Messages _messages;                   // the list of current messages
    cxxtools::Mutex _messagesMutex;       // the messages must be syncronized between threads
    unsigned _max;                        // the maximum number of current messages
    cxxtools::Queue<std::string> _queue;  // the queue, where new messages arrive

    typedef std::set<Session*> Sessions;
    static Sessions _sessions;            // the list of all current sessions
    static cxxtools::Mutex _sessionsMutex;

  public:
    explicit Session(unsigned max = 10);
    ~Session();

    // Returns the current messages.
    // When new messages have arrived in the queue they are first
    // transfered to the list.
    Messages messages();

    // Returns the current messages.
    // It waits up to the specified time for new messages to arrive.
    Messages messages(cxxtools::Seconds wait);

    // Adds a new message to the queue.
    void addMessage(const std::string& message)
    {
      _queue.put(message);
    }

    // Adds a new message to each session.
    static void broadcastMessage(const std::string& message);
};

#endif // SESSION_H

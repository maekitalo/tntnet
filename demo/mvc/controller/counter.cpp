/*
 * Copyright (C) 2013 Tommi Maekitalo
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

#include <tnt/component.h>
#include <tnt/componentfactory.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <cxxtools/log.h>

// include the model
#include <counter.h>

// define a log category
log_define("controller.counter")

// define a component, which is callable from within tntnet.xml
class CounterController : public tnt::Component
{
  public:
    virtual unsigned operator() (tnt::HttpRequest& request,
      tnt::HttpReply& reply, tnt::QueryParams& qparam);
};

// A static factory is used to instantiate the component.
// This also defines the name of the component, which is used
// in the mapping.
static tnt::ComponentFactoryImpl<CounterController>
    factory("controller/counter");

// The operator() is the main method of the component. It is the
// starting point of our component.
unsigned CounterController::operator() (tnt::HttpRequest& request,
  tnt::HttpReply& reply, tnt::QueryParams& qparam)
{
  // This definition imports the counter variable from the
  // session.
  TNT_SESSION_GLOBAL_VAR(Counter, counter, ());

  log_debug("counter controller called with qparams=" << qparam.getUrl());

  // Check if the user has pressed the increment button.
  // The `qparam` variable has all the query parameters. Submitting a form sets
  // the query parameter to the value of the button.
  // Using `arg<bool>` checks whether the value is not empty.
  //
  if (qparam.arg<bool>("increment"))
  {
    counter.increment();
  }

  // Same as above with the decrement button.
  if (qparam.arg<bool>("decrement"))
  {
    counter.decrement();
  }

  return DECLINED;
}

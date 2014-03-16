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

log_define("controller.counter")

class CounterController : public tnt::Component
{
  public:
    virtual unsigned operator() (tnt::HttpRequest& request,
      tnt::HttpReply& reply, tnt::QueryParams& qparam);
};

static tnt::ComponentFactoryImpl<CounterController> factory("controller/counter");

unsigned CounterController::operator() (tnt::HttpRequest& request,
  tnt::HttpReply& reply, tnt::QueryParams& qparam)
{
  TNT_SESSION_GLOBAL_VAR(Counter, counter, ());

  log_debug("counter controller called with qparams=" << qparam.getUrl());

  if (qparam.arg<bool>("increment"))
  {
    counter.increment();
  }

  if (qparam.arg<bool>("decrement"))
  {
    counter.decrement();
  }

  return DECLINED;
}

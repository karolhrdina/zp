/*  =========================================================================
    zpasync - Asynchronous job execution actor

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroMQ Pipes Project.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZPASYNC_H_INCLUDED
#define ZPASYNC_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _zpasync_t zpasync_t;

//  @interface
//  Create new zpasync actor instance.
//  @TODO: Describe the purpose of this actor!
//
//      zactor_t *zpasync = zactor_new (zpasync, NULL);
//
//  Destroy zpasync instance.
//
//      zactor_destroy (&zpasync);
//
//  Enable verbose logging of commands and activity:
//
//      zstr_send (zpasync, "VERBOSE");
//
//  Start zpasync actor.
//
//      zstr_sendx (zpasync, "START", NULL);
//
//  Stop zpasync actor.
//
//      zstr_sendx (zpasync, "STOP", NULL);
//
//  This is the zpasync constructor as a zactor_fn;
ZP_EXPORT void
    zpasync_actor (zsock_t *pipe, void *args);

//  Self test of this actor
ZP_EXPORT void
    zpasync_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif

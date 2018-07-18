/*  =========================================================================
    zp_async - Asynchronous job execution actor

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroMQ Pipes Project.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZP_ASYNC_H_INCLUDED
#define ZP_ASYNC_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _zp_async_t zp_async_t;

//  @interface
//  Create new zp_async actor instance.
//  @TODO: Describe the purpose of this actor!
//
//      zactor_t *zp_async = zactor_new (zp_async, NULL);
//
//  Destroy zp_async instance.
//
//      zactor_destroy (&zp_async);
//
//  Enable verbose logging of commands and activity:
//
//      zstr_send (zp_async, "VERBOSE");
//
//  Start zp_async actor.
//
//      zstr_sendx (zp_async, "START", NULL);
//
//  Stop zp_async actor.
//
//      zstr_sendx (zp_async, "STOP", NULL);
//
//  This is the zp_async constructor as a zactor_fn;
ZP_EXPORT void
    zp_async_actor (zsock_t *pipe, void *args);

//  Self test of this actor
ZP_EXPORT void
    zp_async_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif

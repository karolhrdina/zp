/*  =========================================================================
    zpasync - Asynchronous job execution actor

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroMQ Pipes Project.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    zpasync - Asynchronous job execution actor
@discuss
@end
*/

#include "zp_classes.h"

#define DEFAULT_POLL_INTERVAL   1000

//  Structure of our actor

struct _zpasync_t {
    zsock_t *pipe;              //  Actor command pipe
    bool terminated;            //  Did caller ask us to quit?
    bool verbose;               //  Verbose logging enabled?
    bool started;               //  Did caller ask us to start?
    uint32_t poll_interval;     //  Polling interval

    zpoller_t *poller;          //  Socket poller
    zproc_t *proc;              //  The Heart of this CLASS ;)
    //  TODO: Declare properties
};


//  --------------------------------------------------------------------------
//  Create a new zpasync instance

static zpasync_t *
zpasync_new (zsock_t *pipe, void *args)
{
    zpasync_t *self = (zpasync_t *) zmalloc (sizeof (zpasync_t));
    assert (self);

    self->pipe = pipe;
    self->terminated = false;
    self->verbose = false;
    self->started = false;
    self->poll_interval = DEFAULT_POLL_INTERVAL;

    self->poller = zpoller_new (self->pipe, NULL);
    assert (self->poller);

    self->proc = NULL;

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zpasync instance

static void
zpasync_destroy (zpasync_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zpasync_t *self = *self_p;

        //  Free actor properties
        zpoller_destroy (&self->poller);
        
        if (self->proc && zproc_running (self->proc)) {
            zproc_kill (self->proc, SIGKILL);
            zclock_sleep (200);
        }
        zproc_destroy (&self->proc);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Actor receive api helpers

static void
recv_api_command_start (zpasync_t *self, zmsg_t *message)
{
    assert (self);
    assert (message);

    self->started = true;
    zstr_send (self->pipe, "OK");
    return;
}

static void
recv_api_command_stop (zpasync_t *self, zmsg_t *message)
{
    assert (self);
    assert (message);

    self->started = false;
    zstr_send (self->pipe, "OK");
    return;
}

static void
recv_api_command_run (zpasync_t *self, zmsg_t *message)
{
    assert (self);
    assert (message);

    if (!self->started) {
        zstr_sendx (self->pipe, "ERROR", "...reason...", NULL);
        return;
    }

    if (self->proc) {
        zpoller_remove (self->poller, (void *) zproc_stdout (self->proc));
        if (zproc_running (self->proc)) {
            zproc_kill (self->proc, SIGKILL);
            zclock_sleep (200);
        }
        zproc_destroy (&self->proc);
    }

    self->proc = zproc_new ();
    assert (self->proc);
    
    zproc_set_stdout (self->proc, NULL);

    int rv = zpoller_add (self->poller, (void *) zproc_stdout (self->proc));
    assert (rv == 0);

    zlist_t *args = zlist_new ();
    assert (args);
    zlist_autofree (args);

    char *arg = zmsg_popstr (message);
    while (arg) {
        zlist_append (args, (void *) arg);
        arg = zmsg_popstr (message);
    }

    if (zlist_size (args) == 0) {
        zlist_destroy (&args);
        zstr_sendx (self->pipe, "ERROR", "...reason...", NULL);
        return;
    }
    zproc_set_args (self->proc, &args);

    zproc_run (self->proc);
    
    zstr_send (self->pipe, "OK");
    return;
}

static void
recv_api_command_verbose (zpasync_t *self, zmsg_t *message)
{
    assert (self);
    assert (message);

    self->verbose = true;
    zstr_send (self->pipe, "OK");
    return;
}

//  --------------------------------------------------------------------------
//  Here we handle incoming message from the node

static void
zpasync_recv_api (zpasync_t *self)
{
    //  Get the whole message of the pipe in one go
    zmsg_t *request = zmsg_recv (self->pipe);
    if (!request)
       return;        //  Interrupted

    char *command = zmsg_popstr (request);
    if (!command) {
        zstr_sendx (self->pipe, "ERROR", "Invalid actor command.", NULL);
        zmsg_destroy (&request);
        return;
    }

    if (streq (command, "START"))
        recv_api_command_start (self, request);
    else
    if (streq (command, "STOP"))
        recv_api_command_stop (self, request);
    else
    if (streq (command, "RUN"))
        recv_api_command_run (self, request);
    else
    if (streq (command, "VERBOSE"))
        recv_api_command_verbose (self, request);
    else
    if (streq (command, "$TERM"))
        //  The $TERM command is send by zactor_destroy() method
        self->terminated = true;
    else
        zstr_sendx (self->pipe, "ERROR", "Invalid actor command.", NULL);

    zstr_free (&command);
    zmsg_destroy (&request);
}


//  --------------------------------------------------------------------------
//  This is the actor which runs in its own thread.

void
zpasync_actor (zsock_t *pipe, void *args)
{
    zpasync_t * self = zpasync_new (pipe, args);
    if (!self)
        return;          //  Interrupted

    //  Signal actor successfully initiated
    zsock_signal (self->pipe, 0);

    while (!self->terminated) {
        zsock_t *which = (zsock_t *) zpoller_wait (self->poller, 0);
        if (which == self->pipe)
            zpasync_recv_api (self);
       //  Add other sockets when you need them.
    }
    zpasync_destroy (&self);
}

//  --------------------------------------------------------------------------
//  Self test of this actor.

void
zpasync_test (bool verbose)
{
    printf (" * zpasync: ");
    //  @selftest

    //  Simple create/destroy test
    zactor_t *zpasync = zactor_new (zpasync_actor, NULL);
    assert (zpasync);

    zactor_destroy (&zpasync);
    assert (zpasync == NULL);
    zactor_destroy (&zpasync);
    //  @end

    printf ("OK\n");
}

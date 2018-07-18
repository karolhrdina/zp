/*  =========================================================================
    zp_proto - ZeroMQ Pipes protocol

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of the ZeroMQ Pipes Project.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZP_PROTO_H_INCLUDED
#define ZP_PROTO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new zp_proto
ZP_EXPORT zp_proto_t *
    zp_proto_new (void);

//  Destroy the zp_proto
ZP_EXPORT void
    zp_proto_destroy (zp_proto_t **self_p);

//  Self test of this class
ZP_EXPORT void
    zp_proto_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif

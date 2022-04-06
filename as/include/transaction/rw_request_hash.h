/*
 * rw_request_hash.h
 *
 * Copyright (C) 2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/
 */

#pragma once

//==========================================================
// Includes.
//

#include <stdint.h>

#include "citrusleaf/cf_digest.h"

#include "base/transaction.h"


//==========================================================
// Forward declarations.
//

struct as_transaction_s;
struct rw_request_s;


//==========================================================
// Typedefs & constants.
//

typedef enum {
	// These values go on the wire, so mind backward compatibility if changing.
	RW_FIELD_OP,
	RW_FIELD_RESULT,
	RW_FIELD_NAMESPACE,
	RW_FIELD_NS_IX,
	RW_FIELD_GENERATION,
	RW_FIELD_DIGEST,
	RW_FIELD_RECORD,
	RW_FIELD_UNUSED_7,
	RW_FIELD_UNUSED_8,
	RW_FIELD_UNUSED_9,
	RW_FIELD_TID,
	RW_FIELD_UNUSED_11,
	RW_FIELD_INFO,
	RW_FIELD_UNUSED_13,
	RW_FIELD_UNUSED_14,
	RW_FIELD_UNUSED_15,
	RW_FIELD_LAST_UPDATE_TIME,
	RW_FIELD_UNUSED_17,
	RW_FIELD_UNUSED_18,
	RW_FIELD_REGIME,

	NUM_RW_FIELDS
} rw_msg_field;

#define RW_OP_UNUSED_1 1
#define RW_OP_WRITE_ACK 2
#define RW_OP_DUP 3
#define RW_OP_DUP_ACK 4
#define RW_OP_REPL_CONFIRM 5
#define RW_OP_REPL_PING 6
#define RW_OP_REPL_PING_ACK 7
#define RW_OP_REPL_WRITE 8

#define RW_INFO_UNUSED_1		0x0001 // was XDR (used in 4.9 & 5.0.0.3)
#define RW_INFO_NO_REPL_ACK		0x0002
#define RW_INFO_UNUSED_4		0x0004
#define RW_INFO_UNUSED_8		0x0008
#define RW_INFO_UNUSED_10		0x0010
#define RW_INFO_UNUSED_20		0x0020
#define RW_INFO_UNUSED_40		0x0040
#define RW_INFO_UNUSED_80		0x0080 // was sindex-touched (used up to 5.6)
#define RW_INFO_UNUSED_100		0x0100
#define RW_INFO_UNREPLICATED	0x0200 // enterprise only

typedef struct rw_request_hkey_s {
	uint32_t	ns_ix;
	cf_digest	keyd;
} __attribute__((__packed__)) rw_request_hkey;


//==========================================================
// Public API.
//

void as_rw_init();

uint32_t rw_request_hash_count();
transaction_status rw_request_hash_insert(rw_request_hkey* hkey, struct rw_request_s* rw, struct as_transaction_s* tr);
void rw_request_hash_delete(rw_request_hkey* hkey, struct rw_request_s* rw);
struct rw_request_s* rw_request_hash_get(rw_request_hkey* hkey);

void rw_request_hash_dump();

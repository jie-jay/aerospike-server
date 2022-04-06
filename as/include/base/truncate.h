/*
 * truncate.h
 *
 * Copyright (C) 2017 Aerospike, Inc.
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

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "cf_mutex.h"
#include "shash.h"


//==========================================================
// Forward declarations.
//

struct as_index_s;
struct as_namespace_s;


//==========================================================
// Typedefs & constants.
//

#define MAX_TRUNCATE_THREADS 128

typedef enum {
	TRUNCATE_IDLE,
	TRUNCATE_RUNNING,
	TRUNCATE_RESTART
} truncate_state;

typedef struct as_truncate_s {
	uint64_t lut;
	cf_shash* startup_set_hash; // relevant only for enterprise edition
	truncate_state state;
	cf_mutex state_lock;
	uint32_t n_threads_running;
	uint32_t pid;
	uint64_t n_records_this_run;
	uint64_t n_records;
} as_truncate;


//==========================================================
// Public API.
//

void as_truncate_init(void);
void as_truncate_list_cenotaphs(struct as_namespace_s* ns);
bool as_truncate_lut_is_truncated(uint64_t rec_lut, struct as_namespace_s* ns, const char* set_name, uint32_t set_name_len);
void as_truncate_done_startup(struct as_namespace_s* ns);
bool as_truncate_cmd(const char* ns_name, const char* set_name, const char* lut_str);
bool as_truncate_undo_cmd(const char* ns_name, const char* set_name);
bool as_truncate_now_is_truncated(struct as_namespace_s* ns, uint16_t set_id);
bool as_truncate_record_is_truncated(const struct as_index_s* r, struct as_namespace_s* ns);


//==========================================================
// For enterprise separation only.
//

typedef struct truncate_hval_s {
	uint64_t cenotaph:1;
	uint64_t unused:23;
	uint64_t lut:40;
} truncate_hval;

void truncate_startup_hash_init(struct as_namespace_s* ns);
void truncate_action_startup(struct as_namespace_s* ns, const char* set_name, uint64_t lut);

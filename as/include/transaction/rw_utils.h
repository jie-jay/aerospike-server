/*
 * rw_utils.h
 *
 * Copyright (C) 2016-2021 Aerospike, Inc.
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

#include "citrusleaf/cf_digest.h"

#include "msg.h"
#include "node.h"

#include "base/cfg.h"
#include "base/datamodel.h"
#include "base/transaction.h"
#include "base/transaction_policy.h"
#include "sindex/secondary_index.h"
#include "transaction/rw_request.h"


//==========================================================
// Forward declarations.
//

struct as_bin_s;
struct as_exp_s;
struct as_index_s;
struct as_index_ref_s;
struct as_index_tree_s;
struct as_msg_s;
struct as_msg_op_s;
struct as_namespace_s;
struct as_remote_record_s;
struct as_storage_rd_s;
struct as_transaction_s;
struct cl_msg_s;
struct rw_request_s;


//==========================================================
// Typedefs & constants.
//

typedef struct index_metadata_s {
	uint32_t void_time;
	uint64_t last_update_time;
	uint16_t generation;

	bool has_bin_meta; // relevant only for data-in-memory

	bool xdr_write; // relevant only for enterprise edition

	bool tombstone; // relevant only for enterprise edition
	bool cenotaph; // relevant only for enterprise edition
	bool xdr_tombstone; // relevant only for enterprise edition
	bool xdr_nsup_tombstone; // relevant only for enterprise edition
	bool xdr_bin_cemetery; // relevant only for enterprise edition
} index_metadata;

typedef struct now_times_s {
	uint64_t now_ns;
	uint64_t now_ms;
} now_times;

// For now, use only for as_msg record_ttl special values.
#define TTL_NAMESPACE_DEFAULT	0
#define TTL_NEVER_EXPIRE		((uint32_t)-1)
#define TTL_DONT_UPDATE			((uint32_t)-2)


//==========================================================
// Public API.
//

bool convert_to_write(struct as_transaction_s* tr, struct cl_msg_s** p_msgp);
int validate_delete_durability(struct as_transaction_s* tr);
bool xdr_allows_write(struct as_transaction_s* tr);
void send_rw_messages(struct rw_request_s* rw);
void send_rw_messages_forget(struct rw_request_s* rw);
int repl_state_check(struct as_index_s* r, struct as_transaction_s* tr);
void will_replicate(struct as_index_s* r, struct as_namespace_s* ns);
bool write_is_full_drop(const struct as_transaction_s* tr);
bool sufficient_replica_destinations(const struct as_namespace_s* ns, uint32_t n_dests);
bool set_replica_destinations(struct as_transaction_s* tr, struct rw_request_s* rw);
void finished_replicated(struct as_transaction_s* tr);
void finished_not_replicated(struct rw_request_s* rw);
bool set_name_check(const struct as_transaction_s* tr, const struct as_index_s* r);
bool generation_check(const struct as_index_s* r, const struct as_msg_s* m, const struct as_namespace_s* ns);
int set_set_from_msg(struct as_index_s* r, struct as_namespace_s* ns, struct as_msg_s* m);
int handle_meta_filter(const struct as_transaction_s* tr, const struct as_index_s* r, struct as_exp_s** exp);
void destroy_filter_exp(const struct as_transaction_s* tr, struct as_exp_s* exp);
int read_and_filter_bins(struct as_storage_rd_s* rd, struct as_exp_s* exp);
bool check_msg_key(struct as_msg_s* m, struct as_storage_rd_s* rd);
bool get_msg_key(struct as_transaction_s* tr, struct as_storage_rd_s* rd);
int handle_msg_key(struct as_transaction_s* tr, struct as_storage_rd_s* rd);
bool forbid_replace(const struct as_namespace_s* ns);
void prepare_bin_metadata(const struct as_transaction_s* tr, struct as_storage_rd_s* rd);
void stash_index_metadata(const struct as_index_s* r, index_metadata* old);
void unwind_index_metadata(const index_metadata* old, struct as_index_s* r);
void advance_record_version(const struct as_transaction_s* tr, struct as_index_s* r);
void set_xdr_write(const struct as_transaction_s* tr, struct as_index_s* r);
void touch_bin_metadata(struct as_storage_rd_s* rd);
void transition_delete_metadata(struct as_transaction_s* tr, struct as_index_s* r, bool is_delete, bool is_bin_cemetery);
bool forbid_resolve(const struct as_transaction_s* tr, const struct as_storage_rd_s* rd, uint64_t msg_lut);
bool resolve_bin(struct as_storage_rd_s* rd, const struct as_msg_op_s* op, uint64_t msg_lut, uint16_t n_ops, uint16_t* n_won, int* result);
bool udf_resolve_bin(struct as_storage_rd_s* rd, const char* name);
bool delete_bin(struct as_storage_rd_s* rd, const struct as_msg_op_s* op, uint64_t msg_lut, struct as_bin_s* cleanup_bins, uint32_t* p_n_cleanup_bins, int* result);
bool udf_delete_bin(struct as_storage_rd_s* rd, const char* name, struct as_bin_s* cleanup_bins, uint32_t* p_n_cleanup_bins, int* result);
void write_resolved_bin(struct as_storage_rd_s* rd, const struct as_msg_op_s* op, uint64_t msg_lut, struct as_bin_s* b);
void delete_all_bins(struct as_storage_rd_s* rd);
void pickle_all(struct as_storage_rd_s* rd, struct rw_request_s* rw);
void update_sindex(struct as_namespace_s* ns, struct as_index_ref_s* r_ref, struct as_bin_s* old_bins, uint32_t n_old_bins, struct as_bin_s* new_bins, uint32_t n_new_bins);
void remove_from_sindex(struct as_namespace_s* ns, struct as_index_ref_s* r_ref);
void remove_from_sindex_bins(struct as_namespace_s* ns, struct as_index_ref_s* r_ref, struct as_bin_s* bins, uint32_t n_bins);
void write_dim_single_bin_unwind(struct as_bin_s* old_bin, uint32_t n_old_bins, struct as_bin_s* new_bin, uint32_t n_new_bins, struct as_bin_s* cleanup_bins, uint32_t n_cleanup_bins);
void write_dim_unwind(struct as_bin_s* old_bins, uint32_t n_old_bins, struct as_bin_s* new_bins, uint32_t n_new_bins, struct as_bin_s* cleanup_bins, uint32_t n_cleanup_bins);


static inline bool
set_has_sindex(const as_record* r, as_namespace* ns)
{
	if (ns->sindex_cnt == 0) {
		return false;
	}

	as_set* set = as_namespace_get_record_set(ns, r);

	return set != NULL ? set->n_sindexes != 0 : ns->n_setless_sindexes != 0;
}


static inline bool
respond_on_master_complete(as_transaction* tr)
{
	return tr->origin == FROM_CLIENT &&
			TR_WRITE_COMMIT_LEVEL(tr) == AS_WRITE_COMMIT_LEVEL_MASTER &&
			(tr->flags & AS_TRANSACTION_FLAG_SWITCH_TO_COMMIT_ALL) == 0;
}


// FIXME - switch p_n_bins to uint16_t*.
static inline void
append_bin_to_destroy(as_bin* b, as_bin* bins, uint32_t* p_n_bins)
{
	if (as_bin_is_external_particle(b)) {
		bins[(*p_n_bins)++] = *b;
	}
}


// Not a nice way to specify a read-all op - dictated by backward compatibility.
// Note - must check this before checking for normal read op!
static inline bool
op_is_read_all(as_msg_op* op, as_msg* m)
{
	return op->name_sz == 0 && op->op == AS_MSG_OP_READ &&
			(m->info1 & AS_MSG_INFO1_GET_ALL) != 0;
}


static inline bool
is_valid_ttl(uint32_t ttl)
{
	// Note - for now, ttl must be as_msg record_ttl.
	// Note - ttl <= MAX_ALLOWED_TTL includes ttl == TTL_NAMESPACE_DEFAULT.
	return ttl <= MAX_ALLOWED_TTL ||
			ttl == TTL_NEVER_EXPIRE || ttl == TTL_DONT_UPDATE;
}


static inline bool
is_ttl_disallowed(uint32_t ttl, const as_namespace* ns)
{
	// Note: Excludes TTL_NEVER_EXPIRE and TTL_DONT_UPDATE.
	return ((int32_t)ttl > 0 ||
			(ttl == TTL_NAMESPACE_DEFAULT && ns->default_ttl != 0)) &&
					ns->nsup_period == 0 && ! ns->allow_ttl_without_nsup;
}


static inline void
clear_delete_response_metadata(as_transaction* tr)
{
	// If write became delete, respond to origin with no metadata.
	if ((tr->flags & AS_TRANSACTION_FLAG_IS_DELETE) != 0) {
		tr->generation = 0;
		tr->void_time = 0;
		tr->last_update_time = 0;
	}
}


//==========================================================
// Private API - for enterprise separation only.
//

void write_delete_record(struct as_index_s* r, struct as_index_tree_s* tree);

uint32_t dup_res_pack_repl_state_info(const struct as_index_s* r, const struct as_namespace_s* ns);
bool dup_res_should_retry_transaction(struct rw_request_s* rw, uint32_t result_code);
void dup_res_handle_tie(struct rw_request_s* rw, const msg* m, uint32_t result_code);
void apply_if_tie(struct rw_request_s* rw);
void dup_res_translate_result_code(struct rw_request_s* rw);
void dup_res_init_repl_state(struct as_remote_record_s* rr, uint32_t info);

void repl_write_init_repl_state(struct as_remote_record_s* rr, bool from_replica);
conflict_resolution_pol repl_write_conflict_resolution_policy(const struct as_namespace_s* ns);
bool repl_write_should_retransmit_replicas(struct rw_request_s* rw, uint32_t result_code);
void repl_write_send_confirmation(struct rw_request_s* rw);
void repl_write_handle_confirmation(msg* m);

int record_replace_check(struct as_index_s* r, struct as_namespace_s* ns);
void record_replaced(struct as_index_s* r, struct as_remote_record_s* rr);

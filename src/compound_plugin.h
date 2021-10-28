#pragma once

#include "eth_internals.h"
#include "eth_plugin_interface.h"
#include <string.h>

#define NUM_SELECTORS         11
#define NUM_COMPOUND_BINDINGS 9
// Name of the plugin.
#define PLUGIN_NAME "Compound"

// TODO: add doc.
#define TOKEN_SENT_FOUND     1
#define TOKEN_RECEIVED_FOUND (1 << 1)

typedef enum {
    COMPOUND_REDEEM_UNDERLYING = 0,
    COMPOUND_REDEEM,
    COMPOUND_MINT,
    COMPOUND_BORROW,
    COMPOUND_REPAY_BORROW,
    COMPOUND_REPAY_BORROW_ON_BEHALF,
    COMPOUND_TRANSFER,
    COMPOUND_LIQUIDATE_BORROW,
    COMPOUND_MANUAL_VOTE,
    COMPOUND_VOTE_DELEGATE
} compoundSelector_t;

typedef enum {
    MINT_AMOUNT = 0,
    REDEEM_TOKENS,
    REDEEM_AMOUNT,
    RECIPIENT,
    BORROW_AMOUNT,
    BORROWER,
    AMOUNT,
    REPAY_AMOUNT,
    DELEGATEE,
    COLLATERAL,
    PROPOSAL_ID,
    SUPPORT,
    UNEXPECTED_PARAMETER,
} parameter;
typedef struct compound_parameters_t {
    uint8_t selectorIndex;
    uint8_t amount[32];
    uint8_t ticker_1[MAX_TICKER_LEN];
    uint8_t decimals;
} compound_parameters_t;

typedef struct underlying_asset_decimals_t {
    char c_ticker[MAX_TICKER_LEN];
    uint8_t decimals;
} underlying_asset_decimals_t;

extern const uint8_t *const COMPOUND_SELECTORS[NUM_SELECTORS];
const underlying_asset_decimals_t UNDERLYING_ASSET_DECIMALS[NUM_COMPOUND_BINDINGS] = {
    {"cDAI", 18},
    {"CETH", 18},
    {"CUSDC", 6},
    {"CZRX", 18},
    {"CUSDT", 6},
    {"CBTC", 8},
    {"CBAT", 18},
    {"CREP", 18},
    {"cSAI", 18},
};

bool get_underlying_asset_decimals(char *compound_ticker, uint8_t *out_decimals) {
    for (size_t i = 0; i < NUM_COMPOUND_BINDINGS; i++) {
        underlying_asset_decimals_t *binding =
            (underlying_asset_decimals_t *) PIC(&UNDERLYING_ASSET_DECIMALS[i]);
        if (strncmp(binding->c_ticker,
                    compound_ticker,
                    strnlen(binding->c_ticker, MAX_TICKER_LEN)) == 0) {
            *out_decimals = binding->decimals;
            return true;
        }
    }
    return false;
}

typedef struct context_t {
    // For display.
    uint8_t address_one[ADDRESS_LENGTH];
    uint8_t address_two[ADDRESS_LENGTH];
    uint8_t amount[INT256_LENGTH];
    uint8_t proposal_id[INT256_LENGTH];
    uint8_t support[INT256_LENGTH];

    char ticker[MAX_TICKER_LEN];
    uint8_t decimals;
    uint8_t token_found;

    // For parsing data.
    uint8_t next_param;  // Set to be the next param we expect to parse.
    uint16_t offset;     // Offset at which the array or struct starts.
    bool go_to_offset;   // If set, will force the parsing to iterate through parameters until
                         // `offset` is reached.

    // For both parsing and display.
    compoundSelector_t selectorIndex;
} context_t;

// Piece of code that will check that the above structure is not bigger than 5 * 32. Do not remove
// this check.
_Static_assert(sizeof(context_t) <= 5 * 32, "Structure of parameters too big.");

void handle_provide_parameter(void *parameters);
void handle_query_contract_ui(void *parameters);
void handle_init_contract(void *parameters);
void handle_finalize(void *parameters);
void handle_provide_token(void *parameters);
void handle_query_contract_id(void *parameters);
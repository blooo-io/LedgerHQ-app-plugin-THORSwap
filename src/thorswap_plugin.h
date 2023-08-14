#pragma once

#include <string.h>
#include "eth_internals.h"
#include "eth_plugin_interface.h"

#define NUM_THORSWAP_SELECTORS 3

#define PLUGIN_NAME "THORSwap"

#define TOKEN_SENT_FOUND     1       // REMOVE IF NOT USED
#define TOKEN_RECEIVED_FOUND 1 << 1  // REMOVE IF NOT USED

extern const uint8_t PLUGIN_ETH_ADDRESS[ADDRESS_LENGTH];  // REMOVE IF NOT USED
extern const uint8_t NULL_ETH_ADDRESS[ADDRESS_LENGTH];    // REMOVE IF NOT USED

// Returns 1 if corresponding address is the address for the chain token (ETH, BNB, MATIC,
#define ADDRESS_IS_NETWORK_TOKEN(_addr)                    \
    (!memcmp(_addr, PLUGIN_ETH_ADDRESS, ADDRESS_LENGTH) || \
     !memcmp(_addr, NULL_ETH_ADDRESS, ADDRESS_LENGTH))

typedef enum { SWAP, SWAPIN, DEPOSIT_WITH_EXPIRY } selector_t;

extern const uint8_t *const THORSWAP_SELECTORS[NUM_THORSWAP_SELECTORS];

typedef enum {
    SEND_SCREEN,
    RECEIVE_SCREEN,
    RECIPIENT_SCREEN,
    CHAIN_SCREEN,
    MEMO_SCREEN,
    WARN_TOKEN_SCREEN,
    WARN_CHAIN_SCREEN,
    ERROR,
} screens_t;

#define AMOUNT_SENT       0   // Amount sent by the user to the contract.
#define AMOUNT_RECEIVED   1   // Amount sent by the contract to the user.
#define TOKEN_SENT        2   // Token sent by the user to the contract.
#define TOKEN_RECEIVED    3   // Token sent by the contract to the user.
#define RECIPIENT_ADDRESS 4   // Recipient of the contract call.
#define SELL_ASSET_LENGTH 5   // Length of sellAsset.
#define BUY_ASSET_LENGTH  6   // Length of buyAsset.
#define SAVE_OFFSET       7   // To save an offset.
#define MEMO              8   // To save the memo.
#define SKIP              9   // Placeholder variant to be set when skipping a step or more.
#define NONE              10  // Placeholder variant to be set when parsing is done.

// Number of decimals used when the token wasn't found in the CAL.
#define DEFAULT_DECIMAL WEI_TO_ETHER

// Ticker used when the token wasn't found in the CAL.
#define DEFAULT_TICKER ""

// sellAsset and buyAsset are respectively stored in contract_address_sent and
// contract_address_received
#define MAX_ASSET_LENGTH ADDRESS_LENGTH - 1

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
typedef struct plugin_parameters_t {
    uint8_t amount_sent[INT256_LENGTH];
    uint8_t amount_received[INT256_LENGTH];
    uint8_t contract_address_sent[ADDRESS_LENGTH];
    uint8_t contract_address_received[ADDRESS_LENGTH];
    char ticker_sent[MAX_TICKER_LEN];
    char ticker_received[MAX_TICKER_LEN];

    uint16_t offset;
    uint16_t checkpoint;
    uint8_t next_param;
    uint8_t tokens_found;
    uint8_t valid;
    uint8_t decimals_sent;
    uint8_t decimals_received;
    uint8_t selectorIndex;
    uint8_t flags;
    uint8_t skip;
    uint8_t sell_asset_length;
    uint8_t buy_asset_length;
    bool go_to_offset;
} plugin_parameters_t;  // Remove any variable not used
// 32*2 + 2*20 + 11*2 = 126
// 2*2 + 1*11 = 15
// 15+126 = 141

// Piece of code that will check that the above structure is not bigger than 5 * 32.
// Do not remove this check.
_Static_assert(sizeof(plugin_parameters_t) <= 5 * 32, "Structure of parameters too big.");

void handle_provide_parameter(void *parameters);
void handle_query_contract_ui(void *parameters);
void handle_finalize(void *parameters);
void handle_init_contract(void *parameters);
void handle_provide_token(void *parameters);
void handle_query_contract_id(void *parameters);

static inline void printf_hex_array(const char *title __attribute__((unused)),
                                    size_t len __attribute__((unused)),
                                    const uint8_t *data __attribute__((unused))) {
    PRINTF(title);
    for (size_t i = 0; i < len; ++i) {
        PRINTF("%02x", data[i]);
    };
    PRINTF("\n");
}

// The assets are in the following format: Chain.Ticker.Decimals
// with an exception for ethereum where it is Chain.Ticker
// Here are a set of function to parse the assets:

static inline void get_asset_chain(const uint8_t *asset, uint8_t asset_length, char *ticker_out) {
    // Search for the dot
    uint8_t dot_position = 0;
    while (dot_position < asset_length && asset[dot_position] != '.') {
        dot_position++;
    }

    // Check that a dot has been found and that it's not at the begining of the string
    // If no dot has been found, copy the whole string
    if (dot_position) {
        strncpy(ticker_out, (const char *) asset, dot_position);
    }
    ticker_out[dot_position] = '\0';
}

static inline void get_asset_ticker(const uint8_t *asset, uint8_t asset_length, char *ticker_out) {
    // Search for the first dot
    uint8_t dot_position = 0;
    while (dot_position < asset_length && asset[dot_position] != '.') {
        dot_position++;
    }

    // Search for the second dot
    uint8_t second_dot_position = MIN(dot_position + 1, asset_length - 1);
    while (second_dot_position < asset_length && asset[second_dot_position] != '.') {
        second_dot_position++;
    }

    uint8_t ticker_length = second_dot_position - MIN(dot_position + 1, asset_length);
    if (ticker_length) {
        strncpy(ticker_out,
                (const char *) (asset + MIN(dot_position + 1, asset_length)),
                ticker_length);
    }
    ticker_out[ticker_length] = '\0';
}

static inline uint8_t get_asset_decimals(const uint8_t *asset, uint8_t asset_length) {
    // Search for the first dot
    uint8_t dot_position = 0;
    while (dot_position < asset_length && asset[dot_position] != '.') {
        dot_position++;
    }

    // Search for the second dot
    uint8_t second_dot_position = MIN(dot_position + 1, asset_length - 1);
    while (second_dot_position < asset_length && asset[second_dot_position] != '.') {
        second_dot_position++;
    }

    // Check if a second dot is found
    if (second_dot_position < asset_length) {
        // A second dot is found, extract the decimals if available
        uint8_t decimals_length = asset_length - (second_dot_position + 1);
        if (decimals_length > 0) {
            // Convert the decimals substring to a numeric value
            uint8_t decimals = 0;
            for (uint8_t i = 0; i < decimals_length; i++) {
                decimals = decimals * 10 + (asset[second_dot_position + 1 + i] - '0');
            }
            return decimals;
        }
    }
    return DEFAULT_DECIMAL;  // Reached only by ETH.ETH, no decimals found or invalid asset format,
                             // return 18 by default
}

static inline bool is_default_ticker(const char *ticker) {
    return !memcmp(ticker, DEFAULT_TICKER, sizeof(DEFAULT_TICKER));
}

// This function will copy the memo until the second colon into the parsed_memo buffer
// The memo is in the following format: "=:xxxxxx:yyyyyyyy..." and we want to copy "=:xxxxxx:"
static inline void parse_memo(const uint8_t *memo, char *parsed_memo) {
    uint8_t first_colon_position = 0;
    while (first_colon_position < INT256_LENGTH && memo[first_colon_position] != ':') {
        first_colon_position++;
    }
    uint8_t second_colon_position = MIN(first_colon_position + 1, INT256_LENGTH - 1);
    while (second_colon_position < INT256_LENGTH && memo[second_colon_position] != ':') {
        second_colon_position++;
    }

    uint8_t memo_length = MIN(second_colon_position + 1, INT256_LENGTH);
    strncpy(parsed_memo, (const char *) memo, memo_length);
    parsed_memo[memo_length] = '\0';
}
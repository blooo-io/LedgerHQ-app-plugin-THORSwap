#include "thorswap_plugin.h"

// Set UI for the "Send" screen.
static void set_send_ui(ethQueryContractUI_t *msg, plugin_parameters_t *context) {
    strlcpy(msg->title, "Send", msg->titleLength);
    switch (context->selectorIndex) {
        case SWAPIN:
            break;
        case DEPOSIT_WITH_EXPIRY:
            // set network ticker (ETH, BNB, etc) if needed
            if (ADDRESS_IS_NETWORK_TOKEN(context->contract_address_sent)) {
                strlcpy(context->ticker_sent, msg->network_ticker, sizeof(context->ticker_sent));
            }
            break;
        default:
            PRINTF("Unhandled selector Index: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    // Convert to string.
    amountToString(context->amount_sent,
                   INT256_LENGTH,
                   context->decimals_sent,
                   context->ticker_sent,
                   msg->msg,
                   msg->msgLength);
    PRINTF("AMOUNT SENT: %s\n", msg->msg);
}

// Set UI for "Receive" screen.
static void set_receive_ui(ethQueryContractUI_t *msg, plugin_parameters_t *context) {
    strlcpy(msg->title, "Receive", msg->titleLength);
    switch (context->selectorIndex) {
        case SWAP:
            // set network ticker (ETH, BNB, etc) if needed
            if (ADDRESS_IS_NETWORK_TOKEN(context->contract_address_received)) {
                strlcpy(context->ticker_received,
                        msg->network_ticker,
                        sizeof(context->ticker_received));
            }
            break;
        case SWAPIN:
            break;
        default:
            PRINTF("Unhandled selector Index: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    // Convert to string.
    amountToString(context->amount_received,
                   INT256_LENGTH,
                   context->decimals_received,
                   context->ticker_received,
                   msg->msg,
                   msg->msgLength);
    PRINTF("AMOUNT RECEIVED: %s\n", msg->msg);
}

static void set_chain_ui(ethQueryContractUI_t *msg, plugin_parameters_t *context) {
    switch (context->selectorIndex) {
        case SWAPIN:
            strlcpy(msg->title, "To Chain", msg->titleLength);
            char buy_chain[MAX_TICKER_LEN];
            get_asset_chain(context->contract_address_received,
                            context->buy_asset_length,
                            buy_chain);
            strlcpy(msg->msg, buy_chain, msg->msgLength);
            memset(buy_chain, 0, strlen(buy_chain));
            break;
        case SWAP:
        default:
            PRINTF("Unhandled selector Index: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}

static void set_warning_chain_ui(ethQueryContractUI_t *msg,
                                 const plugin_parameters_t *context __attribute__((unused))) {
    strlcpy(msg->title, "WARNING", msg->titleLength);
    strlcpy(msg->msg, "Unknown chain", msg->msgLength);
}

static void set_warning_token_ui(ethQueryContractUI_t *msg,
                                 const plugin_parameters_t *context __attribute__((unused))) {
    strlcpy(msg->title, "WARNING", msg->titleLength);
    strlcpy(msg->msg, "Unknown token", msg->msgLength);
}

static void set_recipient_ui(ethQueryContractUI_t *msg, plugin_parameters_t *context) {
    strlcpy(msg->title, "Recipient", msg->titleLength);
    msg->msg[0] = '0';
    msg->msg[1] = 'x';
    // The recipient address is stored in context->contract_address_sent
    getEthAddressStringFromBinary((uint8_t *) context->contract_address_sent,
                                  msg->msg + 2,
                                  msg->pluginSharedRW->sha3,
                                  0);
}

static void set_memo_ui(ethQueryContractUI_t *msg, plugin_parameters_t *context) {
    strlcpy(msg->title, "Memo", msg->titleLength);
    char memo[INT256_LENGTH];
    parse_memo(context->amount_received, memo);
    strlcpy(msg->msg, memo, msg->msgLength);
    memset(memo, 0, strlen(memo));
}

static screens_t get_screen_swap(ethQueryContractUI_t *msg,
                                 plugin_parameters_t *context __attribute__((unused))) {
    bool token_received_found = context->tokens_found & TOKEN_RECEIVED_FOUND;

    switch (msg->screenIndex) {
        case 0:
            if (token_received_found) {
                return RECEIVE_SCREEN;
            } else {
                return WARN_TOKEN_SCREEN;
            }
            break;
        case 1:
            if (token_received_found) {
                return RECIPIENT_SCREEN;
            } else {
                return RECEIVE_SCREEN;
            }
            break;
        case 2:
            if (token_received_found) {
                return ERROR;
            } else {
                return RECIPIENT_SCREEN;
            }
            break;
        default:
            return ERROR;
    }
}

static screens_t get_screen_swapin(ethQueryContractUI_t *msg,
                                   plugin_parameters_t *context __attribute__((unused))) {
    bool token_sent_found = !is_default_ticker(context->ticker_sent);
    bool token_received_found = !is_default_ticker(context->ticker_received);
    char buy_chain[MAX_TICKER_LEN];
    get_asset_chain(context->contract_address_received, context->buy_asset_length, buy_chain);
    bool chain_found = !is_default_ticker(buy_chain);
    memset(buy_chain, 0, strlen(buy_chain));

    switch (msg->screenIndex) {
        case 0:
            if (token_sent_found) {
                return SEND_SCREEN;
            } else {
                return WARN_TOKEN_SCREEN;
            }
        case 1:
            if (token_sent_found && chain_found) {
                return CHAIN_SCREEN;
            } else if (token_sent_found && !chain_found) {
                return WARN_CHAIN_SCREEN;
            } else {
                return SEND_SCREEN;
            }
        case 2:
            if (token_sent_found && token_received_found) {
                return RECEIVE_SCREEN;
            } else if (token_sent_found && !token_received_found) {
                return WARN_TOKEN_SCREEN;
            } else if (!token_sent_found && chain_found) {
                return CHAIN_SCREEN;
            } else {
                return WARN_CHAIN_SCREEN;
            }
        case 3:
            if (token_sent_found && token_received_found) {
                return ERROR;
            } else if (!token_sent_found && !token_received_found) {
                return WARN_TOKEN_SCREEN;
            } else {
                return RECEIVE_SCREEN;
            }
        case 4:
            if (!token_sent_found && !token_received_found) {
                return RECEIVE_SCREEN;
            } else {
                return ERROR;
            }
        default:
            return ERROR;
    }
}

static screens_t get_screen_deposit_with_expiry(ethQueryContractUI_t *msg,
                                                plugin_parameters_t *context
                                                __attribute__((unused))) {
    bool token_sent_found = context->tokens_found & TOKEN_SENT_FOUND;

    switch (msg->screenIndex) {
        case 0:
            if (token_sent_found) {
                return SEND_SCREEN;
            } else {
                return WARN_TOKEN_SCREEN;
            }
        case 1:
            if (token_sent_found) {
                return MEMO_SCREEN;
            } else {
                return SEND_SCREEN;
            }
        case 2:
            if (token_sent_found) {
                return ERROR;
            } else {
                return MEMO_SCREEN;
            }
        default:
            return ERROR;
    }
}

// Helper function that returns the enum corresponding to the screen that should be displayed.
static screens_t get_screen(ethQueryContractUI_t *msg,
                            plugin_parameters_t *context __attribute__((unused))) {
    switch (context->selectorIndex) {
        case SWAP:
            return get_screen_swap(msg, context);
        case SWAPIN:
            return get_screen_swapin(msg, context);
        case DEPOSIT_WITH_EXPIRY:
            return get_screen_deposit_with_expiry(msg, context);
        default:
            return ERROR;
    }
    return ERROR;
}

void handle_query_contract_ui(void *parameters) {
    ethQueryContractUI_t *msg = (ethQueryContractUI_t *) parameters;
    plugin_parameters_t *context = (plugin_parameters_t *) msg->pluginContext;
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);
    msg->result = ETH_PLUGIN_RESULT_OK;

    screens_t screen = get_screen(msg, context);
    switch (screen) {
        case SEND_SCREEN:
            set_send_ui(msg, context);
            break;
        case RECEIVE_SCREEN:
            set_receive_ui(msg, context);
            break;
        case CHAIN_SCREEN:
            set_chain_ui(msg, context);
            break;
        case MEMO_SCREEN:
            set_memo_ui(msg, context);
            break;
        case WARN_TOKEN_SCREEN:
            set_warning_token_ui(msg, context);
            break;
        case WARN_CHAIN_SCREEN:
            set_warning_chain_ui(msg, context);
            break;
        case RECIPIENT_SCREEN:
            set_recipient_ui(msg, context);
            break;
        default:
            PRINTF("Received an invalid screenIndex %d\n", screen);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}

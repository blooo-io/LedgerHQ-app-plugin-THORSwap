#include "thorswap_plugin.h"

// Set UI for the "Send" screen.
static void set_send_ui(ethQueryContractUI_t *msg, plugin_parameters_t *context) {
    switch (context->selectorIndex) {
        case SWAP:
            strlcpy(msg->title, "Send", msg->titleLength);
            break;
        default:
            PRINTF("Unhandled selector Index: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    // set network ticker (ETH, BNB, etc) if needed
    if (ADDRESS_IS_NETWORK_TOKEN(context->contract_address_sent)) {
        strlcpy(context->ticker_sent, msg->network_ticker, sizeof(context->ticker_sent));
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
    switch (context->selectorIndex) {
        case SWAP:
            strlcpy(msg->title, "Receive", msg->titleLength);
            break;
        default:
            PRINTF("Unhandled selector Index: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    // set network ticker (ETH, BNB, etc) if needed
    if (ADDRESS_IS_NETWORK_TOKEN(context->contract_address_received)) {
        strlcpy(context->ticker_received, msg->network_ticker, sizeof(context->ticker_received));
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

// Set UI for "Warning" screen.
static void set_warning_ui(ethQueryContractUI_t *msg,
                           const plugin_parameters_t *context __attribute__((unused))) {
    strlcpy(msg->title, "WARNING", msg->titleLength);
    strlcpy(msg->msg, "Unknown token", msg->msgLength);
}

static void set_recipient_ui(ethQueryContractUI_t *msg, plugin_parameters_t *context) {
    strlcpy(msg->title, "Recipient", msg->titleLength);
    msg->msg[0] = '0';
    msg->msg[1] = 'x';
    getEthAddressStringFromBinary((uint8_t *) context->recipient_address,
                                  msg->msg + 2,
                                  msg->pluginSharedRW->sha3,
                                  0);
}

// Helper function that returns the enum corresponding to the screen that should be displayed.
static screens_t get_screen(ethQueryContractUI_t *msg,
                            plugin_parameters_t *context __attribute__((unused))) {
    uint8_t index = msg->screenIndex;

    bool token_received_found = context->tokens_found & TOKEN_RECEIVED_FOUND;

    switch (context->selectorIndex) {
        case SWAP:
            switch (index) {
                case 0:
                    if (token_received_found) {
                        return RECEIVE_SCREEN;
                    } else {
                        return WARN_SCREEN;
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
            break;
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
        case WARN_SCREEN:
            set_warning_ui(msg, context);
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

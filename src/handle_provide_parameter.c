#include "thorswap_plugin.h"

// Store the amount received in the form of a string, without any ticker or decimals. These will be
// added when displaying.
static void handle_amount_received(ethPluginProvideParameter_t *msg, plugin_parameters_t *context) {
    copy_parameter(context->amount_received, msg->parameter, INT256_LENGTH);
}

static void handle_amount_sent(ethPluginProvideParameter_t *msg, plugin_parameters_t *context) {
    copy_parameter(context->amount_sent, msg->parameter, INT256_LENGTH);
}

static void handle_token_received(ethPluginProvideParameter_t *msg, plugin_parameters_t *context) {
    copy_address(context->contract_address_received, msg->parameter, INT256_LENGTH);
}

static void handle_recipient_address(ethPluginProvideParameter_t *msg,
                                     plugin_parameters_t *context) {
    switch (context->selectorIndex) {
        case SWAP:
            // Using context->contract_address_sent to store recipient address
            copy_address(context->contract_address_sent, msg->parameter, INT256_LENGTH);
            return;
        default:
            PRINTF("Unhandled selector Index: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}

static void handle_swap(ethPluginProvideParameter_t *msg, plugin_parameters_t *context) {
    switch (context->next_param) {
        case TOKEN_RECEIVED:
            handle_token_received(msg, context);
            printf_hex_array("Token sent: 0x", ADDRESS_LENGTH, context->contract_address_received);
            context->next_param = TOKEN_SENT;
            break;
        case TOKEN_SENT:
            // Do nothing, skip this step
            context->next_param = RECIPIENT_ADDRESS;
            break;
        case RECIPIENT_ADDRESS:
            handle_recipient_address(msg, context);
            printf_hex_array("Recipient: 0x", ADDRESS_LENGTH, context->contract_address_sent);
            context->next_param = AMOUNT_RECEIVED;
            break;
        case AMOUNT_RECEIVED:
            handle_amount_received(msg, context);
            printf_hex_array("Amount received: 0x", INT256_LENGTH, context->amount_received);
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_swapin(ethPluginProvideParameter_t *msg, plugin_parameters_t *context) {
    switch (context->next_param) {
        case SKIP:
            // aggregatorConfig already skipped by passing here
            // Skip params offset and memo offset
            context->skip = 2;
            context->next_param = SAVE_OFFSET;
            break;
        case SAVE_OFFSET:
            // Save sellAsset offset
            context->offset = U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->offset));
            context->next_param = AMOUNT_SENT;
            break;
        case AMOUNT_SENT:
            // Save sellAmount
            handle_amount_sent(msg, context);
            printf_hex_array("Amount sent: 0x", INT256_LENGTH, context->amount_sent);
            // Skip buyAsset offset
            context->skip = 1;
            context->next_param = AMOUNT_RECEIVED;
            break;
        case AMOUNT_RECEIVED:
            // Save buyAmountExpected
            handle_amount_received(msg, context);
            printf_hex_array("Amount received: 0x", INT256_LENGTH, context->amount_received);
            // Go to memo offset
            context->go_to_offset = true;
            context->next_param = SELL_ASSET_LENGTH;
            break;
        case SELL_ASSET_LENGTH: {
            // Truncate sellAsset length if it's too long
            int length = U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(uint16_t));
            context->sell_asset_length = MIN(MAX_ASSET_LENGTH, length);
            PRINTF("sellAsset length: %d\n", context->sell_asset_length);
            // Calculate the number of chunks that sellAsset takes
            int nb_chunks_sell_asset = 1 + (length / 32);
            // Calculate and save the offset to the parameter following sellAsset (buyAsset)
            context->offset = msg->parameterOffset + nb_chunks_sell_asset * 32;
            context->next_param = TOKEN_SENT;
            break;
        }
        case TOKEN_SENT:
            // Save sellAsset in contract_address_sent
            memcpy(context->contract_address_sent, msg->parameter, context->sell_asset_length);
            PRINTF("sellAsset: %s\n", context->contract_address_sent);
            // Skip next sellAsset chunks if any
            context->go_to_offset = true;
            context->next_param = BUY_ASSET_LENGTH;
            break;
        case BUY_ASSET_LENGTH: {
            // Truncate buyAsset length if it's too long and save it
            int length = U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(uint16_t));
            context->buy_asset_length = MIN(MAX_ASSET_LENGTH, length);
            PRINTF("buyAsset length: %d\n", context->buy_asset_length);
            context->next_param = TOKEN_RECEIVED;
            break;
        }
        case TOKEN_RECEIVED:
            // Save buyAsset in contract_address_received
            memcpy(context->contract_address_received, msg->parameter, context->buy_asset_length);
            PRINTF("buyAsset: %s\n", context->contract_address_received);
            // Ignore next buyAsset chunks if any
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(void *parameters) {
    ethPluginProvideParameter_t *msg = (ethPluginProvideParameter_t *) parameters;
    plugin_parameters_t *context = (plugin_parameters_t *) msg->pluginContext;

    // We use `%.*H`: it's a utility function to print bytes. You first give
    // the number of bytes you wish to print (in this case, `PARAMETER_LENGTH`) and then
    // the address (here `msg->parameter`).
    PRINTF("plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;
    if (context->skip) {
        // Skip this step and decrease skipping counter.
        context->skip--;
    } else {
        // Skip until we reach the offset.
        if ((context->go_to_offset) && msg->parameterOffset < context->offset + SELECTOR_SIZE) {
            PRINTF("offset: %d, parameterOffset: %d\n", context->offset, msg->parameterOffset);
            return;
        }
        context->go_to_offset = false;
        switch (context->selectorIndex) {
            case SWAP:
                handle_swap(msg, context);
                break;
            case SWAPIN:
                handle_swapin(msg, context);
                break;
            default:
                PRINTF("Selector Index %d not supported\n", context->selectorIndex);
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                break;
        }
    }
}
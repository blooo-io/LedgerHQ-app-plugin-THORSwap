#include "thorswap_plugin.h"

// Store the amount received in the form of a string, without any ticker or decimals. These will be
// added when displaying.
static void handle_amount_received(ethPluginProvideParameter_t *msg, plugin_parameters_t *context) {
    copy_parameter(context->amount_received, msg->parameter, INT256_LENGTH);
}

static void handle_token_received(ethPluginProvideParameter_t *msg, plugin_parameters_t *context) {
    copy_address(context->contract_address_received, msg->parameter, INT256_LENGTH);
}

static void handle_recipient_address(ethPluginProvideParameter_t *msg,
                                     plugin_parameters_t *context) {
    copy_address(context->recipient_address, msg->parameter, INT256_LENGTH);
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
            printf_hex_array("Recipient: 0x", ADDRESS_LENGTH, context->recipient_address);
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
        if ((context->offset) && msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            PRINTF("offset: %d, parameterOffset: %d\n", context->offset, msg->parameterOffset);
            return;
        }
        context->offset = 0;
        switch (context->selectorIndex) {
            case SWAP:
                handle_swap(msg, context);
                break;
            default:
                PRINTF("Selector Index %d not supported\n", context->selectorIndex);
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                break;
        }
    }
}
#include "thorswap_plugin.h"

static void received_network_token(plugin_parameters_t *context) {
    context->decimals_received = WEI_TO_ETHER;
    context->tokens_found |= TOKEN_RECEIVED_FOUND;
}

void handle_finalize(void *parameters) {
    ethPluginFinalize_t *msg = (ethPluginFinalize_t *) parameters;
    plugin_parameters_t *context = (plugin_parameters_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;

    if (context->valid) {
        switch (context->selectorIndex) {
            case SWAP:
                msg->numScreens = 2;
                break;
            case SWAPIN:
                msg->numScreens = 3;
                break;
            default:
                PRINTF("Selector Index %d not supported\n", context->selectorIndex);
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                break;
        }
        if (!ADDRESS_IS_NETWORK_TOKEN(context->contract_address_received)) {
            // Address is not network token (0xeee...) so we will need to look up the token in the
            // CAL.
            printf_hex_array("Setting address received to: ",
                             ADDRESS_LENGTH,
                             context->contract_address_received);
            msg->tokenLookup2 = context->contract_address_received;
        } else {
            received_network_token(context);
            msg->tokenLookup2 = NULL;
        }

        msg->result = ETH_PLUGIN_RESULT_OK;
    } else {
        PRINTF("Context not valid\n");
        msg->result = ETH_PLUGIN_RESULT_FALLBACK;
    }
}

#include "thorswap_plugin.h"

static void received_network_token(plugin_parameters_t *context) {
    context->decimals_received = WEI_TO_ETHER;
    context->tokens_found |= TOKEN_RECEIVED_FOUND;
}

void handle_provide_token(void *parameters) {
    ethPluginProvideInfo_t *msg = (ethPluginProvideInfo_t *) parameters;
    plugin_parameters_t *context = (plugin_parameters_t *) msg->pluginContext;
    PRINTF("Plugin provide tokens : 0x%p, 0x%p\n", msg->item1, msg->item2);
    switch (context->selectorIndex) {
        case SWAP:
            if (ADDRESS_IS_NETWORK_TOKEN(context->contract_address_received)) {
                received_network_token(context);
            } else if (msg->item2 != NULL) {
                context->decimals_received = msg->item2->token.decimals;
                strlcpy(context->ticker_received,
                        (char *) msg->item2->token.ticker,
                        sizeof(context->ticker_received));
                context->tokens_found |= TOKEN_RECEIVED_FOUND;
            } else {
                // CAL did not find the token and token is not ETH.
                context->decimals_received = DEFAULT_DECIMAL;
                strlcpy(context->ticker_received, DEFAULT_TICKER, sizeof(context->ticker_sent));
                // // We will need an additional screen to display a warning message.
                msg->additionalScreens++;
            }
            break;
        case SWAPIN:
            context->decimals_sent =
                get_asset_decimals(context->contract_address_sent, context->sell_asset_length);

            get_asset_ticker(context->contract_address_sent,
                             context->sell_asset_length,
                             context->ticker_sent);

            if (is_default_ticker(context->ticker_sent)) {
                msg->additionalScreens++;
            }
            context->decimals_received =
                get_asset_decimals(context->contract_address_received, context->buy_asset_length);

            get_asset_ticker(context->contract_address_received,
                             context->buy_asset_length,
                             context->ticker_received);
            if (is_default_ticker(context->ticker_received)) {
                msg->additionalScreens++;
            }
            break;
        default:
            PRINTF("Selector Index %d not supported\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}
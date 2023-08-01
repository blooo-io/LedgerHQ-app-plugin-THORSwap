#include "thorswap_plugin.h"

// Need more information about the interface for plugins? Please read the README.md!

// You can check TSSwapGeneric methods here :
// https://etherscan.io/address/0x213255345a740324cbCE0242e32076Ab735906e2
//
//
static const uint8_t THORSWAP_SWAP_SELECTOR[SELECTOR_SIZE] = {0xb5, 0xb4, 0x52, 0x7a};

// Array of all the different THORSwap selectors.
const uint8_t *const THORSWAP_SELECTORS[NUM_THORSWAP_SELECTORS] = {
    THORSWAP_SWAP_SELECTOR,
};

// Ask dummy address ETH
// Remove if not used
const uint8_t PLUGIN_ETH_ADDRESS[ADDRESS_LENGTH] = {0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                                                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                                                    0xee, 0xee, 0xee, 0xee, 0xee, 0xee};

// Remove if not used
const uint8_t NULL_ETH_ADDRESS[ADDRESS_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

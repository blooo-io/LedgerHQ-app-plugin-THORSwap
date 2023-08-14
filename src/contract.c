#include "thorswap_plugin.h"

// Need more information about the interface for plugins? Please read the README.md!

// You can check TSSwapGeneric methods here :
// https://etherscan.io/address/0x213255345a740324cbCE0242e32076Ab735906e2
//
// You can check TSLedgerAdapter methods here :
// https://etherscan.io/address/0xaD545d047E34862121695F8E37aeEB71324a9E78
//
// You can check THORChain_Router methods here :
// https://etherscan.io/address/0xd37bbe5744d730a1d98d8dc97c42f0ca46ad7146

static const uint8_t THORSWAP_SWAP_SELECTOR[SELECTOR_SIZE] = {0xb5, 0xb4, 0x52, 0x7a};
static const uint8_t THORSWAP_SWAPIN_SELECTOR[SELECTOR_SIZE] = {0x4a, 0x95, 0x96, 0x1e};
static const uint8_t THORSWAP_DEPOSIT_WITH_EXPIIRY_SELECTOR[SELECTOR_SIZE] = {0x44,
                                                                              0xbc,
                                                                              0x93,
                                                                              0x7b};

// Array of all the different THORSwap selectors.
const uint8_t *const THORSWAP_SELECTORS[NUM_THORSWAP_SELECTORS] = {
    THORSWAP_SWAP_SELECTOR,
    THORSWAP_SWAPIN_SELECTOR,
    THORSWAP_DEPOSIT_WITH_EXPIIRY_SELECTOR};

const uint8_t NULL_ETH_ADDRESS[ADDRESS_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

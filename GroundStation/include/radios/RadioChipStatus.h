#pragma once
#include <stdint.h>
#include <RadioLib.h>

// RadioLib functions typically return int16_t status codes
using RadioChipStatus = int16_t;

namespace RadioStatus {

// Checks if we error is none, will be false even for crc error
inline bool ok(RadioChipStatus s) {
  return s == RADIOLIB_ERR_NONE;
}

inline bool crcErr(RadioChipStatus s) {
  return s == RADIOLIB_ERR_CRC_MISMATCH;
}

// Error is anything that's not OK and not the CRC special case.
inline bool err(RadioChipStatus s) {
  return (s != RADIOLIB_ERR_NONE) && (s != RADIOLIB_ERR_CRC_MISMATCH);
}

} // namespace RadioStatus

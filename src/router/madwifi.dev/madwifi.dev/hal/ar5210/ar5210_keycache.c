/*
 * Copyright (c) 2002-2006 Sam Leffler, Errno Consulting
 * Copyright (c) 2002-2006 Atheros Communications, Inc.
 * All rights reserved.
 *
 * $Id: //depot/sw/branches/sam_hal/ar5210/ar5210_keycache.c#1 $
 */
#include "opt_ah.h"

#ifdef AH_SUPPORT_AR5210

#include "ah.h"
#include "ah_internal.h"

#include "ar5210/ar5210.h"
#include "ar5210/ar5210reg.h"

#define	AR_KEYTABLE_SIZE	64
#define	KEY_XOR			0xaa

/*
 * Return the size of the hardware key cache.
 */
u_int
ar5210GetKeyCacheSize(struct ath_hal *ah)
{
	return AR_KEYTABLE_SIZE;
}

/*
 * Return the size of the hardware key cache.
 */
HAL_BOOL
ar5210IsKeyCacheEntryValid(struct ath_hal *ah, u_int16_t entry)
{
	if (entry < AR_KEYTABLE_SIZE) {
		u_int32_t val = OS_REG_READ(ah, AR_KEYTABLE_MAC1(entry));
		if (val & AR_KEYTABLE_VALID)
			return AH_TRUE;
	}
	return AH_FALSE;
}

/*
 * Clear the specified key cache entry.
 */
HAL_BOOL
ar5210ResetKeyCacheEntry(struct ath_hal *ah, u_int16_t entry)
{
	if (entry < AR_KEYTABLE_SIZE) {
		OS_REG_WRITE(ah, AR_KEYTABLE_KEY0(entry), 0);
		OS_REG_WRITE(ah, AR_KEYTABLE_KEY1(entry), 0);
		OS_REG_WRITE(ah, AR_KEYTABLE_KEY2(entry), 0);
		OS_REG_WRITE(ah, AR_KEYTABLE_KEY3(entry), 0);
		OS_REG_WRITE(ah, AR_KEYTABLE_KEY4(entry), 0);
		OS_REG_WRITE(ah, AR_KEYTABLE_TYPE(entry), 0);
		OS_REG_WRITE(ah, AR_KEYTABLE_MAC0(entry), 0);
		OS_REG_WRITE(ah, AR_KEYTABLE_MAC1(entry), 0);
		return AH_TRUE;
	}
	return AH_FALSE;
}

/*
 * Sets the mac part of the specified key cache entry and mark it valid.
 */
HAL_BOOL
ar5210SetKeyCacheEntryMac(struct ath_hal *ah, u_int16_t entry, const u_int8_t *mac)
{
	u_int32_t macHi, macLo;

	if (entry < AR_KEYTABLE_SIZE) {
		/*
		 * Set MAC address -- shifted right by 1.  MacLo is
		 * the 4 MSBs, and MacHi is the 2 LSBs.
		 */
		if (mac != AH_NULL) {
			macHi = (mac[5] << 8) | mac[4];
			macLo = (mac[3] << 24)| (mac[2] << 16)
			      | (mac[1] << 8) | mac[0];
			macLo >>= 1;
			macLo |= (macHi & 1) << 31;	/* carry */
			macHi >>= 1;
		} else {
			macLo = macHi = 0;
		}

		OS_REG_WRITE(ah, AR_KEYTABLE_MAC0(entry), macLo);
		OS_REG_WRITE(ah, AR_KEYTABLE_MAC1(entry),
			macHi | AR_KEYTABLE_VALID);
		return AH_TRUE;
	}
	return AH_FALSE;
}

/*
 * Sets the contents of the specified key cache entry.
 */
HAL_BOOL
ar5210SetKeyCacheEntry(struct ath_hal *ah, u_int16_t entry,
                       const HAL_KEYVAL *k, const u_int8_t *mac, int xorKey)
{
	u_int32_t key0, key1, key2, key3, key4;
	u_int32_t keyType;
	u_int32_t xorMask= xorKey ?
		(KEY_XOR << 24 | KEY_XOR << 16 | KEY_XOR << 8 | KEY_XOR) : 0;

	if (entry >= AR_KEYTABLE_SIZE)
		return AH_FALSE;
	if (k->kv_type != HAL_CIPHER_WEP) {
		HALDEBUG(ah, "%s: cipher %u not supported\n",
			__func__, k->kv_type);
		return AH_FALSE;
	}

	/* NB: only WEP supported */
	if (k->kv_len < 40 / NBBY)
		return AH_FALSE;
	if (k->kv_len <= 40 / NBBY)
		keyType = AR_KEYTABLE_TYPE_40;
	else if (k->kv_len <= 104 / NBBY)
		keyType = AR_KEYTABLE_TYPE_104;
	else
		keyType = AR_KEYTABLE_TYPE_128;

	key0 = LE_READ_4(k->kv_val+0) ^ xorMask;
	key1 = (LE_READ_2(k->kv_val+4) ^ xorMask) & 0xffff;
	key2 = LE_READ_4(k->kv_val+6) ^ xorMask;
	key3 = (LE_READ_2(k->kv_val+10) ^ xorMask) & 0xffff;
	key4 = LE_READ_4(k->kv_val+12) ^ xorMask;
	if (k->kv_len <= 104 / NBBY)
		key4 &= 0xff;

	/*
	 * Note: WEP key cache hardware requires that each double-word
	 * pair be written in even/odd order (since the destination is
	 * a 64-bit register).  Don't reorder these writes w/o
	 * understanding this!
	 */
	OS_REG_WRITE(ah, AR_KEYTABLE_KEY0(entry), key0);
	OS_REG_WRITE(ah, AR_KEYTABLE_KEY1(entry), key1);
	OS_REG_WRITE(ah, AR_KEYTABLE_KEY2(entry), key2);
	OS_REG_WRITE(ah, AR_KEYTABLE_KEY3(entry), key3);
	OS_REG_WRITE(ah, AR_KEYTABLE_KEY4(entry), key4);
	OS_REG_WRITE(ah, AR_KEYTABLE_TYPE(entry), keyType);
	return ar5210SetKeyCacheEntryMac(ah, entry, mac);
}
#endif /* AH_SUPPORT_AR5210 */

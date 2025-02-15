/*
 * Some TDMA support code for mac80211.
 *
 * Copyright 2011-2013	Stanislav V. Korsakov <sta@stasoft.net>
*/

#ifndef	_COMPRESS_H
#define	_COMPRESS_H
#include "../ieee80211_i.h"

#if IS_ENABLED(CPTCFG_MAC80211_COMPRESS)
static void mac80211_compress_uninit(struct ieee80211_sub_if_data *);
static void mac80211_compress_init(struct ieee80211_sub_if_data *);
static bool mac80211_tx_compress(struct ieee80211_sub_if_data *, struct sk_buff *);
static  size_t decompress_wrapper(struct ieee80211_sub_if_data *sdata, char *in, size_t inlen, u8 compression);

#endif

#endif

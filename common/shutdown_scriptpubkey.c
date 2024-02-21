#include "config.h"
#include <bitcoin/script.h>
#include <common/shutdown_scriptpubkey.h>

/* BOLT #2:
 * 3. if (and only if) `option_shutdown_anysegwit` is negotiated:
 *      * `OP_1` through `OP_16` inclusive, followed by a single
 *         push of 2 to 40 bytes
 *         (witness program versions 1 through 16)
 */
static bool is_valid_witnessprog(const u8 *scriptpubkey, size_t scriptpubkey_len)
{
	size_t pushlen;

	if (scriptpubkey_len < 2)
		return false;

	switch (scriptpubkey[0]) {
	case OP_1:
	case OP_2:
	case OP_3:
	case OP_4:
	case OP_5:
	case OP_6:
	case OP_7:
	case OP_8:
	case OP_9:
	case OP_10:
	case OP_11:
	case OP_12:
	case OP_13:
	case OP_14:
	case OP_15:
	case OP_16:
		break;
	default:
		return false;
	}

	pushlen = scriptpubkey[1];
	/* Must be all of the rest of scriptpubkey */
	if (2 + pushlen != scriptpubkey_len) {
		return false;
	}

	return pushlen >= 2 && pushlen <= 40;
}

bool valid_shutdown_scriptpubkey(const u8 *scriptpubkey,
				 bool anysegwit,
				 bool allow_oldstyle)
{
	const size_t script_len = tal_bytelen(scriptpubkey);
	if (allow_oldstyle) {
		if (is_p2pkh(scriptpubkey, script_len, NULL)
		    || is_p2sh(scriptpubkey, script_len, NULL))
			return true;
	}

	return is_p2wpkh(scriptpubkey, script_len, NULL)
		|| is_p2wsh(scriptpubkey, script_len, NULL)
		|| (anysegwit && is_valid_witnessprog(scriptpubkey, script_len));
}

#include "base64.h"

namespace alm
{

const char base64::table[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
		'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
		'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/',
		'\0' };

const char base64::pad = '=';

const short base64::reverse_table[256] = { -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -1, -1, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62,
		-2, -2, -2, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2,
		-2, -2, -2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
		17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2, -2, 26, 27, 28,
		29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
		47, 48, 49, 50, 51, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2 };

const std::string base64::encode(const std::string& src) {
	std::string retval;
	std::string::size_type length(src.size());
	retval.resize(((length + 2) / 3) * 4);
	unsigned char *p(reinterpret_cast<unsigned char*>(&retval[0]));
	const unsigned char *current((const unsigned char*)&src[0]);

	while (length > 2) {
		*p++ = table[current[0] >> 2];
		*p++ = table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
		*p++ = table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
		*p++ = table[current[2] & 0x3f];

		current += 3;
		length -= 3; /* we just handle 3 octets of data */
	}

	if (length != 0) {
		*p++ = table[ current[0] >> 2];
		if (length > 1) {
			*p++ = table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
			*p++ = table[(current[1] & 0x0f) << 2];
			*p++ = pad;
		}
		else {
			*p++ = table[(current[0] & 0x03) << 4];
			*p++ = pad;
			*p++ = pad;
		}
	}

	retval.resize( (std::string::size_type) (p - reinterpret_cast<unsigned char*>(&retval[0]) ) );
	return retval;
}

const std::string base64::decode(const std::string& src) {
	std::string::size_type length(src.size());

	std::string retval;
	retval.resize(length, 0);
	int ch, i(0), j(0), k;

	const unsigned char *current((const unsigned char*)&src[0]);
	unsigned char *result(reinterpret_cast<unsigned char*>(&retval[0]));

	while( length-- > 0 )
	{
		ch = *current++;

		if ( ch == pad ) {
			if (*current != '=' && (i%4) == 1) {
				throw std::invalid_argument("Input is an invalid base64 encoded string");
			}
			continue;
		}

		ch = reverse_table[ch];
		if (ch == -1) {
			continue;
		}
		else if (ch == -2) {
			throw std::invalid_argument("Input is an invalid base64 encoded string");
		}

		switch (i % 4)
		{
		case 0:
			result[j] = ch << 2;
			break;
		case 1:
			result[j++] |= ch >> 4;
			result[j] = (ch & 0x0f) << 4;
			break;
		case 2:
			result[j++] |= ch >>2;
			result[j] = (ch & 0x03) << 6;
			break;
		case 3:
			result[j++] |= ch;
			break;
		}
		i++;
	}

	k = j;
	if (ch == pad) {
		switch(i % 4)
		{
		case 1:
			throw std::invalid_argument("Input is an invalid base64 encoded string");
		case 2:
			k++;
		case 3:
			result[k] = 0;
		}
	}
	retval.resize(j);
	return retval;
}

}

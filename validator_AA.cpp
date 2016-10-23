#include "a5_1.h"
#include <vector>
#include <math.h>

const unsigned state_len = 64;
const unsigned keystream_len = 64;
const unsigned input_known_bits = 53;

int main(int argc, char *argv[])
{
	unsigned long long uint_hex_state53 = 0x148000ad800123;
	unsigned long long uint_hex_keystream = 0xb3f4c70cdd61313f;
	unsigned long long cur_uint_hex_keystream;
	
	std::cout << "state, hex: " << std::hex << uint_hex_state53 << std::endl;
	std::vector<std::vector<bool>> state_vec;
	unsigned addit_bits_number = keystream_len - input_known_bits;
	std::vector<std::vector<bool>> regBlast11varsValues;
	regBlast11varsValues.resize(1<<addit_bits_number);
	std::vector<bool> b_vec;
	b_vec.resize(addit_bits_number);
	a5_1 a5_1_obj;
	unsigned long long tmp;
	std::vector<bool> keystream;
	keystream.resize(keystream_len);

	std::vector<bool> cur_state;
	cur_state.resize(state_len);
	unsigned k = 0;
	while (uint_hex_state53) {
		if (k<regClen)
			cur_state[regAlen + regBlen + k] = (uint_hex_state53 & 1) ? true : false;
		else if ((k>=regClen) && (k<regClen + midB))
			cur_state[regAlen + k - regClen] = (uint_hex_state53 & 1) ? true : false;
		else if ((k >= regClen + midB) && (k<regClen + midB + regAlen))
			cur_state[k - regClen - midB] = (uint_hex_state53 & 1) ? true : false;
		uint_hex_state53 >>= 1;
		k++;
	}
	
	for (unsigned long long index = 0; index < regBlast11varsValues.size(); index++) {
		for (auto &x : b_vec)
			x = 0;
		tmp = index;
		k = 0;
		while (tmp) {
			b_vec[k++] = (tmp & 1) ? true : false;
			tmp >>= 1;
		}
		regBlast11varsValues[index] = b_vec;
	}
	
	for (unsigned long long index = 0; index < regBlast11varsValues.size(); index++) {
		for ( unsigned i=0; i < regBlast11varsValues[index].size(); i++)
			cur_state[regAlen + midB + i] = regBlast11varsValues[index][i];
		a5_1_obj.setKey(cur_state);

		for (unsigned i = 0; i<keystream_len; i++)
			keystream[i] = a5_1_obj.getNextBit();

		//std::reverse(keystream.begin(), keystream.end());

		cur_uint_hex_keystream = 0;
		for (unsigned long long i = 0; i < keystream.size(); i++)
			cur_uint_hex_keystream += keystream[i] ? pow(2, i) : 0;
		if (cur_uint_hex_keystream == uint_hex_keystream)
			std::cout << "keystream, hex: " << std::hex << cur_uint_hex_keystream << std::endl;
	}
	
	return 0;
}
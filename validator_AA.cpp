#include "a5_1.h"
#include <vector>
#include <math.h>
#include <bitset>
#include <fstream>
#include <sstream>

const unsigned state_len = 64;
const unsigned keystream_len = 64;
const unsigned input_known_bits = 53;
const unsigned knownBitsRegB = 11;

int main(int argc, char *argv[])
{
	/*
	8e25d640207ffeaf - поток 
	1f7ffd607f5ffb   - заполнение-коллизия, мой формат // ‭ 11111011111111111110101100000011111110101111111111011‬
	febff7ffff5fdfff - заполнение-оригинал, твой формат // 1111111010111111111101111111111111111111010111111101111111111111
	febff6003f5fdfff - заполнение-решение, твой формат, андерсоновские биты заполнены нулями
	febff6001607dfff - заполнение-коллизия, твой формат, андерсоновские биты заполнены нулями
	*/
	//unsigned long long uint_hex_state53 = 0xc0a8e6d2131a7;
	//unsigned long long uint_hex_keystream = 0x770c0410869366f1;
	//
	unsigned long long uint_hex_state53 = 0;
	unsigned long long uint_hex_keystream = 0;

#ifdef _DEBUG
	argc = 2;
	argv[1] = "anderson_attack_a5_exp4-keystream7-wu2192";
#endif

	if (argc < 2) {
		std::cerr << "Usage : [input file - 64-bit keystream, 53-bit state of A5/1";
		exit(1);
	}
	
	std::string ifile_name = argv[1];
	std::ifstream ifile(ifile_name.c_str());
	if (!ifile.is_open()) {
		std::cerr << "couldn't open file " << ifile_name << std::endl;
		exit(1);
	}
	std::stringstream sstream;
	std::string str;
	getline(ifile, str);
	sstream << str;
	sstream >> std::hex >> uint_hex_keystream >> uint_hex_state53;
	ifile.close();

	//std::cout << "state, hex: " << std::hex << uint_hex_state53 << std::endl;
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
	unsigned k = 0;

	std::vector<bool> cur_state;
	cur_state.resize(state_len);
	for (auto &x : cur_state)
		x = false;

	/*//cur_state[13] = cur_state[21] = cur_state[23] = cur_state[43] = cur_state[54] = cur_state[56] = false;
	std::bitset<keystream_len> bs_stream;
	std::bitset<state_len> bs_state;
	a5_1_obj.setKey(cur_state);
	for (unsigned i = 0; i < cur_state.size(); i++)
		if (cur_state[i]) bs_state.set(i);
	for (unsigned i = 0; i < keystream_len; i++) {
		keystream[i] = a5_1_obj.getNextBit();
		std::cout << (keystream[i] ? "1" : "0");
		if (keystream[i])
			bs_stream.set(i);
	}
	std::cout << std::endl;
	//std::cout << "bitset uulong " << std::hex << bs_stream.to_ullong() << std::endl;
	//std::cout << std::endl;
	cur_uint_hex_keystream = 0;
	for (unsigned long long i = 0; i < keystream.size(); i++)
		cur_uint_hex_keystream += keystream[i] ? (unsigned long long)pow(2, i) : 0;
	//std::cout << "stream from input 11...1 " << cur_uint_hex_keystream << std::endl;
	k = 0;
	tmp = cur_uint_hex_keystream;
	//std::cout << "stream" << std::endl;
	while (tmp) {
		std::cout << (tmp & 1) ? "1" : "0";
		tmp >>= 1;
		k++;
	}

	std::cout << std::endl;
	std::cout << "state hex " << std::hex << bs_state.to_ullong() << std::endl;
	std::cout << "stream hex " << std::hex << bs_stream.to_ullong() << std::endl;
	*/
	k = 0;
	bool tmp_b;
	while (uint_hex_state53) {
		tmp_b = (uint_hex_state53 & 1) ? true : false;
		if (k<regClen)
			cur_state[regAlen + regBlen + k] = tmp_b;
		else if ((k>=regClen) && (k<regClen + knownBitsRegB))
			cur_state[regAlen + k - regClen] = tmp_b;
		else if ((k >= regClen + knownBitsRegB) && (k<regClen + knownBitsRegB + regAlen))
			cur_state[k - regClen - knownBitsRegB] = tmp_b;
		uint_hex_state53 >>= 1;
		k++;
	}
	
	std::cout << "state" << std::endl;
	for (unsigned i = 0; i < cur_state.size(); i++) {
		if ( (i < regAlen + knownBitsRegB) || (i >= regAlen + regBlen) ) // 0..29 30..40 41..63
			std::cout << std::dec << (cur_state[i] ? "1" : "0");
		else
			std::cout << std::dec << "-";
			//std::cout << std::dec << (cur_state[i] ? "" : "-") << i + 1 << " 0" << std::endl;
	}
	std::cout << std::endl;
	k = 0;
	tmp = uint_hex_keystream;
	std::cout << "stream" << std::endl;
	while (tmp) {
		std::cout << (tmp & 1) ? "1" : "0";
		tmp >>= 1;
		k++;
	}
	std::cout << std::endl;
	
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
	
	unsigned long long cur_uint_hex_whole_state = 0;
	unsigned long long cur_uint_hex_keystream = 0;
	for (unsigned long long index = 0; index < regBlast11varsValues.size(); index++) {
		for ( unsigned i=0; i < regBlast11varsValues[index].size(); i++)
			cur_state[regAlen + knownBitsRegB + i] = regBlast11varsValues[index][i];
		a5_1_obj.setKey(cur_state);
		for (unsigned i = 0; i<keystream_len; i++)
			keystream[i] = a5_1_obj.getNextBit();

		cur_uint_hex_keystream = 0;
		for (unsigned long long i = 0; i < keystream.size(); i++)
			cur_uint_hex_keystream += keystream[i] ? (unsigned long long)pow(2, i) : 0;
		if (cur_uint_hex_keystream == uint_hex_keystream) {
			std::cout << "correct. keystream, hex: " << std::hex << cur_uint_hex_keystream << std::endl;
			cur_uint_hex_whole_state = 0;
			for (unsigned long long i = 0; i < cur_state.size(); i++)
				cur_uint_hex_whole_state += cur_state[i] ? (unsigned long long)pow(2, i) : 0;
			std::cout << "correct. whole state, hex: " << std::hex << cur_uint_hex_whole_state << std::endl;
			break;
		}
	}

	std::string ofile_name = ifile_name + "_out";
	std::ofstream ofile(ofile_name.c_str());
	ofile << "correct. keystream, hex: " << std::hex << cur_uint_hex_keystream << std::endl;
	ofile << "correct. whole state, hex: " << std::hex << cur_uint_hex_whole_state << std::endl;
	ofile.close();
	
	return 0;
}
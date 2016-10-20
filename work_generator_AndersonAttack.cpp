#include <iostream>
#include <string>
#include <vector>
#include <fstream>

/*
Work generator for a BOINC project aimed at solving cryptanalysis problems for A5/1 via еру Anderson approach.
Format of WUs: 
keystream(uint64) index_from(uint64) index_to(uint64),
where keystream is a known 64-bit keystream, 
index_from - first index of a interavl
index_to - last one, [index_from, index),
*/

const unsigned long long KEYS_PER_SECOND = (unsigned long long)1.4e10;
const unsigned long long SECONDS_PER_WU = 120;

int main( int argc, char *argv[])
{
	// Known keystreams - each of them corresponds to a cryptanalysis instance for A5/1.
	std::vector<std::string> known_keystream_vec{
	"1000111101100110110010010110000100001000001000000011000011101110",
	"1011011110010111011001000011000001101010000010011010100101110101",
	"0010010001000001111100110110111111101101010111001101001010111011",
	"1001101111111010111011010111111010011011010000101011001111001001",
	"0100101010001000010010000001100100111100010000100111101001001001",
	"1110000111011110111011000111011010111101001001100010110011011100",
	"1110111001101000101110111000010010001001010010111110011011000000",
	"1011100100100100000000111111111011010000110001001001001011010110",
	"0011001010001011010010101110011010101010111000100001101001100011",
	"0011011101001110000001100100101111000001111111101101110111100000" };
	unsigned long long subproblems_total_number = (unsigned long long)pow(2,53);
	unsigned long long subproblems_per_wu = KEYS_PER_SECOND * SECONDS_PER_WU;
	unsigned long long wu_total_number = (unsigned long long)ceil((double)subproblems_total_number / (double)subproblems_per_wu);

	unsigned long long known_keystream_hex;
	std::vector<unsigned long long> known_keystream_hex_vec;
	for ( auto &x : known_keystream_vec ) {
		known_keystream_hex = 0;
		for (unsigned i = 0; i < x.size(); i++)
			known_keystream_hex += x[i] == '1' ? (unsigned long long)pow(2,i) : 0;
		known_keystream_hex_vec.push_back(known_keystream_hex);
	}
	
	unsigned long long cur_index_from, cur_index_to;
	std::ofstream ofile("in_tmp");
	cur_index_from = 42*subproblems_per_wu;
	cur_index_to = 43*subproblems_per_wu;
	ofile << std::hex << known_keystream_hex_vec[0]; 
	ofile << " ";
	ofile << std::hex << cur_index_from;
	ofile << " ";
	ofile << std::hex << cur_index_to;
	ofile << std::endl;
	ofile.close();
	
	return 0;
}
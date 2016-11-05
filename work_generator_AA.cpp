/*
#ifndef _WIN32
#include <mysql.h>
#endif
*/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <sstream>

/*
Work generator for a BOINC project aimed at solving cryptanalysis problems for A5/1 via the Anderson approach.
Format of WUs: 
keystream(uint64) index_from(uint64) index_to(uint64),
keystream - a known 64-bit keystream
index_from - first index of a interavl
index_to - last one
[index_from, index)
*/

const unsigned long long VARS_COUNT_VARS = 53;
const unsigned long long SUBPROBLEMS_PER_CHUNK_VARS = 28;
const unsigned long long SEARCH_SPACE_VARS = VARS_COUNT_VARS - SUBPROBLEMS_PER_CHUNK_VARS;
const unsigned long long CHUNKS_PER_SECOND_VARS = 6;
const unsigned long long SECONDS_PER_WU_VARS = 7;
const unsigned long long WU_VARS = SEARCH_SPACE_VARS - CHUNKS_PER_SECOND_VARS - SECONDS_PER_WU_VARS;

int main( int argc, char *argv[])
{
	/*
#ifndef _WIN32
	pass_file_name = argv[1];
	std::cout << "pass_file_name " << pass_file_name << std::endl;

	char *host = "localhost";
	char *db;
	char *user;
	char *pass;
	MYSQL *conn;

	std::ifstream pass_file;
	pass_file.open(pass_file_name.c_str());
	if (!pass_file.is_open()) {
		std::cerr << "psswd_file not open" << std::endl;
		exit(1);
	}
	std::string str;
	getline(pass_file, str);
	db = new char[str.length() + 1];
	strcpy(db, str.c_str());
	getline(pass_file, str);
	user = new char[str.length() + 1];
	strcpy(user, str.c_str());
	getline(pass_file, str);
	pass = new char[str.length() + 1];
	strcpy(pass, str.c_str());
	std::cout << "db " << db << std::endl;
	std::cout << "user " << user << std::endl;
	pass_file.close();
	pass_file.clear();
	
	conn = mysql_init(NULL);
	if (conn == NULL)
		std::cerr << "Error: can't create MySQL-descriptor\n" << std::endl;
	std::cout << "connected to db" << std::endl;
	
	if (!mysql_real_connect(conn, host, user, pass, db, 0, NULL, 0)) {
		std::cerr << "Error: can't connect to MySQL server" << std::endl;
		exit(1);
	}
	delete[] db;
	delete[] user;
	delete[] pass;
#endif
	*/

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
	unsigned long long wu_total_number = 1ULL << WU_VARS;
	unsigned long long known_keystream_hex;
	std::stringstream sstream;
	std::vector<unsigned long long> known_keystream_hex_vec;
	for (unsigned long long i = 0; i < known_keystream_vec.size(); i++) {
		known_keystream_hex = 0;
		for (unsigned long long j = 0; j < known_keystream_vec[i].size(); j++)
			known_keystream_hex += known_keystream_vec[i][j] == '1' ? (1ULL << j) : 0;
		known_keystream_hex_vec.push_back(known_keystream_hex);
	}
	
	std::string cur_wu_input_file_name, system_str, wu_name;
	std::fstream temp_wu_file;
	unsigned long long cur_index_from = 0, cur_index_to = 0;
	for (unsigned long long keystream_index = 0; keystream_index < known_keystream_hex_vec.size(); keystream_index++) {
		for (unsigned long long wu_index = 0; wu_index < wu_total_number; wu_index++) {
			cur_index_from = wu_index << SUBPROBLEMS_PER_CHUNK_VARS + (SEARCH_SPACE_VARS - WU_VARS);
			cur_index_to = (wu_index + 1) << SUBPROBLEMS_PER_CHUNK_VARS + (SEARCH_SPACE_VARS - WU_VARS);
			
			sstream << "anderson_attack_a5-keystream" << keystream_index << "-wu" << wu_index;
			wu_name = sstream.str();
			cur_wu_input_file_name = "input_" + wu_name;
			sstream.str(""); sstream.clear();

			temp_wu_file.open("tmp_wu_file", std::ios_base::out);
			if (!temp_wu_file.is_open()) {
				std::cerr << "Failed to create wu-input.txt" << std::endl;
				exit(1);
			}

			// write input data to WU file
			temp_wu_file << std::hex << known_keystream_hex_vec[keystream_index];
			temp_wu_file << " ";
			temp_wu_file << std::hex << cur_index_from;
			temp_wu_file << " ";
			temp_wu_file << std::hex << cur_index_to;
			temp_wu_file << std::endl;
			temp_wu_file.close();
			temp_wu_file.clear();
			
			system_str = "cp tmp_wu_file `./bin/dir_hier_path " + cur_wu_input_file_name + "`";
			std::cout << "before system command : " << system_str << std::endl; 
			system(system_str.c_str());
			std::cout << "done" << std::endl;
			system_str = "./bin/create_work -appname anderson_attack_a5 -wu_name " + wu_name +
				" -wu_template ./templates/workunit_template_andersonattack.xml" +
				" -result_template ./templates/result_template_andersonattack.xml " + cur_wu_input_file_name;
			std::cout << "before system command : " << system_str << std::endl;
			system(system_str.c_str());
			std::cout << "done" << std::endl;
		}
	}
	
	return 0;
}
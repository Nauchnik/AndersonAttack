#include <vector>
#include <iostream>

const int regAlen = 19;
const int regBlen = 22;
const int regClen = 23;
const int midA = 8;
const int midB = 10;
const int midC = 10;

bool regA[regAlen];
bool regB[regBlen];
bool regC[regClen];

class a5_1{
public:
	bool shift_rslosA();
	bool shift_rslosB();
	bool shift_rslosC();
	bool majority( bool A, bool B, bool C );
	bool getNextBit();
	void setKey( const std::vector<bool> &key );
	void getState( std::vector<bool> &state );
};

bool a5_1 :: shift_rslosA(){
	bool x = regA[18];
	bool y = regA[18]^regA[17]^regA[16]^regA[13];
	for(int j = 18; j > 0; j=j-1)
		regA[j] = regA[j-1];
	regA[0] = y;
	return x;
}

bool a5_1 :: shift_rslosB(){
	bool x = regB[21];
	bool y = regB[21]^regB[20];
	for(int j = 21; j > 0; j=j-1)
		regB[j] = regB[j-1];
	regB[0] = y;
	return x;
}

bool a5_1 :: shift_rslosC(){
	bool x = regC[22];
	bool y = regC[22]^regC[21]^regC[20]^regC[7];
	for(int j = 22; j > 0; j=j-1)
		regC[j] = regC[j-1];
	regC[0] = y;
	return x;
}

bool a5_1 :: majority(bool A, bool B, bool C){
    return A&B|A&C|B&C;
}

bool a5_1 :: getNextBit() {
	bool maj = majority(regA[midA], regB[midB], regC[midC]);
	if (maj == regA[midA]) shift_rslosA();
	if (maj == regB[midB]) shift_rslosB();
	if (maj == regC[midC]) shift_rslosC();
	return regA[18] ^ regB[21] ^ regC[22];
}

void a5_1 :: setKey( const std::vector<bool> &key )
{
	if ( key.size() < regAlen + regBlen + regClen ) {
		std::cerr << "key.size() < sum length of registers a5/1" << std::endl;
		exit(1);
	}
	unsigned i = 0;
	for(unsigned j=0; j < regAlen; j++)
		regA[j] = key[i++];
	for(unsigned j=0; j < regBlen; j++)
		regB[j] = key[i++];
	for(unsigned j=0; j < regClen; j++)
		regC[j] = key[i++];
}

void a5_1 :: getState(std::vector<bool> &state )
{
	state.clear();
	for(unsigned i = 0; i < regAlen; i++)
		state.push_back( regA[i] );
	for(unsigned i = 0; i < regBlen; i++)
		state.push_back( regB[i] );
	for(unsigned i = 0; i < regClen; i++)
		state.push_back( regC[i] );
}
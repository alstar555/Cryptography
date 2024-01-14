#include <iostream>
#include <string>
#include <vector>
using namespace std;

//Permutation Tables
const int IP[8] = {2, 6, 3, 1, 4, 8, 5, 7};
const int inv_IP[8] = {4, 1, 3, 5, 7, 2, 8, 6};
const int P10[10] = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6};
const int P8[8] = {6, 3, 7, 4, 8, 5, 10, 9};
const int P4[4] = {2, 4, 3, 1};

//Expansion Table
const int E[8] = {4, 1, 2, 3, 2, 3, 4, 1};

//S-Boxes
const string S0[4][4] = {
	{"01", "00", "11", "10"},
	{"11", "10", "01", "00"},
	{"00", "10", "01", "11"},
	{"11", "01", "11", "10"}
};
const string S1[4][4] = {
	{"00", "01", "10", "11"},
	{"10", "00", "01", "11"},
	{"11", "00", "01", "00"},
	{"10", "01", "00", "11"}
};

string permutation(int bits, const int* table, string bin){
	string temp = "";
	for(int i=0; i!= bits; i++){
		int j = table[i] -1;
		temp += bin[j];
	}
	cout << "Permutation: " << temp << endl;
	return temp;
}

string xor_operator(int bits, string bin1, string bin2){
	string temp = "";
	for(int i=0; i!= bits; i++){
		int a = bin1[i] - '0';
        int b = bin2[i] - '0';
        temp += (a ^ b) + '0';
	}
	cout << "xor: " << temp << endl;
	return temp;
}

string SBox_operator(int bits, const string (*S)[4], string bin){
	string temp = "";
	int row;
	int col;
	temp += bin[0];
	temp += bin[3];
	row = stoi(temp, nullptr, 2);
	temp = "";
	temp += bin[1];
	temp += bin[2];
	col = stoi(temp, nullptr, 2);
	temp = S[row][col];
	cout << "SBox: " << temp << endl;
	return temp;
}

vector<string> generate_subkeys(string key_10bit){
	cout << "\nGenerating Keys ..." << endl;
	vector<string> subkeys_8bit;
	string subkey="";
	string temp_subkey="";

	//P10
	subkey = permutation(10, P10, key_10bit);

	//LS-1
	for(int i=1; i!= 5; i++){
		temp_subkey += subkey[i];
	}
	temp_subkey += subkey[0];     

	for(int i=6; i!= 10; i++){
		temp_subkey += subkey[i]; 
	}
	temp_subkey += subkey[5];

	//P8
	subkey = permutation(8, P8, temp_subkey);
	cout << "k1: " << subkey << endl;
	subkeys_8bit.push_back(subkey);

	//LS-2
	subkey = "";
	for(int i=2; i!= 5; i++){
		subkey += temp_subkey[i]; //10000 01100
	}
	subkey += temp_subkey[0];      //00010 10001
	subkey += temp_subkey[1];

	for(int i=7; i!= 10; i++){
		subkey += temp_subkey[i]; 
	}
	subkey += temp_subkey[5];
	subkey += temp_subkey[6];

	//P8
	temp_subkey = permutation(8, P8, subkey);
	cout << "k2: " << temp_subkey << endl;
	subkeys_8bit.push_back(temp_subkey);

	return subkeys_8bit;

}


string DES_encrypt(string key_10bit, string msg_8bit, bool decrypt = false){
	int num_rounds = 2;
	string ciphertext_8bit="";
	string ciphertext_4bit="";
	string temp_ciphertext_8bit="";
	string temp_ciphertext_4bit="";
	vector<string> subkeys_8bit;

	//Generate the subkeys
	subkeys_8bit = generate_subkeys(key_10bit);

	//IP - Initial Permutation of msg
	ciphertext_8bit = permutation(8,IP, msg_8bit);
	string ip_ciphertext_8bit = ciphertext_8bit;

	int start_round = 0;
	int end_round = num_rounds;
	int direction =1;
	int key_count;
	if(decrypt==true){
		cout << "\nDecrypting ..." << endl;
		key_count = num_rounds-1;
		
	}else{
		cout << "\nEncrypting ..." << endl;
	}
	for(int round=0; round!=num_rounds; round ++){
		cout << "\tRound: " << round << endl;
		//Expand R
		temp_ciphertext_8bit = permutation(8, E, ciphertext_8bit.substr(4, 8));

		//xor with key. decrypt keys in reverse
		if(decrypt==true){
			ciphertext_8bit = xor_operator(8, temp_ciphertext_8bit, subkeys_8bit[key_count]);
			key_count--;
		}else{
			ciphertext_8bit = xor_operator(8, temp_ciphertext_8bit, subkeys_8bit[round]);
		}
		
		//S0-Box R
		temp_ciphertext_4bit = SBox_operator(8, S0, ciphertext_8bit);

		//S1-Box R
		temp_ciphertext_4bit += SBox_operator(8, S1, ciphertext_8bit.substr(4, 8));

		//P4 R
		ciphertext_4bit = permutation(4, P4, temp_ciphertext_4bit);

		//xor with R with L
		temp_ciphertext_4bit = xor_operator(4, ciphertext_4bit, ip_ciphertext_8bit);

		if(round != 1){
			//Swap
			ciphertext_8bit = ip_ciphertext_8bit.substr(4,8);
			ciphertext_8bit += temp_ciphertext_4bit;
			ip_ciphertext_8bit = ciphertext_8bit;
		}else{
			ciphertext_8bit = temp_ciphertext_4bit;
			ciphertext_8bit += ip_ciphertext_8bit.substr(4,8);
		}
	}


	//IP-1
	temp_ciphertext_8bit = ciphertext_4bit = permutation(8, inv_IP, ciphertext_8bit);

	return temp_ciphertext_8bit;

}


int main(){

	//example key and msg:
	string key_10bit = "1100011110";
	string msg_8bit = "00101000";
	// string key_10bit = "1010000010";
	// string msg_8bit = "01110010";

	string ciphertext_8bit;
	string decipheredtext_8bit;

	ciphertext_8bit = DES_encrypt(key_10bit, msg_8bit);
	decipheredtext_8bit = DES_encrypt(key_10bit, ciphertext_8bit, true); 

	cout << "\ninput:" << endl;
	cout << "key_10bit:\t" << key_10bit << endl;
	cout << "msg_8bit:\t" << msg_8bit << endl;
	cout << "\noutput:" << endl;
	cout << "ciphertext_8bit:\t" << ciphertext_8bit << endl;
	cout << "decipheredtext_8bit:\t" << decipheredtext_8bit << endl;
	cout << "msg_8bit == decipheredtext_8bit:" << (msg_8bit==decipheredtext_8bit) << endl;

	return 0;
}

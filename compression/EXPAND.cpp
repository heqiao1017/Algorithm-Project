#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <bitset>

using namespace std;

int N, L, S;
int W, F;

string search_buffer;
int sb_len = 0;//search buffer length

string bits_buffer = "";

bool is_end = false;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*get parameters from iostream: stdin*/
void get_parameters_from_stdin() {
    char c;
    string str;
    for (int i = 0; i < 3; i++) {
        if (!cin.eof() && cin.get(c)) {
            str += c;
        }
        else {
            cerr<<"Error: Lack parameter values"<<endl;
            exit(1);
        }
    }
    N = str[0] - '0';
    L = str[1] - '0';
    S = str[2] - '0';
    cerr<<"N: "<<N<<" L: "<<L<<" S: "<<S<<endl;
}

/*get parameters from fstream: file argument*/
void get_parameters(ifstream& infile) {
    char c;
    string str;
    for (int i = 0; i < 3; i++) {
        if (!infile.eof() && infile.get(c)) {
            str += c;
        }
        else {
            cerr<<"Error: Lack parameter values"<<endl;
            exit(1);
        }
    }
    N = str[0] - '0';
    L = str[1] - '0';
    S = str[2] - '0';
    cerr<<"N: "<<N<<" L: "<<L<<" S: "<<S<<endl;
}

/*initialize the window by setting the search buffer to be empty and search buffer length to be zero*/
void initilize_window() {
    search_buffer = "";
    sb_len = 0;
    W = pow(2, N);
    F = pow(2, L);
}

/* convert number in 8-bit binary format of a string */
string int_to_binary(int num) {
    string result;
    bitset<8> x(num);
    result = x.to_string();
    return result;
}

/* convert 8-bit binary into int value*/
int binary_to_int(string binary) {
    int str_len = (int)binary.length();
    
    int sum = 0;
    for (int i = str_len - 1; i >= 0; i--) {
        sum += (binary[i] - '0') * pow(2, str_len - 1 - i);
    }
    
    return sum;
}

/*read strlen of string literal from stdin and append the string literals to the search buffer*/
void read_strlen_literal_from_stdin(int strlen) {
    char c;
    string literals = "";
    int i = strlen;
    
    while (i > 0 && !cin.eof()) {
        cin.get(c);
        putc(c, stdout);//output the string literal to stdout
        literals += c;
        i--;
    }
    
    //append the string literals to the search buffer
    search_buffer += literals;
    sb_len += strlen;
    if (sb_len > W - F) {
        search_buffer = search_buffer.substr(sb_len - W + F);
        sb_len  = W - F;
    }
}

/*read strlen of string literal from file and append the string literals to the search buffer*/
void read_strlen_literal(int strlen, ifstream& infile) {
    char c;
    string literals = "";
    int i = strlen;
    
    while (i > 0 && !infile.eof()) {
        infile.get(c);
        putc(c, stdout);//output the string literal to stdout
        literals += c;
        i--;
    }
    
    //append the string literals to the search buffer
    search_buffer += literals;
    sb_len += strlen;
    if (sb_len > W - F) {
        search_buffer = search_buffer.substr(sb_len - W + F);
        sb_len  = W - F;
    }
}


/*read x bits from stdin (can be N or S) to get the corresponding offset or strlen*/
int read_X_bits_from_stdin(int x) {
    char c;
    while (bits_buffer.length() < x && !cin.eof()) {
        cin.get(c);
        string bits = int_to_binary((int)c);
        bits_buffer += bits;
    }
    string x_str = bits_buffer.substr(0, x);//the first x bits will be the bits to be encoded
    bits_buffer = bits_buffer.substr(x);//delete the decoded bits buffer from the buffer
    
    int len = binary_to_int(x_str);//convert string bits into int value
    
    if (x == N && len < 0) {
        cerr << "ERROR: offset cannot less than 0 !\n";
        exit(1);
    }
    return len;
}

/*read x bits from file (can be N or S) to get the corresponding offset or strlen*/
int read_X_bits(ifstream& infile, int x) {
    char c;
    while (bits_buffer.length() < x && !infile.eof()) {
        infile.get(c);
        string bits = int_to_binary((int)c);
        bits_buffer += bits;
    }
    string x_str = bits_buffer.substr(0, x);//the first x bits will be the bits to be encoded
    bits_buffer = bits_buffer.substr(x);//delete the decoded bits buffer from the buffer
    
    int len = binary_to_int(x_str);//convert string bits into int value
    
    if (x == N && len < 0) {
        cerr << "ERROR: offset cannot less than 0 !\n";
        exit(1);
    }
    return len;
}

/**/
void output_match(int off, int len) {
    string str;
    if (len > off) {//there is overlap, get part of match from the prefix and part of match from itself
        str = search_buffer.substr(sb_len - off, off);
        int left = len - off;
        string tmp = "";
        int i = 0;
        while (left > 0) {
            tmp += str[i%off];
            left--;
            i++;
        }
        str += tmp;
    }
    else {
        str = search_buffer.substr(sb_len - off, len);//get the match from the prefix
    }
    
    //output the prefix match to the stdout
    cout<<str;
    
    //append the match into the search buffer
    search_buffer += str;
    sb_len += len;
    if (sb_len > W - F) {
        search_buffer = search_buffer.substr(sb_len - W + F);
        sb_len  = W - F;
    }
}

/*read L bits from stdin to indicate whether it is string literal or window match*/
void read_L_bits_from_stdin() {
    char c;
    while (bits_buffer.length() < L && !cin.eof()) {
        cin.get(c);
        string bits = int_to_binary((int)c);
        bits_buffer += bits;
    }
    
    string L_str = bits_buffer.substr(0, L);//first L bits will be the code either 0 or offset > 0
    bits_buffer = bits_buffer.substr(L);//delete the L bits from the bits buffer
    
    int len = binary_to_int(L_str);
    
    if (len < 0) {
        cerr << "ERROR: code or offset of L bits cannot be less than 0!\n";
        exit(1);
    }
    else if (len == 0) {//indicate string literal
        //get S bits of strlen, then clear the bits_buffer, then read strlen of chars from stream
        int strlen = read_X_bits_from_stdin(S);
        if (strlen == 0) {//reach the end
            is_end = true;
            return;
        }
        bits_buffer = "";
        read_strlen_literal_from_stdin(strlen);
    }
    else {//inidcate there is window match
        //get N bits of offset
        len++;
        int off = read_X_bits_from_stdin(N);
        output_match(off,len);
    }
    
}

/*read L bits from file to indicate whether it is string literal or window match*/
void read_L_bits(ifstream& infile) {
    char c;
    while (bits_buffer.length() < L && !infile.eof()) {
        infile.get(c);
        string bits = int_to_binary((int)c);
        bits_buffer += bits;
    }
    
    string L_str = bits_buffer.substr(0, L);//first L bits will be the code either 0 or offset > 0
    bits_buffer = bits_buffer.substr(L);//delete the L bits from the bits buffer
    
    int len = binary_to_int(L_str);
    
    if (len < 0) {
        cerr << "ERROR: code or offset of L bits cannot be less than 0!\n";
        exit(1);
    }
    else if (len == 0) {//indicate string literal
        //get S bits of strlen, then clear the bits_buffer, then read strlen of chars from stream
        int strlen = read_X_bits(infile, S);
        if (strlen == 0) {//reach the end
            is_end = true;
            return;
        }
        bits_buffer = "";
        read_strlen_literal(strlen, infile);
    }
    else {//inidcate there is window match
        //get N bits of offset
        len++;
        int off = read_X_bits(infile, N);
        output_match(off,len);
    }
    
}

int main(int argc,char *argv[])
{
    ifstream infile;
    if (argc == 1) {//read from standard input stream
        get_parameters_from_stdin();
        initilize_window();
        
        while (!cin.eof()) {
            if (is_end) {
                break;
            }
            read_L_bits_from_stdin();
        }
    }
    else if (argc > 1) {//read from file
        infile.open(argv[1], ios::in|ios::binary);
        if (!infile.is_open()) {
            cerr << "ERROR: Unable to open file!\n";
            exit(1);
        }
        
        get_parameters(infile);
        initilize_window();
        
        while (!infile.eof()) {
            if (is_end) {
                break;
            }
            read_L_bits(infile);
        }
    }
    else {
        cerr << "ERROR: too many arguments!\n";
        exit(1);
    }
    fflush(stdout);
    if (infile.is_open()) infile.close();
    return 0;
}

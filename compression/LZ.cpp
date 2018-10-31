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
#include <cstdio>
#include <ctime>

using namespace std;
const int NO_OF_CHARS=256;
const int MAX_BIT = 16;

//input file name
char * filename = NULL;

//default of parameters
int N =11, L = 4, S = 3;

int W, F, MAX_LEN, LONGEST_MATCHING;

string search_buffer = "";
int sb_len = 0;

string lookahead_buffer = "";
int la_len = 0;

//continues_literal_len start with 0
int continues_literal_len = 0;

//buffer to save stirng literal
string continues_string_literal="";

struct node
{
    int offset;
    node* next;
};

node* offset_array[NO_OF_CHARS];

typedef pair<int, int> len_offset;//pair of double token: <len, offset>

string output_buffer = "";
int output_buffer_len = 0;

/* number of bytes output */
int output_size = 3;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* set N, or L, or S and filename, if no filename or illegal parameter, print error and exit */
void set_parameters(int argc,char *argv[]) {
    if (argc < 2) {
        cerr << "ERROR: No filname specified!\n";
        exit(1);
    }
    
    //parse arguments
    for (int i = 1; i < argc; i++) {
        string str(argv[i]);
        if (argv[i][0] == '-' &&
            str.substr(0, 2) != "-N" &&
            str.substr(0, 2) != "-L" &&
            str.substr(0, 2) != "-S") {
            cerr << "ERROR: illegal parameter!\n";
            exit(1);
        }
        if (argv[i][0] != '-') {//check for options
            if (filename == NULL)
                filename = argv[i];
            else {
                cerr << "ERROR: too many filenames!\n";
                exit(1);
            }
        }
        else {//get N or L or S
            int num = stoi(str.substr(str.find("=") + 1));
            if (argv[i][1] == 'N') {
                if (num < 9 || num > 14) {
                    cerr << "ERROR: exceed range of N!\n";
                    exit(1);
                }
                N = num;
            }else if (argv[i][1] == 'L') {
                if (num < 3 || num > 4) {
                    cerr << "ERROR: exceed range of L!\n";
                    exit(1);
                }
                L = num;
            }else {
                if (num < 1 || num > 5) {
                    cerr << "ERROR: exceed range of S!\n";
                    exit(1);
                }
                S = num;
            }
        }
    }
    
    /*   #Initialize useful variables#
         MAX_LEN : max len of string literal
         LONGEST_MATCHING: longest matching of prefix buffer(search buffer: SB)
         W: window size
         F: max LA(lookahead) buffer size   */
    MAX_LEN = pow(2, S) - 1;
    LONGEST_MATCHING = pow(2, L) - 1;
    W = pow(2, N);
    F = pow(2, L);
}

/* output three bytes of N, L, S */
void output_first_three_bytes() {
    char n = N + '0', l = L + '0', s = S + '0';
    cout << n << l << s;
}

/*initilaize each index to be nullptr*/
void initialize_offset_array() {
    for (int i = 0; i < NO_OF_CHARS; i++) {
        offset_array[i] = nullptr;
    }
}

/* initialize window with empty search_buffer and at most F length of lookahead_buffer */
void initilize_window(ifstream& infile) {
    search_buffer = "";
    sb_len = 0;
    
    lookahead_buffer = "";
    la_len = 0;
    char c;

    //fill the lookahead_buffer with F characters
    for (int i = 0; i < F; i++) {
        if (!infile.eof() && infile.get(c)) {
            lookahead_buffer += c;
            la_len += 1;
        }
    }
}

/* shfit window by the number of characters encoded */
void shift_window(ifstream& infile, int shift_num, string match_str) {
    char c;
    
    if (sb_len >= W - F) {
        /* pop the current number of encoded characters in the beginning from the window */
        search_buffer = search_buffer.substr(shift_num);
        search_buffer += match_str;
    }
    else {
        if (sb_len + shift_num > W - F) {//append encoded characters and deleting the beginning from the window
            search_buffer = search_buffer.substr(sb_len + shift_num - (W - F));
            search_buffer += match_str;
            sb_len = W - F;
        }
        else {//window is not filled, append encoded characters without deleting the beginning from the window
            search_buffer += match_str;
            sb_len += shift_num;
        }
        
    }
    
    //pop the encoded characters from lookahead_buffer and read in new un-encoded characters
    lookahead_buffer = lookahead_buffer.substr(shift_num);
    for (int i = 0; i < shift_num && !infile.eof(); i++) {
        if (!infile.eof() && infile.get(c)) {
            lookahead_buffer += c;
        }
    }
    la_len = (int)lookahead_buffer.length();
}


/* find longes match in the search buffer, return pair of <match_len, offset> */
pair<int, int> search_longest_match() {
    pair<int, int> to_return;
    int match_len = 1;
    int search_index = 1;//search index in the lookahead_buffer
    int offset = -1;
    
    unsigned char start_char = (unsigned char)lookahead_buffer[0];
    node *head = nullptr;
    head = offset_array[start_char];
    if (head == nullptr) {
        to_return.first = match_len;//if match_len is 1, then the encoded char will be treated as string literal
        to_return.second = offset;
        return to_return;
    }

    while (head != nullptr) {//check the linked list of all the nodes with start character matching with start character of lookahead_buffer

        offset = head->offset;//here the offset means the index of start char in the search buffer

        while (offset + search_index < sb_len && search_index < la_len && search_index < LONGEST_MATCHING
               && search_buffer.substr(offset, search_index + 1) == lookahead_buffer.substr(0, search_index + 1)) {

            search_index++;
        }
        if (offset + search_index == sb_len && search_buffer.substr(offset) == lookahead_buffer.substr(0, search_index)){
            while (search_index < la_len && search_index < LONGEST_MATCHING &&
                   lookahead_buffer[ offset + search_index - sb_len ] == lookahead_buffer[search_index]) {//check the overhead

                search_index++;
            }
        }
        else if (offset + search_index > sb_len) {
            while (search_index < la_len && search_index < LONGEST_MATCHING ) {//check the overhead

                string str1 = search_buffer.substr(offset) + lookahead_buffer.substr(0, offset + search_index - sb_len + 1);
                string str2 = lookahead_buffer.substr(0, search_index + 1);
                if (str1 != str2) {
                    break;
                }
                search_index++;
            }
        }

        if (search_index > match_len) {//always maintain the largest match length
            match_len = search_index;
            to_return.second = offset;
        }

        head = head->next;//check the next node
    }
    
    to_return.first = match_len;
    to_return.second = sb_len - to_return.second;
    return to_return;
}

/*delete the nodes whose offset is between 0 to shifts - 1 and update other nodes with adding shifts*/
void delete_index(int shifts) {
    for (int i = 0; i < NO_OF_CHARS; i++) {
        node *prev = nullptr;
        node *head = nullptr;
        head = offset_array[i];
        while (head != nullptr) {
            if ( head->offset / shifts < 1) {//delete node
                if (prev == nullptr) {//first node
                    offset_array[i] = head->next;
                }
                else {//internal node
                    prev->next = head->next;
                }
                node * tmp = new node;
                tmp = head;
                head = head->next;
                delete tmp;
                tmp = nullptr;
            }
            else {//update node
                head->offset -= shifts;
                prev = head;
                head = head->next;
            }
        }
    }
}

/*update the offset array by insert each start character with the index in the search buffer, always insert at the head of the linked list*/
void update_offset_array(int shifts, string match_pattern) {
    int to_add = W - F - shifts;

    //delete index of 0 ~ shifts - 1 and decrement each index by shifts
    if (sb_len >= W - F) {
        delete_index(shifts);
    }
    else if (sb_len + shifts > W - F) {
        delete_index(sb_len + shifts - (W - F));
    }
    else {
        to_add = sb_len;
    }

    //insert match_pattern into search_buffer
    for (int i = 0; i < shifts; i++) {
        node* head = nullptr;
        head = offset_array[(unsigned char)match_pattern[i]];

        //insert a new node
        node * to_insert = new node;
        to_insert->offset = i + to_add;
        to_insert->next = nullptr;

        offset_array[(unsigned char)match_pattern[i]] = to_insert;

        if (head == nullptr) {
            offset_array[(unsigned char)match_pattern[i]]->next = nullptr;
        }
        else {
            offset_array[(unsigned char)match_pattern[i]]->next = head;
        }
    }
}



/* convert number in binary within the size of it's corresponding size */
string int_to_binary(int num, int bit_len) {
    if (num > pow(2, bit_len) - 1) {
        cerr << "ERROR: len/offset/strlen exceed the max bit length!\n";
        exit(1);
    }
    string result;
    bitset<16> x(num);
    result = x.to_string().substr(16 - bit_len, bit_len);
    return result;
}

/* output the 8 bits in one byte to stdout
 input: str of length 8 */
void output_byte(string str) {
    int str_len = (int)str.length();
    int sum = 0;
    for (int i = str_len - 1; i >= 0; i--) {
        sum += (str[i] - '0') * pow(2, str_len - 1 - i);
    }
    if (str_len < 8) {//if it is less that 8 bits, append zeros at the end to makeup 8 bits
        sum = sum << (8 - str_len);
    }
    putc(sum, stdout);//output the byte
    output_size++;
}

/*keep and bits buffer to combine every 8 bits into a byte then output each byte*/
void send_bits(string str_bits, string literal) {
    int len = (int)str_bits.length();
    output_buffer += str_bits;
    output_buffer_len += len;
    
    while (output_buffer_len >= 8) {
        output_byte(output_buffer.substr(0, 8));
        output_buffer_len -= 8;
        output_buffer = output_buffer.substr(8);
    }
    
    if (literal != "") {//if there is string literal, then output all the current left bits buffer, even though it is less than 8 bits, we append the trailing zeros to make it one byte
        if (output_buffer_len > 0) {
            output_byte(output_buffer);
            output_buffer = "";
            output_buffer_len = 0;
        }
        cout<<literal;//output the string literal
        output_size += literal.length();
    }
}

/*check the number of matches, if it is 1, then append char into continuous string literal; if > 1, then found a match, output double token*/
void check_output(pair<int, int> len_off, string match_pattern) {
    int match_len = len_off.first;
    if (match_len == 1) {//string literal
        if (MAX_LEN >= continues_literal_len + 1) {//append to continues_string_literal until MAX_LEN
            continues_string_literal += match_pattern;
            continues_literal_len++;
        }
        else {//meet the max limit, then output MAX_LEN of string literals
            string code = int_to_binary(0, L);
            string len = int_to_binary(MAX_LEN, S);
            send_bits(code + len, continues_string_literal);
            continues_string_literal = "";
            continues_literal_len = 0;
            continues_string_literal += match_pattern;
            continues_literal_len++;
        }
        return;
    }

    if (continues_literal_len > 0) {//before output the window match, output the previous continuous string literal<code, strlen, chars>
        string code = int_to_binary(0, L);
        string len = int_to_binary(continues_literal_len, S);
        send_bits(code + len, continues_string_literal);
        continues_string_literal = "";
        continues_literal_len = 0;
    }

    //find a longest match of length 2 or more, output encoding of <len, offset>
    string len = int_to_binary(match_len - 1, L);
    string off = int_to_binary(len_off.second, N);
    send_bits(len + off, "");
}

/*output all the left bits and output EOF*/
void output_eof() {
    if (continues_literal_len > 0) {//if there are continues_literal left, then output them
        string code = int_to_binary(0, L);
        string len = int_to_binary(continues_literal_len, S);
        send_bits(code + len, continues_string_literal);
        continues_string_literal = "";
        continues_literal_len = 0;
    }

    //output L+S bits of 0
    string str1 = int_to_binary(0, L);
    string str2 = int_to_binary(0, S);
    send_bits(str1 + str2, "");

    //if there are bits left to output, output them
    if (output_buffer_len > 0) {
        output_byte(output_buffer);
    }

    fflush(stdout);
}


int main(int argc,char *argv[])
{
    clock_t start;
    double duration;
    start = clock();
    
    //get parameters of N, L, S
    set_parameters(argc, argv);
    
    ifstream infile(filename,ios::binary);
    if (!infile.is_open()) {
        cerr << "ERROR: Unable to open file!\n";
        exit(1);
    }

    output_first_three_bytes();
    initilize_window(infile);
    initialize_offset_array();

    //if there are characters available to encode
    while (la_len > 0) {

        //find longest match from the prefix
        pair<int, int> to_return = search_longest_match();

        int shift_num = to_return.first;
        string match_pattern = lookahead_buffer.substr(0, shift_num);

        update_offset_array(shift_num, match_pattern);

        shift_window(infile, shift_num, match_pattern);

        //check if there are bytes to output
        check_output(to_return, match_pattern);
    }
    
    //indicate the ending of the file compression
    output_eof();

    if (infile.is_open()) infile.close();

    //get the file size
    ifstream file(filename,ios::binary | ios::ate);
    long long file_len = file.tellg();
    if (infile.is_open()) infile.close();
    
    //stop the timer
    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    
    //output informative message to stderr
    cerr << "N: " << N << " L: " << L << " S: "<< S << endl;
    cerr<<"file length: "<< file_len <<'\n';
    cerr<<"output length: "<< output_size <<'\n';
    cerr<<"compression saving: " << (file_len - output_size) / (file_len * 1.0) * 100 << "%" << endl;
    cerr<<"duration: "<< duration <<'\n';

    return 0;
}

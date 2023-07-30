#include<iostream>
#include<vector>
#include<bitset>
#include<sstream>
#include<string>
#include<iomanip>

#define ROTRIGHT(word,bits) (((word) >> (bits)) | ((word) << (32-(bits))))
#define SSIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SSIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))
#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define BSIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define BSIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))


class BLOCKS{
    private:
        std::vector<unsigned long> _blocks;
        unsigned long W[64];
        std::string final_hash;
        std::string get_hex_string(unsigned long in);
        unsigned long H0 = 0x6a09e667;
        unsigned long H1 = 0xbb67ae85;
        unsigned long H2 = 0x3c6ef372;
        unsigned long H3 = 0xa54ff53a;
        unsigned long H4 = 0x510e527f;
        unsigned long H5 = 0x9b05688c;
        unsigned long H6 = 0x1f83d9ab;
        unsigned long H7 = 0x5be0cd19;

    public:
        void print_hash_values();
        BLOCKS(std::string m);
        void add_padding();
        void resize();
        void create_message_schedule();
        void compression();
        std::vector<unsigned long> get_vector();
        std::string get_final_hash();
        void print_w();
};

std::string BLOCKS::get_hex_string(unsigned long in){
    std::bitset<32> bs(in);
    unsigned n = bs.to_ulong();

    std::stringstream sstream;
    sstream << std::hex << std::setw(8) << std::setfill('0') << n;
    std::string temp;
    sstream >> temp;

    return temp;
}

void BLOCKS::print_hash_values(){
    std::cout<<"  H0 : "<<get_hex_string(H0)<<std::endl;
    std::cout<<"  H1 : "<<get_hex_string(H1)<<std::endl;
    std::cout<<"  H2 : "<<get_hex_string(H2)<<std::endl;
    std::cout<<"  H3 : "<<get_hex_string(H3)<<std::endl;
    std::cout<<"  H4 : "<<get_hex_string(H4)<<std::endl;
    std::cout<<"  H5 : "<<get_hex_string(H5)<<std::endl;
    std::cout<<"  H6 : "<<get_hex_string(H6)<<std::endl;
    std::cout<<"  H7 : "<<get_hex_string(H7)<<std::endl;
}

BLOCKS::BLOCKS(std::string m){
    for (int i=0;i<m.length();i++){
        std::bitset<8> _char(m.c_str()[i]);
        _blocks.push_back(_char.to_ulong());
    }
}

void BLOCKS::add_padding(){
    int l = _blocks.size() * 8,
        k = 447 - l;

    _blocks.push_back(0x80); // Add 1 to the End to differentiate between padding and the given number.
    k -= 7;

    for(int i=0;i<k/8;i++){
        _blocks.push_back(0x00000000);
    }

    // Appending length to the end.
    std::bitset<64> big_64bit_blob(l);
    // Split up that 64 bit blob into 8 bit sections.
    std::string big_64bit_string = big_64bit_blob.to_string();

    // Push the first block into the 56th position.
    std::bitset<8> temp_string_holder1(big_64bit_string.substr(0,8));
    _blocks.push_back(temp_string_holder1.to_ulong());

    // Push all the rest of the 8 bit blocks in.
    for(int i = 8; i < 63; i=i+8){
        std::bitset<8> temp_string_holder2(big_64bit_string.substr(i,8));
        _blocks.push_back(temp_string_holder2.to_ulong());
    }
}

void BLOCKS::resize(){
    std::vector<unsigned long> output(16);

    for(int i = 0; i < 64; i = i + 4) {
        std::bitset<32> temp(0);

        temp = (unsigned long) _blocks[i] << 24;
        temp |= (unsigned long) _blocks[i + 1] << 16;
        temp |= (unsigned long) _blocks[i + 2] << 8;
        temp |= (unsigned long) _blocks[i + 3];

        output[i/4] = temp.to_ulong();
    }
    _blocks = output;

}

void BLOCKS::create_message_schedule(){
    for(int t = 0; t <= 15; t++){
        W[t] = _blocks[t] & 0xFFFFFFFF;
    }
    for(int t = 16; t <= 63; t++) {
        W[t] = SSIG1(W[t-2]) + W[t-7] + SSIG0(W[t-15]) + W[t-16];

        // making sure we are still dealing with 32 bit numbers.
        W[t] = W[t] & 0xFFFFFFFF;
    }
}

void BLOCKS::compression(){
    unsigned long k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,
        0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
        0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,0xe49b69c1,0xefbe4786,
        0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,
        0x06ca6351,0x14292967,0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
        0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,
        0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,
        0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
        0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };

    unsigned long temp1;
    unsigned long temp2;
    unsigned long a = H0;
    unsigned long b = H1;
    unsigned long c = H2;
    unsigned long d = H3;
    unsigned long e = H4;
    unsigned long f = H5;
    unsigned long g = H6;
    unsigned long h = H7;

    for( int t = 0; t < 64; t++){
        temp1 = h + EP1(e) + CH(e,f,g) + k[t] + W[t];
        temp2 = EP0(a) + MAJ(a,b,c);
        h = g;
        g = f;
        f = e;
        e = (d + temp1) & 0xFFFFFFFF; // Makes sure that we are still using 32 bits.
        d = c;
        c = b;
        b = a;
        a = (temp1 + temp2) & 0xFFFFFFFF; // Makes sure that we are still using 32 bits.
    }

    H0 = (H0 + a) & 0xFFFFFFFF;
    H2 = (H2 + c) & 0xFFFFFFFF;
    H1 = (H1 + b) & 0xFFFFFFFF;
    H3 = (H3 + d) & 0xFFFFFFFF;
    H4 = (H4 + e) & 0xFFFFFFFF;
    H5 = (H5 + f) & 0xFFFFFFFF;
    H6 = (H6 + g) & 0xFFFFFFFF;
    H7 = (H7 + h) & 0xFFFFFFFF;

    final_hash = get_hex_string(H0) +
                 get_hex_string(H1) +
                 get_hex_string(H2) +
                 get_hex_string(H3) +
                 get_hex_string(H4) +
                 get_hex_string(H5) +
                 get_hex_string(H6) +
                 get_hex_string(H7);
}

std::vector<unsigned long> BLOCKS::get_vector(){
    return _blocks;
}

void BLOCKS::print_w(){
    for ( int i = 0; i< 64 ;i++){
        std::cout<<"  "<<std::bitset<32>(W[i])<<std::endl;
    }
}

std::string BLOCKS::get_final_hash(){
    return final_hash;
}

int main(){
    std::cout<< " Enter the Message : "; std::string message; std::cin>>message;
    BLOCKS blocks(message);
    blocks.add_padding();

    std::cout<< "Blocks after Adding Padding and Appending the 64 bits of Length of the Message to the end : \n ";

    int PADDING = 4;
    for ( auto block : blocks.get_vector() ) {
        if ( PADDING == 0 ) {
            std::cout<< " \n " ;
            PADDING = 4;
        }
        std::cout<<std::bitset<8>(block)<<" ";
        PADDING--;
    }

    std::cin.ignore();
    std::cin.get();
    blocks.resize();

    std::cout<< "\nResizing all the 8bit blocks to 32bit for the Message Schedule W[64] Array : \n W[0 .. 15] := [ \n ";
    PADDING = 2;
    for ( auto block : blocks.get_vector() ) {
        if ( PADDING == 0 ) {
            std::cout<< " \n " ;
            PADDING = 2;
        }
        std::cout<<std::bitset<32>(block)<<"  ";
        PADDING--;
    }; std::cout<<" ] \n";

    std::cin.get();
    blocks.create_message_schedule();

    std::cout<<"\nW[64] Array after the message Schedule: \n";
    blocks.print_w();

    std::cin.get();
    blocks.compression();
    std::cout<<"\n Hash Values after compression : \n";
    blocks.print_hash_values();
    std::cin.get();
    std::cout<<"\n Final Hash : "<< blocks.get_final_hash();
}

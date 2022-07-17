#include <iostream>
#include <random>
#include <bitset>

using namespace std;

unsigned int pool = 0b10101001;

std::random_device rd; //random num from hardware
std::mt19937 gen(rd()); //seed the generator

int count_set_bits(unsigned int v)
{
    unsigned int c; // c accumulates the total bits set in v
    for (c = 0; v; c++)
    {
    v &= v - 1; // clear the least significant bit set
    }
    return c;
}

unsigned int nthset(uint32_t x, int n) {
    int i = 1;
    int c = 0;
    while (c < n)
    {
        if(i & x)
        {
            c++;
        }
        i <<= 1;
    }
    return i>>1;
}

unsigned int pull_from_pool()
{
    //get a random audio file
    //get number of bits set to 1
    int n = count_set_bits(pool);

    //select random number in range of bits set to 1
    std::uniform_int_distribution<int> audio_time(1,n);
    
    int rand_num = audio_time(gen);

    cout<<rand_num<<endl;

    //get the nth bit set to 1
    return (nthset(pool, rand_num));
}

int main()
{
    int n = pull_from_pool();

    cout << "n = " << bitset<8>(n)  << endl;

    return 0;
}

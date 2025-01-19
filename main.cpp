//
//  main.cpp
//  CS_300_HW3
//
//  Created by Zenia Mazhar on 7.12.2024.
//
// Purpose: This is a program that converts binary images to run length encoding strings and makes use of a hash table to store them in different indices, and deals with collisions by using linear probing where the element is placed in the next empty cell after the one where the collision occurred. One can store images and then enter a query to find out if it exists in the hash table, and retrieve it.
// Input, output: input is the image number X, which the program uses to add imageX.txt to the hash table. Or, 'query' can be entered after which the query image number Y will be asked, and the program will attempt to find 'queryY.txt' in the hash table. The user may exit the program anytime by typing 'exit'.
// Author: Zenia Mazhar

#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <array>
#include <vector>

using namespace std;

//hash table class

class HashTable
{
public:
    explicit HashTable(int size = 20); //constructor with default size set to 20, but can be changed to another number by giving it as a parameter
    HashTable( const HashTable & rhs )
    :array( rhs.array ), currentSize( rhs.currentSize ) { } //copy constructor
    bool find( const string & x ) const; //to find a RLE (or any other kind of string that is being stored in the hash table, depending on what the class is being used for) in the hash table, which is provided as the parameter. Returns true if it was able to find it, and false if it was unsuccessful.
    void makeEmpty( ); //to make a hash table empty
    void insert( const string & x ); //to insert a string to the hash table. it doesn't insert it if it is a duplicate.
    void remove( const string & x ); //to remove a string in the hash table.
    const HashTable & operator=( const HashTable & rhs ); //overloaded assignment operator used to copy a hash table into another hash table object.
    enum EntryType { ACTIVE, EMPTY, DELETED }; //to denote if the cell in the hash table is active (contains a string), is empty (doesn't contain a string), or deleted (there was a string stored in it but it was deleted)
private:
    struct HashEntry //struct to hold information about a cell in the hash table
    {
        EntryType info;
        string element;
        HashEntry( const string & e = string( ), EntryType i = EMPTY)
        : element( e ), info( i ){ }
    };
    vector<HashEntry> array; //the hash table vector-based array
    int currentSize; //the current size of the hash table array (including empty cells)
    int findPos( const string & x ) const; //to find either: 1) where a specific string x is stored in the hash table, or 2) where a new string x should be inserted in the hash table
    int hash( const string & key) const; //to apply the hash function
    void rehash( ); //to resize the array to a prime number twice the size
    int nextPrime(int n); //finds the next prime that comes after integer n
    bool isPrime(int n); //to check if an integer n is prime or not
};

//hash table class implementation

HashTable::HashTable( int size )
: array( nextPrime(size) )
{
    makeEmpty( );
}
void HashTable::makeEmpty()
{
    for (int i = 0; i<array.size(); i++)
    {
        array[i].info = EMPTY;
        array[i].element = "";
        
    }
    
}

int HashTable::findPos( const string & x ) const
{

    int p = hash( x );

    while( array[ p ].info != EMPTY && array[ p ].element != x )
    {
        p ++; //increment index
        if( p >= array.size( ) )
        {
            p -= array.size( ); //perform mod if needed
        }
    }
    return p;
}

int HashTable::hash(const string & key) const {
    int hashedObj = 0;
    int base = 257; //prime base to minimize collisions
    int mod = array.size();

    for (int i = 0; i < key.size(); i++) {
        int value = (key[i] == 'W') ? 1 : (key[i] == 'B') ? 0 : key[i] - '0';
        hashedObj = (hashedObj * base % mod + value) % mod;
    }

    return hashedObj;
}


string flatten_str(string file, string filetype){
    
    string result;
    file = filetype + file + ".txt";
    ifstream ifstrm(file);
    
    if (!ifstrm.is_open())
    {
           cout << "Error: Could not open the file!" << endl;
           return ""; // Return an empty string or handle the error appropriately
    }
    
    string line;
    
    while(getline(ifstrm,line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        result+=line;
    }
    ifstrm.close();
    return result;
}
void HashTable::insert( const string & x )
{
    int pos = findPos(x);

       if (array[pos].info == ACTIVE)
           return; // Avoid duplicates

       array[pos] = HashEntry(x, ACTIVE);

       if (++currentSize >= array.size() / 2)
       {
           rehash(); // Expand table if load factor exceeds 0.5
       }
}
void HashTable::remove( const string & x )
{
    int currentPos = findPos( x );

    if( array[currentPos].info == ACTIVE )
    {
        array[ currentPos ].info = DELETED;
        
    }
}
bool HashTable::find( const string & x ) const
{
    int currentPos = findPos( x );
    
    if (array[currentPos].info == ACTIVE)
    {
        return true;
    }
    
    return false;
}

void HashTable::rehash()
{
    vector<HashEntry> oldArray = array;

    // Create new double-sized, empty table
    array.resize( nextPrime( 2 * oldArray.size( ) ));
    for( int j = 0; j < array.size( ); j++ )
    {
        array[ j ].info = EMPTY;
        array[j].element = "";
    }
    

    // Copy table over
    currentSize = 0;
    for( int i = 0; i < oldArray.size( ); i++ )
    if( oldArray[ i ].info == ACTIVE )
    insert( oldArray[ i ].element );
}
 int HashTable::nextPrime( int n )
 {
    if( n % 2 == 0 )
    {
        n++;
        for( ; !isPrime( n ); n += 2 ){}
    }
    return n;
 }

 bool HashTable::isPrime( int n )
 {
     if(n == 2 || n == 3)
     {
         return true;
     }
     if( n == 1 || n % 2 == 0 )
     {
         return false;
     }
     for( int i = 3; i * i <= n; i += 2 )
     {
         if( n % i == 0 )
         {
             return false;
         }
         
     }
     return true;
 }

string RLE(string flattened_img)
{
    string rle = "";
    /*Require: Flattened string of binary image, flattened
     Ensure: Encoded RLE string, rle
     1: Initialize an empty string rle
     2: Initialize count as 1
     3: Set current to the first character of flattened
     4: for each character in flattened starting from the second do
     5: if character equals current then
     6: Increment count by 1
     7: else
     8: Append count and current (as ”W” or ”B”) to rle
     9: Set current to the new character
     10: Reset count to 1
     11: end if
     12: end for
     13: Append count and current (as ”W” or ”B”) to rle (for the last sequence)
     14: return rle*/
    char curr = flattened_img[0];
    int count = 1;
    int i = 1;
    for (;i<flattened_img.length();i++)
    {
        if (flattened_img[i] == curr)
        {
            count++; //increment if current character is same as previous one
        }
        else
        {
            if (flattened_img[i-1] == '1'){
                rle += ( to_string(count) + ("W") );
                
                
            }
            else{
                rle += (to_string(count) + ("B"));
            }
           
            count = 1;
        }
            
        curr = flattened_img[i];
        
    }
    if (flattened_img[i-1] == '1'){
        rle += (to_string(count) + ("W"));
        
    }
    else{
        rle += (to_string(count) + ("B"));
    }
    
    return rle;
    
    
}

int main() {
    string input;
    //make hashtable
    HashTable RLE_hashTable;
    
    while (input != "exit"){
        cout << "Enter image number to insert into the hash table (or 'query' to continue): "<<endl;
        cin >> input;
        try {
            //integer was entered
                int number = stoi(input); // convert the string input to an integer
            
                //read file and flatten string
                input = flatten_str(input, "image");
                
                //covert string to RLE
                input = RLE(input);
                    
                //insert RLE to hash table
                RLE_hashTable.insert(input);
                cout <<"Image "<< number <<" inserted into the hash table."<<endl;
            }
        catch (const invalid_argument& e)
            {
            if (input == "exit")
            {
                //exit
                cout << "Exiting the program!"<<endl;
            }
                
            else if (input == "query")
            {
                string query;
                while (query != "exit"){
                    //query processing
                    
                    cout << "Enter image number to query (or 'exit' to quit): "<<endl;
                    cin >> query;
                    input = query;
                    if (query == "exit")
                    {
                        //exit
                        cout << "Exiting the program!"<<endl;
                    }
                    else
                    {
                        //integer was entered
                        int number = stoi(query); // convert the string input to an integer
                        
                        
                        //read file and flatten string
                        string result;
                        result = flatten_str(query, "query");
                        
                        //covert string to RLE
                        result = RLE(result);
                        
                        //attempt to find RLE in hash table
                        bool found_img = RLE_hashTable.find(result);
                        
                        if (!found_img) //couldn't find RLE in hash table
                        {
                            cout << "No match for the image with encoding: "<<result<<endl;
                        }
                        
                        else
                        {
                            cout <<"RLE String for"<<" query"<<number<<".txt found in hash table."<<endl;
                            string file;
                            file = "query" + query + ".txt";
                            ifstream ifstrm(file);
                            
                            if (!ifstrm.is_open())
                            {
                                cout << "Error: Could not open the file!" << endl;
                            }
                            
                            string line;
                            while(getline(ifstrm,line))
                            {
                                if (!line.empty() && line.back() == '\r') {
                                        line.pop_back();
                                    }
                                cout<<line<<endl;
                            }
                            
                            ifstrm.close();
                            
                        }
                        
                    }
                    
                }
            }
            
            
        }
        
    }
   
    return 0;
}

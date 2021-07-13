#include <iostream>
#include <cstring>
#include <string>
#include <string.h>
#include <list>
#include <vector>
#include <algorithm>
typedef vector<unsigned char> bytearray;
using namespace std;
#define windowSize 255
#define bufferSize 
/*using byte = unsigned char;
class bytearray
{
private:
	byte abc[3];
};*/
string findlongestmatch(string s,string dictionary, int index)
{
	for (int k = index+1;k < dictionary.length();k++)
	{
		//while (k!=dict)
		for (int j = 0;j < index;j++)
		{
			if (dictionary[k] + dictionary[k + 1])
		}
	}
}
string findmatch(string &dictionary){
	int p = 0;
	int n = 0;
	int i, j;
	string s;
	for (i = 0;i < dictionary.length();i++)
	{
		for (j = 0;j < i;j++)
		{
			if (dictionary[i] = dictionary[j]){  
				s = dictionary.substr(0, i);
				findlongestmatch(s, dictionary,i);
			}
				
				
		}
	}
}

int main() {
	string s;
	bytearray arr;
	cin >> s;
	for (int i = 0; i<s.length(); i++)
		arr.push_back(s[i]);

	return 0;
}
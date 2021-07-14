#include <vector>
#include <sstream>
#include <QVector>
#include <QByteArray>
#include <QString>
#include <lz77.hpp>
//#include "Windows.h"
using namespace std;
QVector <QString> split(QString str, char delimiter) {
    QVector<QString> internal;
    stringstream ss(str.toUtf8().constData());
    string tok;

    while (getline(ss, tok, delimiter)) {
        internal.push_back(QString::fromUtf8(tok.c_str()));
	}

	return internal;
}
QString LZ77(QByteArray input, int option)
{
    QString result;
	int length, char_info_selc = 0;
	if (option == 1)
	{
		length = (int)input.length();
		int** result_ary = new int*[3];
		for (int i = 0; i < length; ++i)
			result_ary[i] = new int[length];
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < length; j++)
				result_ary[i][j] = 0;
		}
		int** char_info = new int*[3];
        //< length
        for (int i = 0; i < 3; ++i)
			char_info[i] = new int[length];
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < length; j++)
				char_info[i][j] = 0;
		}
		result_ary[0][0] = 0;
		result_ary[1][0] = 0;
        result_ary[2][0] =input[0];
		int result_count = 1;
		for (int i = 1; i < length; i++)
		{
			for (int j = 0; j < i; j++)
			{
				if (input[i] == input[j])
				{
					char_info[0][char_info_selc] = i - j;
					char_info_selc++;
				}
			}
			for (int j = 0; j < length; j++)
			{
				if (char_info[0][j] != 0)
		        {
					int start = i - char_info[0][j];
					int count = 1;
					for (int k = 0; k < length; k++)
					{
						if (input[start + count] == input[i + count])
							count++;
						else
						{
							char_info[1][j] = count;
							if (i != (length - 1))
							{
								if (char_info[0][j] + count == length)
									char_info[2][j] = 0;
								else
									char_info[2][j] = input[char_info[0][j] + count];
							}
							else
								char_info[2][j] = NULL;	
							break;
						}
					}
				}
			}
			int large = 0;
			for (int k = 1; k < length; k++)
			{
				if (char_info[1][large] == char_info[1][k])
					large = k;
				else if (char_info[1][large] < char_info[1][k])
					large = k;
			}
			if (char_info[1][large] == 0)
				char_info[2][large] = input[i];	
			else
			{
				i += char_info[1][large];
				char_info[2][large] = input[i];	
			}
			result_ary[0][result_count] = char_info[0][large];
			result_ary[1][result_count] = char_info[1][large];
			result_ary[2][result_count] = char_info[2][large];
			result_count++;
			for (int z = 0; z < 2; z++)
			{
				for (int j = 0; j < length; j++)
					char_info[z][j] = 0;
			}
			char_info_selc = 0;
		}
		for (int j = 0; j < length; j++)
		{
			if (result_ary[0][j] == 0 && result_ary[1][j] == 0)
			{
				if (result_ary[2][j] != NULL || result_ary[2][j] != 0)
				{
					char z = result_ary[2][j];
                    result += QString(QString::number(result_ary[0][j]) + "," + QString::number(result_ary[1][j]) + "," + z + " ");
				}
			}
			else{
                result += QString(QString::number(result_ary[0][j]) + "," + QString::number(result_ary[1][j]) + ",0 ");}
		}
		return result;
	}
	else if (option == 2)
	{
        QVector<QString> s_input = split(input, ' ');

		for (int i = 0; i < s_input.size(); ++i)
		{
            QVector<QString> ss_input = split(s_input[i], ',');

            int p = ss_input[0].toInt(),
                l = ss_input[1].toInt();
            QString ch;
            if (ss_input[2][0] == '0')
                ch = ' ';
			else
				ch = ss_input[2];
			if (p != 0){
				int result_len = (int)result.length();
				for (int x = 0; x < l; x++)
					result += result[result_len - p + x];
			}
			if (ch[0] != '0' || ch[0] != NULL)
				result += ch;
        }
		return result;
	}
}
/*int main()
{
    string input, result;
    int method, option;
    method =1;
	cin >> option;
    if (option == 1)
	{
		cin.ignore();
		getline(cin, input);
		if (method == 1)
			result = LZ77(input, 1);
		cout << result << endl;
		exit(0);
	}
	else if (option==2)
	//Decompression//
	 {
		cin.ignore();
		getline(cin, input);
		if (method == 1)
			result = LZ77(input, 2);
		cout << result << endl;
		exit(0);
	 }
	cin.ignore();
	return 0;
}*/

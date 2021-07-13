#include <QString>
#include <QVector>
#include <QByteArray>
using namespace std;
#define windowSize 255
#define bufferSize 
struct Match_Pointer{
    qsizetype begin;
    qsizetype length;
    Match_Pointer(qsizetype begin, qsizetype length);
};
QString findlongestmatch(QString s,QString dictionary, int index)
{
    int k=index;
    QString p="";
    while (k!=dictionary.length())
	{
		for (int j = 0;j < index;j++)
		{
            if (dictionary[k]==s[j]){
                p+=s[j];
                k++;
            }
		}
       return
               // index , p.length(),'????'
	}
}
QString findmatch(QString &dictionary){
	int i, j;
    QString s,pointer;
	for (i = 0;i < dictionary.length();i++)
	{
		for (j = 0;j < i;j++)
		{
            if (dictionary[i] == dictionary[j]){
                s = dictionary.mid(0, i);
				findlongestmatch(s, dictionary,i);
			}
		}
	}
    return pointer;
}

int main() {
    QString s;
    QByteArray arr;
    //cin >> s;
	for (int i = 0; i<s.length(); i++)
		arr.push_back(s[i]);

	return 0;
}

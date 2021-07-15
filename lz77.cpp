#include <QVector>
#include <QString>
#include "lz77.hpp"
#include <QFile>
#include <iterator>
Match_Pointer::Match_Pointer(int begin, int length){
    this->begin = begin;
    this->length = length;
}

Match_Pointer _largest_match(QByteArray::iterator window, QByteArray::iterator look_ahead_buffer){
    QByteArray::iterator i = window, j = look_ahead_buffer;
    QByteArray longest_match("");
    QByteArray current_match("");
    Match_Pointer current(0, 0);
    Match_Pointer longest(0, 0);
    bool match_found = false;
    while(i != look_ahead_buffer){
        if (*i == *j){
            if(!match_found)
                current.begin = static_cast<int>(look_ahead_buffer - i);
            match_found = true;
            current_match.push_back(*i);
            i++;
            j++;
        }
        else{
            if(match_found){
                if (current_match.length() > longest_match.length()){
                    longest_match = current_match;
                    longest.begin = current.begin;
                    longest.length = longest.begin - static_cast<int>(look_ahead_buffer - i);
                }
                current_match = QByteArray("");
                current.begin = 0;
                current.length = 0;
                j = look_ahead_buffer;
                match_found = false;
            }
            else
                i++;
        }
    }
    if (current_match.length() > longest_match.length()){
        longest_match = current_match;
        longest.begin = current.begin;
        longest.length = longest.begin - static_cast<int>(look_ahead_buffer - i);
    }
    return longest;
}

QString minimize(QString file){
    QString minimized_file = "";
    for(int i = 0; i < file.length(); i++){
        if ((i < file.length() - 1) && (file[i] == ' ' && file[i + 1] != ' ')){
            minimized_file += file[i];
            continue;
        } else if ((i < file.length() - 1) && (file[i] == ' ' && file[i + 1] == ' ')){
            continue;
        }
        if(file[i] == '\n' || file[i] == '\t' || file[i] =='\r' || file[i] == '\v' || file[i] == '\f')
            continue;
        minimized_file += file[i];
    }
    return minimized_file;
}

QByteArray compression(QString& file){
    QByteArray minified_file(minimize(file).toUtf8());
    QByteArray compressed("");
    for(QByteArray::iterator it = minified_file.begin(); it != minified_file.end(); it++){
        QByteArray::iterator window = it - 255 > minified_file.begin() ? it - 255 : minified_file.begin();
        Match_Pointer match = _largest_match(window, it);
        compressed.push_back(match.begin);
        if (match.begin != 0){
            it += match.length - 1;
            compressed.push_back(match.length);
        }
        else{
            compressed.push_back(*it);
        }
    }
    return compressed;
}

QString decompression(QByteArray& compressed){
    QByteArray file("");
    unsigned int file_index = 0;
    for(int i = 0; i < compressed.length();){
        int begin = static_cast<int>(static_cast<unsigned char>(compressed[i]));
        if (begin == 0){
            file.push_back(compressed[i+1]);
            file_index++;
            i+=2;
        }
        else {
           int previous = file_index;
           int length = static_cast<int>(static_cast<unsigned char>(compressed[i+1]));
           for(unsigned int j = previous - begin; j < previous - begin + length; j++){
               file.push_back(file[j]);
               file_index++;
           }
           i+=2;
        }
    }
    return QString(file);
}

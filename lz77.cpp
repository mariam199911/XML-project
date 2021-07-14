#include <QVector>
#include <QByteArray>
#include <QString>
#include <lz77.hpp>
using namespace std;
Match_Pointer::Match_Pointer(qsizetype begin, qsizetype length)
{
    this->begin=begin;
    this->length=length;
};
Match_Pointer _largest_match(QByteArray::iterator window, QByteArray::iterator look_ahead_buffer){
    QByteArray::iterator i = window, j = look_ahead_buffer;
    QByteArray longest_match("");
    QByteArray current_match("");
    Match_Pointer current_match_pointer(0, 0);
    Match_Pointer longest_match_pointer(0, 0);
    bool in_a_matching_sequence = false;
    while(i != look_ahead_buffer){
        if (*i == *j){
            if(!in_a_matching_sequence)
                current_match_pointer.begin = static_cast<qsizetype>(look_ahead_buffer - i);
            in_a_matching_sequence = true;
            current_match.push_back(*i);
            i++;
            j++;
        }
        else{
            if(in_a_matching_sequence){
                if (current_match.length() > longest_match.length()){
                    longest_match = current_match;
                    longest_match_pointer.begin = current_match_pointer.begin;
                    longest_match_pointer.length = longest_match_pointer.begin - static_cast<qsizetype>(look_ahead_buffer - i);
                }
                current_match = QByteArray("");
                current_match_pointer.begin = 0;
                current_match_pointer.length = 0;
                j = look_ahead_buffer;
                in_a_matching_sequence = false;
            }
            else
                i++;
        }
    }
    if (current_match.length() > longest_match.length()){
        longest_match = current_match;
        longest_match_pointer.begin = current_match_pointer.begin;
        longest_match_pointer.length = longest_match_pointer.begin - static_cast<int>(look_ahead_buffer - i);
    }
    return longest_match_pointer;
}

QString minify(QString file){
    QString minified_file = "";
    for(int i = 0; i < file.length(); i++){
        if ((i < file.length() - 1) && (file[i] == ' ' && file[i + 1] != ' ')){
            minified_file += file[i];
            continue;
        } else if ((i < file.length() - 1) && (file[i] == ' ' && file[i + 1] == ' ')){
            continue;
        }
        if(file[i] == '\n' || file[i] == '\t' || file[i] =='\r' || file[i] == '\v' || file[i] == '\f')
            continue;
        minified_file += file[i];
    }
    return minified_file;
}

QByteArray compress(QString& file){
    QByteArray minified_file(minify(file).toUtf8());
    QByteArray compressed_byte_array("");
    for(QByteArray::iterator it = minified_file.begin(); it != minified_file.end(); it++){
        QByteArray::iterator window = it - 255 > minified_file.begin() ? it - 255 : minified_file.begin();
        Match_Pointer match = _largest_match(window, it);
        compressed_byte_array.push_back(match.begin);
        if (match.begin != 0){
            it += match.length - 1;
            compressed_byte_array.push_back(match.length);
        }
        else{
            compressed_byte_array.push_back(*it);
        }
    }
    return compressed_byte_array;
}

QString decompress(QByteArray& compressed_byte_array){
    QByteArray file("");
    unsigned int file_index = 0;
    for(qsizetype i = 0; i < compressed_byte_array.length();){
        qsizetype begin = static_cast<qsizetype>(static_cast<unsigned char>(compressed_byte_array[i]));
        if (begin == 0){
            file.push_back(compressed_byte_array[i+1]);
            file_index++;
            i+=2;
        }
        else {
           int prev_file_index = file_index;
           qsizetype length = static_cast<qsizetype>(static_cast<unsigned char>(compressed_byte_array[i+1]));
           for(unsigned int j = prev_file_index - begin; j < prev_file_index - begin + length; j++){
               file.push_back(file[j]);
               file_index++;
           }
           i+=2;
        }
    }
    return QString(file);
}

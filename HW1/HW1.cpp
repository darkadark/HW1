#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class Error : public std::runtime_error {
public:
    explicit Error(const std::string& _Message) : runtime_error(_Message) {}
    Error(const std::string& _filename, const std::string& _Message) : runtime_error("Parse error: file: " + _filename + ". Message: " + _Message) {}
};


std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

void parse(const std::string& filename, const std::string& delimiter, std::vector<std::string>& headers, std::vector<std::vector<double>>& values) {
    std::string s;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    if (file.peek() == EOF) {
        throw std::runtime_error("File is empty: " + filename);
    }

    if (!getline(file, s)) {
        throw Error(filename, "Could not read headers.");
    }

    headers = split(s, delimiter);
    
    std::vector<std::string> str_values;
    int line = 2;
    while (getline(file, s)) {
        str_values = split(s, delimiter);
        if (str_values.size() != headers.size()) {
            throw Error(filename, "Invalid values count. Line: " + std::to_string(line));
        }
        std::vector<double> double_values;
        for (int i = 0; i < str_values.size(); ++i) {
            try{ double_values.push_back(std::stod(str_values[i])); }
            catch (const std::exception& ex) {
                throw Error(filename, "Parse failed. Line: " + std::to_string(line) + ". Position: " + std::to_string(i+1) + ". " + ex.what() + ": " + str_values[i]);
            }
        }
        values.push_back(double_values);
        ++line;
    }

    file.close();
}

int main(int argc, char* argv[])
{
    const std::string delimiter = ",";
    
    std::vector<std::string> headers;
    std::vector<std::vector<double>> values;
    
    std::string filename = argc > 1 ? argv[1] : "HW1.txt";
    try {
        parse(filename, delimiter, headers, values);
        for (int i = 0; i < headers.size(); ++i) {
            if (i > 0)
                std::cout << delimiter;
            std::cout << headers[i];
        }
        std::cout << std::endl;
        for (int i = 0; i < values.size(); ++i) {
            const std::vector<double>& vi = values[i];
            for (int j = 0; j < vi.size(); ++j) {
                if (j > 0)
                    std::cout << delimiter;
                std::cout << vi[j];
                            
            }
            std::cout << std::endl;
        }
    }
    catch (const Error& ex) {
        std::cout << ex.what() << std::endl;
        return 1;
    }
    catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
        return 2;
    }
    
    return 0;
}


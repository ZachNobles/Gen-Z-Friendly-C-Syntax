#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

//compiles the define statements in the header file into a map that will be used to replace keywords
void create_dictionary(std::ifstream& syntax_file, std::unordered_map<std::string, std::string>& dictionary, const std::string mode) {
    std::string line1, line2;
    while (syntax_file >> line1) {
        if (line1 == "#define") {
            syntax_file >> line1 >> line2;
            if (mode == "backward") {
                dictionary[line1] = line2;
            } else if (mode == "forward") {
                dictionary[line2] = line1;

                //extra case for people that cut out std::, so "cout" and "std::cout" are both properly translated 
                if (line2.length() > 5 && line2.substr(0, 5) == "std::") {
                    dictionary[line2.substr(5, line2.length() - 5)] = line1;
                }
            }
        }
    }
}


int main(int argc, char* argv[]) {
    //if you change the file name, change this variable to the new name of the file.
    const std::string syntax_file_name = "gen_z_syntax.h";

    if (argc != 3) {
        std::cerr << "Wrong argument count. Usage:  ./translator.exe [target file] [what to translate to (either 'genz or 'cpp')]\n";
        exit(1);
    }

    const std::string input_file_name = argv[1];

    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        std::cerr << "Couldn't open file \"" << argv[1] << "\" :(\n";
        exit(1);
    }

    std::ifstream syntax_file(syntax_file_name);
    if (!syntax_file.is_open()) {
        std::cerr << "Couldn't open syntax file; make sure it is in the directory and the syntax_file_name variable matches the file name\n";
        exit(1);
    }



    std::string translation = argv[2];
    std::unordered_map<std::string, std::string> dictionary;

    if (translation != "genz" && translation != "cpp") {
        std::cerr << "Unrecognized translation request \"" << translation << "\" - please input 'genz' or 'cpp'\n";
        exit(1);
    }

    std::ofstream output(translation + "_" + input_file_name);

    if (translation == "genz") {
        //add an include for the syntax header 
        output << "#include \"" + syntax_file_name + "\"\n";
        create_dictionary(syntax_file, dictionary, "forward");
    } else if (translation == "cpp") {
        create_dictionary(syntax_file, dictionary, "backward");
    }

    syntax_file.close();


    std::string line = "";
    char letter;
    std::unordered_set<char> delimiters = {';', '[', ']', '(', ')'};

    //find and replace all keywords
    while (input.good()) {
        letter = input.get();
        if (std::isspace(letter) || delimiters.find(letter) != delimiters.end()) {
            if (dictionary.find(line) != dictionary.end()) {
                output << dictionary[line];
            } else {
                output << line;
            }
            line = "";
            output << letter;
        } else {
            line += letter;
        }
    }

    if (!line.empty()) {
        line.pop_back();
        output << line;
    }

    input.close();
    output.close();
    std::cout << "Translated program has been saved to \"" + translation + "_" + input_file_name + "\"  :)\n";
    return 0;
}
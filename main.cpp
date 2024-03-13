#include <iostream>
#include <stack>
#include <functional>
#include <map>


std::vector<int> data;

std::vector<int> ret_addr_stack;

std::map<std::string, std::function<void(void)>> word_map;

void word_add() {
    int first_word = data.back();
    data.pop_back();
    int second_word = data.back();
    data.pop_back();
    int result = first_word + second_word;
    data.push_back(result);
}

void word_sub() {
    int first_word = data.back();
    data.pop_back();
    int second_word = data.back();
    data.pop_back();
    int result = first_word - second_word;
    data.push_back(result);
}

void word_mul() {
    int first_word = data.back();
    data.pop_back();
    int second_word = data.back();
    data.pop_back();
    int result = second_word * first_word;
    data.push_back(result);
}

void word_div() {
    int first_word = data.back();
    data.pop_back();
    int second_word = data.back();
    data.pop_back();
    int result = second_word / first_word;
    data.push_back(result);
}

// TODO implement the proper behavior with signs
void word_mod() {

    int first_word = data.back();
    data.pop_back();
    int second_word = data.back();
    data.pop_back();
    int result = second_word % first_word;
    data.push_back(result);
}

// TODO implement the proper behavior with sings, like in word_mod
void word_div_mod() {
    int first_word = data.back();
    data.pop_back();
    int second_word = data.back();
    data.pop_back();
    int rem_res = second_word % first_word;
    int div_res = second_word / first_word;
    data.push_back(rem_res);
    data.push_back(div_res);
}

void word_negate() {
    int first_word = data.back();
    data.pop_back();
    data.push_back(-1 * first_word);
}

void word_print_top() {
    std::cout << data.back() << std::endl;
}

void word_pop_top() {
    std::cout << data.back() << std::endl;
    data.pop_back();
}

void word_print_all() {
    for (auto&& val : data) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}


void word_drop() {
    data.pop_back();
}

void word_2drop() {
    word_drop();
    word_drop();
}

void word_dup() {
    data.push_back(data.back());
}

void word_over() {
    data.push_back(data.at(data.size() - 2));
}

void word_swap() {
    int first_word = data.back();
    std::cout << "first word is: " << first_word << std::endl;
    int second_word = data.at(data.size() - 2);
    std::cout << "second word is: " << second_word << std::endl;
    data.back() = second_word;
    data.at(data.size() - 2) = first_word;
}

void word_rot() {
    int third_word = data.back();
    int second_word = data.at(data.size() - 2);
    int first_word = data.at(data.size() - 3);
    data.at(data.size() - 1) = first_word;
    data.at(data.size() - 2) = third_word;
    data.at(data.size() - 3) = second_word;
}

void word_less() {
    int second_word = data.back();
    data.pop_back();
    int first_word = data.back();
    data.pop_back();
    bool comp_res = second_word < first_word;
    if (comp_res) {
        data.push_back(0);
    } else {
        data.push_back(-1);
    }
}

void execute_word(std::string& word) {
    if (word_map.count(word)) {
        auto fun = word_map[word];
        fun();
        return;
    }
    // TODO error handling, if its not a valid int
    int parsed = std::stoi(word);
    data.push_back(parsed);
}

void execute_vector_of_words(std::vector<std::string>* words);

void create_function(std::string name, std::vector<std::string>* words) {
    word_map.emplace(name,[words] {
        execute_vector_of_words(words);
    });

}

void execute_vector_of_words(std::vector<std::string>* words) {
    for (int i = 0; i < words->size(); ++i) {
        auto&& word = words->at(i);
        if (word == ":") {
            // TODO Do functions ever need to be deallocated??
            // falls ja, hier gescheid managen
            std::vector<std::string>* fun_words = new std::vector<std::string>();
            i++;
            word = words->at(i);
            std::string name = word;
            i++;
            word = words->at(i);
            // Kommentare, die die Funktion beschreiben
            if (word == "(") {
                while (word != ")") {
                    i++;
                    word = words->at(i);
                }
            }
            // das ) überspringen
            i++;
            word = words->at(i);
            while (word != ";") {
                fun_words->push_back(word);
                i++;
                word = words->at(i);
            }
            create_function(name, fun_words);
            continue;
        }
        if (word == "if") {
            // TODO if else, nicht nur if


            int flag = data.back();
            data.pop_back();
            // if the branch isnt taken, set i to the word before the then
            // the iteration will increment to then, which gets ingored and the execution proceeds normally afterwards
            if (flag == 0) {
                int target_i = i;
                auto&& target_word = word;
                while (target_word != "then") {
                    target_i++;
                    target_word = words->at(target_i);
                }
                target_i--;
                i = target_i;
                continue;
            }
            // das nächste word wird normal ausgeführt
            // wenn das then erreicht wird, wird es übersprungen und danach ist das if vorbei
            continue;
        }
        if (word == "then") {
            continue;
        }
        execute_word(word);
    }
}


int main() {

    // arithmetic
    word_map.emplace("+", word_add);
    word_map.emplace("-", word_sub);
    word_map.emplace("/", word_div);
    word_map.emplace("*", word_mul);
    word_map.emplace("mod", word_mod);
    word_map.emplace("/mod", word_div_mod);
    word_map.emplace("negate", word_negate);

    // stack manipulation
    word_map.emplace("drop", word_drop);
    word_map.emplace("2drop", word_2drop);
    word_map.emplace("dup", word_dup);
    word_map.emplace("over", word_over);
    word_map.emplace("swap", word_swap);
    word_map.emplace("rot", word_rot);

    // related to if
    word_map.emplace("<", word_less);

    word_map.emplace("print_top", word_print_top);
    word_map.emplace("pop_top", word_pop_top);
    word_map.emplace(".", word_pop_top);
    word_map.emplace("print_all", word_print_all);
    word_map.emplace(".s", word_print_all);

    word_map.emplace("testing", [] { std::cout << "hello world" << std::endl; });

    std::cout << "welcome to simpelforth: " << std::endl;
    while (true) {
        std::string line;
        std::getline(std::cin, line);
        std::string word;
        std::vector<std::string> words;
        for (char ind_char : line) {
            if (ind_char == ' ') {
                words.push_back(word);
                word = "";
            } else {
                word += ind_char;
            }
        }
        // if the input doesnt end with ' ', but \n, the last word will be ignored otherwise
        if (line.at(line.size() - 1) != ' ')  {
            words.push_back(word);
        }
        // TODO handle : , ...

        // TODO handle multi line functions
        execute_vector_of_words(&words);
        //std::cin >> line;
    }
    return 0;
}

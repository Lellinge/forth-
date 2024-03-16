#include <iostream>
#include <stack>
#include <functional>
#include <map>


std::vector<int> data;

std::vector<int> jump_stack;

std::map<std::string, std::function<void(void)>> word_map;
std::map<std::string, int32_t> variable_map;
std::vector<void *> index_to_pointer;

void *conv_index_pointer(int index) {
    return index_to_pointer.at(index);
}

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

// TODO implement the proper behavior with signs, like in word_mod
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

void word_store() {
    int index = data.back();
    data.pop_back();
    int value = data.back();
    data.pop_back();
    int* address = static_cast<int *>(conv_index_pointer(index));
    *address = value;
}

void word_fetch() {
    int index = data.back();
    data.pop_back();
    int* address = static_cast<int *>(conv_index_pointer(index));
    data.push_back(*address);
}

void execute_word(std::string& word) {
    if (word_map.count(word)) {
        auto fun = word_map[word];
        fun();
        return;
    }
    if (variable_map.count(word)) {
        int index = variable_map[word];
        data.push_back(index);
        return;
    }
    if (word == "bye") {
        std::exit(0);
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
        // TODO evtl. ein switch
        // TODO schleifen mit stacks und springen implementieren. Das ganze suchen ist suboptimal
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
            bool has_opening_parens = false;
            if (word == "(") {
                has_opening_parens = true;
                while (word != ")") {
                    i++;
                    word = words->at(i);
                }
            }
            // das ) überspringen
            if (has_opening_parens) {
                i++;
            }
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

            // TODO rekursives if
            // allgemein, rekursives if kann der code nicht

            // gibt es ein else for dem then ?
            bool found_else = false;
            int target_i = i;
            int else_i = -1;
            // offensichtlich ist das if, was wir gerade bearbeiten, nicht das then und auch nicht das else
            target_i++;
            auto target_word = words->at(target_i);

            // um bei z.B. diesem code "if < if 1 then else > if 2 then then" das passende then/else zu finden
            int if_depth = 0;
            while (target_word != "then" || if_depth != 0) {
                if (target_word == "if") {
                    if_depth++;
                }
                if (target_word == "then") {
                    if_depth--;
                }
                // keine else von genesteten ifs finden
                if (target_word == "else" && if_depth == 0) {
                    found_else = true;
                    else_i = target_i;
                }
                target_i++;
                target_word = words->at(target_i);
            }
            target_i--;
            int pos_before_then = target_i;

            int flag = data.back();
            data.pop_back();
            // if the branch isnt taken, set i to the word before the then
            // the iteration will increment to then, which gets ingored and the execution proceeds normally afterwards
            if (flag == 0) {
                if (!found_else) {
                    i = pos_before_then;
                    continue;
                }
                // Not sure this is still true
                // next iteration increments i to the first word after then

                // the iteration will set i to the word after the else.
                i = else_i;
                continue;
            }
            // also ist die flag wahr
            // nur falls es ein else gibt muss was gemacht werden
            if (found_else) {
                // wenn das else erreicht wird, wird zu diesem gesprungen
                // target_i ist 1 vor dem then, also aufs then springen
                jump_stack.push_back(target_i + 1);
            }
            // das nächste word wird normal ausgeführt
            // wenn das then erreicht wird, wird es übersprungen und danach ist das if vorbei
            // wenn es ein else gibt, wird wenn es erreicht wird zum then gesprungen
            continue;
        }
        if (word == "then") {
            continue;
        }
        if (word == "begin") {
            continue;
        }
        if (word == "else") {
            i = jump_stack.back();
            jump_stack.pop_back();
            continue;
        }
        if (word == "again" || word == "repeat") {
            // jumps back to the relevant begin
            // TODO genestete loops korrekt behandeln
            // mindestens ein word zurück muss sein ("begin again ")
            i--;
            auto find_word = words->at(i);
            int loop_depth = 0;
            while (find_word != "begin" || loop_depth != 0) {
                if (find_word == "again" || find_word == "repeat") {
                    loop_depth++;
                }
                if (find_word == "begin") {
                    loop_depth--;
                }
                i--;
                find_word = words->at(i);
            }
            continue;
        }
        if (word == "while") {
            int flag = data.back();
            data.pop_back();
            if (flag == 0) {
                // also falsch
                // geht hinter dem passenden repeat weiter
                // mindestens eins abstand (begin while repeat)
                i++;
                auto find_word = words->at(i);
                int loop_depth = 0;
                while (find_word != "repeat" || loop_depth != 0) {
                    if (find_word == "while") {
                        loop_depth++;
                    }
                    if (find_word == "repeat") {
                        loop_depth--;
                    }
                    i++;
                    find_word = words->at(i);
                }
                // i ist jetzt auf dem repeat. Mit der iterations ist es dann dahinter
            }
            continue;
        }
        if (word == "until") {
            int flag = data.back();
            data.pop_back();
            if (flag != 0) {
                std::string find_word;
                int loop_depth = 0;
                do {
                    if (find_word == "repeat" || find_word == "again") {
                        loop_depth++;
                    }
                    if (find_word == "begin" ) {
                        loop_depth--;
                    }
                    i--;
                    find_word = words->at(i);
                } while (find_word != "begin" || loop_depth != 0);
            }
            continue;
        }
        if (word == "variable") {
            std::string name = words->at(i + 1);
            // größe 3 -> 0 1 2
            // also gibt das immer den nächsten index
            // wenn der vector mehr als 32bit braucht, ist das wie mehr als 32bit pointer brauchen
            // es emuliert 32bit pointer, also hat es dieselben probleme
            int index = (int) index_to_pointer.size();
            variable_map.emplace(name, index);
            index_to_pointer.push_back(malloc(8));
            // den namen der variable überspringen
            i++;
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

    // variablen/speicher
    word_map.emplace("!", word_store);
    word_map.emplace("@", word_fetch);

    word_map.emplace("print_top", word_print_top);
    word_map.emplace("pop_top", word_pop_top);
    word_map.emplace(".", word_pop_top);
    word_map.emplace("print_all", word_print_all);
    word_map.emplace(".s", word_print_all);

    word_map.emplace("testing", [] { std::cout << "hello world" << std::endl; });

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

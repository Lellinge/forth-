#include <iostream>
#include <stack>
#include <functional>
#include <map>


std::vector<int> data;

std::vector<int> jump_stack;
std::vector<u_int8_t *> return_stack;

std::map<std::string, std::function<void(void)>> word_map;
std::map<std::string, u_int8_t *> compiled_word_map;
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

void word_invert() {
    int first_word = data.back();
    data.pop_back();
    data.push_back(~first_word);
}

void word_and() {
    int first_word = data.back();
    data.pop_back();
    int second_word = data.back();
    data.pop_back();
    int result = first_word & second_word;
    data.push_back(result);
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

void word_max() {
    int second_word = data.back();
    data.pop_back();
    int first_word = data.back();
    data.pop_back();
    int result = second_word > first_word ? second_word : first_word;
    data.push_back(result);
}

void word_min() {
    int second_word = data.back();
    data.pop_back();
    int first_word = data.back();
    data.pop_back();
    int result = second_word < first_word ? second_word : first_word;
    data.push_back(result);
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

enum opcodes : u_int8_t {
    OP_RETURN = 0,
    OP_CALL = 1,
    OP_ADD = 2,
    OP_SUB = 3,
    OP_NUMBER = 4,
    OP_CALL_NATIVE = 5,
    // TODO die weiteren ops
};

// Implementiert einen bytecode interpreter
// Aktuell sehr unvollständig
u_int8_t *compile_fun(std::string name, std::vector<std::string>* words) {
    int number_words_required = 0;
    number_words_required += 1; // OP_RETURN am ende
    number_words_required += words->size(); // jedes word oder zahl hat einen opcode
    // extra platz für die tatsächlichen Zahlen
    for (auto&& word : *words) {
        for (auto&& ind_char : word) {
            if (!isdigit(ind_char)) {
                // das nächste word muss geprüft werden, ob es eine Zahl ist. Dieses ist es schonmal nicht.
                goto next_word_iteration;
            }
        }
        // ein int_32 für die zahl
        number_words_required += 4;
        next_word_iteration:;
    }
    // werden worte aufgerufen?
    // nativer code (C++) und bytecode Aufrufe brauchen beide 1 OP + 8bytes pointer
    for (auto&& word : *words) {
        // native funktionen
        if (word_map.contains(word)) {
            number_words_required += 8;
            continue;
        }
        // bytecode
        if (compiled_word_map.contains(word)) {
            number_words_required += 8;
        }
    }
    u_int8_t *buf = static_cast<u_int8_t *>(malloc(number_words_required));
    // letzte befehl muss ein return sein
    buf[number_words_required - 1] = OP_RETURN;
    int index_buf = 0;
    for (int i = 0; i < words->size(); ++i) {
        // C++ funktionen aufrufen
        if (word_map.contains(words->at(i))) {
            buf[index_buf] = OP_CALL_NATIVE;
            index_buf++;

            std::function<void()>* ptr = &word_map.at(words->at(i));
            // converts the pointer into an array of bytes
            u_int8_t* ptr_as_bytes = static_cast<u_int8_t *>(static_cast<void *>(&ptr));
            buf[index_buf + 0] = ptr_as_bytes[0];
            buf[index_buf + 1] = ptr_as_bytes[1];
            buf[index_buf + 2] = ptr_as_bytes[2];
            buf[index_buf + 3] = ptr_as_bytes[3];
            buf[index_buf + 4] = ptr_as_bytes[4];
            buf[index_buf + 5] = ptr_as_bytes[5];
            buf[index_buf + 6] = ptr_as_bytes[6];
            buf[index_buf + 7] = ptr_as_bytes[7];
            index_buf += 8;
            continue;
        }
        // bytecode call
        if (compiled_word_map.contains(words->at(i))) {
            buf[index_buf] = OP_CALL;
            index_buf++;
            u_int8_t *ptr = compiled_word_map.at(words->at(i));
            // converts the pointer into a array of bytes
            u_int8_t* ptr_as_bytes = static_cast<u_int8_t *>(static_cast<void *>(&ptr));
            buf[index_buf + 0] = ptr_as_bytes[0];
            buf[index_buf + 1] = ptr_as_bytes[1];
            buf[index_buf + 2] = ptr_as_bytes[2];
            buf[index_buf + 3] = ptr_as_bytes[3];
            buf[index_buf + 4] = ptr_as_bytes[4];
            buf[index_buf + 5] = ptr_as_bytes[5];
            buf[index_buf + 6] = ptr_as_bytes[6];
            buf[index_buf + 7] = ptr_as_bytes[7];
            index_buf += 8;
            continue;
        }
        bool is_number = true;
        for (int j = 0; j < words->at(i).size(); ++j) {
            if (!isdigit(words->at(i).at(j))) {
                is_number = false;
                break;
            }
        }
        if (is_number) {
            buf[index_buf] = OP_NUMBER;
            index_buf++;
            int int_word = std::atoi(words->at(i).c_str());
            u_int8_t* int_arr = static_cast<u_int8_t *>(static_cast<void*>(&int_word));
            buf[index_buf] = int_arr[0];
            buf[index_buf + 1] = int_arr[1];
            buf[index_buf + 2] = int_arr[2];
            buf[index_buf + 3] = int_arr[3];
            index_buf += 4;
            continue;
        }
        if (words->at(i) == "+") {
            buf[index_buf] = OP_ADD;
        }
        if (words->at(i) == "-") {
            buf[index_buf] = OP_SUB;
        }
        index_buf++;
    }
    return buf;
}

void execute_bytecode(u_int8_t* buf) {
    int buf_index = 0;
    while (buf[buf_index] != OP_RETURN || !return_stack.empty()) {
        int temp = buf[buf_index];
        switch (buf[buf_index]) {
            case OP_NUMBER:
                int number;
                // packt die vier bytes aus dem buf wieder in einen int.
                // Wahrscheinlich könnte man das auch in einer Zeile machen, aber es wird vermutlich eh zu einem read optimiert.
                ((u_int8_t *)&number)[0] = buf[buf_index + 1];
                ((u_int8_t *)&number)[1] = buf[buf_index + 2];
                ((u_int8_t *)&number)[2] = buf[buf_index + 3];
                ((u_int8_t *)&number)[3] = buf[buf_index + 4];
                data.push_back(number);
                // die 4bytes nach op_add mit der nummer überspringen
                buf_index += 4;
                break;
            case OP_ADD:
                word_add();
                break;
            case OP_RETURN:
                return;
            case OP_CALL_NATIVE:
                std::function<void()> *fun;
                // entpacken des pointers aus dem bytecode
                // wird vermutlich in einen load optimiert.
                ((u_int8_t *)&fun)[0] = buf[buf_index + 1];
                ((u_int8_t *)&fun)[1] = buf[buf_index + 2];
                ((u_int8_t *)&fun)[2] = buf[buf_index + 3];
                ((u_int8_t *)&fun)[3] = buf[buf_index + 4];
                ((u_int8_t *)&fun)[4] = buf[buf_index + 5];
                ((u_int8_t *)&fun)[5] = buf[buf_index + 6];
                ((u_int8_t *)&fun)[6] = buf[buf_index + 7];
                ((u_int8_t *)&fun)[7] = buf[buf_index + 8];
                (*fun)();
                buf_index += 8;
                break;
            default:
                std::cout << "unhandled bytecode" << (int) buf[buf_index] << std::endl;
                return;
        }
        buf_index++;
    }
}
void compile_function(std::string name, std::vector<std::string>* words) {
    std::cout << "name in compile_function is: " << name << std::endl;
    u_int8_t* buf = compile_fun(name, words);
    word_map.emplace(name,[buf] {
        execute_bytecode(buf);
    });
}


void execute_vector_of_words(std::vector<std::string>* words) {
    for (int i = 0; i < words->size(); ++i) {
        auto&& word = words->at(i);
        // TODO evtl. ein switch
        // TODO schleifen mit stacks und springen implementieren. Das ganze suchen ist suboptimal
        if (word == ":" || word == ":comp") {
            std::cout << "word is: " << word << std::endl;
            auto prev_word = word;
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
            if (prev_word == ":comp") {
                std::cout << "is :comp" << std::endl;
                compile_function(name, fun_words);
            } else {
                create_function(name, fun_words);
            }
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
    word_map.emplace("invert", word_invert);
    word_map.emplace("and", word_and);

    // stack manipulation
    word_map.emplace("drop", word_drop);
    word_map.emplace("2drop", word_2drop);
    word_map.emplace("dup", word_dup);
    word_map.emplace("over", word_over);
    word_map.emplace("swap", word_swap);
    word_map.emplace("rot", word_rot);
    word_map.emplace("max", word_max);
    word_map.emplace("min", word_min);

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

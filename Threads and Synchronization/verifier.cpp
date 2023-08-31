#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <set>
#include <map>

using namespace std;
static const regex int_reg = regex(R"(\d+)");

void print_errors(const string &line, const string &message) {
    fprintf(stderr, "Error at line \"%s\": %s\n", line.c_str(), message.c_str());
}

int main() {
    int phonecalls = 100;
    vector<regex> formats;
    vector<string> expected_formats{"Attempting to connect format: ", "Connects to an available line format: ",
                                    "Speaking to operator format: ", "Proposed a question format: ",
                                    "Hung up format: "};
    vector<string> template_string;
    vector<pair<int, int>> dump;
    map<int, int> index;
    vector<int> ids;
    string s;
    smatch match;
    int id;

    while (getline(cin, s)) {
        regex_search(s, match, int_reg);
        int i = 0;
        for (const auto &format: formats) {
            if (regex_match(s, regex(format))) {
                id = stoi(match[0]);
                goto broken;
            }
            i++;
        }
        if (formats.size() == 5) break;
        id = stoi(match[0]);
        s.replace(match.prefix().length(), match[0].length(), R"(\d+)");
        formats.emplace_back(s);
        fprintf(stderr, "%s%s\n", expected_formats[i].c_str(), s.c_str());
        template_string.push_back(s);
        broken:;
        if (index.find(id) == index.end()) {
            index[id] = ids.size();
        }
        ids.push_back(id);
        dump.emplace_back(i, id);
    }
    vector<bool> called(phonecalls, false);
    vector<bool> lined(phonecalls, false);
    vector<bool> oped(phonecalls, false);
    vector<bool> asked(phonecalls, false);
    vector<bool> left(phonecalls, false);

    set<int> lines;
    set<int> ops;

    for (const auto &data: dump) {
        int type = data.first;
        int display_id = data.second;
        id = index[data.second];

        string line = template_string[type];
        line.replace(line.find(R"(\d+)"), 3, to_string(display_id));
        printf("%s\n", line.c_str());
        switch (type) {
            case 0:
                if (called[id]) {
                    print_errors(line, "This phone line has already called before!");
                }
                if (lined[id]) {
                    print_errors(line, "This phone has already taken an open line before calling!");
                }
                if (oped[id]) {
                    print_errors(line, "This phone has already reached an operator before calling!");
                }
                if (asked[id]) {
                    print_errors(line, "This phone has already asked a question before calling!");
                }
                if (left[id]) {
                    print_errors(line, "This phone has already hung up before calling!");
                }
                called[id] = true;
                break;
            case 1:
                if (!called[id]) {
                    print_errors(line, "This phone hasn't called but it has reached an open line!");
                }
                if (lined[id]) {
                    print_errors(line, "This phone has already taken an open line before!");
                }
                if (oped[id]) {
                    print_errors(line, "This phone has already reached an operator before taking this line!");
                }
                if (asked[id]) {
                    print_errors(line, "This phone has already asked a question before taking this line!");
                }
                if (left[id]) {
                    print_errors(line, "This phone has already hung up before taking this line!");
                }
                if (lines.size() == 5) {
                    string lines_string;
                    for (const auto &item: lines) {
                        lines_string.append(to_string(ids[item])).append(" ");
                    }
                    print_errors(line, "The 5 lines are full ( " + (lines_string) +
                                       ") but this phone still connected to one!");
                }
                lined[id] = true;
                lines.insert(id);
                break;
            case 2:
                if (!called[id]) {
                    print_errors(line, "This phone hasn't called but it has reached an open line!");
                }
                if (!lined[id]) {
                    print_errors(line, "This phone hasn't taken an open line but it has reached an operator!");
                }
                if (oped[id]) {
                    print_errors(line, "This phone has already reached an operator before!");
                }
                if (asked[id]) {
                    print_errors(line, "This phone has already asked a question before reaching this operator!");
                }
                if (left[id]) {
                    print_errors(line, "This phone has already hung up before reaching this operator!");
                }
                if (ops.size() == 3) {
                    string ops_string;
                    for (const auto &item: ops) {
                        ops_string.append(to_string(ids[item])).append(" ");
                    }
                    print_errors(line,
                                 "The 3 operators are full ( " + (ops_string) + ") but this phone still reached one!");
                }
                oped[id] = true;
                ops.insert(id);
                break;
            case 3:
                if (!called[id]) {
                    print_errors(line, "This phone hasn't called but it has asked a question!");
                }
                if (!lined[id]) {
                    print_errors(line, "This phone hasn't taken an open line but it has asked a question!");
                }
                if (!oped[id]) {
                    print_errors(line, "This phone hasn't reached an operator but it has asked a question!");
                }
                if (asked[id]) {
                    print_errors(line, "This phone has already asked a question before!");
                }
                if (left[id]) {
                    print_errors(line, "This phone has already hung up before asking this question!");
                }
                if (!ops.erase(id)) {
                    print_errors(line, "This phone isn't currently connected to an operator to even ask a question!");
                }
                asked[id] = true;
                break;
            case 4:
                if (!called[id]) {
                    print_errors(line, "This phone hasn't called but it hung up!");
                }
                if (!lined[id]) {
                    print_errors(line, "This phone hasn't taken an open line but it hung up!");
                }
                if (!oped[id]) {
                    print_errors(line, "This phone hasn't reached an operator but it hung up!");
                }
                if (!asked[id]) {
                    print_errors(line, "This phone hasn't asked a question but it hung up!");
                }
                if (left[id]) {
                    print_errors(line, "This phone has already hung up before!");
                }
                if (!lines.erase(id)) {
                    print_errors(line, "This phone isn't connected even connected to a line to hang up!");
                }
                left[id] = true;
                break;
            default:
                __builtin_unreachable(); // unnecessary but I never got to use this so might as well throw it in here
        }
    }
    fprintf(stderr, "Verifier finished!\n");
    return 0;
}

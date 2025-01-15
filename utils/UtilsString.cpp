#include "UtilsString.hpp"
#include <algorithm>

using namespace util::string;

static constexpr char Spaces[] = "\t\n\r ";

StringTitlefier::StringTitlefier(const std::string& str, std::vector<char>&& _predecessors)
    : s{ str }, predecessors{ std::move(_predecessors) }
{
    ;
}

StringTitlefier::StringTitlefier(std::string&& str, std::vector<char>&& _predecessors)
    : s(std::move(str)), predecessors{ std::move(_predecessors) }
{
    ;
}

char StringTitlefier::operator()(char ch) {
    size_t curOffset = offset++;
    if (curOffset == 0) {
        return ::toupper(ch);
    }
    else {
        if (std::find(predecessors.begin(), predecessors.end(), s[curOffset - 1]) != predecessors.end()) {
            return ::toupper(ch);
        }
        else {
            return ::tolower(ch);
        }
    }
}

std::string_view util::string::strip(std::string_view v) {
    auto p1 = v.find_first_not_of(Spaces);
    auto p2 = v.find_last_not_of(Spaces);
    if (p1 == std::string_view::npos) {
        return {};
    }
    return v.substr(p1, p2 ? p2 - p1 + 1 : v.npos);
}

std::vector<std::string_view> util::string::split(std::string_view v, const std::string& delim) {
    if (v.empty()) return {};
    size_t pos = 0;
    std::vector<std::string_view> res;
    do {
        size_t newPos = v.find(delim, pos);
        if (newPos != std::string_view::npos) {
            res.push_back(v.substr(pos, newPos - pos));
            newPos += delim.size();
        }
        else {
            res.push_back(v.substr(pos));
        }
        pos = newPos;
    } while (pos != std::string_view::npos);
    return res;
}

std::string util::string::v2str(std::string_view v) {
    return std::string(v.data(), v.size());
}

util::string::Splitter::SplitIterator::SplitIterator(pointer ptr, const std::string& delim, size_t _pos)
    : sv{ ptr }, delim{ delim }, prevPos{ _pos }, pos{ _pos }, count{ 0 }
{
    ++(*this);
}

util::string::Splitter::SplitIterator::reference util::string::Splitter::SplitIterator::operator*() const {
    if (prevPos == sv.npos) {
        return sv.substr(0, 0);
    }
    return sv.substr(prevPos, pos - prevPos);
}

util::string::Splitter::SplitIterator::pointer util::string::Splitter::SplitIterator::operator->() {
    if (prevPos == sv.npos) {
        return sv.substr(0, 0);
    }
    return sv.substr(prevPos, pos - prevPos);
}

// Prefix increment
util::string::Splitter::SplitIterator util::string::Splitter::SplitIterator::operator++() {
    if (pos == sv.npos) {
        prevPos = pos;
        return *this;
    }
    prevPos = pos;
    if (count) {
        prevPos += delim.size();
    }
    pos = sv.find(delim, prevPos);
    ++count;
    return *this;
}

// Postfix increment
util::string::Splitter::SplitIterator util::string::Splitter::SplitIterator::operator++(int) {
    SplitIterator tmp = *this;
    ++(*this);
    return tmp;
}

util::string::Splitter::Splitter(std::string_view _sv, const std::string& _delim)
    : sv{ _sv }, delim{ _delim }
{

}

util::string::Splitter::Splitter(std::string_view _sv, std::string&& _delim)
    : sv{ _sv }, delim{ std::move(_delim) }
{

}

util::string::Splitter::SplitIterator util::string::Splitter::begin() {
    return SplitIterator(sv, delim, 0);
}

util::string::Splitter::SplitIterator util::string::Splitter::end() {
    return SplitIterator(sv, delim, sv.npos);
}

std::string util::string::escapeQuotes(std::string_view str) {
    std::string res;
    res.reserve(str.size());
    size_t offset = 0;
    for (size_t i = 0; i < str.size(); ++i) {
        switch (str[i]) {
        case '\'':
        {
            res.append(str.substr(offset, i - offset));
            res.append("\\\'");
            offset = i + 1;
            break;
        }
        case '"':
        {
            res.append(str.substr(offset, i - offset));
            res.append("\\\"");
            offset = i + 1;
            break;
        }
        case '\\':
        {
            res.append(str.substr(offset, i - offset));
            res.append("\\\\");
            offset = i + 1;
            break;
        }
        }
    }
    res.append(str.substr(offset, str.npos));
    return res;
}

std::string util::string::escapeUnsafe(std::string_view str) {
    std::string res;
    res.reserve(str.size());
    size_t offset = 0;
    for (size_t i = 0; i < str.size(); ++i) {
        switch (str[i]) {
        case '\0':
        {
            res.append(str.substr(offset, i - offset));
            offset = i + 1;
            break;
        }
        case '\b':
        {
            res.append(str.substr(offset, i - offset));
            offset = i + 1;
            break;
        }
        case '\n':
        {
            res.append(str.substr(offset, i - offset));
            res.append("\\\n");
            offset = i + 1;
            break;
        }
        case '\r':
        {
            res.append(str.substr(offset, i - offset));
            res.append("\\\r");
            offset = i + 1;
            break;
        }
        case '\t':
        {
            res.append(str.substr(offset, i - offset));
            res.append("    ");
            offset = i + 1;
            break;
        }
        case '\Z':
        {
            res.append(str.substr(offset, i - offset));
            offset = i + 1;
            break;
        }
        case '\'':
        {
            res.append(str.substr(offset, i - offset));
            res.append("\\\'");
            offset = i + 1;
            break;
        }
        case '"':
        {
            res.append(str.substr(offset, i - offset));
            res.append("\\\"");
            offset = i + 1;
            break;
        }
        case '\\':
        {
            res.append(str.substr(offset, i - offset));
            res.append("\\\\");
            offset = i + 1;
            break;
        }
            /*case '%':
        {
            res.append(str.substr(offset, i - offset));
            res.append("\\\%");
            offset = i + 1;
            break;
        }
        case '\_':
        {
            res.append(str.substr(offset, i - offset));
            res.append("\\\_");
            offset = i + 1;
            break;
        }*/
        }
    }
    res.append(str.substr(offset, str.npos));
    return res;
}

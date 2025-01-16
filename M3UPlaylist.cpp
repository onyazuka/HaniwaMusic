#include "M3UPlaylist.hpp"
#include <format>
#include <algorithm>
#include "utils/UtilsString.hpp"

using namespace m3u;
using namespace util::string;

EXTINF EXTINF::fromStr(const std::string& s) {
    auto sv = strip(s);
    auto parts = split(sv, ",");
    EXTINF res;
    if (parts.size() != 1 && parts.size() != 2) {
         throw std::runtime_error("invalid string");
    }
    res.duration = std::stoull(std::string(parts[0].data(), parts[0].size()));
    if (parts.size() == 2) {
        res.title = std::string(parts[1].data(), parts[1].size());
    }
    return res;
}

void M3UEntry::dump(std::ostream& os) const {
    for (const auto& [key, val] : lParams) {
        os << std::format("{0}{1}{2}\n", key, val.empty() ? "" : ":", val);
    }
    os << _path << std::endl;
}

void M3UPlaylist::add(const M3UEntry& entry) {
    _entries.push_back(entry);
}

void M3UPlaylist::dump(std::ostream& os) const {
    os << "#EXTM3U" << std::endl;
    if (auto iter = gParams.find("#EXTENC"); iter != gParams.end()) {
        os << std::format("{0}{1}{2}\n", iter->first, iter->second.empty() ? "" : ":", iter->second);
    }
    for (const auto& [key, val] : gParams) {
        if (key == "#EXTM3U" || key == "#EXTENC") {
            continue;
        }
        os << std::format("{0}{1}{2}\n", key, val.empty() ? "" : ":", val);
    }
    for (const auto& entry : _entries) {
        entry.dump(os);
    }
}

M3UPlaylist M3UPlaylist::fromStream(std::istream& is) {
    M3UReader reader;
    while (is) {
        is >> reader;
    }
    return reader.playlist();
}

M3UWriter::M3UWriter(M3UPlaylist&& __playlist)
    : _playlist{std::move(__playlist)}
{
    ;
}

M3UWriter& M3UWriter::operator<<(const std::string& path) {
    writePath(path);
    return *this;
}

M3UWriter& M3UWriter::operator<<(std::pair<std::string, const std::string&> param) {
    writeParam(param);
    return *this;
}

M3UWriter& M3UWriter::operator<<(std::pair<size_t, const std::string&> param) {
    writeExtinf(param);
    return *this;
}

void M3UWriter::writePath(const std::string& path) {
    entry.setPath(path);
    _playlist.add(entry);
    entry = M3UEntry();
}

void M3UWriter::writeParam(std::pair<std::string, const std::string&> param) {
    std::transform(param.first.begin(), param.first.end(), param.first.begin(), [](char ch) { return std::toupper(ch); } );
    // no entries yet, so writing as global param
    if (_playlist.entries().empty() && entry.params().empty()) {
        _playlist.params()[std::format("#{}", param.first)] = param.second;
    }
    else {
        entry.params()[std::format("#{}", param.first)] = param.second;
    }
}

void M3UWriter::writeExtinf(std::pair<size_t, const std::string&> param) {
    entry.params()["#EXTINF"] = std::format("{}{}{}", param.first, param.second.empty() ? "" : ",", param.second);
}

bool M3UWriter::dumpToFile(const std::string& path) const {
    std::ofstream ofs = std::ofstream(path);
    if (!ofs) {
        return false;
    }
    _playlist.dump(ofs);
    return true;
}

std::string M3UWriter::dumpToString() const {
    std::stringstream ss;
    _playlist.dump(ss);
    return ss.str();
}

std::istream& m3u::operator>>(std::istream& is, M3UReader& reader) {
    if (!is) {
        return is;
    }
    std::string s;
    std::getline(is, s);
    try {
        std::string_view sv = strip(s);
        if (sv.empty()) {
            return is;
        }
        if (sv[0] == '#') {
            auto parts = split(sv,":");
            if (parts.size() != 2 && parts.size() != 1) {
                return is;
            }
            std::transform(parts[0].begin(), parts[0].end(), (char*)parts[0].begin(), [](char ch) { return std::toupper(ch); } );
            if (reader._playlist.entries().empty() && reader.entry.params().empty() && parts[0] != "#EXTINF") {
                reader._playlist.params()[std::string(parts[0].data(), parts[0].size())] = parts.size() == 2 ? std::string(parts[1].data(), parts[1].size()) : "";
            }
            else {
                reader.entry.params()[std::string(parts[0].data(), parts[0].size())] = parts.size() == 2 ? std::string(parts[1].data(), parts[1].size()) : "";
            }
        }
        else {
            reader.entry.setPath(std::string(sv.data(), sv.size()));
            reader._playlist.add(reader.entry);
            reader.entry = M3UEntry();
        }
    }
    catch (...) {
        return is;
    }

    return is;
}

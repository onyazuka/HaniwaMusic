#include "M3UPlaylist.hpp"
#include <format>
#include "utils/UtilsString.hpp"

using namespace m3u;
using namespace util::string;

IM3UEntry::~IM3UEntry() {
    ;
}

EXTINF::EXTINF(size_t durationS, const std::string& title)
    : _durationS{durationS}, _title{title}
{
    ;
}

void EXTINF::dump(std::ostream& os) const {
    os << std::format("#EXTINF:{0}{1}{2}", durationS(), title().empty() ? "" : ",", title()) << std::endl;
}

void PLAYLIST::dump(std::ostream& os) const {
    os << std::format("#PLAYLIST:{0}", _title);
}

PLAYLIST::PLAYLIST(const std::string& title)
    : _title{title}
{
    ;
}

void M3UEntry::dump(std::ostream& os) const {
    if (oExtinf) {
        oExtinf->dump(os);
    }
    os << path << std::endl;
}

void M3UPlaylist::add(const M3UEntry& entry) {
    _entries.push_back(entry);
}

void M3UPlaylist::dump(std::ostream& os) const {
    for (const auto& entry : _entries) {
        entry.dump(os);
    }
}

M3UPlaylist M3UPlaylist::fromStream(std::istream& is) {
    M3UWriter writer;
    std::string s;
    while (is) {
        std::getline(is, s);
        try {
            s = strip(s);
            if (s[0] == '#') {
                auto parts = split(s,":");
                if (parts.size() != 2) {
                    continue;
                }


                if (parts[0] == "#EXTINF")
                {
                    auto subparts = split(parts[1], ",");
                    switch (subparts.size()) {
                    case 1:
                        writer << EXTINF(std::stoull(strip(subparts[0]).data()));
                        break;
                    case 2:
                        writer << EXTINF(std::stoull(strip(subparts[0]).data()), strip(subparts[1]).data());
                    default:
                        continue;
                    }
                }

                else if (parts[0] == "#PLAYLIST") {
                    writer << PLAYLIST(strip(parts[1]).data());
                }

                // unknown/unsupported parts
                else {
                    continue;
                }
            }
            else {
                writer << s;
            }
        }
        catch (...) {
            continue;
        }
    }
    return writer.playlist();
}

M3UWriter::M3UWriter() {
    entry.reset(new M3UEntry());
}

M3UWriter& M3UWriter::operator<<(const EXTINF& extinf) {
    entry->oExtinf.emplace(extinf);
    return *this;
}

M3UWriter& M3UWriter::operator<<(const PLAYLIST& playlist) {
    entry->oPlaylist.emplace(playlist);
    return *this;
}

M3UWriter& M3UWriter::operator<<(const std::string& path) {
    entry->path = path;
    _playlist.add(*entry);
    entry.reset(new M3UEntry());
    //ss << path << std::endl;
    return *this;
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

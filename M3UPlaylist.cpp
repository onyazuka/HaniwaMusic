#include "M3UPlaylist.hpp"
#include <format>

using namespace m3u;

EXTINF::EXTINF(size_t durationS, const std::string& title)
    : _durationS{durationS}, _title{title}
{
    ;
}

void EXTINF::dump(std::ostream& os) const {
    os << std::format("#EXTINF:{0}{1}{2}", durationS(), title().empty() ? "" : ",", title()) << std::endl;
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

M3UWriter::M3UWriter() {
    entry.reset(new M3UEntry());
}

M3UWriter& M3UWriter::operator<<(const EXTINF& extinf) {
    entry->oExtinf.emplace(extinf);
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

#ifndef M3UPLAYLIST_HPP
#define M3UPLAYLIST_HPP
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <optional>
#include <memory>

namespace m3u {

    class EXTINF {
    public:
        EXTINF(size_t durationS, const std::string& title = "");
        inline size_t durationS() const { return _durationS; }
        inline const std::string& title() const { return _title; }
        void dump(std::ostream& os) const;
    private:
        const size_t _durationS;
        const std::string _title;
    };

    struct M3UEntry {
        std::optional<EXTINF> oExtinf;
        std::string path;
        void dump(std::ostream& os) const;
    };

    class M3UPlaylist {
    public:
        void add(const M3UEntry& entry);
        inline const std::vector<M3UEntry>& entries() const { return _entries; }
        void dump(std::ostream& os) const;
    private:
        std::vector<M3UEntry> _entries;
    };

    class M3UWriter {
    public:
        M3UWriter();
        M3UWriter& operator<<(const EXTINF& extinf);
        M3UWriter& operator<<(const std::string& path);
        const M3UPlaylist& playlist() const { return _playlist; }
        bool dumpToFile(const std::string& path) const;
        std::string dumpToString() const;
    private:
        std::shared_ptr<M3UEntry> entry;
        M3UPlaylist _playlist;
    };

}

#endif // M3UPLAYLIST_HPP

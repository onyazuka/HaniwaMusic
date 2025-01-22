#ifndef M3UPLAYLIST_HPP
#define M3UPLAYLIST_HPP
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <optional>
#include <memory>

namespace m3u {

    struct EXTINF {
        size_t duration;
        std::string title;
        static EXTINF fromStr(const std::string& s);
    };

    class M3UEntry {
    public:
        M3UEntry() = default;
        void dump(std::ostream& os) const;
        inline const std::string& path() const { return _path; }
        inline void setPath(const std::string& p) { _path = p; }
        size_t duration() const;
        inline std::map<std::string, std::string>& params() { return lParams; }
    private:
        std::map<std::string, std::string> lParams;
        std::string _path;
    };

    class M3UPlaylist {
    public:
        M3UPlaylist() = default;
        void add(const M3UEntry& entry);
        inline const std::vector<M3UEntry>& entries() const { return _entries; }
        void dump(std::ostream& os) const;
        std::string title() const;
        static M3UPlaylist fromStream(std::istream& is);
        inline std::map<std::string, std::string>& params() { return gParams; }
    private:
        std::vector<M3UEntry> _entries;
        std::map<std::string, std::string> gParams;
    };

    class M3UWriter {
    public:
        M3UWriter() = default;
        M3UWriter(M3UPlaylist&& playlist);
        M3UWriter& operator<<(const std::string& path);
        M3UWriter& operator<<(std::pair<std::string, const std::string&> param);
        M3UWriter& operator<<(std::pair<size_t, const std::string&> param);
        void writePath(const std::string& path);
        void writeParam(std::pair<std::string, const std::string&> param);
        void writeExtinf(std::pair<size_t, const std::string&> param);
        const M3UPlaylist& playlist() const { return _playlist; }
        bool dumpToFile(const std::string& path) const;
        std::string dumpToString() const;
    private:
        M3UEntry entry;
        M3UPlaylist _playlist;
    };

    class M3UReader {
    public:
        M3UReader() = default;
        friend std::istream& operator>>(std::istream& is, M3UReader& reader);
        const M3UPlaylist& playlist() const { return _playlist; }
    private:
        M3UEntry entry;
        M3UPlaylist _playlist;
    };

}

#endif // M3UPLAYLIST_HPP
